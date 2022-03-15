////////////////////////////////////////////////////////////////////////////////
// File : GLCamere.h                                                            
// Chứa Các lớp và các hàm sử dụng độc lập không liên quan đến các lớp tạo ra   
//                                                                              
// Created : 31/08/2021             Author  : Thuong.NV                         
////////////////////////////////////////////////////////////////////////////////

#ifndef GLCAMERA_H
#define GLCAMERA_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GL3DFun.h"

using namespace std;
using namespace glm;
using namespace sp;


#define ZOOM_MIN         0.01f
#define ZOOM_MAX         100.f
#define ZOOM_FACTOR      0.9f

enum CameraType
{
    C2D,
    C3D,
};

enum CameraMode
{
    CMODE_NORMAL,
    CMODE_NOZOOM,
};

//==============================================================================
//Camera base                                                                   
//==============================================================================
class GLCamera
{
protected:
        CameraType      m_iType;        // Loại camera

        glm::vec3       m_position;     // Vị trí camera
        glm::vec3       m_direction;    // Hướng camera trỏ tới (ngược hướng với hướng camera thực tế)
                                        // Nó nên là vector đơn vị (normal)
        glm::vec3       m_up;           // Hướng lên camera


        int             m_iWidth;       // Chiều rộng của view
        int             m_iHeight;      // Chiều cao của view
        float           m_fNear;        // Mặt phẳng gần
        float           m_fFar;         // Mặt phẳng xa


        glm::mat4       m_viewMat;      // view matrix
        glm::mat4       m_projMat;      // projection matrix 
        glm::mat4       m_modelMat;     // model matrix (sử dụng OpenGL 1.1)

public:
        GLCamera()
        {

        }

        virtual ~GLCamera()
        {

        }

public:
        virtual void UpdateViewMatrix() = 0;
        virtual void UpdateProjMatrix() = 0;
        virtual void UseMatrix(CameraMode mode = CMODE_NORMAL) const = 0;
        virtual void LoadMatrix(GLuint program) const = 0;

public:
        //==============================================================================
        // Chuyển đổi từ tọa độ [Trái trên] sang tọa độ tại [Trung tâm] trên view       
        // Tọa độ [x, y] đầu vào là tọa độ trên view  (LT là left top)                  
        //==============================================================================
        glm::vec2 PointLT2Center(const vec2& p) const
        {
            vec2 point = { 0, 0 };  // Để mặc định
            point.x =  (p.x - m_iWidth /2);
            point.y = -(p.y - m_iHeight/2);
            return point;
        }

        //==============================================================================
        // Chuyển đổi từ tọa độ [Trái trên] sang tọa độ tại [Trung tâm] trên view   [3D]
        // Tọa độ [x, y, z] đầu vào là tọa độ trên view  (LT là left top)               
        //==============================================================================
        glm::vec3 PointLT2Center(const vec3& p) const
        {
            vec3 point = { 0, 0 ,  p.z};  // Để mặc định
            point.x =  (p.x - m_iWidth /2);
            point.y = -(p.y - m_iHeight/2);
            return point;
        }

public:
        CameraType GetCameraType() const
        {
            return m_iType;
        }
        glm::mat4 GetViewMatrix()
        {
            return m_viewMat;
        }
        glm::mat4 GetProjectionMatrix() const
        {
            return m_projMat;
        }

        void InitView(int width, int height, float _near, float _far)
        {
            m_fNear   = _near;
            m_fFar    = _far;
            m_iWidth  = width;
            m_iHeight = height;
        }

        void SetUpCamera(glm::vec3 pos, glm::vec3 dir, glm::vec3 up = glm::vec3(0, 1, 0))
        {
            m_position  = pos;
            m_direction = dir;
            m_up        = up;
        }

        void SetViewSize(int width, int height)
        {
            m_iWidth  = width;
            m_iHeight = height;
        }

        void SetDistPlane(float _near, float _far)
        {
            m_fNear = _near;
            m_fFar  = _far;
        }

        glm::vec3 GetPosition()
        {
            return m_position;
        }

        void SetPosition(glm::vec3 pos)
        {
            m_position  = pos;
        }

