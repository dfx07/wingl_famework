////////////////////////////////////////////////////////////////////////////////////
// File: GLObject.h     -- Load file obj                       
// For conditions of distribution and use, see copyright notice in readme.txt       
//                                                                                  
// Copyright (C) 2020-2021 : 31/08/2021             Author  : Thuong.NV             
////////////////////////////////////////////////////////////////////////////////////

#ifndef GLOBJECT_H
#define GLOBJECT_H


#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <GLMtl.h>

using namespace std;

//==================================================================================
// Cấu trúc dữ liệu sử dụng trong file obj                                          
//==================================================================================
struct obj_v
{
    float x;
    float y;
    float z;

    obj_v()
        :x(0.f), y(0.f), z(0.f){}

    obj_v(float _x, float _y, float _z)
        :x(_x), y( _y), z( _z){}
};

struct obj_vn
{
    float x;
    float y;
    float z;

    obj_vn()
        :x(0.f), y(0.f), z(0.f){}

    obj_vn(float _x, float _y, float _z)
        :x(_x), y(_y), z(_z){}
};

struct obj_vt
{
    float u;
    float v;
    float w;

    obj_vt()
        :u(0.f), v(0.f), w(0.f){}

    obj_vt(float _u, float _v, float _w)
        :u(_u), v(_v), w(_w){}
};

struct obj_f
{
    int iv;
    int ivn;
    int ivt;

    obj_f()
        :iv(0), ivn(0), ivt(0){}

    obj_f(int _iv, int _ivt, int _ivn)
        :iv(_iv), ivn(_ivn), ivt(_ivt){}
};

enum class FaceType
{
    FACE_POINT,
    FACE_LINE ,
    FACE_TRIG ,
    FACE_QUADS,
    NO_SUPPORT,
};

struct tagDBInfo
{
    int     m_ivsize;
    int     m_ivtsize;
    int     m_ivnsize;
};

struct tagFaceInfo
{
    int     line_size;
    int     point_size;
    int     trig_size;
    int     quads_size;
};

//==================================================================================
// Lớp tagMaterialDrawInfo : Chứa thông tin về dữ liệu vẽ                           
//==================================================================================
class tagMaterialDrawInfo
{
public:
    GLuint          m_VaoID;     // Vao ID sử dụng
    GLuint          m_TextureID; // Texture ID sử dụng
    GLint           m_DrawSize;  // Số lượng đỉnh vẽ
    INT             m_DrawType;  // Loại vẽ sử dụng cho DrawArrays : GL_TRIANGLES | GL_QUADS 

    const mtl_info* m_Material;

public:
    tagMaterialDrawInfo()
    {
        m_VaoID     = 0;
        m_TextureID = 0;
        m_DrawSize  = 0;
        m_Material  = NULL;
    }

    tagMaterialDrawInfo(GLuint          vaoid,
                        GLuint          textureid,
                        GLint           isize,
                        const mtl_info* mtl)
    {
        m_VaoID     = vaoid;
        m_TextureID = textureid;
        m_DrawSize  = isize;
        m_Material  = mtl;
    }

    void SetVaoID(GLuint vaoID)
    {
        m_VaoID = vaoID;
    }

    void SetTextureID(GLuint textureid)
    {
        m_TextureID = textureid;
    }

    void SetDrawSize(GLint  isize)
    {
        m_DrawSize = isize;
    }

    void SetDrawType(INT drawType)
    {
        m_DrawType = drawType;
    }

    void SetMTLInfo(const mtl_info* mtl)
    {
        m_Material = mtl;
    }
};

class tagFaceDrawInfo
{
public:
    // Cái này sẽ chứa vertex | normal vertex | normal texture
    vector<vec3>    m_DataPoints;   // 1 điểm
    vector<vec3>    m_DataLines;    // 2 điểm
    vector<vec3>    m_DataTrigs;    // 3 điểm
    vector<vec3>    m_DataQuads;    // 4 điểm

    // Chứa ánh sáng | màu sắc | độ bóng | thông tin texture
    const mtl_info* m_UseMtlInfo;       // Không được xóa nó

public:

    tagFaceDrawInfo()
    {
        m_UseMtlInfo = NULL;
        m_DataPoints.reserve(10);
        m_DataLines.reserve(10);
        m_DataTrigs.reserve(1000);
        m_DataQuads.reserve(1000);
    }

public:
    void UseMaterial(mtl_info* mtl)
    {
        m_UseMtlInfo = mtl;
    }

