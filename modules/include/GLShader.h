////////////////////////////////////////////////////////////////////////////////////
// File: Shader.cpp   -- Create shader                                              
// Copyright (C) 2020-2021 Thuong.NV   Created : 02/12/2021                         
// For conditions of distribution and use, see copyright notice in readme.txt       
////////////////////////////////////////////////////////////////////////////////////

#ifndef GLSHADER_H
#define GLSHADER_H

#include <GLCom.h>

using namespace std;

//==============================================================================
//GLShader class                                                                
//==============================================================================
class GLShader
{
private:
    GLuint m_Program        = 0;
    GLuint m_VertexShader   = 0;
    GLuint m_FragmentShader = 0;
    GLuint m_GeometryShader = 0;

private:
    static string GetShaderLog(GLuint shader)
    {
        string errmsg = ""; int len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        if (len > 0)
        {
            int chwt = 0;
            char* log = new char[len +1];
            glGetShaderInfoLog(shader, len, &chwt, log);
            errmsg = log;
            delete[] log;
        }
        return errmsg;
    }

    static string GetProgramLog(GLuint program)
    {
        string errmsg = ""; int len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        if (len > 0)
        {
            int chwt = 0;
            char* log = new char[len + 1];
            glGetProgramInfoLog(program, len, &chwt, log);
            errmsg = log;
            delete[] log;
        }
        return errmsg;
    }

    static bool IsErrorShader(GLuint shader)
    {
        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        return isCompiled == GL_FALSE ? true: false;
    }

    static bool IsErrorProgram(GLint program)
    {
        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
        return isLinked == GL_FALSE ? true : false;
    }

    static bool CheckOpenGLError(string& err)
    {
        bool foundErr = false; err = "";
 
        int glErr = glGetError();

        while (glErr != GL_NO_ERROR)
        {
            err += ">>" + glErr;
            foundErr = true;
            glErr = glGetError();
        }
        return foundErr;
    }

    static GLuint LoadShader(const char* fpath, GLenum type)
    {
        if (!fpath) return 0;

        GLuint shader = 0; string context_file, err_msg;

        FileStatus status = DF_ReadContentFile(fpath, context_file);
        if (status != FileStatus::OK)
        {
            cout << "[System] Can't open file :" + string(fpath) << endl;
        }
        const char* code = context_file.c_str();

        shader = glCreateShader(type);
        glShaderSource(shader, 1, &code, NULL);
        glCompileShader(shader);
        if (IsErrorShader(shader))
        {
            err_msg = GetShaderLog(shader);
            cout << "====================================================" << endl;
            cout << "[ ERROR ] ofLoadShader: create failed !" << endl;
            cout <<  err_msg << endl;
            cout << "====================================================" << endl;
            glDeleteShader(shader); // Don't leak the shader.

            shader = 0;
        }
        return shader;
    }

    void SetDefault()
    {
        m_VertexShader   = 0;
        m_FragmentShader = 0;
        m_GeometryShader = 0;
        m_Program        = 0;
    }

    void DestroyShader()
    {
        glDetachShader(m_Program, m_VertexShader);
        glDetachShader(m_Program, m_FragmentShader);
        glDetachShader(m_Program, m_GeometryShader);

        glDeleteShader(m_VertexShader);
        glDeleteShader(m_FragmentShader);
        glDeleteShader(m_GeometryShader);
    }

    void Destroy()
    {
        this->DestroyShader();
        glDeleteProgram(m_Program);
        this->SetDefault();
    }

public:
    GLShader()
    {
        SetDefault();
    }

    ~GLShader()
    {
        Destroy();
    }

    GLuint GetProgram()
    {
        return m_Program;
    }

    void Begin()
    {
        glUseProgram(m_Program);
    }

    void End()
    {
        glUseProgram(0);
    }

    //==============================================================================
    // Thiết lập giá trị cho attribute shader                                       
    //==============================================================================
    void BindTexture2D(const GLuint& id)
    {
        glBindTexture(GL_TEXTURE_2D, id);
    }

    void SetUniformValue(const char* _name, const GLuint& _value)
    {
        GLint iLoc = glGetUniformLocation(m_Program, _name);
        glUniform1ui(iLoc, _value);
    }

    void SetUniformValue(const char* _name, const GLfloat& _value)
    {
        GLint iLoc = glGetUniformLocation(m_Program, _name);
        glUniform1f(iLoc, _value);
    }

    void SetUniformValue(const char* _name, const GLfloat* _value, const int& _size)
    {
        GLint iLoc = glGetUniformLocation(m_Program, _name);
        glUniform1fv(iLoc, _size, _value);
    }

    void SetUniformValue(const char* _name, const GLint* _value, const int& _size)
    {
        GLint iLoc = glGetUniformLocation(m_Program, _name);
        glUniform1iv(iLoc, _size, _value);
    }

    void SetUniformValue(const char* _name, const GLint& _value)
    {
        GLint iLoc = glGetUniformLocation(m_Program, _name);
        glUniform1i(iLoc, _value);
    }

    void SetUniformValue(const char* _name, const mat4& _value)
    {
        GLint iLoc = glGetUniformLocation(m_Program, _name);
        glUniformMatrix4fv(iLoc, 1, GL_FALSE, glm::value_ptr(_value));
    }

    void SetUniformValue(const char* _name, const vec2& _value)
    {
        GLint iLoc = glGetUniformLocation(m_Program, _name);
        glUniform2f(iLoc, _value.x, _value.y);
    }

    //==============================================================================
    // Tạo shader progam : Hỗ trợ VertexShader, FragmentShader, geometryShader      
    //==============================================================================
    bool Create(const char* vertexPath, const char* fragmentPath = NULL, const char* geometryPath = NULL)
    {
        m_VertexShader   = LoadShader(vertexPath,   GL_VERTEX_SHADER);
        m_FragmentShader = LoadShader(fragmentPath, GL_FRAGMENT_SHADER);
        m_GeometryShader = LoadShader(geometryPath, GL_GEOMETRY_SHADER);

        // Create program
        m_Program =  glCreateProgram();
        if(m_VertexShader  > 0)  glAttachShader(m_Program, m_VertexShader   );
        if(m_FragmentShader> 0)  glAttachShader(m_Program, m_FragmentShader );
        if(m_GeometryShader> 0)  glAttachShader(m_Program, m_GeometryShader );

        glLinkProgram(m_Program);
        if (IsErrorProgram(m_Program))
        {
            string err_msg = GetProgramLog(m_Program);
            cout << "=========================================" << endl;
            cout << "[ERROR] ofprogram: PROGRAM link failed !" << endl;
            cout << err_msg << endl;
            cout << "=========================================" << endl;
            this->Destroy();
            return false;
        }
        return true;
    }
};

#endif // !GLSHADER_H