        // Vị trí camera dự vào vị trí target
        void SetPosition(float phi, float theta, float distance, glm::vec3 target)
        {
            // Tính điểm tại vị trí tâm
            float x = cos(phi) * sin(theta);
            float y = sin(phi);
            float z = cos(phi) * cos(theta);
            vec3 position = vec3(x, y, z) * distance;

            // Dịch chuyển điểm đó đến target
            float dis  = glm::distance(target, vec3(0, 0, 0));
            if (fabs(dis) > 0.1)
            {
                vec3 vut   = glm::normalize(target - vec3(0, 0, 0));

                m_position  = position + (vut * dis);
                m_direction = glm::normalize(m_position - target);
            }
            else
            {
                m_position = position;
            }
        }

        void Move(glm::vec3 mov)
        {
            m_position += mov;
            this->UpdateViewMatrix();
        }

        void UpdateMatrix()
        {
            this->UpdateViewMatrix();
            this->UpdateProjMatrix();
        }
};

//==============================================================================
//Camera 2D                                                                     
//==============================================================================
class GLCamera2D : public GLCamera
{

private:

        float           m_fZoom = 1;  // Tỷ lệ zoom

        glm::mat4       m_projMatNozoom; //projection matrix nozoom
public:
        GLCamera2D()
        {
            m_iType = CameraType::C2D;
            m_fZoom = 1.0;
        }
public:
        //==============================================================================
        // Chuyển đổi tọa độ điểm từ view sang tọa độ thực tế                           
        // Tọa độ [x, y] đầu vào là tọa độ trên view  (Left Top)                        
        //==============================================================================
        glm::vec2 PointLocal2Global(const vec2& p)
        {
            vec2 point = {0 , 0};    // Để mặc định
            point.x = m_position.x + (p.x - float(m_iWidth /2))/m_fZoom;
            point.y = m_position.y - (p.y - float(m_iHeight/2))/m_fZoom;
            return point;
        }

        //==============================================================================
        // Chuyển đổi tọa độ điểm từ thực thế sang tọa độ view                          
        // Tọa độ [x, y] đầu vào là tọa độ điểm thực tế  (Left Top)                     
        //==============================================================================
        glm::vec2 PointGlobal2Local(const vec2& p)
        {
            vec2 point = {0 , 0};   // Để mặc định
            point.x = (float(m_iWidth /2) - m_fZoom*(m_position.x -p.x));
            point.y = (m_fZoom*(m_position.y -p.y) + float(m_iHeight/2));
            return point;
        }

public:
        mat4  GetMatrixNozoom()
        {
            return  m_projMatNozoom;
        }
        void UpdateViewMatrix()
        {
            m_viewMat  = glm::lookAt(m_position, m_position - m_direction, m_up);
        }

        void UpdateProjMatrix()
        {
            // Projection matrix không sử dụng zoom
            float left   = -float(m_iWidth  / 2);
            float right  =  float(m_iWidth  / 2);
            float bottom = -float(m_iHeight / 2);
            float top    =  float(m_iHeight / 2);

            m_projMatNozoom  = glm::ortho(left, right, bottom, top, float(m_fNear), float(m_fFar));

            // Projection matrix sử dụng thuộc tính zoom
            left         =  left  / m_fZoom;
            right        =  right / m_fZoom;
            bottom       =  bottom/ m_fZoom;
            top          =  top   / m_fZoom;

            m_projMat        = glm::ortho(left, right, bottom, top, float(m_fNear), float(m_fFar));
        }

        void UseMatrix(CameraMode mode = CameraMode::CMODE_NORMAL) const
        {
            // Use OpenGL >= 1.1 có thể sử dụng shader
            if (mode == CameraMode::CMODE_NOZOOM)
            {
                glMatrixMode(GL_PROJECTION);
                glLoadMatrixf(glm::value_ptr(m_projMatNozoom));

                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
            }
            else
            {
                glMatrixMode(GL_PROJECTION);
                glLoadMatrixf(glm::value_ptr(m_projMat));

                glMatrixMode(GL_MODELVIEW);
                glLoadMatrixf(glm::value_ptr(m_viewMat));
            }
        }

