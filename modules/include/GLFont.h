////////////////////////////////////////////////////////////////////////////////////
// File: GLFont.cpp   -- Handle draw text                                           
// For conditions of distribution and use, see copyright notice in readme.txt       
//                                                                                  
// Copyright (C) 2020-2021 : 31/08/2021             Author  : Thuong.NV             
////////////////////////////////////////////////////////////////////////////////////

#ifndef GLFONT_H
#define GLFONT_H


#include <GL/glew.h>
#include <GL/GLU.h>
#include <map>

#include <GLCom.h>

#include "FTGL/FTGLOutlineFont.h"
#include "FTGL/FTGLPolygonFont.h"
#include "FTGL/FTGLBitmapFont.h"
#include "FTGL/FTGLTextureFont.h"
#include "FTGL/FTGLPixmapFont.h"

using namespace std;
using namespace glm;

class GLFont;
class GLFontManager;

enum FontType
{
    FTX_None   ,
    FTX_OutLine,
    FTX_Polygon,
    FTX_Texture,
    FTX_Bitmap ,
    FTX_Pixmap ,
    FTX_SP_MAX
};

typedef map<unsigned int,const GLFont*>  FontInfo;

//==================================================================================
// Class GLFont : Thao tác với font được load từ file                               
//==================================================================================
class GLFont
{
private:
    int            m_iType;
    FTFont*        m_Font;

public:
    GLFont() 
    {
        m_Font  = NULL;
        m_iType = FTX_None;
    }
    ~GLFont()
    {
        delete m_Font;
        m_Font = NULL;
    }

public:

    unsigned int GetSize() const
    {
        if (!m_Font) return 0;
        return m_Font->FaceSize();
    }

    FontType GetType() const
    {
        return (FontType)m_iType;
    }

    bool SetSize(unsigned int fontsize)
    {
        if (!m_Font || !m_Font->FaceSize(fontsize))
        {
            assert_m(0, "[Error:Font] : Set font size failed !");
            return false;
        }
        return true;
    }

    bool Load(string fname, unsigned int fontsize, FontType type)
    {
        switch (type)
        {
            case FTX_OutLine:
                m_Font = new FTGLOutlineFont(fname.c_str());
                m_iType = type;
                break;
            case FTX_Polygon:
                m_Font = new FTGLPolygonFont(fname.c_str());
                m_iType = type;
                break;
            case FTX_Texture:
                m_Font = new FTGLTextureFont(fname.c_str());
                m_iType = type;
                break;
            case FTX_Bitmap:
                m_Font = new FTGLBitmapFont(fname.c_str());
                m_iType = type;
                break;
            case FTX_Pixmap:
                m_Font = new FTGLPixmapFont(fname.c_str());
                m_iType = type;
                break;
            default:
                m_Font  = NULL;
                m_iType = FTX_None;
                break;
        }
        // Set font size failed !
        if (!this->SetSize(fontsize))
        {
            delete m_Font;
            m_iType = FTX_None;
            return false;
        }
        return true;
    }

    friend vec3 RenderText(const GLFont* font, const char* str, vec3 pos);
};




//==================================================================================
// Class GLFontManager : Quản lý các font đã được load                              
// Dựa vào dịnh danh tên và kích thước (font size)                                  
//==================================================================================
class GLFontManager
{
private:
    std::map<string, FontInfo>  fonts;

public:
    GLFontManager(){ }
    ~GLFontManager()
    {
        for (auto it = fonts.begin(); it != fonts.end(); it++)
        {
            for (auto itFont = it->second.begin(); 
                itFont != it->second.end(); itFont++)
            {
                delete itFont->second;
            }
        }
    }

private:

    //==================================================================================
    // Tìm kiểm danh sách font size bằng tên                                            
    //==================================================================================
    const FontInfo* FindFontInfo(string strNameFont) const
    {
        auto found = fonts.find(strNameFont);
        if (found != fonts.end())
        {
            return &found->second;
        }
        return NULL;
    }

    //==================================================================================
    // Tìm kiểm font đầu tiên trong danh sách                                           
    //==================================================================================
    const GLFont* FindFontFirst(string strNameFont) const
    {
        const FontInfo* found = FindFontInfo(strNameFont);
        if (found != NULL && !found->empty())
        {
            auto FontFound = found->begin();
            return FontFound->second;
        }
        return NULL;
    }

    //==================================================================================
    // Tìm font tương ứng với tên và kích thước                                         
    //==================================================================================
    const GLFont* FindFont(string strNameFont, unsigned int iFontsize) const
    {
        const FontInfo* found = FindFontInfo(strNameFont);
        if (found != NULL)
        {
            auto FontFound = found->find(iFontsize);

            if (FontFound != found->end() && FontFound->second)
            {
                return FontFound->second;
            }
        }
        return NULL;
    }

public:

    //==================================================================================
    // Lấy font đầu tiên trong danh sách                                                
    //==================================================================================
    const GLFont* GetFont(string strNameFont) const
    {
        return FindFontFirst(strNameFont);
    }

    //==================================================================================
    // Lấy font tương ứng với tên và kích thước                                         
    //==================================================================================
    const GLFont* GetFont(string strNameFont, unsigned int iFontsize) const
    {
        return FindFont(strNameFont, iFontsize);
    }

    //==================================================================================
    // Thêm một font mới với key là tên và kích thước                                   
    //==================================================================================
    const GLFont* LoadFont(string strKeyName, string strPath, unsigned int iFontsize, FontType iType)
    {
        const GLFont* fontFound = FindFont(strKeyName, iFontsize);
        if (fontFound) { return fontFound; }

        // Trường hợp chưa được load -> Load mới
        GLFont* font = new GLFont();
        if (font->Load(strPath, iFontsize, iType))
        {
            fonts[strKeyName].insert(std::make_pair(iFontsize, font));
            return font;
        }
        else
        {
            delete font;
        }
        return NULL;
    }
};

#endif // !GLFONT_H