    void Reserve(const tagFaceInfo& finf)
    {
        // Số lượng đỉnh và mỗi đỉnh có 3 đơn vị
        m_DataPoints.reserve(finf.point_size * 1 * 3 + 3);
        m_DataLines.reserve (finf.line_size  * 2 * 3 + 3);
        m_DataTrigs.reserve (finf.trig_size  * 3 * 3 + 3);
        m_DataQuads.reserve (finf.quads_size * 4 * 3 + 3);
    }
    //==================================================================================
    // Đẩy vertex vào data                                                              
    //==================================================================================
    void AddVertex(FaceType type, float x, float y, float z)
    {
        switch (type)
        {
            case FaceType::FACE_POINT:
                m_DataPoints.push_back(vec3(x, y, z));
                break;
            case FaceType::FACE_LINE:
                m_DataLines.push_back(vec3(x, y, z));
                break;
            case FaceType::FACE_TRIG:
                m_DataTrigs.push_back(vec3(x, y, z));
                break;
            case FaceType::FACE_QUADS:
                m_DataQuads.push_back(vec3(x, y, z));
                break;
            default:
                // Trường hợp surface> 4 này chưa được control
                assert(0);
                break;
        }
    }
    //==================================================================================
    //  Đẩy vertex normal vào data                                                      
    //==================================================================================
    void AddVertexNormal(FaceType type, float x, float y, float z)
    {
        switch (type)
        {
            case FaceType::FACE_POINT:
                m_DataPoints.push_back(vec3(x, y, z));
                break;
            case FaceType::FACE_LINE:
                m_DataLines.push_back(vec3(x, y, z));
                break;
            case FaceType::FACE_TRIG:
                m_DataTrigs.push_back(vec3(x, y, z));
                break;
            case FaceType::FACE_QUADS:
                m_DataQuads.push_back(vec3(x, y, z));
                break;
            default:
                // Trường hợp surface> 4 này chưa được control
                assert(0);
                break;
        }
    }
    //==================================================================================
    // Đẩy một TextureCoord vào dạng dữ liệu                                            
    //==================================================================================
    void AddTextureCoord(FaceType type, float x, float y, float z)
    {
        switch (type)
        {
            case FaceType::FACE_POINT:
                m_DataPoints.push_back(vec3(x, y, z));
                break;
            case FaceType::FACE_LINE:
                m_DataLines.push_back(vec3(x, y, z));
                break;
            case FaceType::FACE_TRIG:
                m_DataTrigs.push_back(vec3(x, y, z));
                break;
            case FaceType::FACE_QUADS:
                m_DataQuads.push_back(vec3(x, y, z));
                break;
            default:
                // Trường hợp surface> 4 này chưa được control
                assert(0);
                break;
        }
    }

    //==================================================================================
    // Tìm vector normal của surface sử dụng 3 điểm đầu                                 
    // Chú ý : Các điểm trong face phải nằm trên một mặt phẳng và chỉ tính 3 điểm đầu   
    //==================================================================================
    vec3 GetNormalSurface(const vector<obj_v*>& vertexs)
    {
        int iCntVextex = vertexs.size();
        if (iCntVextex <= 2) return vec3(0.f, 0.f, 0.f);

        vec3 p1 = { vertexs[0]->x , vertexs[0]->y, vertexs[0]->z };
        vec3 p2 = { vertexs[1]->x , vertexs[1]->y, vertexs[1]->z };
        vec3 p3 = { vertexs[2]->x , vertexs[2]->y, vertexs[2]->z };

        vec3 edge1 = p1 - p2;
        vec3 edge2 = p2 - p3;

        vec3 crs    = glm::cross(edge1, edge2);
        vec3 normal = glm::normalize(crs);
        return {normal.x , normal.y , normal.z};
    }

