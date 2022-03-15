////////////////////////////////////////////////////////////////////////////////////
// File: GLRESOURCE_H   -- Create and handle resource : image , texture, sound      
// Copyright (C) 2020-2021 Thuong.NV   Created : 31/08/2021                         
// For conditions of distribution and use, see copyright notice in readme.txt       
////////////////////////////////////////////////////////////////////////////////////

#ifndef GLRESOURCE_H
#define GLRESOURCE_H

#include <map>
#include <GLCom.h>
#include <std_image.h>

using namespace std;

//==================================================================================
// Class GLImage: Thông tin về image                                                
//==================================================================================
class GLImage
{
private:
    int            m_width;
    int            m_height;
    int            m_nChanels;

    unsigned char* m_Data;

private:

    //------------------------------------------------------------------------------
    // Thiết lập thông số image về mặc định
    //------------------------------------------------------------------------------
    void Default()
    {
        m_width     = 0;
        m_height    = 0;
        m_nChanels  = 0;
    }

    void Free()
    {
        stbi_image_free(m_Data);
    }

public:
    GLImage()
    {
        this->Default();
    }

    ~GLImage()
    {
        this->Free();
        this->Default();
    }

    bool IsOk() { m_Data ? true : false; }

    bool LoadFromFile(string fname)
    {
        this->Default();
        int req_Channel = 0;

        //Sử dụng thư viện stb_image - v2.27
        m_Data = stbi_load(fname.c_str(), &m_width, &m_height, &m_nChanels, req_Channel);

        return m_Data ? true : false;
    }

    bool LoadFromBitmap(HBITMAP BitmapHandle)
    {
        this->Default();

        HDC DC = CreateCompatibleDC(NULL);
        if (DC)
        {
            BITMAP Bmp      = { 0 };
            BITMAPINFO Info = { 0 };

            std::memset(&Info, 0, sizeof(BITMAPINFO)); //not necessary really..
            HBITMAP OldBitmap = (HBITMAP)SelectObject(DC, BitmapHandle);
            GetObject(BitmapHandle, sizeof(Bmp), &Bmp);

            Info.bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
            Info.bmiHeader.biWidth  = m_width  = Bmp.bmWidth;
            Info.bmiHeader.biHeight = m_height = Bmp.bmHeight;
            Info.bmiHeader.biPlanes = 1;
            Info.bmiHeader.biBitCount    = Bmp.bmBitsPixel;
            Info.bmiHeader.biCompression = BI_RGB;
            Info.bmiHeader.biSizeImage   = ((m_width * Bmp.bmBitsPixel + 31) / 32) * 4 * m_height;

            m_Data = (unsigned char *)stbi__malloc(Info.bmiHeader.biSizeImage);
            GetDIBits(DC, BitmapHandle, 0, m_height, m_Data, &Info, DIB_RGB_COLORS);
            SelectObject(DC, OldBitmap);

            m_height = std::abs(m_height);
            DeleteDC(DC);
        }

        return m_Data ? true : false;
    }

    friend class GLTexture;
};

//==================================================================================
// Class GLTexture: Thông tin về texture                                            
//==================================================================================
class GLTexture
{
private:
    GLuint      m_iTextureID;
    int         m_iWidth;
    int         m_iHeight;

private:
    void Default()
    {
        m_iTextureID = 0;
        m_iWidth     = 0;
        m_iHeight    = 0;
    }

    void Destroy()
    {
        glDeleteTextures(1, &m_iTextureID);
        this->Default();
    }

    bool GenTextureID()
    {
        this->Destroy();
        glGenTextures(1, &m_iTextureID);
        return m_iTextureID > 0 ? true : false;
    }

