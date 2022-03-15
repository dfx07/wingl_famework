////////////////////////////////////////////////////////////////////////////////
// File : GLCom.h                                                               
// Chứa Các lớp và các hàm sử dụng độc lập không liên quan đến các lớp tạo ra   
//                                                                              
// Created : 31/08/2021             Author  : Thuong.NV                         
////////////////////////////////////////////////////////////////////////////////

#ifndef GLCOM_H
#define GLCOM_H

#include <Windows.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <string>
#include <gdiplus.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sys/types.h>
#include <sys/stat.h>

using namespace std;
using namespace Gdiplus;
using namespace glm;


//=============================================================================
//↓ Debug code
#undef assert
#ifdef _UNICODE
#       define concate(s1, s2)      s1 ## s2
#else
#       define concate(s1, s2)      s2
#endif // UNICODE

#       define frefix_f(s)          concate(L,s)

#ifdef _DEBUG
#       define assert(f)            _ASSERT(f)
#       define assert_m(f, msg)     _ASSERT_EXPR(f , frefix_f(msg))

#else
#       define assert(f) 
#       define assert_m(f, msg)
#endif

#define msg_s               MB_ICONWARNING | MB_OK
#define bug_msg(title, msg) MessageBox(NULL,frefix_f(msg),frefix_f(title), msg_s)

#define STBGLDEF_STBGLDEF

#ifdef STBGLDEF_STBGLDEF
#define STBGLDEF static
#else
#define STBGLDEF extern
#endif


//↑ Debug code
//=============================================================================

//=============================================================================
//↓ OpenGL define

#define   X_AXIS       vec3(1.0, 0.0, 0.0)
#define   Y_AXIS       vec3(0.0, 1.0, 0.0)
#define   Z_AXIS       vec3(0.0, 0.0, 1.0)


#define   GL_WIH_COL   vec3(1.0, 1.0, 1.0)
#define   GL_RED_COL   vec3(1.0, 0.0, 0.0)
#define   GL_GRE_COL   vec3(0.0, 1.0, 0.0)
#define   GL_BLU_COL   vec3(0.0, 0.0, 1.0)
#define   GL_BLA_COL   vec3(0.0, 0.0, 0.0)

//=============================================================================
//↓ Logic define

#define NUM_DECISION  0.00001
#define V3ZERO        {0.f, 0.f, 0.f}

//=============================================================================
//↓ Disable warning

#pragma warning(disable : 4267) // Ép kiểu
#pragma warning(disable : 4996) // Hàm không an toàn


//=============================================================================
//↓ Function define 

#define DELETE_VECTOR_PTR(v)         \
for (int i = 0; i < v.size(); i++)   \
{                                    \
    delete v[i];                     \
}                                    \
v.clear()                            \

#define DELETE_MAP_PTR(ma)                       \
for (auto it = ma.begin(); it != ma.end(); it++) \
{                                                \
    delete it->second;                           \
}                                                \
ma.clear()                                       \


#define DELETE_PTR(ptr)                          \
delete ptr;                                      \
ptr = NULL                                       \


enum class FileStatus
{
    ERR_WRITE , // write faild
    ERR_OPEN  , // Load faild

    NOEXITS   , // Không tồn tại
    DIR       , // Là thư mục
    FILE      , // Là file 
    OK        , // Đọc OK
    NONE      , // Không xác định được trạng thái
};

class GDIplusToken
{
public:
    ULONG_PTR               m_id;
    GdiplusStartupInput     m_prop;

public:
    GDIplusToken()
    {
        m_id = NULL;
    }
    bool IsOk()
    {
        return (m_id != NULL) ? true : false;
    }
};


// Chứa thông tin đồ họa sử dụng OpenGL
struct WndRender
{
    HDC             m_hDc;
    HGLRC           m_hGLRC;
};

// Chứa thông tin về màn hình hiển thị
struct MonitorInfo
{
    DWORD    WIDTH  ;
    DWORD    HEIGHT ;
    DWORD    DISFREQ;
    DWORD    VERSION;
    string   NAME   ;

    MonitorInfo()
    {
        WIDTH   = 640;
        HEIGHT  = 480;
        DISFREQ = 24;
        NAME    = "None";
    }
};

