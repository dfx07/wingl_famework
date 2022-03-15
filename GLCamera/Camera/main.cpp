#include <GLWindow.h>
#include "GLCamera.h"
#include "GL3DFun.h"


GLCamera3D cam3d;

glm::vec3 pos = {4, 4, 4};
glm::vec3 dir = {0, 0 ,-1};
glm::vec3 up  = {0, 0, 1};
vec3 target     (0, 0, 0);

/*
*   - Tạo ra mặt phẳng grid trong 3D  [OK]
*   - Quay camera tại chỗ dạng view được object [OK] cần tích hợp nó vào camera + zoom vào thay đổi target
*   - Zoom vào một điểm trong camera 2D
*   - Vẽ được đối tượng lên và quan sát ở chết độ view
*   - Tạo view phép chiếu 
*/


static const GLfloat g_vertex_buffer_data[] = {
    -1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
};

// One color for each vertex. They were generated randomly.
static const GLfloat g_color_buffer_data[] = {
    0.583f,  0.771f,  0.014f,
    0.609f,  0.115f,  0.436f,
    0.327f,  0.483f,  0.844f,
    0.822f,  0.569f,  0.201f,
    0.435f,  0.602f,  0.223f,
    0.310f,  0.747f,  0.185f,
    0.597f,  0.770f,  0.761f,
    0.559f,  0.436f,  0.730f,
    0.359f,  0.583f,  0.152f,
    0.483f,  0.596f,  0.789f,
    0.559f,  0.861f,  0.639f,
    0.195f,  0.548f,  0.859f,
    0.014f,  0.184f,  0.576f,
    0.771f,  0.328f,  0.970f,
    0.406f,  0.615f,  0.116f,
    0.676f,  0.977f,  0.133f,
    0.971f,  0.572f,  0.833f,
    0.140f,  0.616f,  0.489f,
    0.997f,  0.513f,  0.064f,
    0.945f,  0.719f,  0.592f,
    0.543f,  0.021f,  0.978f,
    0.279f,  0.317f,  0.505f,
    0.167f,  0.620f,  0.077f,
    0.347f,  0.857f,  0.137f,
    0.055f,  0.953f,  0.042f,
    0.714f,  0.505f,  0.345f,
    0.783f,  0.290f,  0.734f,
    0.722f,  0.645f,  0.174f,
    0.302f,  0.455f,  0.848f,
    0.225f,  0.587f,  0.040f,
    0.517f,  0.713f,  0.338f,
    0.053f,  0.959f,  0.120f,
    0.393f,  0.621f,  0.362f,
    0.673f,  0.211f,  0.457f,
    0.820f,  0.883f,  0.371f,
    0.982f,  0.099f,  0.879f
};

vector<vec3> grid;

GLuint buf;
float a = 0;
float dis = 8;

glm::mat4 model_cube_mat;
glm::vec3 pos_cube = { 0 , 0 , 4 };

glm::vec3 pos_start = { 3, 2, 3};
glm::vec3 pos_end = { 0, 0 ,0 };

glm::vec2 cur_new;
glm::vec2 cur_old;


bool isGyre= false;

glm::vec3 point1 = {2, 2, 2 };

glm::vec3 point2 = {2, 2, 4};

glm::vec3 point  = {4, 2, 3};

glm::vec3 perp_p = { 0 , 0 , 0 };


float phi   = glm::radians(45.0);
float theta = glm::radians(20.0);

void DrawPoint(vec3 point)
{
    glPointSize(5.0);
    glBegin(GL_POINTS);
    {
        glVertex3f(point.x , point.y , point.z);
    }
    glEnd();
}

void DrawOxyz()
{
    glLineWidth(2.0);
    glBegin(GL_LINES);
    {
        glColor3f(1.0f, 0.0, 0.0);
        glVertex3f(0.001, 0.001 , 0.001);
        glVertex3f(4 , 0.001 , 0.001);

        glColor3f(0.0 , 1.0f, 0.0);
        glVertex3f(0.001, 0.001 , 0.001);
        glVertex3f(0.001 , 4 , 0.001);

        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(0 , 0 , 0);
        glVertex3f(0 , 0 , 4);
    }
    glEnd();
    glLineWidth(1.0);
}


