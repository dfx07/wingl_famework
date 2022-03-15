#include <GLWindow.h>
#include <GLFont.h>
#include <GLCamera.h>
#include <GLRenderer.h>
#include <GLShader.h>
#include <GLResource.h>


GLCamera2D    cam2d;
GLFontManager font;
GLRenderer    render;
GLShader      shader;

GLuint imgID  = 0;
GLuint imgID2 = 0;


GLTextureManager  textures;

unsigned int VBO;
unsigned int VAO;


float img_w = 0;
float img_h = 0;

int* buffwater;
int* buffwater2;

int maxbuff = 0;

glm::vec3 pos = { 0, 0, 0 };
glm::vec3 dir = { 0, 0, 1 };
glm::vec3 up = { 0, 1, 0 };

float vertices[] = {
   -100.f,  100.f, 0.0f, 0.0f, 0.0f,
   -100.f, -100.f, 0.0f, 0.0f, 1.0f,
    100.f,  100.f, 0.0f, 1.0f, 0.0f,
    100.f,  100.f, 0.0f, 1.0f, 0.0f,
   -100.f, -100.f, 0.0f, 0.0f, 1.0f,
    100.f, -100.f, 0.0f, 1.0f, 1.0f
};

void OnCreate(Window* win)
{
    cam2d.InitView(win->GetWidth(), win->GetHeight(), 0.1, -1000);
    cam2d.SetUpCamera(pos, dir, up);
    cam2d.UpdateMatrix();

    GLTextureOption op;
    op.m_format = GL_RGBA;
    textures.CreateTexture("minion", "minion.png", op);
    textures.CreateTexture("wall"  , "wall.png", op);

    imgID  = textures.Get("minion")->GetTextureID();
    imgID2 = textures.Get("wall")->GetTextureID();

    img_w = textures.Get("minion")->GetImageWidth();
    img_h = textures.Get("minion")->GetImageHeight();

    maxbuff = img_w* img_h;

    buffwater  = new int[maxbuff]{0};
    buffwater2 = new int[maxbuff]{0};

    win->MakeContext();
    {
        shader.Create("vertex.glsl", "frag.glsl");

        glGenBuffers(1, &VBO);
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, imgID);

            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, imgID2);

            // 1. copy our vertices array in a buffer for OpenGL to use
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            // 2. then set our vertex attributes pointers
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // 3. set texture coordinate attributes pointers
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        glBindVertexArray(0);
    }
}

void OnDestroy(Window* win)
{

}

int GetAt(int*& buff, int x, int y)
{
    int index = y* img_w + x;
    if (index >= 0 && index < maxbuff)
    {
        return buff[index];
    }
    return 0;
}

void SetAt(int*& buff, int x, int y, int _value)
{
    int index = y* img_w + x;
    if (index >= 0 && index < maxbuff)
    {
        buff[index] = _value;
    }
}

void OnProcess(Window* win)
{
    //for (int i = 0; i < 3; i++)
    //{
    //    int index = rand() % (maxbuff);
    //    buffwater[index] += 200 + 1;
    //}

    //int temp = 0;
    //int dumpv2 = 8;
    //for (int i = 1; i < img_w - 1; i++)
    //{
    //    for (int j = 1; j< img_h - 1; j++)
    //    {
    //        //buffer2[i][j] = ((buffer1[i][j - 1] + buffer1[i][j + 1] + buffer1[i - 1][j] + buffer1[i + 1][j]) >> 1) - buffer2[i][j];
    //        //buffer2[i][j] -= buffer2[i][j] >> dumpv2;

    //        temp = GetAt(buffwater, i, j - 1) + GetAt(buffwater, i, j + 1) + GetAt(buffwater, i - 1, j) + GetAt(buffwater, i + 1, j);
    //        temp = (temp >> 1) - GetAt(buffwater2, i, j);
    //        temp -= temp >> dumpv2;
    //        SetAt(buffwater2, i, j, temp);
    //    }
    //}
    //int* temptr = buffwater;
    //buffwater = buffwater2;
    //buffwater2 = temptr;
}

void Onkeyboard(Window* win)
{
    if (win->GetKeyboardStatus(KeyA))
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
        shader.SetUniformValue("mv_matrix"  , cam2d.GetViewMatrix());
        shader.SetUniformValue("proj_matrix", cam2d.GetProjectionMatrix());

        shader.SetUniformValue("texture1", 0);
        shader.SetUniformValue("texture2", 1);

        shader.SetUniformValue("width",  img_w);
        shader.SetUniformValue("height", img_h);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, (sizeof(vertices)/(sizeof(*vertices)))/5);
    }
    shader.End();
}
void OnFree()
{
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    delete[] buffwater;
    delete[] buffwater2;
}


int main()
{
    GLWindow window;

    WndProp adven;
    adven.m_iAntialiasing = 8;
    //adven.m_bFullScreen   = true;

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


    //Window* win1 = new Window("GLWindow1",800, 100, 900, 580);

    //win1->SetupAdvanced(adven);

    //win1->SetProcessfunc(OnProcess);
    //win1->SetOnDrawfunc(OnDraw);
    //win1->SetOnCreatedfunc(OnCreate);
    //win1->SetOnDestroyfunc(OnDestroy);
    //win1->SetOnKeyboardfunc(Onkeyboard);
    //win1->SetOnMouseScrollfunc(OnMouseScroll);
    //win1->SetOnMouseMovefunc(OnMouseMove);
    //win1->SetOnResizefunc(OnResize);
    //win1->SetOnMouseButtonfunc(OnButton);


    window.AddWindow(win);
    //window.AddWindow(win1);

    while (window.Closed())
    {
        window.Process();
        window.Draw();
        window.PollEvent();
    }

    OnFree();
}
