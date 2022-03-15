////////////////////////////////////////////////////////////////////////////////////
// File : GL2DFun.h   -- Xử lý 2D                                                   
// For conditions of distribution and use, see copyright notice in readme.txt       
//                                                                                  
// Copyright (C) 2020-2021 : 31/08/2021             Author  : Thuong.NV             
////////////////////////////////////////////////////////////////////////////////////

#ifndef GL2DFUN_H
#define GL2DFUN_H

#define  M2_ALG_EPSILON     0.0001         // Coincident angle radius
#define  M2_EPSILON         0.01           // Coincident

#define  M2_2PI             6.28318530718  // 2*PI
#define  M2_PI              3.14159265359  // PI
#define  M2_HPI             1.57079632679  // PI/2

#include <GL/glew.h>
#include <glm/glm.hpp>
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
    inline ostream& operator<<(ostream& os, const glm::vec2& p)
    {
        cout << "(" << p.x << ", " << p.y << ")";
        return os;
    }
}

//==================================================================================
// Chuyển từ tọa độ 3D sang 2D                                                      
// Return : Giá trị float                                                           
//==================================================================================
static vec2 M2D_V3(const vec3& v)
{
    return vec2(v.x, v.y);
}

//==================================================================================
// Tính cross product của 2 vector 2D                                               
// Return : Giá trị float                                                           
//==================================================================================
static float M2D_Cross(const vec2& v1, const vec2& v2)
{
    return (v1.x * v2.y) - (v1.y*v2.x);
}

//==================================================================================
// Vị trí tương đối của 3 điểm trong mặt mảng Oxy                                   
// Return : -1 nằm phía bên trái theo hướng p1p2                                    
//           0 nằm trên mặt phẳng                                                   
//           1 nằm phía bên phải theo thướng p1p2                                   
//==================================================================================
static int M2D_Orientation(const vec2& p1,const vec2& p2,const vec2& p)
{
    vec2 p1p2 = p2 - p1; vec2 p1p  = p  - p1;
    // Tính cross product giữa hai vector
    float fCrs = M2D_Cross(p1p2, p1p);

    // Xác định 3 điểm thẳng hàng
    if (fabs(fCrs) < M2_EPSILON)
    {
        return 0;
    }
    return (fCrs > 0)? 1 : -1;
}

//==================================================================================
// Vị trí tương đối của 3 điểm mặt phằng có cùng z                                  
// Return : -1 nằm phía bên trái theo hướng p1p2                                    
//           0 nằm trên mặt phẳng                                                   
//           1 nằm phía bên phải theo thướng p1p2                                   
//==================================================================================
static int M2D_Orientation(const vec3& p1,const vec3& p2,const vec3& p)
{
    vec2 p1p2 = M2D_V3(p2 - p1); vec2 p1p  = M2D_V3(p  - p1);
    // Tính cross product giữa hai vector
    float fCrs = M2D_Cross(p1p2, p1p);

    // Xác định 3 điểm thẳng hàng
    if (fabs(fCrs) < M2_EPSILON)
    {
        return 0;
    }
    return (fCrs > 0)? 1 : -1;
}

//==================================================================================
// Chia polygon lõm và lồi thành các tam giác không tính trong trường hợp đặc biệt  
// Return : Mảng tọa độ 3 điểm của một tam giác                                     
//==================================================================================
static vector<vec2> M2D_DivPoly2Trig(const vector<vec2>& poly)
{
    if (poly.size() <= 3) return poly;

    vec2 p1, p2 , p3;
    vector <vec2> outPoly;
    vector <vec2> polyTemp = poly;
    int  iSizeOld;
    while (polyTemp.size() > 3)
    {
        p1 = polyTemp.front();
        p2 = polyTemp.back();

        // Tìm điểm tiếp theo phù hợp và xóa nó đi
        iSizeOld = polyTemp.size();
        for (int i = 1 ; i< iSizeOld ; i++)
        {
            if (M2D_Orientation(p1, p2, p3) > 1)
            {
                outPoly.push_back(p1);
                outPoly.push_back(p2);
                outPoly.push_back(p3);
                polyTemp.erase(polyTemp.begin());
                break;
            }
        }
        // Trường hợp không thể tìm được điểm thì out luôn
        if (polyTemp.size() == iSizeOld)
        {
            break;
        }
    }

    for (int i = 0; i < polyTemp.size(); i++)
    {
        outPoly.push_back(polyTemp[i]);
    }
}

