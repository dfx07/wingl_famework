////////////////////////////////////////////////////////////////////////////////////
// File: GLWindow.cpp   -- Create and handle event window                           
// Copyright (C) 2020-2022 Thuong.NV   Created : 12/05/2022                         
// For conditions of distribution and use, see copyright notice in readme.txt       
////////////////////////////////////////////////////////////////////////////////////

//==================================================================================
// Các hàm sử dụng trong OpenGL có cải tiến giúp quá trình code nhanh hơn           
//==================================================================================

#ifndef GLSP_H
#define GLSP_H

#include <GL/glew.h>
#include <glm/glm.hpp>

using namespace glm;

#define GL_COLOR_BLACK  {0.f, 0.f, 0.f}
#define GL_COLOR_WHITE  {1.f, 1.f, 1.f}
#define GL_COLOR_RED    {1.f, 0.f, 0.f}
#define GL_COLOR_GREEN  {0.f, 1.f, 0.f}
#define GL_COLOR_BLUE   {0.f, 0.f, 1.f}

namespace glsp
{
    //==================================================================================
    // Vẽ một điểm                                                                      
    //==================================================================================
    void glDrawPoint(vec3 p, vec3 color, float sizewidth)
    {
        glPointSize(sizewidth);
        glBegin(GL_POINTS);
        {
            glVertex3f(p.x, p.y, p.z);
            glColor3f(color.r, color.g, color.b);
        }
        glEnd();
    }
    //==================================================================================
    // Vẽ một đoạn thẳng                                                                
    //==================================================================================
    void glDrawLine(vec3 p1, vec3 p2, vec3 col1 = GL_COLOR_BLACK, vec3 col2 = GL_COLOR_BLACK, float sizewidth =1.f)
    {
        glBegin(GL_LINES);
        {
            glLineWidth(sizewidth);
            glVertex3f(p1.x, p1.y, p1.z);
            glColor3f(col1.r, col1.g, col1.b);
            glVertex3f(p2.x, p2.y, p2.z);
            glColor3f(col2.r, col2.g, col2.b);
        }
        glEnd();
    }
}


#endif //!GLSP_H


