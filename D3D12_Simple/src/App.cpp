//-------------------------------------------------------------------------------------------------
// File : App.cpp
// Desc : Application Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <App.h>


///////////////////////////////////////////////////////////////////////////////////////////////////
// App class
///////////////////////////////////////////////////////////////////////////////////////////////////

App::App()
{
}

App::~App()
{
}

bool App::InitApp()
{
    return true;
}


bool App::InitWnd()
{
    return true;
}

bool App::InitD3D()
{
    return true;
}

void App::TermApp()
{
}

void App::TermWnd()
{
}

void App::TermD3D()
{
}

void App::MainLoop()
{
}

void App::Run()
{
    if ( InitApp() )
    {
        MainLoop();
    }

    TermApp();
}

bool App::OnInit()
{
    return true;
}

void App::OnTerm()
{
}

void App::OnFrameMove()
{
}

void App::OnFrameRender()
{
}

void App::OnResize()
{
}


LRESULT CALLBACK App::MsgProc( HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp )
{
    return DefWindowProc( hWnd, uMsg, wp, lp );
}