//==================================================================================
// Chia polygon lõm và lồi thành các tam giác không tính trong trường hợp đặc biệt  
// Sử dụng với tọa độ không gian 3D (có cùng tọa độ z)                              
// Return : Mảng tọa độ 3 điểm của một tam giác                                     
//==================================================================================
static vector<vec3> M2D_DivPoly2Trig(const vector<vec3>& poly)
{
    if (poly.size() <= 3) return poly;

    int  iSizeOld; vec3 p1, p2 , p3;
    vector <vec3> outPoly, polyTemp = poly;

    while (polyTemp.size() > 3)
    {
        p1 = polyTemp.front();
        p2 = polyTemp.back();

        // Tìm điểm tiếp theo phù hợp và xóa nó đi
        iSizeOld = polyTemp.size();
        for (int i = 1 ; i< iSizeOld ; i++)
        {
            p3 = polyTemp[i];
            if (M2D_Orientation(p1, p2, p3) == -1)
            {
                outPoly.push_back(p1);
                outPoly.push_back(p2);
                outPoly.push_back(p3);
                polyTemp.erase(polyTemp.begin());
                break;
            }
        }
        // Trường hợp không thể tìm được điểm thì out luôn
        if (polyTemp.size() == iSizeOld)
        {
            break;
        }
    }

    // Trường hợp còn thừa thì đẩy hết ra ngoài @@
    for (int i = 0; i < polyTemp.size(); i++)
    {
        outPoly.push_back(polyTemp[i]);
    }
    return outPoly;
}

//==================================================================================
// Lấy ra dữ liệu cần để vẽ grid 2D                                                 
// Return : Dữ liệu cần thiết cho việc vẽ grid                                      
//==================================================================================
static glm::vec2 M2D_PointLocal2Global( vec2 posCam,        // Vị trí của camera (2D)
                                        float width,        // Độ rộng của view      
                                        float height,       // Độ cao của view       
                                        float x,            // Tọa độ x vị trí local 
                                        float y,            // Tọa độ y vị trí local 
                                        float zoom)         // Tỷ lệ zoom            
{
    vec2 point = {0 , 0};    // Để mặc định
    point.x = posCam.x + (x - float(width /2))/zoom;
    point.y = posCam.y - (y - float(height/2))/zoom;
    return point;
}

//==================================================================================
// Lấy ra dữ liệu cần để vẽ grid 2D                                                 
// Return : Dữ liệu cần thiết cho việc vẽ grid                                      
//==================================================================================
static glm::vec2 M2D_PointGlobal2Local( vec2 posCam,       // Vị trí của camera (2D)
                                        float width,       // Độ rộng của view      
                                        float height,      // Độ cao của view       
                                        float x,           // Tọa độ x vị trí global 
                                        float y,           // Tọa độ y vị trí global 
                                        float zoom)        // Tỷ lệ zoom            
{
    vec2 point = {0 , 0};   // Để mặc định
    point.x = (float(width /2)    - zoom*(posCam.x -x));
    point.y = (zoom*(posCam.y -y) + float(height/2));
    return point;
}


//==================================================================================
// Lấy ra dữ liệu cần để vẽ grid 2D                                                 
// Return : Dữ liệu cần thiết cho việc vẽ grid                                      
//==================================================================================
static vector<vec3> M2D_GetGridData(const vec2& posCam,    // Vị trí của camera (2D)
                                    float width,           // Độ rộng của view      
                                    float height,          // Độ cao của view       
                                    float zoom,            // Tỷ lệ zoom            
                                    float cellwidth,       // ???                   
                                    float deep,            // Độ sau của grid       
                                    vec3  color)           // Màu grid              
{
    float fUint = cellwidth; vec3 p1, p2, pText, pMark;
    // Nó gần như là tọa độ chuột mặc định
    vec2 minbound = M2D_PointLocal2Global(posCam, width, height, 0.0  , 0.0, zoom);
    vec2 maxbound = M2D_PointLocal2Global(posCam, width, height, width, height, zoom);

    vector<vec3> grid;

    float xmin = glm::min(minbound.x, maxbound.x);
    float ymin = glm::min(minbound.y, maxbound.y);

    float xmax =  glm::max(minbound.x, maxbound.x);
    float ymax =  glm::max(minbound.y, maxbound.y);

    // Tìm điểm bắt đầu gần với minbound x và y nhất
    int startx = int(xmin / fUint);
    int starty = int(ymin / fUint);

    float delta = 3.0f / zoom;
    // Đoạn này nhầm float ->int
    for (float x = startx*fUint; x <= xmax; x += fUint)
    {
        p1.x = x; p1.y = ymin; p1.z = deep;
        p2.x = x; p2.y = ymax; p2.z = deep;
        pText.x = x+delta; pText.y = delta    ; pText.z = deep;
        pMark.x = 1.0f   ; pMark.y = 0.0f     ; pMark.z = 0.0f;

        grid.push_back(p1);
        grid.push_back(p2);
        grid.push_back(color);
        grid.push_back(color);
        grid.push_back(pText);
        grid.push_back(pMark);
    }

    for (float y = starty*fUint; y <= ymax; y += fUint)
    {
        p1.x = xmin;  p1.y = y; p1.z = deep;
        p2.x = xmax;  p2.y = y; p2.z = deep;
        pText.x = delta  ; pText.y = y+ delta ; pText.z = deep;
        pMark.x = 0.0f   ; pMark.y = 1.0f     ; pMark.z = 0.0f;

        grid.push_back(p1);
        grid.push_back(p2);
        grid.push_back(color);
        grid.push_back(color);
        grid.push_back(pText);
        grid.push_back(pMark);
    }
    return grid;
}