void DrawLine(vec3 start_point, vec3 end_point)
{
    glBegin(GL_LINES);
    {
        glVertex3f(start_point.x , start_point.y , start_point.z);
        glVertex3f(end_point.x, end_point.y, end_point.z);
    }
    glEnd();
}

void DrawLine(vec3 start_point, vec3 ndir , float dis)
{
    vec3 end_point =start_point + ndir * dis;
    glBegin(GL_LINE);
    {
        glVertex3f(start_point.x , start_point.y , start_point.z);
        glVertex3f(end_point.x, end_point.y, end_point.z);
    }
    glEnd();
}

void DrawCube()
{
    // Draw cube
    glVertexPointer(3, GL_FLOAT, 0, g_vertex_buffer_data);
    glColorPointer(3, GL_FLOAT, 0, g_color_buffer_data);
    glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles -> 6 squares
}

void DrawGrid()
{
    // Draw grid 
    glVertexPointer(3, GL_FLOAT, 2*sizeof(vec3), &grid[0]);
    glColorPointer(3, GL_FLOAT, 2*sizeof(vec3), &grid[1]);
    glDrawArrays(GL_LINES, 0, grid.size()/2);
}

void OnCreate(Window* win)
{

    cam3d.InitView(win->GetWidth() , win->GetHeight(),0.1, 1000);
    cam3d.SetUpCamera(pos, dir, up);
    cam3d.SetFieldOfView(60.0);
    cam3d.SetUpOrbitTarget(pos, target);

    cam3d.UpdateMatrix();

    grid    = M3D_GetGrid(vec3(0, 0 ,0 ), 100 , 100 , 2, 5 );
    pos_end = M3D_GetPerpPoint2Plan(pos_start, vec3(1, 0, 0), vec3(0, 0, 1), vec3(1, 0, 1));

    perp_p = M3D_PerpPoint2Segment(point, point1, point2);

    if (win->MakeCurrentContext())
    {
        model_cube_mat = glm::translate(glm::mat4(1.0f), pos_cube);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        glShadeModel(GL_SMOOTH);

        //glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    }
}

void OnMouseMove(Window* win)
{
    int x, y;
    float phi1, theta1;
    win->GetCursorPos(x, y);
    //cout << "(" << x << ": " << y << ")" << endl;
    vec3 cam_pos = cam3d.GetPosition();

    //cout << cam_pos << endl;
    int a = 10;

    //cam3d.UpdateDir(vec3(0.01, 0.012, -0.3));
    
    //phi1 += 0.01;
    ////theta1 = /*- (cur_new.x - cur_old.x)**/0.005 + theta1;
    ////phi1   = /* - (cur_new.y - cur_old.y)*0.005 + */phi1;


    //cam3d.SetPosition(phi1, theta1, 9.8, vec3(0, 0, 0));
    if (isGyre)
    {
        win->GetCursorPos(cur_new.x , cur_new.y);


        //M3D_GetAnglePositionOxyz(cam_pos, target, phi1, theta1);
        ////cout << glm::degrees(theta1)  << "<->" <<  glm::degrees(theta) << endl;
        //cout << glm::degrees(phi1)  << "<->" << glm::degrees(phi) << endl;

        float delta_theta =   (cur_new.x - cur_old.x)*0.009;
        float delta_phi   =   (cur_new.y - cur_old.y)*0.009;

        cam3d.OrbitTarget(delta_phi, delta_theta);
        //cam3d.OrbitMove1(1, -1);
        //cam3d.OrbitMove1(- (cur_new.x - cur_old.x)*0.009, (cur_new.y - cur_old.y)*0.009);
        //cam3d.OrbitMove1(- (cur_new.x - cur_old.x), (cur_new.y - cur_old.y));
        //float temp = (cur_new.y - cur_old.y)*0.005 + phi;

        ////phi   =   (cur_new.y - cur_old.y)*0.005 + phi;
        //if (fabs(temp) <= M3_HPI)
        //{
        //    phi   =   (cur_new.y - cur_old.y)*0.005 + phi;
        //}


        //float dis_screen = glm::distance(cur_new, cur_old);

        //float radious = glm::distance(vec3(0, 0, 0) , cam_pos);
        //vec3 pp = M3D_GetPerpPoint2Plan(cam_pos, glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, 1));

        //float a = glm::angle(vec3(0, 0, 1), pp);

        //float ThetaNow = M3D_Angle2Vector(vec3(0, 0, 1), pp, 1);
        //float PhiNow = M3D_Angle2Vector(cam_pos, pp, 2);



        //phi = glm::min( 180.0f, glm::max( 0.0f, phi ) );

        //cam_pos.x = radious * glm::sin(theta * pi<float>() / 360)
        //    * glm::cos( phi * pi<float>() / 360 );
        //cam_pos.y = radious * glm::sin( phi * pi<float>() / 360 );
        //cam_pos.z = radious * glm::cos( theta * pi<float>() / 360 )
        //    * glm::cos( phi * pi<float>() / 360 );
        //cout << phi << endl;

        //if (theta > M3_2PI)
        //{
        //    int temp = theta / M3_2PI;
        //    theta = theta - temp* M3_2PI;
        //}

        //else if (theta < 0)
        //{
        //    int temp = theta / M3_2PI;
        //    theta = theta + abs(temp+1)* M3_2PI;
        //}

        //cam3d.SetPosition(phi, theta, 8, target);

        cur_old = cur_new;
    }

    //float dis = glm::distance(vec3(0, 0, 0) , cam_pos);
    //float ro = sqrtf(abs((dis*dis) - cam_pos.y* cam_pos.y));
    //cam_pos = vec3(ro*sin(a), cam_pos.y, ro*  cos(a));

    //cam3d.SetPosition(cam_pos);
    cam3d.UpdateMatrix();
}


void OnKeyBoard(Window* win)
{

    if(win->GetKeyboardStatus(KeyA))
    {
        cam3d.OrbitMove(1.2f);
    }
}


void OnDraw(Window* win)
{
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cam3d.UseMatrix();
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    //DrawLine(point1, point2);
    //DrawPoint(point);
    //DrawPoint(perp_p);

    DrawPoint(target);



    DrawGrid();
    DrawOxyz();
    //DrawLine(pos_start , pos_end);

    cam3d.UseModelMatrix(model_cube_mat);
    DrawCube();
}

void OnPaint(Window* win)
{
    WndGDIplus* graphics = win->GetGraphicsFromHWND();
    FontFamily  fontFamily(L"Times New Roman");
    Font        font(&fontFamily, 24, FontStyleRegular, UnitPixel);
    PointF      pointF(30.0f, 10.0f);
    SolidBrush  solidBrush(Color(255, 0, 0, 255));

    graphics->DrawString(L"Hello", -1, &font, pointF, &solidBrush);
    delete graphics;
}

void OnMouseButton(Window* win)
{
    isGyre = win->GetMouseButtonStatus(GLMouse::LeftButton);
    if (isGyre)
    {
        win->GetCursorPos(cur_old.x , cur_old.y);
        vec3 cam_pos = cam3d.GetPosition();
    }
}

void OnMouseScroll(Window* win)
{
    int zDelta =  win->GetMouseScroll();
    float sensitive = 1.1f;
    cam3d.ZoomTo(zDelta* sensitive);
    cam3d.UpdateMatrix();
}

int main()
{
    GLWindow window;

    WndProp adven;
    adven.m_iAntialiasing = 8;
    adven.m_bGDIplus      = true;

    Window* win = new Window("Camera" , 500 , 100);
    win->SetupAdvanced(adven);

    win->SetOnDrawfunc(OnDraw);
    win->SetOnCreatedfunc(OnCreate);
    win->SetOnMouseMovefunc(OnMouseMove);
    win->SetOnKeyboardfunc(OnKeyBoard);
    win->SetOnMouseScrollfunc(OnMouseScroll);
    win->SetOnMouseButtonfunc(OnMouseButton);
    win->SetOnPaintfunc(OnPaint);

    window.AddWindow(win);

    while (window.Closed())
    {
        window.Draw();
        window.PollEvent();
    }

}