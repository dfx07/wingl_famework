////////////////////////////////////////////////////////////////////////////////////
// File : GLUtils.h            -- Các hàm sử dụng tiện ích sử dụng chung            
// For conditions of distribution and use, see copyright notice in readme.txt       
//                                                                                  
// Copyright (C) 2020-2021 : 31/08/2021             Author  : Thuong.NV             
////////////////////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/wglew.h>

#include <glm/glm.hpp>


using namespace glm;
using namespace std;

#define GL_COLOR_DEF {0.0, 0.0, 0.0}

namespace GLsp
{
    void DrawTriangle(glm::vec3 p1 , glm::vec3 p2, glm::vec3 p3, glm::vec3 color = GL_COLOR_DEF)
    {
        glColor3f(color.r, color.g, color.b);
        glBegin(GL_TRIANGLES);
        {
            glVertex3f(p1.x, p1.y, p1.z);
            glVertex3f(p2.x, p2.y, p2.z);
            glVertex3f(p3.x, p3.y, p3.z);
        }
        glEnd();
    }

    void DrawLine(glm::vec3 p1, glm::vec3 p2, glm::vec3 color = GL_COLOR_DEF)
    {
        glColor3f(color.r, color.g, color.b);
        glBegin(GL_LINES);
        {
            glVertex3f(p1.x, p1.y, p1.z);
            glVertex3f(p2.x, p2.y, p2.z);
        }
        glEnd();
    }

    void DrawPoint(glm::vec3 p1, float pointsize = 2.0f, glm::vec3 color = GL_COLOR_DEF)
    {
        float pointsizeold;
        glGetFloatv(GL_POINT_SIZE, &pointsizeold);

        glPointSize(pointsize);
        glColor3f(color.r, color.g, color.b);
        glBegin(GL_POINTS);
        {
            glVertex3f(p1.x, p1.y, p1.z);
        }
        glEnd();
        glPointSize(pointsizeold);
    }

    void DrawCube(glm::vec3 pos,float scale = 1.f)
    {
        glPushMatrix();

        glTranslatef(pos.x, pos.y, pos.z);
        glScalef(scale, scale, scale);
        //CUBE
        glBegin(GL_QUADS);
        {
            //front
            glNormal3f(1.0f,  0.0f,  0.0f);
            //glColor3f(color.r, color.g, color.b);
            glVertex3f(1.0f,  1.0f,  1.0f);
            glVertex3f(1.0f, -1.0f,  1.0f);
            glVertex3f(1.0f, -1.0f, -1.0f);
            glVertex3f(1.0f,  1.0f, -1.0f);

            //top
            glNormal3f( 0.0f,  0.0f, -1.0f);
            //glColor3f(color.r, color.g, color.b);
            glVertex3f( 1.0f,  1.0f, -1.0f);
            glVertex3f( 1.0f, -1.0f, -1.0f);
            glVertex3f(-1.0f, -1.0f, -1.0f);
            glVertex3f(-1.0f,  1.0f, -1.0f);

            // back
            glNormal3f(-1.0f,  0.0f,  0.0f);
            //glColor3f(color.r, color.g, color.b);
            glVertex3f(-1.0f,  1.0f, -1.0f);
            glVertex3f(-1.0f, -1.0f, -1.0f);
            glVertex3f(-1.0f, -1.0f,  1.0f);
            glVertex3f(-1.0f,  1.0f,  1.0f);

            // bottom
            glNormal3f( 0.0f,  0.0f, 1.0f);
            //glColor3f(color.r, color.g, color.b);
            glVertex3f(-1.0f,  1.0f, 1.0f);
            glVertex3f(-1.0f, -1.0f, 1.0f);
            glVertex3f( 1.0f, -1.0f, 1.0f);
            glVertex3f( 1.0f,  1.0f, 1.0f);

            // left
            glNormal3f( 0.0f,  1.0f,  0.0f);
            //glColor3f(color.r, color.g, color.b);
            glVertex3f(-1.0f,  1.0f, -1.0f);
            glVertex3f(-1.0f,  1.0f,  1.0f);
            glVertex3f( 1.0f,  1.0f,  1.0f);
            glVertex3f( 1.0f,  1.0f, -1.0f);

            //right
            glNormal3f( 0.0f,  -1.0f,  0.0f);
            //glColor3f(color.r, color.g, color.b);
            glVertex3f(-1.0f,  -1.0f,  1.0f);
            glVertex3f(-1.0f,  -1.0f, -1.0f);
            glVertex3f( 1.0f,  -1.0f, -1.0f);
            glVertex3f( 1.0f,  -1.0f,  1.0f);

        }
        glEnd();

        glPopMatrix();
    }
};

