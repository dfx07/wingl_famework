#include <GLWindow.h>
#include <GLCamera.h>
#include <GLWinFont.h>


GLCamera2D    cam2d;

GLWinFontRender textrender;

glm::vec3 pos = { 0, 0, 8 };
glm::vec3 dir = { 0, 0, 1 };
glm::vec3 up = { 0, 1, 0 };

void OnCreate(Window* win)
{
    cam2d.InitView(win->GetWidth(), win->GetHeight(), 100.0, -1000);
    cam2d.SetUpCamera(pos, dir, up);
    cam2d.UpdateMatrix();

    textrender.Init(win->GetHDC(), win->GetWidth(), win->GetHeight());
    textrender.LoadFont("consolas", 18, GLWinFontRender::FontType::Bold);
}

void OnDestroy(Window* win)
{

}
void OnProcess(Window* win)
{
    double delta = win->GetFrameTime();


}

void Onkeyboard(Window* win)
{
    if (win->GetKeyboardStatus(KeyA))
    {
    }
    else if (win->GetKeyboardStatus(KeyD))
    {
    }
}

void OnMouseScroll(Window* win)
{
    int delta = win->GetMouseScroll();
}

void OnResize(Window* win)
{
    cam2d.SetViewSize(win->GetWidth(), win->GetHeight());
    cam2d.UpdateMatrix();
    textrender.UpdateView(win->GetWidth(), win->GetHeight());
}

void OnMouseMove(Window* win)
{

}

void OnMouseButton(Window* win)
{

}


void OnDraw(Window* win)
{
    glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    char fpsStr[80];
    sprintf(&fpsStr[0], "fps: %d - frametime : %f ms", win->GetFPS(), win->GetFrameTime());

    textrender.Use();
    textrender.Write(10, 20, fpsStr);
    textrender.DontUse();

    // Example : Unicode character
    //const wchar_t* wtext = L"thuong☑";
    //textrender.Use();
    //textrender.WriteEach(10, 10, wtext);
    //textrender.DontUse();
}

int main()
{
    GLWindow window;

    WndProp adven;
    adven.m_iAntialiasing = 8;

    Window* win = new Window("GLWindow", 500, 100, 900, 580);
    win->SetupAdvanced(adven);

    win->SetProcessfunc(OnProcess);
    win->SetOnDrawfunc(OnDraw);
    win->SetOnCreatedfunc(OnCreate);
    win->SetOnDestroyfunc(OnDestroy);
    win->SetOnKeyboardfunc(Onkeyboard);
    win->SetOnMouseScrollfunc(OnMouseScroll);
    win->SetOnMouseMovefunc(OnMouseMove);
    win->SetOnResizefunc(OnResize);
    win->SetOnMouseButtonfunc(OnMouseButton);

    window.AddWindow(win);

    while (window.Closed())
    {
        window.Process();
        window.Draw();
        window.PollEvent();
    }
}