    //==================================================================================
    // Đẩy dữ liệu dạng vector: vertex, vn, vt vào dữ liệu                              
    //==================================================================================
    void AddData( const FaceType         type,
                  const vector<obj_v*> & data_v,
                  const vector<obj_vn*>& data_vn,
                  const vector<obj_vt*>& data_vt)
    {
        vec3 v, vn, vt; // Dữ liệu tạm thời của từng đỉnh

        //Chú ý kích thước của data_v, data_vn, data_vt phải bằng nhau
        for (int i = 0; i < data_v.size(); i++)
        {
            if (data_v[i])
            {
                v  = { data_v[i]->x  , data_v[i]->y  , data_v[i]->z};
            }
            else
            {
                v = V3ZERO;
            }

            //Phát sinh : khi không có vertexnormal của surface thì tính vertexnormal
            if (data_vn[i])
            {
                vn = { data_vn[i]->x  , data_vn[i]->y  , data_vn[i]->z};
            }
            else
            {
                vn = GetNormalSurface(data_v);
            }

            // Đoạn này hơi lạ vì nó có uv âm không biết có đúng không
            // Đéo hiểu luôn lúc lấy âm lúc lấy dương liệu nó có phụ thuộc vào trục không
            // Khả năng do quay trục -> v sẽ ngược lại với v
            if (data_vt[i])
            {
                vt = { data_vt[i]->u , -data_vt[i]->v , data_vt[i]->w };
            }
            else
            {
                vt = V3ZERO;
            }

            switch (type)
            {
                case FaceType::FACE_POINT:
                    m_DataPoints.push_back(v);
                    m_DataPoints.push_back(vn);
                    m_DataPoints.push_back(vt);
                    break;
                case FaceType::FACE_LINE:
                    m_DataLines.push_back(v);
                    m_DataLines.push_back(vn);
                    m_DataLines.push_back(vt);
                    break;
                case FaceType::FACE_TRIG:
                    m_DataTrigs.push_back(v);
                    m_DataTrigs.push_back(vn);
                    m_DataTrigs.push_back(vt);
                    break;
                case FaceType::FACE_QUADS:
                    m_DataQuads.push_back(v);
                    m_DataQuads.push_back(vn);
                    m_DataQuads.push_back(vt);
                    break;
                default:
                    // Trường hợp surface> 4 này chưa được control
                    break;
            }
        }
    }
};

//==================================================================================
// Chứa toàn bộ dữ liệu vẽ và thông tin material của một object                     
//==================================================================================
class DrawDataObject
{
public:
    vector<const tagFaceDrawInfo*> m_Data;

public:
    DrawDataObject(int _reSize = 5)
    {
        // Tính toán dự trữ memory trước khi đẩy ra giúp cải thiện tốc độ
        m_Data.reserve(_reSize);
    }

    ~DrawDataObject()
    {
        this->Destroy();
    }

    void Destroy()
    {
        DELETE_VECTOR_PTR(m_Data);
    }
public:
    const tagFaceDrawInfo* operator[](int index)
    {
        return m_Data[index];
    }
    void Add(const tagFaceDrawInfo* finf)
    {
        m_Data.push_back(finf);
    }
    void Clear()
    {
        this->Destroy();
    }
    int GetSize()
    {
        return (int)m_Data.size();
    }
};

//==================================================================================
// Cấu trúc dữ liệu sử dụng trong file obj                                          
//==================================================================================

enum class ObjKey
{
    UNKNOWN = -1,
    GROUD_NAME,
    FACE,
    MTL_LIB,
    USE_MTL,
    OBJ_NAME,
    COMMENT,
    VERTEX,
    VERTEX_NORMAL,
    VERTEX_TEXTURE,
};



//==================================================================================
// Đọc chuỗi đầu tiên của một dòng string từ vị trí con trỏ file                    
//==================================================================================
ObjKey ReadKey(FILE* file)
{
    string buff = ReadFirstString(file);
    if (!buff.compare("#"))         return ObjKey::COMMENT;
    else if(!buff.compare("v"))     return ObjKey::VERTEX;
    else if(!buff.compare("vn"))    return ObjKey::VERTEX_NORMAL;
    else if(!buff.compare("vt"))    return ObjKey::VERTEX_TEXTURE;
    else if(!buff.compare("o"))     return ObjKey::OBJ_NAME;
    else if(!buff.compare("f"))     return ObjKey::FACE;
    else if(!buff.compare("g"))     return ObjKey::GROUD_NAME;
    else if(!buff.compare("usemtl"))return ObjKey::USE_MTL;
    else if(!buff.compare("mtllib"))return ObjKey::MTL_LIB;

    return ObjKey::UNKNOWN;
}