//==================================================================================
// Lấy thông tin thông số màn hình                                                  
// return : MonitorInfo (Thông số)                                                  
//==================================================================================
STBGLDEF MonitorInfo GetMonitorInforEx()
{
    MonitorInfo infor;
    DEVMODE devmode;

    devmode.dmSize = sizeof(DEVMODE);
    BOOL bResult = EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);

    if (bResult)
    {
        infor.WIDTH   = devmode.dmPelsWidth;
        infor.HEIGHT  = devmode.dmPelsHeight;
        infor.DISFREQ = devmode.dmDisplayFrequency;
        infor.VERSION = devmode.dmDriverVersion;
        infor.NAME    = string((const char*)&(devmode.dmDeviceName[0]), CCHDEVICENAME);
    }
    return infor;
}


//==================================================================================
// Hàm sử dụng chung cho xử lý GDI plus                                             
//==================================================================================
STBGLDEF GDIplusToken CreateGDIplusEx()
{
    GDIplusToken        gdiToken;
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;

    Gdiplus::Status status = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    if (status == Gdiplus::Status::Ok)
    {
        gdiToken.m_id    = gdiplusToken;
        gdiToken.m_prop  = gdiplusStartupInput;
    }
    else
    {
        gdiToken.m_id = NULL;
    }
    return gdiToken;
}

STBGLDEF bool DeleteGDIplusEx(GDIplusToken& gdiToken)
{
    GdiplusShutdown(gdiToken.m_id);
    return true;
}


//==================================================================================
// Hàm sử dụng chung cho xử lý OpenGL                                               
//==================================================================================
STBGLDEF LRESULT CALLBACK WndDummyMainProc(HWND hWnd, UINT message, WPARAM wParam,
                                                                  LPARAM lParam)
{
    return DefWindowProc(hWnd, message, wParam, lParam);
}

//==================================================================================
// Khởi tạo một lớp class cho window                                                
// return : 0 Khởi tại thất bại                                                     
//          1 Thành công                                                            
//==================================================================================
STBGLDEF int RegisterWindowClass(const char * strClassName, WNDPROC Proc,             
                               HINSTANCE hInst = GetModuleHandle(NULL))
{
    WNDCLASSEX  wClass;
    ZeroMemory(&wClass, sizeof(WNDCLASSEX));
    wClass.cbClsExtra    = NULL;
    wClass.cbSize        = sizeof(WNDCLASSEX);
    wClass.cbWndExtra    = NULL;
    wClass.hbrBackground = (HBRUSH)(COLOR_MENU);
    wClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wClass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wClass.hIconSm       = NULL;
    wClass.hInstance     = hInst;
    wClass.lpfnWndProc   = (WNDPROC)Proc;
    wClass.lpszClassName = strClassName;
    wClass.lpszMenuName  = NULL;
    wClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

    if (!RegisterClassEx(&wClass))
    {
        bug_msg("Error", "Could not register class!");
        return 0;
    }
    return 1;
}

//==================================================================================
// Before we can load extensions, we need a dummy OpenGL context, created using a   
// dummy window.We use a dummy window because you can only set the pixel format for 
// a window once. For the real window, we want to use wglChoosePixelFormatARB (so we
// can potentially specify options that aren't available in PIXELFORMATDESCRIPTOR), 
// but we can't load and use that before we have a context.                         

// Khởi tạo và load OpenGL extension (không sử dụng nhiều lần)                      
// Return : true    [Khởi tại thất bại]                                             
//          false   [Thành công       ]                                             
//==================================================================================
STBGLDEF bool LoadOpenGLExtension()
{
    bool bInitOk = false;
    if (RegisterWindowClass("DummyClass", WndDummyMainProc))
    {
        HWND hWnd_dummy = CreateWindowExA(
                          0,"DummyClass", "Dummy OpenGL Window", 0,
                          CW_USEDEFAULT,CW_USEDEFAULT,
                          CW_USEDEFAULT,CW_USEDEFAULT,
                          0,0,NULL,0);

        PIXELFORMATDESCRIPTOR pixelFormat = {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
            32,                   // Colordepth of the framebuffer.
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24,                   // Number of bits for the depthbuffer
            8,                    // Number of bits for the stencilbuffer
            0,                    // Number of Aux buffers in the framebuffer.
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
        };

        HDC hDC_dummy    = GetDC(hWnd_dummy);
        int iPixelFormat = ChoosePixelFormat(hDC_dummy, &pixelFormat);
        SetPixelFormat(hDC_dummy, iPixelFormat, &pixelFormat);

        HGLRC hglrc_dummy = wglCreateContext(hDC_dummy);
        if (wglMakeCurrent(hDC_dummy, hglrc_dummy)) {

            if (glewInit() != GLEW_OK)  // Load library OpenGL extension
            {
                // Load OpenGL extension failed;
                assert_m(0, "Load OpenGL extension failed");
                bInitOk = false;
            }
            else
            {
                bInitOk = true;
            }
        }

        wglMakeCurrent(hDC_dummy, 0);
        wglDeleteContext(hglrc_dummy);
        ReleaseDC(hWnd_dummy, hDC_dummy);
        DestroyWindow(hWnd_dummy);
    }

    return bInitOk;
}

