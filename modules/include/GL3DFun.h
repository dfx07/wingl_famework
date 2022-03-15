////////////////////////////////////////////////////////////////////////////////////
// File : GL3DFun.h                                                                 
// Hàm và xử lý 3D                                                                  
//                                                                                  
// Created : 31/08/2021             Author  : Thuong.NV                             
////////////////////////////////////////////////////////////////////////////////////

#ifndef GL3DFUN_H
#define GL3DFUN_H

#define  M3_ALG_EPSILON     0.0001f         // Coincident angle radius
#define  M3_EPSILON         0.01f           // Coincident

#define  M3_2PI             6.28318530718f  // 2*PI
#define  M3_PI              3.14159265359f  // PI
#define  M3_HPI             1.57079632679f  // PI/2

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>

using namespace std;
using namespace glm;


//==================================================================================
// Tìm hình chiếu của một điểm đến mặt phẳng cho bởi 3 điểm                         
//==================================================================================
namespace sp
{
    inline ostream& operator<<(ostream& os, const glm::vec3& p)
    {
        cout << "(" << p.x << ", " << p.y << ", " << p.z << ")";
        return os;
    }
}

//==================================================================================
// Vị trí của một điểm với một mặt phẳng cho bởi điểm và vector pháp tuyến          
// Return : -1 ngược bên với vector pháp tuyến                                      
//           0 nằm trên mặt phẳng                                                   
//           1 cùng bên với vector pháp tuyến                                       
//==================================================================================
static int  M3D_RelPoint2Plan(const vec3&  point,// Điểm kiểm tra                   
                              const vec3&  vp,   // Điểm thuộc mặt phẳng            
                              const vec3&  vn)   // Vector pháp tuyến mặt phẳng     
{
    vec3 vn1  = glm::normalize(point - vp);
    float dot = glm::dot(vp, vn);

    // Nằm trên mặt phẳng
    if (fabs(dot) < M3_EPSILON)
    {
        return 0;
    }
    // Cùng bên với hướng vector pháp tuyến
    else if (dot > 0.0)
    {
        return 1;
    }
    // Ngược bên hướng với vector pháp tuyến
    return -1;
}

//==================================================================================
// Kiểm tra điểm ở giữ hai mặt phẳng                                                
//==================================================================================
static int  M3D_PointIn2Plan(const vec3&  point, // Điểm kiểm tra                   
                             const vec3&  vp1,   // Điểm thuộc mặt phẳng 1          
                             const vec3&  vn1,   // Vector pháp tuyến mặt phẳng 1   
                             const vec3&  vp2,   // Điểm thuộc mặt phẳng 2          
                             const vec3&  vn2)   // Vector pháp tuyến mặt phẳng 2   
{
    int rel1 = M3D_RelPoint2Plan(point, vp1, vn1);
    int rel2 = M3D_RelPoint2Plan(point, vp2, vn2);

    // Trường hợp ngược hướng
    if (rel1 * rel2 < 0)
    {
        return 1;
    }
    return -1;
}

//==================================================================================
// Tìm hình chiếu của một điểm xuống một đoạn thẳng                                 
//==================================================================================
static glm::vec3 M3D_PerpPoint2Segment(const vec3&  point,// Điểm chiếu              
                                   const vec3&  pA,       // Điểm đầu của đoạn thẳng 
                                   const vec3&  pB)       // Điểm cuối của đoạn thẳng
{
    glm::vec3 point_perp(0, 0, 0);
    float disAB   = glm::distance(pA, pB);
    if (disAB >= M3_EPSILON)
    {
        vec3 d = (pB - pA) / disAB;
        vec3 v = (point - pA);

        float t = glm::dot(d, v);
        point_perp = pA + t* d;
    }
    else
    {
        point_perp = pA;
    }
    return point_perp;
}

//==================================================================================
// Tìm khoảng cách từ một điểm đến một đoạn thẳng                                   
//==================================================================================
static float M3D_DistancePoint2Segment(const vec3&  point, // Điểm lấy khoảng cách  
                                       const vec3&  pA,    // Điểm đầu của đoạn thẳng 
                                       const vec3&  pB)    // Điểm cuối của đoạn thẳng
{
    float distance = 0;
    float disAB   = glm::distance(pA, pB);
    if (disAB >= M3_EPSILON)
    {
        vec3 d = (pA - pB) / disAB;
        vec3 v = (pA - point);

        float t = glm::dot(d, v);
        vec3 p = pA + t* d;

        distance = glm::distance(p, point);
    }
    else
    {
        distance = disAB;
    }
    return distance;
}

