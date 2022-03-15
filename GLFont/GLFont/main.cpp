#include <GLWindow.h>
#include "GL3DFun.h"
#include "GLUtils.h"
#include "GLCamera.h"
#include "GLFont.h"
#include <codecvt>
#include "GLRenderer.h"


GLCamera2D    cam2d;
GLFontManager font;
GLRenderer    render;

glm::vec3 pos = {0, 0, 8};
glm::vec3 dir = {0, 0, 1};
glm::vec3 up  = {0, 1, 0};


glm::vec3 point1 = {  30 , 100 , 10 };
glm::vec3 point2 = {  30 , -100 , 10 };
glm::vec3 point3 = { -80 ,-60 , 10 };

vector<vec3> strbuf;
vector<vec3> grid;

glm::mat4 trans = glm::mat4(1.0f);

float roate = 0;
bool  bMove = false;
float cellwidth = 100.f;
vec2  pCursorOld;
vec2  pCursor;


void Onkeyboard(Window* win)
{
    if (win->GetKeyboardStatus(KeyA))
    {
        point1.z = point2.z = point3.z += 0.1;
    }
    else if (win->GetKeyboardStatus(KeyD))
    {
        point1.z = point2.z = point3.z -= 0.1;
    }
}

void OnCreate(Window* win)
{
    cam2d.InitView(win->GetWidth(), win->GetHeight(), 100.0, -1000);
    cam2d.SetUpCamera(pos, dir, up);
    cam2d.UpdateMatrix();

    string path = "fonts/arial.ttf";

    font.LoadFont("ARIAL", path.c_str(), 12, FontType::FTX_Polygon);

    //vector<vec3> poly = { {-100, 100, 10},  {-100, -100, 10}, {100, -100, 10}, {79, 30, 10}, {-29, 29, 10} };

    //grid = M2D_GetGridData(pos, win->GetWidth(), win->GetHeight(), cam2d.GetZoom(), cellwidth, 10, GL_WIH_COL);

    //for (int i = 0; i < grid.size(); i+=4)
    //{
    //    render.AddLine(grid[i], grid[i+1], grid[i+2], grid[i+3]);
    //}

    //render.AddPoly(poly, GL_WIH_COL);
    //render.AddLine(point1, point2, GL_RED_COL, GL_BLU_COL);

    //render.AddText("2020.12.14 N.V.Thuong", {20, 20,10}, GL_WIH_COL,0,font.GetFont("ARIAL"));

    render.UpdateRender();
}

void OnMouseScroll(Window* win)
{
    int delta = win->GetMouseScroll();

    vec2 pCursor;
    win->GetCursorPos(pCursor.x , pCursor.y);
    cam2d.ZoomTo(pCursor.x, pCursor.y, float(delta / 10.0f));
    //cout << delta << " -> " << cam2d.GetZoom() << endl;
    cam2d.UpdateMatrix();

    //cellwidth = M2D_CalCellWidth(cam2d.GetPosition(), win->GetWidth(), win->GetHeight(), cam2d.GetZoom(), cellwidth);
    //grid      = M2D_GetGridData(cam2d.GetPosition(), win->GetWidth(), win->GetHeight(), cam2d.GetZoom(), cellwidth, 10, GL_WIH_COL);

    //render.Clear();
    //for (int i = 0; i < grid.size(); i+=5)
    //{
    //    render.AddLine(grid[i], grid[i+1], grid[i+2], grid[i+3]);
    //    render.AddText(std::to_string(grid[i].x), grid[i+4], GL_WIH_COL, 0, font.GetFont("ARIAL", 12));
    //}
    //render.UpdateRender();
}
void OnResize(Window* win)
{
    cam2d.SetViewSize(win->GetWidth(), win->GetHeight());
}

void OnMouseMove(Window* win)
{
    win->GetCursorPos(pCursor.x, pCursor.y);
    //vec2 pos = cam2d.ConvertLeftTop2Center(pCursor.x, pCursor.y);
    //cout << pos << endl;

    if (bMove)
    {
        render.Clear();

        float deltax = pCursor.x - pCursorOld.x;
        float deltay = pCursor.y - pCursorOld.y;

        cam2d.Move(deltax, deltay);

        pCursorOld = pCursor;
        cam2d.UpdateMatrix();

        //grid = M2D_GetGridData(cam2d.GetPosition(), win->GetWidth(), win->GetHeight(), cam2d.GetZoom(), cellwidth, 10, GL_WIH_COL);

        //for (int i = 0; i < grid.size(); i+=4)
        //{
        //    render.AddLine(grid[i], grid[i+1], grid[i+2], grid[i+3]);
        //}
        //render.UpdateRender();

    }
}

void OnButton(Window* win)
{
    bMove = win->GetMouseButtonStatus(LeftButton);
    if(bMove)
    {
        win->GetCursorPos(pCursorOld.x, pCursorOld.y);
    }
}


void OnDraw(Window* win)
{
    glClearColor(1.0, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render.Clear();
    render.UseCamera(&cam2d);
    render.AddLine({ 100 ,0 , 10 }, { -100 ,0 , 10 }, GL_BLA_COL, GL_BLA_COL);
    render.AddLine({ 0   ,100 , 10 }, {0, -100 , 10 }, GL_BLA_COL, GL_BLA_COL);

    cellwidth = M2D_CalCellWidth(cam2d.GetPosition(), win->GetWidth(), win->GetHeight(), cam2d.GetZoom(), cellwidth);
    grid      = M2D_GetGridData(cam2d.GetPosition(), win->GetWidth(), win->GetHeight(), cam2d.GetZoom(), cellwidth, 10, NormalColor(192, 192, 192));
    for (int i = 0; i < grid.size(); i+=6)
    {
        render.AddLine(grid[i], grid[i+1], grid[i+2], grid[i+3]);

        if (grid[i+5].x == 1.0f)
        {
            render.AddText(Number2String(grid[i].x, 2), grid[i+4], GL_BLA_COL, 0, font.GetFont("ARIAL", 12), false);
        }
        else
        {
            render.AddText(Number2String(grid[i].y, 2), grid[i+4], GL_BLA_COL, 0, font.GetFont("ARIAL", 12), false);
        }
    }
    //vector<vec3> poly = { {-100, 100, 10},  {-100, -100, 10}, {100, -100, 10}, {79, 30, 10}, {-29, 29, 10} };
    //render.AddPoly(poly, GL_RED_COL, false);

    render.UpdateRender();
    render.Draw();
}

int main()
{
    GLWindow window;

    WndProp adven;
    adven.m_iAntialiasing = 8;

    Window* win = new Window("Font" , 500 , 100, 900, 580);
    win->SetupAdvanced(adven);

    win->SetOnDrawfunc(OnDraw);
    win->SetOnCreatedfunc(OnCreate);
    win->SetOnKeyboardfunc(Onkeyboard);
    win->SetOnMouseScrollfunc(OnMouseScroll);
    win->SetOnMouseMovefunc(OnMouseMove);
    win->SetOnResizefunc(OnResize);
    win->SetOnMouseButtonfunc(OnButton);

    window.AddWindow(win);

    while (window.Closed())
    {
        window.Draw();
        window.PollEvent();
    }
}


