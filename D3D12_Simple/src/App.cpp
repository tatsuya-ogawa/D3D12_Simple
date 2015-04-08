//-------------------------------------------------------------------------------------------------
// File : App.cpp
// Desc : Application Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <App.h>
#include <cstdio>
#include <array>

#ifndef DLOG
#if defined(DEBUG) || defined(_DEBUG)
#define DLOG(x, ...)       printf_s("[File: %s, Line:%d] "x"\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define DLOG(x, ...)        ((void)0)
#endif
#endif//DLOG

#ifndef ELOG
#define ELOG(x, ...)        fprintf_s(stderr, "[File:%s, Line:%d]"x"\n", __FILE__, __LINE__, ##__VA_ARGS__ )
#endif//ELOG

#ifndef ASDX_WND_CLASSNAME
#define ASDX_WND_CLASSNAME      TEXT("asdxWindowClass")
#endif//ASDX_WND_CLASSNAME


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
    HRESULT hr = CoInitialize( nullptr );
    if ( FAILED( hr ) )
    {
        ELOG( "Error : Com Library Initialize Failed." );
        return false;
    }

    if ( !InitWnd() )
    {
        ELOG( "Error : InitWnd() Failed." );
        return false;
    }

    if ( !InitD3D() )
    {
        ELOG( "Error : InitD3D() Failed." );
        return false;
    }

    if ( !OnInit() )
    {
        ELOG( "Error : OnInit() Failed." );
        return false;
    }

    return true;
}


bool App::InitWnd()
{
    HINSTANCE hInst = GetModuleHandle( nullptr );
    if ( !hInst )
    {
        ELOG( "Error : GetModuleHandle() Failed." );
        return false;
    }

    WNDCLASSEXW wc;
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MsgProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInst;
    wc.hIcon = LoadIcon( hInst, IDI_APPLICATION );
    wc.hCursor = LoadCursor( nullptr, IDC_ARROW );
    wc.hbrBackground = (HBRUSH)( COLOR_WINDOW + 1 );
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = ASDX_WND_CLASSNAME;
    wc.hIconSm = LoadIcon( hInst, IDI_APPLICATION );

    if ( !RegisterClassExW( &wc ) )
    {
        ELOG( "Error : RegisterClassEx() Failed." );
        return false;
    }

    m_hInst = hInst;

    RECT rc = { 0, 0, 960, 540 };

    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
    AdjustWindowRect( &rc, style, FALSE );

    m_hWnd = CreateWindowW(
        ASDX_WND_CLASSNAME,
        TEXT("SimpleSample"),
        style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        (rc.right - rc.left),
        (rc.bottom - rc.top),
        nullptr,
        nullptr,
        m_hInst,
        nullptr );

    if ( !m_hWnd )
    {
        ELOG( "Error : CreateWindowW() Failed." );
        return false;
    }

    ShowWindow( m_hWnd, SW_SHOWNORMAL );
    UpdateWindow( m_hWnd );
    SetFocus( m_hWnd );

    return true;
}

bool App::InitD3D()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect( m_hWnd, &rc );
    u32 w = rc.right - rc.left;
    u32 h = rc.bottom - rc.top;

    auto createDeviceFlags = D3D12_CREATE_DEVICE_NONE;
#if defined(DEBUG) || defined(_DEBUG)
    createDeviceFlags |= D3D12_CREATE_DEVICE_DEBUG;
