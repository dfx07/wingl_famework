#include <GLWindow.h>
#include <GLFont.h>
#include <GLCamera.h>
#include <GLRenderer.h>
#include <GLShader.h>


GLCamera2D    cam2d;
GLFontManager font;
GLRenderer    render;
GLShader      shader;

unsigned int VBO;
unsigned int VAO;

glm::vec3 pos = { 0, 0, 0 };
glm::vec3 dir = { 0, 0, 1 };
glm::vec3 up  = { 0, 1, 0 };


//float vertices[] = {
//    -0.5f, -0.5f, 0.0f,
//    0.5f, -0.5f, 0.0f,
//    0.0f,  0.5f, 0.0f
//};

float vertices[] = {
    -100.f, -100.f, 0.0f,
     100.f, -100.f, 0.0f,
      0.0f,  100.f, 0.0f
};

void OnCreate(Window* win)
{
    cam2d.InitView(win->GetWidth(), win->GetHeight(), 0.1, -1000);
    cam2d.SetUpCamera(pos, dir, up);
    cam2d.UpdateMatrix();

    win->MakeCurrentContext();
    {
        shader.Create("vertex.glsl", "frag.glsl");

        glGenBuffers(1, &VBO);

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        {
            // 2. copy our vertices array in a buffer for OpenGL to use
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            // 3. then set our vertex attributes pointers
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        }
        glBindVertexArray(0);
    }
}

void OnDestroy(Window* win)
{

}
void OnProcess(Window* win)
{

}

void Onkeyboard(Window* win)
{
    if (win->GetKeyboardStatus(Escapex))
    {
        win->ExitFullScreen();
    }
    else if (win->GetKeyboardStatus(KeyD))
    {
    }
}

void OnMouseScroll(Window* win)
{
    int delta = win->GetMouseScroll();
    float x, y;
    win->GetCursorPos(x, y);
    cam2d.ZoomTo(x, y, delta);
    cam2d.UpdateMatrix();
}

void OnResize(Window* win)
{
    cam2d.SetViewSize(win->GetWidth(), win->GetHeight());
}

void OnMouseMove(Window* win)
{

}



void OnButton(Window* win)
{
}


void OnDraw(Window* win)
{
    glClearColor(0.0, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.Begin();
    {
        cam2d.LoadMatrix(shader.GetProgram());

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    shader.End();
}

int main()
{
    GLWindow window;

    WndProp adven;
    adven.m_iAntialiasing = 8;
    adven.m_bFullScreen   = true;

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
    win->SetOnMouseButtonfunc(OnButton);

    window.AddWindow(win);

    while (window.Closed())
    {
        window.Process();
        window.Draw();
        window.PollEvent();
    }
}