class face_info;

//==================================================================================
// Lớp : object_info                                                                
// Thông tin về các đối tương trong obj và tập hợp các face có trong nó             
//==================================================================================
class data_base
{
public:

    string            m_name;
    vector<string>    m_comment;
    vector<obj_v *>   m_v;
    vector<obj_vn*>   m_vn;
    vector<obj_vt*>   m_vt;

public:
    data_base()
    {
        m_v.reserve(1000);
        m_vn.reserve(1000);
        m_vt.reserve(1000);
    }
    ~data_base()
    {
        DELETE_VECTOR_PTR(m_v);
        DELETE_VECTOR_PTR(m_vn);
        DELETE_VECTOR_PTR(m_vt);
    }
public:
    void SetName(string name)
    {
        m_name = name;
    }
    void AddComment(string cm)
    {
        m_comment.push_back(cm);
    }
    void AddVertex(obj_v* v)
    {
        m_v.push_back(v);
    }
    void AddVertexNormal(obj_vn* v)
    {
        m_vn.push_back(v);
    }
    void AddVertexTexture(obj_vt* v)
    {
        m_vt.push_back(v);
    }

    // Trả về thông tin của điểm nếu có hoặc trả về NULL nếu không có
    obj_v* GetVertex(int index)
    {
        if (0 <= index && index < m_v.size())
        {
            return m_v[index];
        }
        return NULL;
    }
    obj_vn* GetVertexNormal(int index)
    {
        if (0 <= index && index < m_vn.size())
        {
            return m_vn[index];
        }
        return NULL;
    }
    obj_vt* GetTextureCoord(int index)
    {
        if (0 <= index && index < m_vt.size())
        {
            return m_vt[index];
        }
        return NULL;
    }

    tagDBInfo GetInfo()
    {
        tagDBInfo inf;
        inf.m_ivsize  = m_v.size();
        inf.m_ivnsize = m_vn.size();
        inf.m_ivtsize = m_vt.size();
        return inf;
    }
};

//==================================================================================
// Lớp : face_data                                                                  
// Chứa thông tin về từng điểm surface : chỉ số vertex, vertex normal, texture coord
//==================================================================================
class face_data
{
public:
    vector<obj_f*>  m_Data;
public:
    face_data()
    {
        // Số lượng thông tin đỉnh face max =15 $_$
        m_Data.reserve(15);
    }
    ~face_data()
    {
        DELETE_VECTOR_PTR(m_Data);
    }

    // Lấy giá trị là một con trỏ
    obj_f* operator [] (int i) const
    {
        return m_Data[i];
    }

    // Gán giá trị cho nó là một con trỏ
    obj_f*& operator [] (int i)
    {
        return m_Data[i];
    }

    obj_f* at(int i) const
    {
        return m_Data[i];
    }

    // Thông tin chỉ số của vertex, vertexnormal, texture index
    void AddIndexInfo(obj_f* index)
    {
        m_Data.push_back(index);
    }

    // Lấy ra số lượng đỉnh trong face
    int GetSize() const
    {
        return (int) m_Data.size();
    }

    //==================================================================================
    // Chia số polygon có số lượng đỉnh lớn hơn 5 thành các triangle                    
    // Chú ý : polygon là nồi và quay theo một hướng, kích thước của v, vt, vn bằng nhau
    //==================================================================================
    void ConvertPoly2Tri(vector<obj_v *>& v,       // Số vertex          [i]
                         vector<obj_vn*>& vn,      // Số vertexnormal    [i]
                         vector<obj_vt*>& vt,      // Số TexturCoord     [i]
                         vector<obj_v *>& v_out,   // Vertex sau chia    [o]
                         vector<obj_vn*>& vn_out,  // VertexNormal       [o]
                         vector<obj_vt*>& vt_out)  // TexturCoord        [o]
    {
        int iCntVertex = v.size();
        if (iCntVertex <= 0 ) return;

        // Không thực hiện chi với số điểm nhỏ hơn 5
        if (iCntVertex <= 4)
        {
            // Thật cẩn thận khi sử dụng copy vì nó ảnh hưởng đến hiệu xuất
            //std::copy(v.begin(),  v.end()  , back_inserter(v_out));
            //std::copy(vn.begin(), vn.end() , back_inserter(vn_out));
            //std::copy(vt.begin(), vt.end() , back_inserter(vt_out));
            std::swap(v_out,   v);
            std::swap(vn_out, vn);
            std::swap(vt_out, vt);
        }
        // Trường hợp lớn hơn bằng 5 thì chia thành cách các triangle
        else
        {
            v_out.clear(); vn_out.clear(); vt_out.clear();
            v_out. reserve(iCntVertex*3);
            vn_out.reserve(iCntVertex*3);
            vt_out.reserve(iCntVertex*3);

            for (int i = 1; i < iCntVertex -1; i++)
            {
                // Vertex
                v_out.push_back(v[0]);
                v_out.push_back(v[i]);
                v_out.push_back(v[i + 1]);

                // Normal vertex
                vn_out.push_back(vn[0]);
                vn_out.push_back(vn[i]);
                vn_out.push_back(vn[i + 1]);

                // Texture coord
                vt_out.push_back(vt[0]);
                vt_out.push_back(vt[i]);
                vt_out.push_back(vt[i + 1]);
            }
        }
    }