//==================================================================================
// Tìm vị trí trong không gian sử dụng vị trí targe khoảng cách và góc phi, theta   
// * Góc phi và theta chỉ áp dụng cho mặt phẳng song song Oxyz                      
//==================================================================================
static glm::vec3 M3D_CalPosition(vec3  vtarget,  // Vị trí target                   
                                 float distance, // Khoảng cách tới target          
                                 float rphi,     // Góc phụ với Oz(radian)          
                                 float rtheta)   // Góc với Oy    (radian)          
{
    vec3 vRel(0, 0, 0), position;
    // Tính điểm tại với hệ tọa độ gốc
    float x = cos(rphi) * sin(rtheta);
    float z = sin(rphi);
    float y = cos(rphi) * cos(rtheta);
    position = vec3(x, y, z) * distance;

    // Dịch chuyển điểm đó đến target
    float dis  = glm::distance(vtarget, vec3(0, 0, 0));
    if (fabs(dis) >= M3_EPSILON)
    {
        vec3 vut = glm::normalize(vtarget - vec3(0, 0, 0));
        vRel     = position + (vut * dis);
    }
    else
    {
        vRel = position;
    }
    return vRel;
}

//==================================================================================
// Tìm góc giữ hai vector 3D  [0->π] hoặc [0->2π]                                   
//==================================================================================
static float M3D_Angle2Vector(vec3 v1,                 // Vector 1                  
                              vec3 v2,                 // Vector 2                  
                              bool bDg = false,        // false/true [radians/degree]
                              bool bCr = false,        // Lấy góc trong khoảng [0->2π][Ex]
                              vec3 vUp = vec3(0, 1 ,0))// Vector up sử dụng cho[0->2π][Ex]
{
    float angle  = 0;
    float dot    = glm::dot(v1, v2);
    float mag_v1 = glm::distance(v1, vec3(0,0,0));
    float mag_v2 = glm::distance(v2, vec3(0,0,0));

    float div = mag_v1* mag_v2;
    if (fabs(div) >= M3_EPSILON)
    {
        // Trường hợp lấy góc trong khoảng [0->π]
        angle = glm::acos(dot / div);

        // Trường hợp lấy góc trong khoảng [0->2π]
        if (bCr)
        {
            vec3 cross = glm::cross(v1, v2);
            float mult = glm::dot(vUp, cross);
            if (mult > 0.0)
            {
                angle = -angle;
                angle = M3_2PI + angle;
            }
        }
        // Quy đổi góc từ radians -> degrees
        if (bDg)
        {
            angle = glm::degrees(angle);
        }
    }
    return angle;
}

//==================================================================================
// Tìm hình chiếu của một điểm đến mặt phẳng cho bởi một điểm và 1 vector pháp tuyến
//==================================================================================
static glm::vec3 M3D_PerpPoint2Plan( vec3 p,       // Điểm tìm hình chiếu           
                                     vec3 pointp,  // Điểm 1 của mặt phẳng          
                                     vec3 n)       // Vector pháp tuyến mặt phẳng   
{
    vec3 point_perp;
    vec3 p1p  = p - pointp;
    float dis = glm::dot(p1p, n);

    if (fabs(dis) >= M3_EPSILON)
    {
        point_perp = p - n * dis;
    }
    else
    {
        point_perp = p;
    }

    return point_perp;
}

//==================================================================================
// Tìm hình chiếu của một điểm đến mặt phẳng cho bởi 3 điểm                         
//==================================================================================
static glm::vec3 M3D_GetPerpPoint2Plan( vec3 p,         // Điểm tìm hình chiếu      
                                        vec3 pointp1,   // Điểm 1 của mặt phẳng     
                                        vec3 pointp2,   // Điểm 2 của mặt phẳng     
                                        vec3 pointp3)   // Điểm 3 của mặt phẳng     
{
    vec3 point_perp;

    vec3 p1p2 = pointp2 - pointp1;
    vec3 p1p3 = pointp3 - pointp1;
    vec3 p1p =  p       - pointp1; 

    vec3 n = glm::normalize(glm::cross(p1p2, p1p3));
    float dis = glm::dot(p1p, n);

    if (fabs(dis) >= M3_EPSILON)
    {
        point_perp = p - n * dis;
    }
    else
    {
        point_perp = p;
    }

    return point_perp;
}