#endif

    std::array<D3D_DRIVER_TYPE, 4> driverTypes = { 
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
        D3D_DRIVER_TYPE_SOFTWARE
    };

    for( size_t i=0; i<driverTypes.size(); ++i )
    {
        hr = D3D12CreateDevice(
            nullptr,
            driverTypes[i],
            createDeviceFlags,
            D3D_FEATURE_LEVEL_9_1,
            D3D12_SDK_VERSION,
            IID_ID3D12Device,
            (void**)m_Device.GetAddress() );

        if ( SUCCEEDED( hr ) )
        {
            break;
        }
    }

    if ( FAILED( hr ) )
    {
        ELOG( "Error : D3D12CreateDevice()Failed." );
        return false;
    }

    hr = m_Device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT, 
        IID_ID3D12CommandAllocator,
        (void**)m_CmdAllocator.GetAddress() );
    if ( FAILED( hr ) )
    {
        ELOG( "Error : ID3D12Device::CreateCommandAllocator() Failed." );
        return false;
    }

    {
       D3D12_COMMAND_QUEUE_DESC desc;
       ZeroMemory( &desc, sizeof(desc) );
       desc.Type        = D3D12_COMMAND_LIST_TYPE_DIRECT;
       desc.Priority    = 0;
       desc.Flags       = D3D12_COMMAND_QUEUE_NONE;

       hr = m_Device->CreateCommandQueue( &desc, IID_ID3D12CommandQueue, (void**)m_CmdQueue.GetAddress() );
       if ( FAILED( hr ) )
       {
           ELOG( "Error : ID3D12Device::CreateCommandQueue() Failed." );
           return false;
       }
    }

    {
        hr = CreateDXGIFactory( IID_IDXGIFactory, (void**)m_Factory.GetAddress() );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : CreateDXGIFactory() Failed." );
            return false;
        }

        DXGI_SWAP_CHAIN_DESC desc;
        ZeroMemory( &desc, sizeof(desc) );
        desc.BufferCount                        = 2;
        desc.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        desc.BufferDesc.Width                   = w;
        desc.BufferDesc.Height                  = h;
        desc.BufferDesc.RefreshRate.Numerator   = 60;
        desc.BufferDesc.RefreshRate.Denominator = 1;
        desc.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
        desc.OutputWindow                       = m_hWnd;
        desc.SampleDesc.Count                   = 1;
        desc.SampleDesc.Quality                 = 0;
        desc.Windowed                           = TRUE;

        hr = m_Factory->CreateSwapChain( m_CmdQueue.GetPtr(), &desc, m_SwapChain.GetAddress() );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : IDXGIFactory::CreateSwapChain() Failed." );
            return false;
        }
    }

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc;
        ZeroMemory( &desc, sizeof(desc) );

        desc.NumDescriptors = 2;
        desc.Type           = D3D12_RTV_DESCRIPTOR_HEAP;
        desc.Flags          = D3D12_DESCRIPTOR_HEAP_NONE;

        hr = m_Device->CreateDescriptorHeap( &desc, IID_ID3D12DescriptorHeap, (void**)m_DescriptorHeap.GetAddress() );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D12Device::CreateDescriptorHeap() Failed." );
            return false;
        }
    }

    {
        auto rootSig = D3D12_ROOT_SIGNATURE();
        rootSig.Flags = D3D12_ROOT_SIGNATURE_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        asdx::RefPtr<ID3DBlob> rootBlob;
        asdx::RefPtr<ID3DBlob> errorBlob;

        hr = D3D12SerializeRootSignature( &rootSig, D3D_ROOT_SIGNATURE_V1, rootBlob.GetAddress(), errorBlob.GetAddress() );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : D3D12SerializeRootSignature() Failed." );
            return false;
        }

        hr = m_Device->CreateRootSignature(
            1,
            rootBlob->GetBufferPointer(),
            rootBlob->GetBufferSize(),
            IID_ID3D12RootSignature,
            (void**)m_RootSignature.GetAddress() );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D12Device::CreateRootSignature() Failed." );
            return false;
        }
    }

    {
        hr = m_Device->CreateCommandList(
            1,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            m_CmdAllocator.GetPtr(),
            nullptr,
            IID_ID3D12GraphicsCommandList,
            (void**)m_GfxCmdList.GetAddress() );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D12Device::CreateCommandList() Failed." );
            return false;
        }
    }

    {
        hr = m_SwapChain->GetBuffer( 0, IID_ID3D12Resource, (void**)m_ColorTarget.GetAddress() );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : IDXGISwapChain::GetBuffer() Failed." );
            return false;
        }

        m_ColorTargetHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        m_Device->CreateRenderTargetView( m_ColorTarget.GetPtr(), nullptr, m_ColorTargetHandle );
    }

    {
        hr = m_Device->CreateFence( 0, D3D12_FENCE_MISC_NONE, IID_ID3D12Fence, (void**)m_Factory.GetAddress() );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D12Device::CreateFence() Failed." );
            return false;
        }
    }

    {
        m_Viewport.TopLeftX = 0;
        m_Viewport.TopLeftY = 0;
        m_Viewport.Width    = FLOAT(w);
        m_Viewport.Height   = FLOAT(h);
        m_Viewport.MinDepth = 0.0f;
        m_Viewport.MaxDepth = 1.0f;
    }

    m_GfxCmdList->Close();

    return true;
}

void App::TermApp()
{
    OnTerm();

    TermD3D();

    TermWnd();

    CoUninitialize();
}

void App::TermWnd()
{
    if ( m_hInst != nullptr )
    { UnregisterClassW( ASDX_WND_CLASSNAME, m_hInst ); }

    m_hInst = nullptr;
    m_hWnd  = nullptr;
}

void App::TermD3D()
{
}

void App::MainLoop()
{
    MSG msg = { 0 };

    while( WM_QUIT != msg.message )
    {
        auto gotMsg = PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE );

        if ( gotMsg )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            OnFrameMove();
            OnFrameRender();
        }
    }
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
    m_CmdAllocator->Reset();
    m_GfxCmdList->Reset( m_CmdAllocator.GetPtr(), nullptr );

    m_GfxCmdList->RSSetViewports( 1, &m_Viewport );
    SetResourceBarrier( m_GfxCmdList.GetPtr(), m_ColorTarget.GetPtr(), D3D12_RESOURCE_USAGE_PRESENT, D3D12_RESOURCE_USAGE_RENDER_TARGET );

    float clearColor[] = { 0.39f, 0.58f, 0.92f, 1.0f };
    m_GfxCmdList->ClearRenderTargetView( m_ColorTargetHandle, clearColor, nullptr, 0 );
    SetResourceBarrier( m_GfxCmdList.GetPtr(), m_ColorTarget.GetPtr(), D3D12_RESOURCE_USAGE_RENDER_TARGET, D3D12_RESOURCE_USAGE_PRESENT );

    m_GfxCmdList->Close();
    m_CmdQueue->ExecuteCommandLists( 1, CommandListCast( m_GfxCmdList.GetAddress() ) );
    m_SwapChain->Present( 1, 0 );
}

void App::OnResize()
{
}

void App::SetResourceBarrier
(
    ID3D12GraphicsCommandList* pCmdList,
    ID3D12Resource* pResource,
    UINT stateBefore,
    UINT stateAfter
)
{
    D3D12_RESOURCE_BARRIER_DESC desc = {};
    desc.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    desc.Transition.pResource   = pResource;
    desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    desc.Transition.StateBefore = stateBefore;
    desc.Transition.StateAfter  = stateAfter;

    pCmdList->ResourceBarrier( 1, &desc );
}


LRESULT CALLBACK App::MsgProc( HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp )
{
    switch( uMsg )
    {
        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint( hWnd, &ps );
                EndPaint( hWnd, &ps );
            }
            break;

        case WM_DESTROY:
            {
                PostQuitMessage( 0 );
            }
            break;
    }

    return DefWindowProc( hWnd, uMsg, wp, lp );
}