    // Lấy dữ liệu file trả về loại face
    FaceType GetFaceData(data_base*       data      ,// Dữ liệu lưu trữ
                         vector<obj_v*> & vtex      ,// Danh sách tọa độ vertex
                         vector<obj_vn*>& vtexnormal,// Danh sách tọa độ vertex normal 
                         vector<obj_vt*>& vtexcoord )// Danh sách tọa độ texture coord
    {
        int  iSizeFaceInfo = m_Data.size();

        static vector<obj_v*>  vtexTemp;       vtexTemp.clear();
        static vector<obj_vn*> vtexNormalTemp; vtexNormalTemp.clear();
        static vector<obj_vt*> vtexCoordTemp;  vtexCoordTemp.clear();

        vtexTemp      .reserve(iSizeFaceInfo);
        vtexNormalTemp.reserve(iSizeFaceInfo);
        vtexCoordTemp .reserve(iSizeFaceInfo);

            // Thông tin của một face : =1 / =2 / = 3 / = 4 / =5  trong một face
        for (int ix = 0; ix < iSizeFaceInfo; ix++)
        {
            // Lấy thông tin vertex | vertex normal | texture coord
            // Cái này cần đúng thứ tự không là vẽ sai
            vtexTemp.      push_back(data->GetVertex(m_Data[ix]->iv - 1));
            vtexNormalTemp.push_back(data->GetVertexNormal(m_Data[ix]->ivn - 1));
            vtexCoordTemp. push_back(data->GetTextureCoord(m_Data[ix]->ivt - 1));
        }

        // Trong trường hợp lớn hơn 5 sẽ được chi nhỏ thành các tam giác
        ConvertPoly2Tri(vtexTemp, vtexNormalTemp, vtexCoordTemp, vtex, vtexnormal, vtexcoord);

        int iCntVt = vtex.size();

        if (iCntVt == 1)      return FaceType::FACE_POINT;
        else if (iCntVt == 2) return FaceType::FACE_LINE;
        else if (iCntVt == 4) return FaceType::FACE_QUADS;
        else if (iCntVt >= 3) return FaceType::FACE_TRIG;
        else                  return FaceType::NO_SUPPORT;
    }
};

//==================================================================================
// Lớp : face_info                                                                  
// Nó là tập hợp tất cả các surface sử dụng chung mtl                               
//==================================================================================
class face_info
{
public:
    string                  m_name;
    string                  m_use_mtl;
    string                  m_mtlib_name;
    vector<face_data*>      m_faces;
public:
    ~face_info()
    {
        DELETE_VECTOR_PTR(m_faces);
    }
public:
    face_info()
    {
        m_name       = "face_node_default";
        m_mtlib_name = "mtlib_none";
        m_faces.reserve(100);
    }
    void SetGroupName(string name)
    {
        m_name = name;
    }
    void SetUseMTL(string name)
    {
        m_use_mtl = name;
    }
    void SetMTLName(string name)
    {
        m_mtlib_name = name;
    }
    void AddFaceData(face_data* data)
    {
        m_faces.push_back(data);
    }
    int GetFaceSize()
    {
        return (int) m_faces.size();
    }

    // Lấy giá trị là một con trỏ
    face_data* operator [] (int i) const
    {
        return m_faces[i];
    }