//==================================================================================
//Hàm mở rộng của OpenGL với thư viện glm                                           
//==================================================================================
STBGLDEF void glSetTranslate3(vec3 pos)
{
    glTranslatef(pos.x, pos.y, pos.z);
}

STBGLDEF void glSetRotate(float angle, vec3 axis) // angle = degrees
{
    glRotatef(angle, axis.x, axis.y, axis.z);
}

STBGLDEF void glSetColor3(vec3 color)
{
    glColor3f(color.r, color.g, color.b);
}
STBGLDEF void glSetColor4(vec4 color)
{
    glColor4f(color.r, color.g, color.b, color.a);
}
STBGLDEF vec3 NormalColor(float r, float g, float b)
{
    vec3 nColor = vec3(r, g, b)/256.f;
    return nColor;
}

//==================================================================================
//Chuyển số sang ký tự                                                              
//==================================================================================
STBGLDEF string Number2String(float number, int precision = 0, bool bTrailZero = true)
{
    char buffer[256];
    memset(buffer, '\0', 256);
    if (fabs(number - roundf(number)) < NUM_DECISION)
    {
        sprintf(buffer,"%d", int(number));
        return buffer;
    }
    else
    {
        sprintf(buffer,"%.*f", precision, number);
        string temp = buffer;
        if (bTrailZero)
        {
            int index = temp.find_last_not_of('0') + 1;
            if (index < temp.size())
            {
                temp.erase(index, std::string::npos);
            }
        }
        return temp;
    }
}

//==================================================================================
// Tách chuỗi sử dụng delimiter  Sử dụng hàm stringstream                           
//==================================================================================
STBGLDEF vector<string> SplitString(const std::string &str, const char& delim)
{
    vector<string> str_split; str_split.reserve(4);
    stringstream ss(str); string tok = "";
    while (std::getline(ss, tok, delim))
    {
        if (!tok.empty())
        {
            str_split.push_back(tok);
        }
    }
    return str_split;
}

//==================================================================================
// Tách chuỗi sử dụng delimiter  Sử dụng hàm strtok()                               
//==================================================================================
STBGLDEF vector<string> SplitString(const std::string &str, const char* delim)
{
    vector<string> str_split;  str_split.reserve(4);  char* next_token;
    char *token = strtok_s(const_cast<char*>(str.c_str()), delim, &next_token);
    while (token != nullptr)
    {
        str_split.push_back(std::string(token));
        token = strtok_s(nullptr, delim, &next_token);
    }
    return str_split;
}

//==================================================================================
// Lấy tên file và phần mở rộng từ đường dẫn                                        
//==================================================================================
STBGLDEF string StandardFileName(string filename)
{
    // Xóa ký tự new line '\n'
    filename.erase(std::remove(filename.begin(), filename.end(), '\n'), filename.end());

    // Xóa ký tự trắng đầu cuối
    while (!filename.empty() && 
            filename.front() == ' ')
    {
        filename.erase(0, 1);
    }
    // Xóa ký tự đầu
    while (!filename.empty() && 
            filename.back() == ' ')
    {
        filename.pop_back();
    }
    return filename;
}

//==================================================================================
// Lấy tên file và phần mở rộng từ đường dẫn                                        
//==================================================================================
STBGLDEF string GetFileName(string filename, bool bGetExt = true)
{
    string fname = filename.substr(filename.find_last_of("/\\") + 1);
    if (!bGetExt)
    {
        size_t found = fname.find_last_of(".");
        if (found >= 0)
        {
            fname = fname.substr(0, found);
        }
    }
    return fname;
}