//==================================================================================
// Tìm góc phi và theta so với trục oxyz cơ bản                                     
//==================================================================================
static void M3D_GetAnglePositionOxyz(vec3  vPoint,        // vector target          
                                     vec3  vTarget,       // Vị trí trung tâm       
                                     float &phi,          // Góc phụ với Oy(radian) [-π/2->π/2]
                                     float &theta)        // Góc với Oz    (radian) [ 0  -> 2π ]
{
    vec3 vOx = vec3(1, 0, 0);
    vec3 vOy = vec3(0, 1, 0);
    vec3 vOz = vec3(0, 0, 1);
    vec3 vOO = vec3(0, 0, 0);

    vec3 v_uint      = glm::normalize(vPoint - vTarget);
    vec3 v_uint_perp = M3D_GetPerpPoint2Plan(v_uint, vOO, vOx, vOy);

    // Góc giữ vector tơ và trục Oy
    phi   = M3_HPI - M3D_Angle2Vector(v_uint, vOz);
    // Góc giữ hình chiếu của vector xuống Oxz và trục Oz
    theta = M3D_Angle2Vector(v_uint_perp, vOx,false, true);
}

//==================================================================================
// Lấy mảng đường thẳng và màu của grid                                             
//==================================================================================
static vector<vec3> M3D_GetGrid(vec3  center,           // Điểm trung tâm của grid  
                                float width,            // Chiều dài của plan       
                                float height,           // Chiều rộng của plan      
                                float space,            // Khoảng cách mỗi đường    
                                int   majorGridLine)    // Số lượng đến grid chính  
{
    vector<vec3> grids;

    vec3 point1, point2;

    vec3 min_point = center - vec3(width / 2, 0, height / 2);
    vec3 max_point = center + vec3(width / 2, 0, height / 2);

    int loop_x = int((max_point.x - min_point.x) / space);
    int loop_z = int((max_point.z - min_point.z) / space);

    vec3 color_major  = vec3(0.05, 0.05, 0.05);
    vec3 color_normal = vec3(0.6, 0.6, 0.6);

    // get x line
    float x_min = center.x - width / 2;
    for (int i = 0; i < loop_x; i++)
    {
        point1 = vec3(x_min + i*space, center.y- height / 2 , center.z);
        point2 = vec3(x_min + i*space, center.y+ height / 2 , center.z);

        if ( i % majorGridLine == 0)
        {
            grids.push_back(point1);
            grids.push_back(color_major);
            grids.push_back(point2);
            grids.push_back(color_major);
        }
        else 
        {
            grids.push_back(point1);
            grids.push_back(color_normal);
            grids.push_back(point2);
            grids.push_back(color_normal);
        }
    }

    // get y line
    float y_min = center.y - height / 2;
    for (int i = 0; i < loop_z; i++)
    {
        point1 = vec3(center.x - width / 2, y_min + i*space, center.z);
        point2 = vec3(center.y + width / 2, y_min + i*space, center.z);

        if (i % majorGridLine == 0)
        {
            grids.push_back(point1);
            grids.push_back(color_major);
            grids.push_back(point2);
            grids.push_back(color_major);
        }
        else 
        {
            grids.push_back(point1);
            grids.push_back(color_normal);
            grids.push_back(point2);
            grids.push_back(color_normal);
        }
    }
    return grids;
}

//==================================================================================
// Tính toán vector vuông góc với mặt phẳng cho bởi 3 điểm                          
//==================================================================================
glm::vec3 M3D_NormalSurface(const glm::vec3& p1,   // Điểm thứ nhất                 
                            const glm::vec3& p2,   // Điểm thứ hai                  
                            const glm::vec3& p3)   // Điểm thứ ba                   
{
    vec3 edge1 = p1 - p2;
    vec3 edge2 = p2 - p3;

    vec3 crs = glm::cross(edge1, edge2);
    vec3 normal = glm::normalize(crs);

    return normal;
}


#endif // !GL3DFUN_H