    // Gán giá trị cho nó là một con trỏ
    face_data*& operator [] (int i)
    {
        return m_faces[i];
    }
    // Thông tin về dữ liệu face_info
    tagFaceInfo GetInfo()
    {
        tagFaceInfo inf;
        inf.point_size  = 0;
        inf.line_size   = 0;
        inf.trig_size   = 0;
        inf.quads_size  = 0;

        for (int i = 0; i < m_faces.size(); i++)
        {
            int vtsize = m_faces[i]->GetSize();
            switch (vtsize)
            {
                case 1:inf.point_size++ ;break;
                case 2:inf.line_size++  ;break;
                case 3:inf.trig_size++  ;break;
                case 4:inf.quads_size++ ;break;
                default:
                {
                    // Các trường hợp khác sẽ biến đổi về triangle 
                    int trig = vtsize - 2;
                    if (trig > 0) inf.trig_size += trig;
                    break;
                }
            }
        }

        return inf;
    }
};

//==================================================================================
// Lớp : object_info                                                                
// Thông tin về các đối tương trong obj và tập hợp các face có trong nó             
//==================================================================================
struct object_info
{
public:
    string              name;
    vector<face_info*>  face_idx;
public:
    object_info()
    {
        name   = "Default";
        face_idx.reserve(10);
    }
public:
    void AddFaceNode(face_info* pFace)
    {
        face_idx.push_back(pFace);
    }
    void SetName(string _name)
    {
        name = _name;
    }
    int GetFaceSize()
    {
        return (int)face_idx.size();
    }
};

//==================================================================================
// Đối tượng xử lý file .obj                                                        
// Điều kiện obj file :
//          + Không có dấu xuống dòng ở cuối file                                   
//==================================================================================
class GLObjectImport
{
public:
    // Dữ liệu sử dụng chung : vertex, vertex normal , texture coord
    // Mỗi một file obj chỉ có một data_base 
    data_base*              m_base;     // Dữ liệu lưu trữ v, vt, vn

    vector<face_info*>      m_ListFaceInfo;
    vector<object_info*>    m_ListObjInfo;
    vector<string>          m_commad;

    // Các file mtl mà obj sử dụng
    vector<string>          m_ListMtlRes;

    vector<GLMtlImpoter* >  m_MtlImporter;
public:
    ~GLObjectImport()
    {
        delete m_base;
        DELETE_VECTOR_PTR(m_ListFaceInfo);
        DELETE_VECTOR_PTR(m_ListObjInfo);
        DELETE_VECTOR_PTR(m_MtlImporter);
    }
    GLObjectImport()
    {
        m_base = new data_base();
        m_ListFaceInfo.reserve(1000);
    }

private:
    face_info* AddFace()
    {
        face_info* facenode = new face_info();
        m_ListFaceInfo.push_back(facenode);
        return facenode;
    }
    object_info* AddObjectInfor(string name)
    {
        object_info* obj_info = new object_info();
        m_ListObjInfo.push_back(obj_info);
        return obj_info;
    }

    void AddCommand(string cm)
    {
        m_commad.push_back(cm);
    }

    void AddMtlLib(string fname)
    {
        fname = StandardFileName(fname);
        m_ListMtlRes.push_back(fname);
    }

    // Có thể có nhiều file mtl nhưng nó sẽ trả về tên trùng đầu tiên nó gặp
    mtl_info* GetMaterial(string mtlUse)
    {
        mtl_info* pMtlInfor = NULL;
        // Lặp tất cả các file mtl đã load
        for (int i = 0; i < m_MtlImporter.size(); i++)
        {
            GLMtlImpoter* mtl = m_MtlImporter[i];

            pMtlInfor = mtl->GetMTL(mtlUse);
            if (pMtlInfor)
            {
                return pMtlInfor;
            }
        }
        return NULL;
    }

    //==================================================================================
    // Đọc dữ liệu face trên một dòng xác định bởi con trỏ file                         
    // Bằng mặc định -1   Format: v x y z                                               
    // Format : v/[vt]/[vn]
    //==================================================================================
    face_data* ReadNFace(FILE* file)
    {
        face_data* facedata = new face_data();
        int x, y, z; x = y = z = -1;  obj_f* face = NULL;
        while (!feof(file) && fscanf_s(file, "%d/%d/%d ", &x, &y, &z))
        {
            if (x == -1) continue; // Trường hợp này không tính
            face = new obj_f(x, y, z);
            facedata->AddIndexInfo(face);
            x = y = z = -1; face = NULL;
        }
        return facedata;
    }

