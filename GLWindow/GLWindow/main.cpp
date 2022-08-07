#include <GLWindow.h>


bool bfirst = true;
glm::mat4   matView;
glm::mat4   matProject;
float zoom = 1.0f;

MenuContext* menucontext;

void OpenFile(Window* win, Control* mn)
{
    MessageBox(NULL,"OpenFile", "Thong bao", MB_ICONWARNING);
}

void SaveFile(Window* win, Control* mn)
{
    MessageBox(NULL,"SaveFile", "Thong bao", MB_ICONWARNING);
}

void OnButton(Window* win, Control* btn)
{
    MessageBox(NULL,"ON button", "Thong bao", MB_ICONWARNING);
}


void Create(Window* win)
{
    matView = glm::lookAt(glm::vec3(0 ,0 , 8), glm::vec3(0 ,0 , 8) + glm::vec3(0 ,0 , -1), glm::vec3(0.f, 1.f, 0.f));

    int width  = win->GetWidth();
    int height = win->GetHeight();

    GLdouble left   = -GLdouble(width / 2) / zoom;
    GLdouble right  =  GLdouble(width / 2) / zoom;
    GLdouble top    = -GLdouble(height / 2)/ zoom;
    GLdouble bottom =  GLdouble(height / 2)/ zoom;
    glViewport(0, 0, width, height);
    matProject = glm::ortho(left, right, top, bottom, double(0.1), double(-1000));

    menucontext = new MenuContext();
    MenuItemBase item;
    item.SetLabel("Open file");
    item.SetType (MF_STRING);
    item.SetEvent(OpenFile);
    menucontext->AddItem(item);

    item.SetType (MF_SEPARATOR);
    menucontext->AddItem(item);

    item.SetLabel("Save as...");
    item.SetType (MF_STRING);
    item.SetEvent(SaveFile);
    menucontext->AddItem(item);


    //Button* btn = new Button();
    //btn->SetLabel("Click button");
    //btn->SetPosition(10, 10);
    //btn->SetSize(100, 50);
    //btn->SetEvent(OnButton);

    MenuBar* menubar = new MenuBar();

    MenuBarItem menubaritem("File");

    item.SetLabel("Open file");
    item.SetType(MF_STRING);
    item.SetEvent(OpenFile);
    menubaritem.AddItem(item);

    item.SetType(MF_SEPARATOR);
    menubaritem.AddItem(item);

    item.SetLabel("Save file");
    item.SetType(MF_STRING);
    item.SetEvent(OpenFile);
    menubaritem.AddItem(item);

    menubar->AddItem(menubaritem);

    menubaritem.SetText("Edit");

    item.SetLabel("Create connect");
    item.SetType(MF_STRING);
    item.SetEvent(OpenFile);
    menubaritem.AddItem(item);

    item.SetType(MF_SEPARATOR);
    menubaritem.AddItem(item);

    item.SetLabel("Save file");
    item.SetType(MF_STRING);
    item.SetEvent(OpenFile);
    menubaritem.AddItem(item);

    menubar->AddItem(menubaritem);

    Button* button = new Button();
    button->SetLabel("Click");
    button->SetPosition(100, 100);

    win->AddControl(button);
    win->AddControl(menucontext);
    win->AddControl(menubar);
}

void MouseButton(Window* win)
{
    if (win->GetMouseButtonStatus(GLMouse::LeftButton))
    {
        zoom += 0.2f;

        matView = glm::lookAt(glm::vec3(0 ,0 , 8), glm::vec3(0 ,0 , 8) + glm::vec3(0 ,0 , -1), glm::vec3(0.f, 1.f, 0.f));

        int width  = win->GetWidth();
        int height = win->GetHeight();

        GLdouble left   = -GLdouble(width / 2) / zoom;
        GLdouble right  =  GLdouble(width / 2) / zoom;
        GLdouble top    = -GLdouble(height / 2)/ zoom;
        GLdouble bottom =  GLdouble(height / 2)/ zoom;
        glViewport(0, 0, width, height);
        matProject = glm::ortho(left, right, top, bottom, double(0.1), double(-1000));
    }
}

void Keyboard(Window* win)
{
    if (win->GetKeyboardStatus(GLKeyboard::Escapex))
    {
        win->ExitFullScreen();
    }

    else if (win->GetKeyboardStatus(GLKeyboard::KeyA))
    {
        menucontext->Show(POINT{ 0 ,0 });
    }
}


void Draw(Window* win)
{
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(matProject));
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(matView));

    glBegin(GL_TRIANGLES);

    glColor3f(0.5,1.0,0);

    glVertex3f(100.0,100.0 ,100.0);
    glVertex3f(0.0  ,100.0 ,100.0);
    glVertex3f(100.0,0.0   ,100.0);

    glEnd();
}

void Resize(Window* win)
{
    int width       = win->GetWidth();
    int height      = win->GetHeight();
    GLdouble left   = -GLdouble(width / 2) / zoom;
    GLdouble right  =  GLdouble(width / 2) / zoom;
    GLdouble top    = -GLdouble(height / 2)/ zoom;
    GLdouble bottom =  GLdouble(height / 2)/ zoom;
    matProject = glm::ortho(left, right, top, bottom, double(0.1), double(-1000));
}

void Process(Window * pWin)
{
}

int main()
{
    GLWindow window;

    Window *win1 = new Window("OpenGL", 100 ,100);
    win1->SetOnDrawfunc(Draw);
    win1->SetOnCreatedfunc(Create);
    win1->SetOnMouseButtonfunc(MouseButton);
    win1->SetProcessfunc(Process);
    win1->SetOnKeyboardfunc(Keyboard);
    win1->SetOnResizefunc(Resize);

    window.AddWindow(win1);

    while (window.Closed())
    {
        window.Process();

        window.Draw();
        window.PollEvent();
    }
}