        // Use OpenGL + Shader + GLSL
        void LoadMatrix(GLuint program) const
        {
            GLint mvLoc   = glGetUniformLocation(program, "mv_matrix");
            GLint projLoc = glGetUniformLocation(program, "proj_matrix");

            glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(m_viewMat));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(m_projMat));
        }
public:
        //==============================================================================
        // Lấy giá trị zoom                                                             
        //==============================================================================
        float GetZoom()
        {
            return m_fZoom;
        }
        //==============================================================================
        // Cập nhật thông số zoom có giới hạn giá trị                                   
        //==============================================================================
        void UpdateZoom(float zDelta )
        {
            // [Note] Điều chỉnh zoom này có lợi thế là sẽ zoom đều             
            // Khi zoom càng bé thì giảm zoom càng chậm thay cho việc tuyến tính
            // Tương tự nếu tăng sẽ nhanh nếu nếu zoom càng lớn                 
            if (zDelta > 0)
            {
                m_fZoom /= ZOOM_FACTOR;
            }
            else
            {
                m_fZoom *= ZOOM_FACTOR;
            }

            // Xử lý trong trường hợp giới hạn
            if (m_fZoom < ZOOM_MIN)
            {
                m_fZoom = ZOOM_MIN;
            }
            else if (m_fZoom > ZOOM_MAX)
            {
                m_fZoom = ZOOM_MAX;
            }
        }

        //==============================================================================
        // Target camera vào một vị trí (tọa độ local) với lượng zoom delta             
        //==============================================================================
        void ZoomTo(float x, float y, float delta_z)
        {
            // Tính tọa độ thực tế của nó trước và sau khi set zoom
            vec2 pGlobalOld = PointLocal2Global(vec2(x, y));
            this->UpdateZoom(delta_z);
            vec2 pGlobalNew = PointLocal2Global(vec2(x, y));
            vec2 vMove = pGlobalNew - pGlobalOld;

            // Di chuyển vị trí của camera một đoạn vì vị trí x, y được giữ nguyên
            m_position.x -= vMove.x;
            m_position.y -= vMove.y;
            m_position.z  = m_position.z;
        }

        //==============================================================================
        // Dịch chuyển tọa độ camera đi một khoảng delta x và delta y (tọa độ local)    
        //==============================================================================
        void Move(float delx, float dely)
        {
            float deltax_glo = -delx / m_fZoom;
            float deltay_glo =  dely / m_fZoom;

            m_position += vec3(deltax_glo, deltay_glo, 0);
        }
};

//==============================================================================
//Camera 3D                                                                     
//==============================================================================
class GLCamera3D : public GLCamera
{

private:
        float           m_fFov;    // Góc nhìn

        // 2021.09.22 N.V.Thuong [Tính toán cho xoay quanh mục tiêu]
        float           m_fTheta;  // Góc với trục Oz
        float           m_fPhi;    // Góc phụ với trục Oy
        glm::vec3       m_vTarget; // Vị trí camera Target
        float           m_fDis;    // Vị trí camera Target


        int             m_iMode;   // Chế độ Camera đang hoạt động

public:
        GLCamera3D()
        {
            m_iType = CameraType::C3D;
        }

        void UpdateViewMatrix()
        {
            m_viewMat  = glm::lookAt(m_position, m_position - m_direction, m_up);
        }
        void UpdateProjMatrix()
        {
            float aspect = float(m_iWidth) / float(m_iHeight);
            m_projMat  = glm::perspective(glm::radians(m_fFov) ,aspect, m_fNear, m_fFar);
        }

        void UseModelMatrix(mat4 model = mat4(1.0f))
        {
            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(glm::value_ptr(m_viewMat * model));
        }

        // Use OpenGL >= 1.1 có thể sử dụng shader
        void UseMatrix(CameraMode mode = CameraMode::CMODE_NORMAL) const
        {
            glMatrixMode(GL_PROJECTION);
            glLoadMatrixf(glm::value_ptr(m_projMat));
            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(glm::value_ptr(m_viewMat));
        }

        // Use OpenGL + Shader + GLSL
        void LoadMatrix(GLuint program) const
        {
            GLint mvLoc = glGetUniformLocation(program, "mv_matrix");
            GLint projLoc = glGetUniformLocation(program, "proj_matrix");

            glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(m_viewMat));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(m_projMat));
        }