    bool CreateTexture(void * data, int width = 0, int height = 0, int format = GL_RGBA)
    {
        if (this->GenTextureID())
        {
            glBindTexture(GL_TEXTURE_2D, m_iTextureID);

            // set the texture wrapping/filtering options (on the currently bound texture object)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // create texture 2D
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

            // Thiết lập giá trị kích thức sử dụng sau này
            this->m_iWidth  = width;
            this->m_iHeight = height;

            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        return m_iTextureID > 0 ? true : false;
    }
public:
    GLTexture()
    {
        Default();
    }

    ~GLTexture()
    {
        Destroy();
    }

public:
    GLuint GetTextureID()
    {
        return m_iTextureID;
    }

    int GetImageWidth()
    {
        return m_iWidth;
    }
    int GetImageHeight()
    {
        return m_iHeight;
    }

    bool IsOk()
    {
        return m_iTextureID > 0 ? true : false;
    }

    //------------------------------------------------------------------------------
    // Load dữ liệu texture từ mảng dữ liệu 
    //------------------------------------------------------------------------------
    bool CreateFromData(void* DataBuffer, int width, int height, int format)
    {
        if (!DataBuffer) return false;
        return CreateTexture(DataBuffer, width, height, format);
    }

    //------------------------------------------------------------------------------
    // Tạo texture từ image đã được load trước (không hủy image đi)
    //------------------------------------------------------------------------------
    bool CreateFromImage(const GLImage* img, const int _width = 0, const int _height = 0)
    {
        if (!img || !img->m_Data) { return false; }
        // Format source image GL_RGBA or GL_RGB
        int srcFormat = GL_RGB;     // Format default
        if (img->m_nChanels == 3)      srcFormat = GL_RGB;
        else if (img->m_nChanels == 4) srcFormat = GL_RGBA;

        int width  = (_width  <= 0) ? img->m_width  : _width;
        int height = (_height <= 0) ? img->m_height : _height;

        return CreateFromData(img->m_Data, width, height, srcFormat);
    }

    //------------------------------------------------------------------------------
    // Tạo texture từ đường dẫn (có tạo image từ đường dẫn)
    //------------------------------------------------------------------------------
    bool CreateFromSource(const char* path, const int width = 0 , const int height = 0)
    {
        GLImage* img = new GLImage();

        bool bRes   = false;
        bRes |= img->LoadFromFile(path);
        bRes |= CreateFromImage(img, width, height);

        delete img;
        return bRes;
    }

};

class GLManager
{
public:
    virtual int GetType() = 0;
};

//==================================================================================
// Class _GLManger: Lớp quản lý template chung                                      
//==================================================================================
template <typename T> class _GLManager : public GLManager
{
protected:

    // Chú ý T* là con trỏ nên không thể tự hủy cần hủy đối tượng bằng tay
    map<string, T*>     m_Data;

protected:
    virtual ~_GLManager()
    {
        DELETE_MAP_PTR(m_Data);
    }
    virtual int GetType() = 0;

public:

    // Xóa toàn bộ dữ liệu Hàm clear sẽ KHÔNG tự gọi hàm hủy của đối tượng
    void RemoveAll()
    {
        DELETE_MAP_PTR(m_Data);
        m_Data.clear();
    }

    // Lấy dữ liệu trả về null nếu không tìm thấy 
    T* Get(string key) const
    {
        auto found = m_Data.find(key);
        if (found != m_Data.end())
        {
            return found->second;
        }
        return NULL;
    }

    // Thêm nguồn mới chú ý key đã tồn tại nó sẽ chèn vào
    // Chú ý : các đối tượng phải là con trỏ cấp phát new
    bool AddData(string key, T* data)
    {
        m_Data.insert(std::make_pair(key, data));
        return true;
    }

    // Remove một key định nghĩa
    void Remove(string key)
    {
        auto foundT = Get(key);
        if (foundT)
        {
            // Xóa đối tượng của con trỏ T
            delete foundT;
            m_Data.erase(key);
        }
    }
};

//==================================================================================
// Class GLImageManager: Lớp quản lý hình ảnh                                       
//==================================================================================
class GLImageManager : public _GLManager<GLImage>
{
public:
    GLImageManager()
    {

    }
    ~GLImageManager()
    {
    }
    int GetType() { return 2; }

public:
    GLImage* AddImage(string key, string fpath)
    {
        GLImage* image = Get(key);

        if (image)// Image đã tồn tại 
        {
            return image;
        }

        // Load mới một hình ảnh từ đường dẫn
        image = new GLImage();
        if (image->LoadFromFile(fpath))
        {
            m_Data.insert(std::make_pair(key, image));
            return image;
        }
        else
        {
            delete image;
            return NULL;
        }
    }

    GLImage* AddImage(string key, HBITMAP bitmap)
    {
        GLImage* image = Get(key);

        if (image)// Image đã tồn tại 
        {
            return image;
        }

        // Load mới một hình ảnh từ đường dẫn
        image = new GLImage();
        if (image->LoadFromBitmap(bitmap))
        {
            m_Data.insert(std::make_pair(key, image));
            return image;
        }
        else
        {
            delete image;
            return NULL;
        }
    }
};

//==================================================================================
// Class GLTextureManager: Lớp quản lý texture OpenGL                               
//==================================================================================
class GLTextureManager : public _GLManager<GLTexture>
{
private:
    // Sẽ quản lý image không lấy load image nhiều lần
    // Quản lý nó theo đường dẫn <> GLImage
    GLImageManager*     m_ImageMana;

public:
    GLTextureManager()
    {
        m_ImageMana = new GLImageManager();
    }
    ~GLTextureManager()
    {
        delete m_ImageMana;
    }

    int GetType() { return 3; }

public:
    GLTexture* CreateTexture(string key, const GLImage* img)
    {
        GLTexture* texture = this->Get(key);
        if (texture) return texture;  // Texture đã tồn tại 

        cout << "[Create texture] : From GLImage ";

        texture = new GLTexture();

        if (texture->CreateFromImage(img))
        {
            this->AddData(key, texture);
        }

        if(!texture->IsOk())
        {
            DELETE_PTR(texture);
            cout << " -> Failed" << endl;
        }
        else
        {
            cout << " -> OK" << endl;
        }

        return texture;
    }

    GLTexture* CreateTexture(const string key, const string fname)
    {
        GLTexture* texture = this->Get(key);
        if (texture) return texture;  // Texture đã tồn tại 

        cout << "[Create texture] : From " << fname;

        GLImage*  img  = m_ImageMana->AddImage(fname, fname);
        texture = new GLTexture();

        if (texture->CreateFromImage(img))
        {
            this->AddData(key, texture);
        }

        if(!texture->IsOk())
        {
            DELETE_PTR(texture);
            cout << " -> Failed" << endl;
        }
        else
        {
            cout << " -> OK" << endl;
        }

        return texture;
    }
};

#endif // !GLTEXTURE_H