    //==================================================================================
    // Load file .obj                                                                   
    //==================================================================================
    bool LoadObjFile(string filename)
    {
        FILE* file = NULL;
        if (!fopen_s(&file, filename.c_str(), "r"))
        {
            face_info* facenode     = NULL;
            string     strMTLlib    = "";
            string     strUseMTL    = "";
            string     strGroupName = "";

            string          buff    = "";
            obj_v *         vertex  = NULL;
            obj_vn *        vertexn = NULL;
            obj_vt *        vertext = NULL;
            face_data*      facedata= NULL;

            //Mặc định sẽ có một object tên là default
            object_info* obj_info = AddObjectInfor("Default");

            while (!feof(file))
            {
                // Dọc ký tự đầu tiên của dòng dữ liệu
                ObjKey key = ReadKey(file);

                // Xử lý tương ứng với mỗi case bắt buộc đọc hết 1 line
                switch (key)
                {
                case ObjKey::MTL_LIB:
                {
                    buff = ReadLine(file);
                    strMTLlib = buff;
                    this->AddMtlLib(buff);
                    break;
                };
                case ObjKey::USE_MTL:
                {
                    buff = ReadLine(file);
                    strUseMTL = buff;
                    facenode = NULL;
                    break;
                };
                case ObjKey::GROUD_NAME:
                {
                    buff = ReadLine(file);
                    strGroupName = buff;
                    facenode = NULL;
                    break;
                };
                case ObjKey::FACE:
                {
                    if (!facenode)
                    {
                        facenode = this->AddFace();
                        facenode->SetMTLName(strMTLlib);
                        facenode->SetUseMTL(strUseMTL);
                        facenode->SetGroupName(strGroupName);
                        obj_info->AddFaceNode(facenode);
                    }
                    facedata = ReadNFace(file); // Nó cũng giống đọc một dòng
                    facenode->AddFaceData(facedata);
                    break;
                };
                case ObjKey::VERTEX:
                {
                    obj_v* value = Read3Float<obj_v>(file);
                    m_base->AddVertex(value);
                    break;
                }
                case ObjKey::VERTEX_NORMAL:
                {
                    obj_vn* value = Read3Float<obj_vn>(file);
                    m_base->AddVertexNormal(value);
                    break;
                }
                case ObjKey::VERTEX_TEXTURE:
                {
                    obj_vt* value = Read3Float<obj_vt>(file);
                    m_base->AddVertexTexture(value);
                    break;
                }
                case ObjKey::OBJ_NAME:
                {
                    buff = ReadLine(file);
                    // Nếu object trước không có dữ liệu nào thì không cần tạo mới
                    if (obj_info->GetFaceSize() > 0)
                    {
                        obj_info = this->AddObjectInfor(buff);
                    }
                    obj_info->SetName(buff);
                    break;
                }
                case ObjKey::COMMENT:
                {
                    buff = ReadLine(file);
                    m_base->AddComment(buff);
                    break;
                }
                default:
                {
                    ReadLine(file); // bỏ qua dòng này
                    break;
                }
                }
            };

            fclose(file);
            return true;
        }
        else
        {
            cerr << "Error :: Can't load file failed !: "<< filename << endl;
        }
        return false;
    }

    //==================================================================================
    // Load file .mtl sử dụng GLMtlImporter                                             
    //==================================================================================
    bool LoadMtlFile(string fname)
    {
        GLMtlImpoter* pmtl = new GLMtlImpoter();
        if(pmtl->LoadFromFile(fname))
        {
            m_MtlImporter.push_back(pmtl);
            return true;
        }
        return false;
    }

public:
    //==================================================================================
    // Load file .obj và .mtl                                                           
    //==================================================================================
    void LoadFromFile(string filename)
    {
        // Khi load file obj đồng thời sẽ lấy dữ liệu luôn
        if (this->LoadObjFile(filename))
        {
            int err = 0;  string pathMtl = "";
            string dirObjFile = GetDirection(filename);

            for (int i = 0; i < m_ListMtlRes.size() ; i++)
            {
                pathMtl = dirObjFile + "/" + StandardFileName(m_ListMtlRes[i]);
                cout << "[Loading MTL] MTL file resource. From : " << pathMtl;

                // Khi load file mtl đồng thời sẽ lấy dữ liệu luôn
                if (!LoadMtlFile(pathMtl))
                {
                    cout << " => False" << endl;
                    err++;
                }
                else
                {
                    cout << " => OK" << endl;
                }
            }

            if (err > 0)  // Điều khiển trạng thái
            {
                cout << "[Loading MTL] Done : " << err << " error !" << endl;
            }
            else
            {
                cout << "[Loading MTL] OK !" << endl;
            }
        }
    }