private:
        void UpdateOrbitTarget(float phi, float theta)
        {
            //↓ 2021.09.22 N.V.Thuong [Xử lý ngoại lệ và khoảng xoay]
            if (fabs(phi) > M3_HPI)
            {
                phi = (phi < 0.0)?(-M3_HPI+M3_ALG_EPSILON):(M3_HPI-M3_ALG_EPSILON);
            }
            if (theta > M3_2PI)
            {
                int factor = int(theta / M3_2PI);
                theta = theta - factor* M3_2PI;
            }
            else if (theta < 0)
            {
                int factor = int(theta / M3_2PI);
                theta = theta + abs(factor+1)* M3_2PI;
            }
            //↑ 2021.09.22 N.V.Thuong [Xử lý ngoại lệ và khoảng xoay]

            //↓ 2021.09.22 N.V.Thuong [Cập nhật thông số camera]
            m_fPhi     = phi;
            m_fTheta   = theta;
            m_position = M3D_CalPosition(m_vTarget, m_fDis, m_fPhi, m_fTheta);

            if (m_fDis >= M3_EPSILON)
            {
                m_direction = glm::normalize(m_position - m_vTarget);
            }
            else
            {
                m_direction = vec3(0, 0, 0);
            }
            //↑ 2021.09.22 N.V.Thuong [Cập nhật thông số camera]
        }
public:
        void SetFieldOfView(float fov)
        {
            m_fFov  = fov;
        }

        // Thiết lập thông số cho chế độ xoay target
        void SetUpOrbitTarget(glm::vec3 position, glm::vec3 target)
        {
            m_vTarget = target;
            m_fDis    = glm::distance(position, target);
            M3D_GetAnglePositionOxyz(position, target, m_fPhi, m_fTheta);
            this->UpdateOrbitTarget(m_fPhi, m_fTheta);
        }

        void OrbitTarget(float delPhi, float delTheta)
        {
            float phi   = m_fPhi   + delPhi;
            float theta = m_fTheta + delTheta;
            this->UpdateOrbitTarget(phi, theta);
        }

        //==============================================================================
        // Thu vị trí camera vào vị trí target đã được đặt trước                        
        //==============================================================================
        void ZoomTo(float delta)
        {
            vec3 vUint        = glm::normalize(m_direction);
            vec3 vPositionNew = m_position - vUint*delta;
            vec3 vUintNew     = glm::normalize(vPositionNew - m_vTarget);

            float fDot = glm::dot(vUint, vUintNew);
            if (fDot <= 0.0)
            {
                m_position = m_vTarget + vUint;
            }
            else
            {
                m_position = vPositionNew;
            }
            m_fDis = glm::distance(m_position, m_vTarget);
        }

        //==============================================================================
        // Di chuyển camera theo hướng tạo bưởi direction và up                         
        //==============================================================================
        void OrbitMove(float delta)
        {
            vec3 vuint_move = glm::cross(m_up, -m_direction);
            m_position += vuint_move * delta;
            m_vTarget  += vuint_move * delta;
            this->UpdateViewMatrix();
        }

        //==============================================================================
        // Di chuyển camera theo mặt phẳng của camera                                   
        //==============================================================================
        void OrbitMove1(float x , float y)
        {
            float xtemp = m_position.x + x;
            float ytemp = m_position.y + y;

            //↓ 2021.09.22 N.V.Thuong [Tính tọa độ z của camera mới]
            glm::vec3 vn = glm::normalize(m_direction);
            float d = vn.x* m_position.x + vn.y*m_position.y + vn.z *m_position.z;
            float ztemp = (d - (vn.x*xtemp + vn.y*ytemp))/vn.z;

            glm::vec3 temp(xtemp, ytemp ,ztemp);
            if (glm::distance(temp, m_position) > M3_EPSILON)
            {
                vec3 v = glm::normalize(temp - m_position);
                m_position += v*(0.12f) ;
                m_vTarget  += v*(0.12f) ;
                this->UpdateViewMatrix();
            }
        }
};


#endif // !GLCAMERA_H

