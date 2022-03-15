////////////////////////////////////////////////////////////////////////////////////
// File: GLRender.h     -- Handle draw : line , text, polygon                       
// For conditions of distribution and use, see copyright notice in readme.txt       
//                                                                                  
// Copyright (C) 2020-2021 : 31/08/2021             Author  : Thuong.NV             
////////////////////////////////////////////////////////////////////////////////////

#ifndef RENDEREL_H
#define RENDEREL_H

#include "GLFont.h"


//==================================================================================
// Định nghĩa dữ liệu cache lưu trữ dữ liệu                                         
// Bao gồm : Text , Line , Trig                                                     
//==================================================================================
struct tagTextData
{
    string      m_strText;
    float       m_angle  ;
    vec3        m_color  ;
    vec3        m_p      ;
};


struct tagLineData
{
    vec3        m_p1    ;
    vec3        m_p2    ;
    vec3        m_color1;
    vec3        m_color2;
    float       m_think ;
};

struct tagTrigData
{
    vec3        m_p1    ;
    vec3        m_p2    ;
    vec3        m_p3    ;
    vec3        m_color1;
    vec3        m_color2;
    vec3        m_color3;
};


//==================================================================================
// Class TextRenderer : Dùng để chứ dữ liệu và render text                          
//==================================================================================

//==================================================================================
// Đây là thư viện FTFont : Kết xuất chuỗi cho đầu ra (OpenGL)                      
// Trong trường hợp font type là : FTX_Bitmap ,FTX_Pixmap thì sử dụng               
//      glRasterPos3f(x, y, z); (tọa độ vẽ)                                         
// Trong trường hợp font type là : FTX_Texture thì sử dụng                          
//      glEnable(GL_TEXTURE_2D);                                                    
//      glEnable(GL_BLEND);                                                         
//      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                          
// @Trả về : vị trí sau khi ký tự cuối cùng được vẽ                                 
//==================================================================================
vec3 RenderText(const GLFont* font, const char* str, vec3 pos)
{
    FTPoint position(pos.x, pos.y, pos.z);
    position = font->m_Font->Render(str, -1, position);
    return vec3(position.X(), position.Y(), position.Z());
}

class TextRenderer
{
private:
    // !! GLFont không xóa ở đây -> Bên GLFontManager
    map<const GLFont*, vector<tagTextData*>>     m_Data;

public:

    void ClearCache()
    {
        //@_@
    }

    void ClearData()
    {
        for (auto it = m_Data.begin(); it != m_Data.end(); it++)
        {
            vector<tagTextData*>* arrData = &it->second;
            int sizedata = arrData->size();

            for (int i = 0; i < sizedata; i++)
            {
                delete arrData->at(i);
            }
        }
        m_Data.clear();
    }
public:
    void AddText(string text, vec3 pos, vec3 color, float angle, const GLFont* font)
    {
        tagTextData* data = new tagTextData();

        data->m_p        = pos;
        data->m_strText  = text;
        data->m_color    = color;
        data->m_angle    = angle;

        m_Data[font].push_back(data);
    }

    //==================================================================================
    // Thêm text vào dữ liệu                                                            
    //      - pos   : là tọa độ theo trung tâm view                                     
    //      - angle : là góc so với tọa điểm pos (degrees)                              
    // Chú ý: Text không zoom thì pos là tọa độ so với tâm trên view                    
    //==================================================================================
    void AddTextNoZoom(string text, vec3 pos, vec3 color, float angle, const GLFont* font)
    {
        tagTextData* data = new tagTextData();

        data->m_p       = pos;
        data->m_strText = text;
        data->m_color   = color;
        data->m_angle   = angle;

        m_Data[font].push_back(data);
    }

    void Draw()
    {
        if (m_Data.empty()) return;

        for (auto it = m_Data.begin(); it != m_Data.end(); it++)
        {
            vector<tagTextData*>* arrData = &it->second;
            const GLFont* font    = it->first;

            // Tiền xử lý font 
            switch (font->GetType())
            {
                case FTX_Texture:
                {
                    glEnable(GL_TEXTURE_2D);
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    break;
                }
                default: break;
            }

            // Tiến hành vẽ text
            for (int i = 0; i < arrData->size(); i++)
            {
                tagTextData* data = arrData->at(i);

                // Set màu trước khi vẽ 
                glSetColor3(data->m_color);

                switch (font->GetType())
                {
                    case FTX_Polygon:
                    case FTX_Texture:
                    {
                        glPushMatrix();
                        glSetRotate(data->m_angle, Z_AXIS); // Xoay theo trục oz
                        RenderText(font, data->m_strText.c_str(), data->m_p);
                        glPopMatrix();
                        break;
                    }
                    default: break;
                }
            }

            // Kết thúc xử lý
            switch (font->GetType())
            {
                case FTX_Texture:
                {
                    glDisable(GL_TEXTURE_2D);
                    glDisable(GL_BLEND);
                    break;
                }

                case FTX_OutLine:
                {
                    break;
                }
                default: break;
            }
        }
    }
};

//==================================================================================
// Class LineRenderer : Dùng để chứ dữ liệu và render line                          
//==================================================================================
class LineRenderer
{
private:
    vector<tagLineData*>     m_Data;
    vector<GLfloat>          m_DataRender;


public:

    void ClearCache()
    {
        m_DataRender.clear();
    }

    void ClearData()
    {
        for (int i = 0; i < m_Data.size(); i++)
        {
            delete m_Data[i];
        }
        m_Data.clear();
    }

    void UpdateDataRender()
    {
        for (int i = 0; i < m_Data.size(); i++)
        {
            auto data = m_Data[i];
            // Tọa độ điểm 1
            m_DataRender.push_back(data->m_p1.x);
            m_DataRender.push_back(data->m_p1.y);
            m_DataRender.push_back(data->m_p1.z);

            // Màu của điểm 1
            m_DataRender.push_back(data->m_color1.x);
            m_DataRender.push_back(data->m_color1.y);
            m_DataRender.push_back(data->m_color1.z);

            // Tọa độ điểm 2
            m_DataRender.push_back(data->m_p2.x);
            m_DataRender.push_back(data->m_p2.y);
            m_DataRender.push_back(data->m_p2.z);

            // Màu của điểm 2
            m_DataRender.push_back(data->m_color2.x);
            m_DataRender.push_back(data->m_color2.y);
            m_DataRender.push_back(data->m_color2.z);

            // Độ dày
            //m_DataRender.push_back(data->m_color2.x);
        }
    }

    //==================================================================================
    // Thêm một đường thẳng vào dữ liệu                                                 
    // Chú ý: Đường thẳng không zoom thì p1, p2 là tọa độ so với tâm trên view          
    //==================================================================================
    void AddLine(vec3 p1, vec3 p2, vec3 col1, vec3 col2)
    {
        tagLineData* data = new tagLineData();
        data->m_p1     = p1;
        data->m_p2     = p2;
        data->m_color1 = col1;
        data->m_color2 = col2;
        m_Data.push_back(data);
    }

    void Draw()
    {
        if (m_DataRender.empty()) return;

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glVertexPointer(3, GL_FLOAT, 6*sizeof(GLfloat), &m_DataRender[0]);
        glColorPointer(3, GL_FLOAT,  6*sizeof(GLfloat), &m_DataRender[3]);

        glDrawArrays(GL_LINES, 0, m_DataRender.size()/6);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }
};

//==================================================================================
// Class PolyRenderer : Dùng để chứ dữ liệu và render line                          
//==================================================================================
class PolyRenderer
{
private:
    vector<tagTrigData*>   m_Data;
    vector<GLfloat>        m_DataRender;

public:
    void ClearCache()
    {
        m_DataRender.clear();
    }
    void ClearData()
    {
        for (int i = 0; i < m_Data.size(); i++)
        {
            delete m_Data[i];
        }
        m_Data.clear();
    }

    void UpdateDataRender()
    {
        for (int i = 0; i < m_Data.size(); i++)
        {
            auto data = m_Data[i];
            // Tọa độ điểm 1
            m_DataRender.push_back(data->m_p1.x);
            m_DataRender.push_back(data->m_p1.y);
            m_DataRender.push_back(data->m_p1.z);

            // Màu của điểm 1
            m_DataRender.push_back(data->m_color1.x);
            m_DataRender.push_back(data->m_color1.y);
            m_DataRender.push_back(data->m_color1.z);

            // Tọa độ điểm 2
            m_DataRender.push_back(data->m_p2.x);
            m_DataRender.push_back(data->m_p2.y);
            m_DataRender.push_back(data->m_p2.z);

            // Màu của điểm 2
            m_DataRender.push_back(data->m_color2.x);
            m_DataRender.push_back(data->m_color2.y);
            m_DataRender.push_back(data->m_color2.z);

            // Tọa độ điểm 3
            m_DataRender.push_back(data->m_p3.x);
            m_DataRender.push_back(data->m_p3.y);
            m_DataRender.push_back(data->m_p3.z);

            // Màu của điểm 3
            m_DataRender.push_back(data->m_color3.x);
            m_DataRender.push_back(data->m_color3.y);
            m_DataRender.push_back(data->m_color3.z);
        }
    }

    //==================================================================================
    // Thêm một polygon vào dữ liệu (Được chia thành các tam giác)                      
    //      - ap    : mảng điểm các tam giác ghép thành polygon                         
    // Chú ý: Polygon không zoom thì ap là mảng tọa độ so với tâm trên view             
    //==================================================================================
    void AddPoly(const vector<vec3>& ap, vec3 color)
    {
        for (int i = 0; i < ap.size(); i+=3)
        {
            tagTrigData* data = new tagTrigData();

            data->m_p1     = ap[i];
            data->m_p2     = ap[i+1];
            data->m_p3     = ap[i+2];

            data->m_color1 = color;
            data->m_color2 = color;
            data->m_color3 = color;

            m_Data.push_back(data);
        }
    }

    void Draw()
    {
        if (m_DataRender.empty()) return;

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glVertexPointer(3, GL_FLOAT, 6*sizeof(GLfloat), &m_DataRender[0]);
        glColorPointer(3, GL_FLOAT,  6*sizeof(GLfloat), &m_DataRender[3]);

        glDrawArrays(GL_TRIANGLES, 0, m_DataRender.size()/6);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }
};
#endif // !RENDEREL_H