    int GetFaceSize()
    {
        return m_ListFaceInfo.size();
    }

    //==================================================================================
    // Lấy dữ liệu face dạng triangle và quads sử dụng cho vẽ object                    
    //==================================================================================
    void GetFaceVertexData(vector<vec3>& trig, vector<vec3>& poly)
    {
        for (int i = 0; i < m_ListFaceInfo.size(); i++)
        {
            face_info& pFaceInfo = *m_ListFaceInfo[i];

            for (int ii = 0; ii < pFaceInfo.GetFaceSize(); ii++)
            {
                int  iSizeFace = pFaceInfo[ii]->GetSize();

                // Số lượng đỉnh của một mesh = 3 hoặc 4
                //if (iSizeFace == 3)
                //{
                //    for (int ix = 0; ix < 3; ix++)
                //    {
                //        obj_f* faceindex = (*pFaceInfo->m_faces[ii])[ix];
                //        obj_v* pos = m_object->m_v[faceindex->iv - 1];
                //        trig.push_back(vec3(pos->x, pos->y, pos->z));
                //    }
                //}
                //else if (iSizeFace == 4)
                //{
                //    for (int ix = 0; ix < 4; ix++)
                //    {
                //        obj_f* faceindex = pFaceInfo->m_faces[ii]->at(ix);
                //        obj_v* pos = m_object->m_v[faceindex->iv - 1];
                //        poly.push_back(vec3(pos->x, pos->y, pos->z));
                //    }
                //}
            }
        }
    }

    //==================================================================================
    // Lấy dữ liệu thông tin face của model : trig , quads                              
    // Chú ý : nhớ là xóa nó đi sau khi dùng
    //==================================================================================
    DrawDataObject* GetDrawDataObject()
    {
        DrawDataObject* pDrawDataObj =new DrawDataObject(m_ListFaceInfo.size());

        // Cái này là dữ liệu temp của mỗi surface nó phục vụ cho 
        // việc tính VertexNormalInfo nếu nó không có sẽ không xử lý
        vector<obj_v*  > VertexData;
        vector<obj_vn* > VertexNormalData;
        vector<obj_vt* > TextureCoordData;

        for (int i = 0; i < m_ListFaceInfo.size(); i++)
        {
            face_info& pFaceInfo  = *m_ListFaceInfo[i];
            mtl_info* mtl         = GetMaterial(pFaceInfo.m_use_mtl);

            tagFaceDrawInfo* faceInfo = new tagFaceDrawInfo();
            // Tính toán dữ trữ cải thiện hiệu suất
            tagFaceInfo inf = pFaceInfo.GetInfo();
            faceInfo->Reserve(inf);

            faceInfo->UseMaterial(mtl);

            // Số lượng face trong một face info
            for (int ii = 0; ii < pFaceInfo.GetFaceSize(); ii++)
            {
                // Lấy ra danh sách thông tin các đỉnh của face : vị trí , normal , texture
                FaceType iType = pFaceInfo[ii]->GetFaceData(m_base, VertexData, VertexNormalData, TextureCoordData);

                // Không xử lý với các số lượng điểm lớn hơn <= 0
                if (iType == FaceType::NO_SUPPORT) continue;
                // Kiểm tra xem điểm đầu ra có vấn đề gì không 
                // Thiết lập default cho các đỉnh còn thiếu dữ liệu
                faceInfo->AddData(iType, VertexData, VertexNormalData, TextureCoordData);
            }
            // Đẩy dữ liệu ra trong trường hợp
            pDrawDataObj->Add(faceInfo);
        }
        return pDrawDataObj;
    }
};
#endif // !GLOBJECT_H