//==================================================================================
// Lấy ra dữ liệu cần để vẽ grid 2D                                                 
// Return : Dữ liệu cần thiết cho việc vẽ grid                                      
//==================================================================================
static float  M2D_CalCellWidth(vec2  posCam,           // Vị trí của camera (2D)
                               float width,            // Độ rộng của view      
                               float height,           // Độ cao của view       
                               float zoom,             // Tỷ lệ zoom            
                               float cellwidthCurrent) // Độ rộng của width hiện tại
{
    float newCellwidth = cellwidthCurrent;

    // Nó gần như là tọa độ chuột mặc định
    vec2 minbound = M2D_PointGlobal2Local(posCam, width, height, 0.0             , 0.0, zoom);
    vec2 maxbound = M2D_PointGlobal2Local(posCam, width, height, cellwidthCurrent, 0.0, zoom);

    float cellwidth = fabs(minbound.x - maxbound.x);

    if (cellwidth > 100.0f)
    {
        newCellwidth = (cellwidthCurrent) / 2;
    }
    else if (2*cellwidth < 100)
    {
        newCellwidth = (cellwidthCurrent)* 2;
    }
    return newCellwidth;
}

////==================================================================================
//// Lấy ra dữ liệu cần để vẽ grid 2D                                                 
//// Return : Dữ liệu cần thiết cho việc vẽ grid                                      
////==================================================================================
//static vector<vec3> M2D_GetGridData2(const vec2& posCam,    // Vị trí của camera (2D)
//                                    float width,           // Độ rộng của view      
//                                    float height,          // Độ cao của view       
//                                    float zoom,            // Tỷ lệ zoom            
//                                    float cellwidth,       // ???                   
//                                    float deep,            // Độ sau của grid       
//                                    vec3  color)           // Màu grid              
//{
//    float fUint = cellwidth; vec3 p1, p2;
//    // Nó gần như là tọa độ chuột mặc định
//    vec2 minbound = M2D_PointLocal2Global(posCam, width, height, 0.0  , 0.0, zoom);
//    vec2 maxbound = M2D_PointLocal2Global(posCam, width, height, width, height, zoom);
//
//    vector<vec3> grid;
//
//    float xmin = glm::min(minbound.x, maxbound.x);
//    float ymin = glm::min(minbound.y, maxbound.y);
//
//    float xmax =  glm::max(minbound.x, maxbound.x);
//    float ymax =  glm::max(minbound.y, maxbound.y);
//
//    // Tìm điểm bắt đầu gần với minbound x và y nhất
//    int startx = int(xmin / fUint);
//    int starty = int(ymin / fUint);
//
//    for (int x = startx*fUint; x < xmax; x += fUint)
//    {
//        p1.x = x; p1.y = minbound.y; p1.z = deep;
//        p2.x = x; p2.y = maxbound.y; p2.z = deep;
//
//        grid.push_back(p1);
//        grid.push_back(p2);
//        grid.push_back(color);
//        grid.push_back(color);
//    }
//
//    for (int y = starty*fUint; y < ymax; y += fUint)
//    {
//        p1.x = minbound.x;  p1.y = y; p1.z = deep;
//        p2.x = maxbound.x;  p2.y = y; p2.z = deep;
//
//        grid.push_back(p1);
//        grid.push_back(p2);
//        grid.push_back(color);
//        grid.push_back(color);
//    }
//    return grid;
//}


#endif // !GL2DFUN_H