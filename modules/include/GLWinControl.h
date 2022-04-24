﻿////////////////////////////////////////////////////////////////////////////////////
// File: WinControl.cpp   -- Create and handle event control the window             
// Copyright (C) 2020-2022 Thuong.NV   Created : 22/04/2022                         
// For conditions of distribution and use, see copyright notice in readme.txt       
////////////////////////////////////////////////////////////////////////////////////

#include <CommCtrl.h>
#include <WinUser.h>
#include <windows.h>
#include <vector>

class MenuContext;
class MenuBar;
class Window;

using namespace std;

enum CtrlType
{
    BUTTON     ,
    MENUCONTEXT,
    MENUBAR,
};

//===================================================================================
//⮟⮟ Lớp control : Lớp cơ sở triển khai các control của Window                    
//===================================================================================
class Control
{
protected:
    UINT         m_ID;
    HWND         m_hwnd;
    HWND         m_hwndPar;
    CtrlType     m_type;

    bool         m_bEnable;
    bool         m_bVisble;

public:
    Control(CtrlType type)
    { 
        m_ID   = 0;
        m_type = type; 
        m_hwnd = NULL;
    }

public:
    virtual void     OnInitControl(UINT& IDS) = 0;
    virtual CtrlType GetType()       = 0;

    virtual void     OnDestroy() {};
    virtual void     Draw() {};
    virtual void     Event(Window* window, WORD _id, WORD _event){};
    virtual bool     ContainID(INT ID) { return false; };

public:
    HWND GetHwnd()              { return m_hwnd;    }
    INT  GetID()                { return m_ID;      }
    void SetParent(HWND hwnd)   { m_hwndPar = hwnd; }
    void Enable(bool bEnable) 
    {
        m_bEnable = bEnable; 
        EnableWindow(m_hwnd, m_bEnable ? TRUE : FALSE);
    }

    void Visible(bool bVisible)
    {
        m_bVisble = bVisible; 
        ShowWindow(m_hwnd, m_bVisble ? TRUE : FALSE);
    }
};


//===================================================================================
//⮟⮟ Lớp MenuContext : Control quản lý Menucontext                                  
//===================================================================================

class MenuItemBase
{
public:
    UINT        m_ID   ;
    UINT        m_type ;
    string      m_label;

public:
    void(*m_EventFun)(Window* window, Control* ctrl) = NULL;

public:
    MenuItemBase()
    {
        m_ID    = 0;
        m_label = "";
    };

    void   SetEvent(void(*mn)(Window*, Control*)){ m_EventFun = mn; }
    void   SetLabel(string lab) { m_label = lab; }
    void   SetType (UINT type)  { m_type  = type;}
    void   SetID   (UINT id)    { m_ID    = id;  }

    string GetLabel() { return m_label; }
    UINT   GetType()  { return m_type;  }
    UINT   GetID()    { return m_ID;    }
};

class MenuContext : public Control
{
protected:
    HMENU                    m_hMenu;
    vector<MenuItemBase>     m_items;

public:
    MenuContext() : Control(CtrlType::MENUCONTEXT)
    {

    }

private:
    bool CreateMenuContext(UINT& IDS)
    {
        m_hMenu = CreatePopupMenu();

        for (int i = 0; i < m_items.size(); i++)
        {
            m_items[i].SetID(IDS++);

            if (m_items[i].GetType() == MF_SEPARATOR)
            {
                AppendMenu(m_hMenu, MF_SEPARATOR, m_items[i].GetID(), NULL);
            }
            else
            {
                AppendMenu(m_hMenu, m_items[i].GetType(), m_items[i].GetID(), m_items[i].GetLabel().c_str());
            }
        }

        return true;
    }

public:
    virtual CtrlType GetType() { return m_type; };

    virtual void OnInitControl(UINT& IDS)
    {
        if (CreateMenuContext(IDS))
        {

        }
    }

    virtual void Event(Window* window, WORD _id, WORD _event)
    {
        for (int i = 0; i < m_items.size(); i++)
        {
            if (m_items[i].m_ID == _id && m_items[i].m_EventFun)
            {
                m_items[i].m_EventFun(window, this);
                break;
            }
        }
    }

    bool ContainID(INT ID)
    {
        for (int i = 0; i < m_items.size(); i++)
        {
            if (m_items[i].m_ID == ID) return true;
        }
        return false;
    }

public:
    void AddItem(MenuItemBase item)
    {
        m_items.push_back(item);
    }

    void Show(POINT point)
    {
        if (m_hwndPar)
        {
            ClientToScreen(m_hwndPar, &point);
            TrackPopupMenu(m_hMenu, TPM_RIGHTBUTTON, point.x, point.y, 0, m_hwndPar, NULL);
        }
    }

    friend class Window;
};

//===================================================================================
//⮟⮟ Lớp MenuBarContext : Control quản lý MenuBarContext                            
//===================================================================================

class MenuBarItem
{
protected:

    HMENU                  m_hMenu;
    string                 m_text;

    vector<MenuItemBase>   m_items;

public:
    MenuBarItem(string lab = "") : m_text(lab)
    {

    }

private:

    bool CreateMenuBarItem(UINT& IDS)
    {
        m_hMenu  = CreateMenu();
        for (int i = 0; i < m_items.size(); i++)
        {
            m_items[i].SetID(IDS++);

            if (m_items[i].GetType() == MF_SEPARATOR)
            {
                AppendMenu(m_hMenu, MF_SEPARATOR, m_items[i].GetID(), NULL);
            }
            else
            {
                AppendMenu(m_hMenu, m_items[i].GetType(), m_items[i].GetID(), m_items[i].GetLabel().c_str());
            }
        }


        return true;
    }

public:

    void AddItem(MenuItemBase item)
    {
        m_items.push_back(item);
    }

    void SetText(string txt)
    {
        m_text = txt;
    }

    bool ContainID(INT ID)
    {
        for (int i = 0; i < m_items.size(); i++)
        {
            if (m_items[i].m_ID == ID) return true;
        }
        return false;
    }

    bool CallEvent(Window* win, Control* ctrl, INT ID)
    {
        for (int i = 0; i < m_items.size(); i++)
        {
            if (m_items[i].m_ID == ID)
            {
                m_items[i].m_EventFun(win, ctrl);
                return true;
            }
        }
        return false;
    }

    friend class MenuBar;
};



class MenuBar : public Control
{
public:
    HMENU               m_hMenuBar;
    vector<MenuBarItem> m_items;

public:
    MenuBar() : Control(CtrlType::MENUBAR)
    {

    }

protected:

    bool CreateMenuBar(UINT& IDS)
    {
        m_hMenuBar = CreateMenu();

        for (int i = 0; i < m_items.size(); i++) 
        {
            if (m_items[i].CreateMenuBarItem(IDS))
            {
                // TODO : Created menubar ok
                AppendMenu(m_hMenuBar, MF_POPUP, (UINT_PTR)m_items[i].m_hMenu, m_items[i].m_text.c_str());
                SetMenu(m_hwndPar, m_hMenuBar);
            }
            else
            {
                cout << "[X]  Create Menubar failed ! " << endl;
            }
        }
        return true;
    }

public:
    virtual CtrlType GetType() { return m_type; };

    virtual void OnInitControl(UINT& IDS)
    {
        if (CreateMenuBar(IDS))
        {

        }
    }

public:

    void AddItem(MenuBarItem item)
    {
        m_items.push_back(item);
    }

    virtual void Event(Window* window, WORD _id, WORD _event)
    {
        for (int i = 0; i < m_items.size(); i++)
        {
            if (m_items[i].CallEvent(window, this, _id))
                return ;
        }
    }

    bool ContainID(INT ID)
    {
        for (int i = 0; i < m_items.size(); i++)
        {
            if (m_items[i].ContainID(ID)) 
                return true;
        }
        return false;
    }
};


//===================================================================================
//⮟⮟ Lớp Button : Control quản lý giao diện và sự kiện Button                       
//===================================================================================


class Button : public Control
{
protected:
    int         m_x     ;
    int         m_y     ;
    UINT        m_width ;
    UINT        m_height;

    string      m_label ;
    bool        m_bClicked;

    void       (*m_EventFun)(Window* window, Button* btn) =NULL;
public:
    Button() : Control(CtrlType::BUTTON), m_width(0), m_height(0)
    {
    }

public:
    void SetPosition(int x, int y)
    {
        m_x = x;
        m_y = y;
    }
    void SetSize(int width, int height)
    {
        m_width  = width;
        m_height = height;
    }
    void SetLabel(string lab) { m_label = lab; }

public:
    void OnInitControl(UINT& IDS)
    {
        UINT BackupIDS = IDS;
        m_ID = IDS++;
        m_hwnd = (HWND)CreateWindow("BUTTON", m_label.c_str(),                              // Button text 
                                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
                                    m_x,                                                    // x position 
                                    m_y,                                                    // y position 
                                    m_width,                                                // Button width
                                    m_height,                                               // Button height
                                    m_hwndPar,                                              // Parent window
                                    (HMENU)m_ID,                                            // menu.
                                    (HINSTANCE)GetWindowLongPtr(m_hwndPar, GWLP_HINSTANCE),
                                    NULL);
        if (!m_hwnd)  // Hoàn trả nếu nó khởi tạo không thành công
        {
            m_ID = 0;
            IDS  = BackupIDS;
        }
    }

    virtual void Event(Window* window, WORD _id, WORD _event)
    {
        m_bClicked = true;
        m_EventFun(window, this);
    }

    void SetEvent(void(*mn)(Window*, Button*))
    {
        m_EventFun = mn;
    }

    void Draw()
    {
        UpdateWindow(m_hwnd);
    }

    virtual CtrlType GetType() { return m_type; };
};