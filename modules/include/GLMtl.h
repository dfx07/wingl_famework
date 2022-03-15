#ifndef GLMTL_H
#define GLMTL_H

#include <map>
#include <vector>
#include <string>
#include <GLCom.h>

using namespace std;

// Define the illumination model: 
//        illum = 1 a flat material with no specular highlights
//        illum = 2 denotes the presence of specular highlights

struct obj_rgba
{
    float r;
    float g;
    float b;
    float a;

    obj_rgba() :
        r(0.f), g(0.f), b(0.f), a(1.0f) {}
    obj_rgba(float _r, float _g, float _b) :
        r(_r), g(_g), b(_b), a(1.0f) {}
};

class GLMtlImpoter;


enum class MtlKey
{
    NewMtl,         // New mtl
    KA,             // Ambient
    KD,             // Diffuse
    KS,             // Specular
    MapKA,          // Ambient texture
    MapKD,          // Diffuse  texture
    MapKS,          // Specular texture
    NS,             // Shininess of the material
    Comment,        // Comment
    Illum,          // Illumination model
    Unknown,        // unknown
};

class mtl_info
{
public:
    string     m_name;   // material name [identification]
    obj_rgba*  m_Ka;     // ambient color (r,g,b,a)
    obj_rgba*  m_Kd;     // diffuse color (r,g,b,a)
    obj_rgba*  m_Ks;     // specular color(r,g,b,a)
    float      m_Ns;     // shininess of the material
    float      m_Illum;  // define the illumination model


    string     m_map_Ka; // Link material ka
    string     m_map_Kd; // Link material kd
    string     m_map_Ks; // Link material ks

public:
    mtl_info()
    {
        m_map_Ka = "";
        m_map_Kd = "";
        m_map_Ks = "";

        m_Ka = m_Kd = m_Ks = NULL;
    }

    ~mtl_info()
    {
        delete m_Ka;
        delete m_Kd;
        delete m_Ks;
    }

    void SetName(string _name)
    {
        m_name = _name;
    }
    void SetAmbient(obj_rgba* rgba)
    {
        m_Ka = rgba;
    }
    void SetDiffuse(obj_rgba* rgba)
    {
        m_Kd = rgba;
    }
    void SetSpecular(obj_rgba* rgba)
    {
        m_Ks = rgba;
    }
    void SetShininess(float sh)
    {
        m_Ns = sh;
    }
    void SetIllumination(float il)
    {
        m_Illum = il;
    }
    void SetMapKa(string ftexture)
    {
        m_map_Ka = ftexture;
    }
    void SetMapKd(string ftexture)
    {
        m_map_Kd = ftexture;
    }
    void SetMapKs(string ftexture)
    {
        m_map_Ks = ftexture;
    }

    friend class GLMtlImpoter;
};

struct MTLTextureMapData
{
    string  m_name;
    string  m_path_Ka;
    string  m_path_Kd;
    string  m_path_Ks;
};

class GLMtlImpoter
{
private:
    vector<string>          m_comment;
    map<string, mtl_info*>  m_mtl;
    string                  m_res;  // Nguồn file load

public:
    GLMtlImpoter()
    {
        m_res = "";
    }

    ~GLMtlImpoter()
    {
        for (auto it = m_mtl.begin(); it != m_mtl.end(); it++)
        {
            // Xóa bỏ các mtl_obj
            delete it->second;
        }
    }

public:
    void AddComment(string cm)
    {
        m_comment.push_back(cm);
    }

    mtl_info* AddMaterial(string name)
    {
        mtl_info* pMtlObj = new mtl_info();
        pMtlObj->SetName(name);

        m_mtl.insert(std::make_pair(name, pMtlObj));
        return pMtlObj;
    }

private:

    MtlKey GetKey(FILE *file)
    {
        string buff = ReadFirstString(file);
        if (!buff.compare("#"))         return MtlKey::Comment;
        else if (!buff.compare("newmtl"))return MtlKey::NewMtl;
        else if (!buff.compare("Ka"))    return MtlKey::KA;
        else if (!buff.compare("Kd"))    return MtlKey::KD;
        else if (!buff.compare("Ks"))    return MtlKey::KS;
        else if (!buff.compare("Ns"))    return MtlKey::NS;
        else if (!buff.compare("illum")) return MtlKey::Illum;
        else if (!buff.compare("map_Ka"))return MtlKey::MapKA;
        else if (!buff.compare("map_Kd"))return MtlKey::MapKD;
        else if (!buff.compare("map_Ks"))return MtlKey::MapKS;

        return MtlKey::Unknown;
    }

public:
    string GetResource()
    {
        return m_res;
    }

    mtl_info* GetMTL(string useMTL)
    {
        auto found = m_mtl.find(useMTL);

        if (found != m_mtl.end())
        {
            return found->second;
        }
        return NULL;
    }

    bool LoadFromFile(string fname)
    {
        FILE* file = NULL;

        if (!fopen_s(&file, fname.c_str(), "r"))
        {
            string   buff = "";
            float    fnumber = 0.f;
            mtl_info* pMtlObj = NULL;

            while (!feof(file))
            {
                MtlKey key = GetKey(file);

                switch (key)
                {
                case MtlKey::NewMtl:
                {
                    buff = ReadLine(file);
                    pMtlObj = this->AddMaterial(buff);
                    break;
                }
                case MtlKey::Comment:
                {
                    buff = ReadLine(file);
                    this->AddComment(buff);
                    break;
                }
                case MtlKey::KA:
                {
                    // Vì obj_rgba có hàm khởi tạo 3 tham số nên sẽ sử dụng được
                    obj_rgba* rgb = Read3Float<obj_rgba>(file);
                    pMtlObj->SetAmbient(rgb);
                    break;
                }
                case MtlKey::KD:
                {
                    obj_rgba* rgb = Read3Float<obj_rgba>(file);
                    pMtlObj->SetDiffuse(rgb);
                    break;
                }
                case MtlKey::KS:
                {
                    obj_rgba* rgb = Read3Float<obj_rgba>(file);
                    pMtlObj->SetSpecular(rgb);
                    break;
                }
                case MtlKey::MapKA:
                {
                    buff = ReadLine(file);
                    pMtlObj->SetMapKa(buff);
                    break;
                }
                case MtlKey::MapKD:
                {
                    buff = ReadLine(file);
                    pMtlObj->SetMapKd(buff);
                    break;
                }
                case MtlKey::MapKS:
                {
                    buff = ReadLine(file);
                    pMtlObj->SetMapKs(buff);
                    break;
                }
                case MtlKey::NS:
                {
                    fnumber = ReadFloat(file);
                    pMtlObj->SetShininess(fnumber);
                    break;
                }
                case MtlKey::Illum:
                {
                    fnumber = ReadFloat(file);
                    pMtlObj->SetIllumination(fnumber);
                    break;
                }
                default:
                {
                    // Bỏ qua dòng này vì không xác định được key
                    buff = ReadLine(file);
                    break;
                }
                }
            }
            return true;
        }
        else
        {
            cerr << "Error :: Can't load file failed !" << endl;
        }
        return false;
    }

    vector<MTLTextureMapData> GetPathTexture()
    {
        vector<MTLTextureMapData> texture_paths;

        for (auto it = m_mtl.begin(); it != m_mtl.end(); it++)
        {
            MTLTextureMapData temp;
            temp.m_name    = it->second->m_name;
            temp.m_path_Ka = it->second->m_map_Ka;
            temp.m_path_Kd = it->second->m_map_Kd;
            temp.m_path_Ks = it->second->m_map_Ks;
        }
        return texture_paths;
    }
};


#endif // !GLMTL_H
