////////////////////////////////////////////////////////////////////////////////////
// File: GLRender.h     -- Handle draw : line , text, polygon                       
// For conditions of distribution and use, see copyright notice in readme.txt       
//                                                                                  
// Copyright (C) 2020-2021 : 31/08/2021             Author  : Thuong.NV             
////////////////////////////////////////////////////////////////////////////////////

#ifndef GLRENDER_H
#define GLRENDER_H

#include <GLCom.h>
#include <GL2DFun.h>
#include "GLRenderEL.h"

//==================================================================================
// Quy tắc vẽ không zoom rất đơn giản : từ tọa độ thực tế -> tọa độ view            
// thiết lập projection matrix là không zoom và view matrix là identify             
//==================================================================================
class GLRenderer
{
private:
    const GLCamera*     m_Camera;

    // Render data use zoom
    TextRenderer        m_TextRender;
    LineRenderer        m_LineRender;
    PolyRenderer        m_PolyRender;

    // Render data no zoom
    TextRenderer        m_TextRenderNozoom;
    LineRenderer        m_LineRenderNozoom;
    PolyRenderer        m_PolyRenderNozoom;


public:
    GLRenderer()
    {
        m_Camera = NULL;
    }
    ~GLRenderer()
    {
        this->Clear();
    }

public:
    void UseCamera(GLCamera* cam)
    {
        m_Camera = cam;
    }

    void RemoveCache()
    {
        m_LineRender.ClearCache();
        m_TextRender.ClearCache();
        m_PolyRender.ClearCache();

        m_TextRenderNozoom.ClearCache();
        m_LineRenderNozoom.ClearCache();
        m_PolyRenderNozoom.ClearCache();
    }

    void RemoveData()
    {
        m_TextRender.ClearData();
        m_LineRender.ClearData();
        m_PolyRender.ClearData();

        m_TextRenderNozoom.ClearData();
        m_LineRenderNozoom.ClearData();
        m_PolyRenderNozoom.ClearData();
    }

    void Clear()
    {
        this->RemoveCache();
        this->RemoveData();
    }

    void Draw()
    {
        if (!m_Camera) return;

        m_Camera->UseMatrix(CameraMode::CMODE_NORMAL);
        {
            m_TextRender.Draw();
            m_LineRender.Draw();
            m_PolyRender.Draw();
        }

        m_Camera->UseMatrix(CameraMode::CMODE_NOZOOM);
        {
            m_TextRenderNozoom.Draw();
            m_PolyRenderNozoom.Draw();
            m_LineRenderNozoom.Draw();
        }
    }

    void UpdateRender()
    {
        m_LineRender.UpdateDataRender();
        m_PolyRender.UpdateDataRender();

        m_PolyRenderNozoom.UpdateDataRender();
        m_LineRenderNozoom.UpdateDataRender();
    }

    //==================================================================================
    // Thêm một line vào dữ liệu vẽ                                                     
    // Chú ý: pos là tọa độ thực tế                                                     
    //==================================================================================
    void AddLine(vec3 p1, vec3 p2 , vec3 col1 , vec3 col2, bool zoom = true)
    {
        if (zoom)
        {
            m_LineRender.AddLine(p1, p2, col1, col2);
        }
        else
        {
            m_LineRenderNozoom.AddLine(p1, p2, col1, col2);
        }
    }

    //==================================================================================
    // Thêm một text vào dữ liệu vẽ                                                     
    // Chú ý: pos là tọa độ thực tế                                                     
    //==================================================================================
    void AddText(string str, vec3 pos, vec3 color, float angle,const GLFont* font, bool zoom= true)
    {
        if (zoom)
        {
            m_TextRender.AddText(str, pos, color, angle, font);
        }
        else
        {
            if (m_Camera->GetCameraType() == CameraType::C2D)
            {
                GLCamera2D* camera2D = (GLCamera2D*)m_Camera;
                vec2 ptemp= camera2D->PointGlobal2Local(pos);
                pos.x  = ptemp.x;
                pos.y  = ptemp.y;
            }
            pos = m_Camera->PointLT2Center(pos);
            m_TextRenderNozoom.AddTextNoZoom(str, pos, color, angle, font);
        }
    }

    //==================================================================================
    // Thêm một polygon vào dữ liệu vẽ                                                  
    // Chú ý: ap là mảng tọa độ thực tế của polygon                                     
    //==================================================================================
    void AddPoly(const vector<vec3>& ap, vec3 color, bool zoom= true)
    {
        // Đầu tiên cần chia polygon thành các tam giác
        vector<vec3> polytrig = M2D_DivPoly2Trig(ap);

        if (polytrig.empty())
        {
            assert(0); // ("RenderPoly : Poly -> Trig failed !");
            return;
        }

        // Phần này hơi xàm nhưng cũng cho vào @@
        int iOffset = polytrig.size() % 3;
        for (int i = 0; i < iOffset; i++)
        {
            polytrig.push_back(polytrig.back());
        }

        if (zoom)
        {
            m_PolyRender.AddPoly(polytrig, color);
        }
        else
        {
            // Chuyển đổi tọa độ nếu không sử dụng zoom trong trường hợp muốn theo 
            // tọa độ thực thì chỉ cần biến đổi một điểm cố định giống như AddText 
            // vec2 ptemp= camera2D->PointGlobal2Local(pos);
            // pos = m_Camera->PointLT2Center(pos);
            m_PolyRenderNozoom.AddPoly(polytrig, color);
        }
    }
};

#endif // !GLRENDER_H