//==================================================================================
// Lấy đường dẫn từ đường dẫn file                                                  
//==================================================================================
STBGLDEF string GetDirection(string filename)
{
    string direction;
    const size_t last_slash_idx = filename.find_last_of("/\\");
    if (last_slash_idx >= 0)
    {
        direction = filename.substr(0, last_slash_idx);
    }
    return direction;
}

//==================================================================================
// Đọc một giá trị float con trỏ sẽ bị dịch chuyển                                  
//==================================================================================
STBGLDEF float ReadFloat(FILE* file, float defvalue = -1.0f)
{
    float x; x = defvalue;
    fscanf_s(file, "%f", &x);
    return x;
}

//==================================================================================
// Lấy đường dẫn file đang thực thi                                                 
//==================================================================================
STBGLDEF string GetModuleCurrent()
{
    CHAR buff[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, buff, MAX_PATH);
    string path = GetDirection(buff);
    return path;
}

//==================================================================================
// Đọc 3 giá trị float từ luồng file với giá trị mặc định con trỏ sẽ bị dịch chuyển 
// Bằng mặc định -1   Format: v x y z                                               
// Chú ý: Loại dữ liệu phải có hàm khởi tạo 3 tham số                               
//==================================================================================
template<typename T>
STBGLDEF T* Read3Float(FILE* file, float defvalue = -1.0f)
{
    float x, y, z;
    x = y = z = defvalue;
    fscanf_s(file, "%f", &x);
    fscanf_s(file, "%f", &y);
    fscanf_s(file, "%f", &z);
    T* vertex = new T(x, y, z);
    return vertex;
}

//==================================================================================
// Đọc một dòng dữ liệu sang dạng string từ vị trí con trỏ file                     
// Note : Giới hạn 256 ký tự cho một lần đọc                                        
//==================================================================================
STBGLDEF string ReadLine(FILE* file)
{
    char buff[256];
    memset(buff, '\0', 256);
    fgets(buff, 256, file);
    return buff;
}

//==================================================================================
// Đọc chuỗi đầu tiên của một dòng được kết thúc bởi dấu cách                       
// Note : Giới hạn 256 ký tự cho chuỗi                                              
//==================================================================================
STBGLDEF string ReadFirstString(FILE* file)
{
    char buff[256];
    memset(buff, '\0', 256);
    fscanf_s(file, "%s", &buff[0], 256);
    return buff;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Xác định trạng thái của đường dẫn
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
STBGLDEF FileStatus DF_CheckStatusPath(const char* fpath)
{
    struct stat info;

    if (!fpath || stat(fpath, &info) != 0)     return FileStatus::NOEXITS;
    else if (info.st_mode & S_IFDIR) return FileStatus::DIR;
    else if (info.st_mode & S_IFREG) return FileStatus::FILE;

    return FileStatus::NONE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Đọc nội dung của file
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
STBGLDEF FileStatus DF_ReadContentFile(const char* fpath, string& content)
{
    //🠇 Thuong.NV Không sử dụng nó nữa chuyển sang cách khác hay hợp
    //ifstream input_file;
    //string content = "";
    //string strLine = "";

    //size_t length = GetLengthFile(fpath);
    //content.reserve(length);

    //input_file.open(fpath, ios::in);
    //if (input_file.is_open())
    //{
    //    while (std::getline(input_file, strLine))
    //    {
    //        content.append(strLine +"\n");
    //    }
    //}
    //input_file.close();
    //🠅 Thuong.NV Không sử dụng nó nữa chuyển sang cách khác hay hợp

    //🠇 Thuong.NV Đọc một file không sử dụng nối file
    FILE *File = NULL;
    FileStatus status = DF_CheckStatusPath(fpath);

    if (status != FileStatus::FILE)
    {
        return status;
    }

    if (fopen_s(&File, fpath, "rb") != 0)
    {
        return FileStatus::ERR_OPEN;
    }

    fseek(File, 0, SEEK_END);
    long  f_size = ftell(File);
    fseek(File, 0, SEEK_SET);

    char* f_content = new char[f_size + 1];

    fread(f_content, 1, f_size, File);
    fclose(File);

    f_content[f_size] = 0;

    content = f_content;

    delete[] f_content;
    //🠅 Thuong.NV Đọc một file không sử dụng nối file

    return FileStatus::OK;
}
#endif // !GLCOM_H