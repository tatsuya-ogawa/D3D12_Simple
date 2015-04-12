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


namespace /* anonymous */ {

//-------------------------------------------------------------------------------------------------
// Global Variables.
//-------------------------------------------------------------------------------------------------
App*    g_pApp = nullptr;


} // namespace /* anonymous */


///////////////////////////////////////////////////////////////////////////////////////////////////
// App class
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//      �R���X�g���N�^�ł�.
//-------------------------------------------------------------------------------------------------
App::App()
: m_hInst           ( nullptr )
, m_hWnd            ( nullptr )
, m_BufferCount     ( 2 )
, m_SwapChainFormat ( DXGI_FORMAT_R8G8B8A8_UNORM_SRGB )
, m_EventHandle     ( nullptr )
{ /* DO_NOTHING */ }

//-------------------------------------------------------------------------------------------------
//      �f�X�g���N�^�ł�.
//-------------------------------------------------------------------------------------------------
App::~App()
{ TermApp(); }

//-------------------------------------------------------------------------------------------------
//      �����������ł�.
//-------------------------------------------------------------------------------------------------
bool App::InitApp()
{
    // COM���C�u�����̏�����.
    HRESULT hr = CoInitialize( nullptr );
    if ( FAILED( hr ) )
    {
        ELOG( "Error : Com Library Initialize Failed." );
        return false;
    }

    // �E�B���h�E�̏�����.
    if ( !InitWnd() )
    {
        ELOG( "Error : InitWnd() Failed." );
        return false;
    }

    // D3D12�̏�����.
    if ( !InitD3D() )
    {
        ELOG( "Error : InitD3D() Failed." );
        return false;
    }

    // �A�v���P�[�V�����ŗL�̏�����.
    if ( !OnInit() )
    {
        ELOG( "Error : OnInit() Failed." );
        return false;
    }

    // �|�C���^�ݒ�.
    g_pApp = this;

    // ����I��.
    return true;
}

//-------------------------------------------------------------------------------------------------
//      �E�B���h�E�̏����������ł�.
//-------------------------------------------------------------------------------------------------
bool App::InitWnd()
{
    // �C���X�^���X�n���h�����擾.
    HINSTANCE hInst = GetModuleHandle( nullptr );
    if ( !hInst )
    {
        ELOG( "Error : GetModuleHandle() Failed." );
        return false;
    }

    // �g���E�B���h�E�N���X�̐ݒ�.
    WNDCLASSEXW wc;
    wc.cbSize           = sizeof(WNDCLASSEXW);
    wc.style            = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc      = MsgProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = hInst;
    wc.hIcon            = LoadIcon( hInst, IDI_APPLICATION );
    wc.hCursor          = LoadCursor( nullptr, IDC_ARROW );
    wc.hbrBackground    = (HBRUSH)( COLOR_WINDOW + 1 );
    wc.lpszMenuName     = nullptr;
    wc.lpszClassName    = ASDX_WND_CLASSNAME;
    wc.hIconSm          = LoadIcon( hInst, IDI_APPLICATION );

    // �g���E�B���h�E�N���X��o�^.
    if ( !RegisterClassExW( &wc ) )
    {
        ELOG( "Error : RegisterClassEx() Failed." );
        return false;
    }

    // �C���X�^���X�n���h����ݒ�.
    m_hInst = hInst;

    RECT rc = { 0, 0, 960, 540 };

    // �E�B���h�E�̋�`�𒲐�.
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
    AdjustWindowRect( &rc, style, FALSE );

    // �E�B���h�E�𐶐�.
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

    // �G���[�`�F�b�N.
    if ( !m_hWnd )
    {
        ELOG( "Error : CreateWindowW() Failed." );
        return false;
    }

    // �E�B���h�E��\��
    ShowWindow( m_hWnd, SW_SHOWNORMAL );
    UpdateWindow( m_hWnd );

    // �t�H�[�J�X�ݒ�.
    SetFocus( m_hWnd );

    // ����I��.
    return true;
}

//-------------------------------------------------------------------------------------------------
//      D3D12�̏����������ł�.
//-------------------------------------------------------------------------------------------------
bool App::InitD3D()
{
    HRESULT hr = S_OK;

    // �E�B���h�E�����擾.
    RECT rc;
    GetClientRect( m_hWnd, &rc );
    u32 w = rc.right - rc.left;
    u32 h = rc.bottom - rc.top;

    // �f�o�C�X�����t���O.
    auto createDeviceFlags = D3D12_CREATE_DEVICE_NONE;
#if defined(DEBUG) || defined(_DEBUG)
    createDeviceFlags |= D3D12_CREATE_DEVICE_DEBUG;
#endif

    // �h���C�o�[�^�C�v.
    std::array<D3D_DRIVER_TYPE, 4> driverTypes = { 
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
        D3D_DRIVER_TYPE_SOFTWARE
    };

    // �f�o�C�X����.
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
        { break; }
    }

    // �����`�F�b�N.
    if ( FAILED( hr ) )
    {
        ELOG( "Error : D3D12CreateDevice()Failed." );
        return false;
    }

    // �R�}���h�A���P�[�^�𐶐�.
    hr = m_Device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT, 
        IID_ID3D12CommandAllocator,
        (void**)m_CmdAllocator.GetAddress() );
    if ( FAILED( hr ) )
    {
        ELOG( "Error : ID3D12Device::CreateCommandAllocator() Failed." );
        return false;
    }

    // �R�}���h�L���[�𐶐�.
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

    // �X���b�v�`�F�C���𐶐�.
    {
        hr = CreateDXGIFactory( IID_IDXGIFactory, (void**)m_Factory.GetAddress() );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : CreateDXGIFactory() Failed." );
            return false;
        }

        DXGI_SWAP_CHAIN_DESC desc;
        ZeroMemory( &desc, sizeof(desc) );
        desc.BufferCount                        = m_BufferCount;
        desc.BufferDesc.Format                  = m_SwapChainFormat;
        desc.BufferDesc.Width                   = w;
        desc.BufferDesc.Height                  = h;
        desc.BufferDesc.RefreshRate.Numerator   = 60;
        desc.BufferDesc.RefreshRate.Denominator = 1;
        desc.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
        desc.OutputWindow                       = m_hWnd;
        desc.SampleDesc.Count                   = 1;
        desc.SampleDesc.Quality                 = 0;
        desc.Windowed                           = TRUE;

        // �A�_�v�^�[�P�ʂ̏����Ƀ}�b�`����̂� m_Device �ł͂Ȃ� m_CmdQueue�@�Ȃ̂ŁCm_CmdQueue�@��������Ƃ��ēn��.
        hr = m_Factory->CreateSwapChain( m_CmdQueue.GetPtr(), &desc, m_SwapChain.GetAddress() );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : IDXGIFactory::CreateSwapChain() Failed." );
            return false;
        }
    }

    // �f�X�N���v�^�q�[�v�̐���.
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc;
        ZeroMemory( &desc, sizeof(desc) );

        desc.NumDescriptors = 1;
        desc.Type           = D3D12_RTV_DESCRIPTOR_HEAP;
        desc.Flags          = D3D12_DESCRIPTOR_HEAP_NONE;

        hr = m_Device->CreateDescriptorHeap( &desc, IID_ID3D12DescriptorHeap, (void**)m_DescriptorHeap.GetAddress() );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D12Device::CreateDescriptorHeap() Failed." );
            return false;
        }
    }

    // �R�}���h���X�g�̐���.
    {
        hr = m_Device->CreateCommandList(
            1,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            m_CmdAllocator.GetPtr(),
            nullptr,
            IID_ID3D12GraphicsCommandList,
            (void**)m_CmdList.GetAddress() );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D12Device::CreateCommandList() Failed." );
            return false;
        }
    }

    // �o�b�N�o�b�t�@���烌���_�[�^�[�Q�b�g�𐶐�.
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

    // �t�F���X�̐���.
    {
        m_EventHandle = CreateEvent( 0, FALSE, FALSE, 0 );

        hr = m_Device->CreateFence( 0, D3D12_FENCE_MISC_NONE, IID_ID3D12Fence, (void**)m_Fence.GetAddress() );
        if ( FAILED( hr ) )
        {
            ELOG( "Error : ID3D12Device::CreateFence() Failed." );
            return false;
        }
    }

    // �r���[�|�[�g�̐ݒ�.
    {
        m_Viewport.TopLeftX = 0;
        m_Viewport.TopLeftY = 0;
        m_Viewport.Width    = FLOAT(w);
        m_Viewport.Height   = FLOAT(h);
        m_Viewport.MinDepth = 0.0f;
        m_Viewport.MaxDepth = 1.0f;
    }

    // ����I��.
    return true;
}

//-------------------------------------------------------------------------------------------------
//      �I�������ł�.
//-------------------------------------------------------------------------------------------------
void App::TermApp()
{
    // �A�v���P�[�V�����ŗL�̏I������.
    OnTerm();

    // D3D12�̏I������.
    TermD3D();

    // �E�B���h�E�̏I������.
    TermWnd();

    // COM���C�u�����̏I������.
    CoUninitialize();

    // �|�C���^�N���A.
    g_pApp = nullptr;
}

//-------------------------------------------------------------------------------------------------
//      �E�B���h�E�̏I������.
//-------------------------------------------------------------------------------------------------
void App::TermWnd()
{
    if ( m_hInst != nullptr )
    { UnregisterClassW( ASDX_WND_CLASSNAME, m_hInst ); }

    m_hInst = nullptr;
    m_hWnd  = nullptr;
}

//-------------------------------------------------------------------------------------------------
//      D3D12�̏I������.
//-------------------------------------------------------------------------------------------------
void App::TermD3D()
{
    CloseHandle( m_EventHandle );

    m_EventHandle = nullptr;
}

//-------------------------------------------------------------------------------------------------
//      ���C�����[�v�ł�.
//-------------------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------------------
//      �A�v���P�[�V���������s���܂�.
//-------------------------------------------------------------------------------------------------
void App::Run()
{
    // �������ɐ��������烁�C�����[�v�ɓ���.
    if ( InitApp() )
    { MainLoop(); }

    // �I������.
    TermApp();
}

//-------------------------------------------------------------------------------------------------
//      �A�v���P�[�V�����ŗL�̏����������ł�.
//-------------------------------------------------------------------------------------------------
bool App::OnInit()
{
    /* DO_NOTHING */

    return true;
}

//-------------------------------------------------------------------------------------------------
//      �A�v���P�[�V�����ŗL�̏I�������ł�.
//-------------------------------------------------------------------------------------------------
void App::OnTerm()
{
    /* DO_NOTHING */
}

//-------------------------------------------------------------------------------------------------
//      �t���[���J�ڏ����ł�.
//-------------------------------------------------------------------------------------------------
void App::OnFrameMove()
{
    /* DO_NOTHING */
}

//-------------------------------------------------------------------------------------------------
//      �t���[���`�揈���ł�.
//-------------------------------------------------------------------------------------------------
void App::OnFrameRender()
{
    // �r���[�|�[�g��ݒ�.
    m_CmdList->RSSetViewports( 1, &m_Viewport );
    SetResourceBarrier( m_CmdList.GetPtr(), m_ColorTarget.GetPtr(), D3D12_RESOURCE_USAGE_PRESENT, D3D12_RESOURCE_USAGE_RENDER_TARGET );

    // �J���[�o�b�t�@���N���A.
    float clearColor[] = { 0.39f, 0.58f, 0.92f, 1.0f };
    m_CmdList->ClearRenderTargetView( m_ColorTargetHandle, clearColor, nullptr, 0 );
    SetResourceBarrier( m_CmdList.GetPtr(), m_ColorTarget.GetPtr(), D3D12_RESOURCE_USAGE_RENDER_TARGET, D3D12_RESOURCE_USAGE_PRESENT );

    // ��ʂɕ\��.
    Present( 0 );
}

//-------------------------------------------------------------------------------------------------
//      ���T�C�Y���̏����ł�.
//-------------------------------------------------------------------------------------------------
void App::OnResize( u32 width, u32 height )
{
    m_Viewport.Width  = FLOAT( width );
    m_Viewport.Height = FLOAT( height );

    // �����_�[�^�[�Q�b�g��j��.
    m_ColorTarget.Reset();
    m_ColorTargetHandle.ptr = 0;

    // �o�b�N�o�b�t�@�����T�C�Y.
    HRESULT hr = m_SwapChain->ResizeBuffers( m_BufferCount, 0, 0, m_SwapChainFormat, 0 );
    if ( FAILED( hr ) )
    { ELOG( "Error : IDXGISwapChain::ResizeBuffer() Failed." ); }

    // �o�b�N�o�b�t�@���擾.
    hr = m_SwapChain->GetBuffer( 0, IID_ID3D12Resource, (void**)m_ColorTarget.GetAddress() );
    if ( FAILED( hr ) )
    { ELOG( "Error : IDXGISwapChain::GetBuffer() Failed." ); }

    // �����_�[�^�[�Q�b�g�𐶐�.
    m_ColorTargetHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    m_Device->CreateRenderTargetView( m_ColorTarget.GetPtr(), nullptr, m_ColorTargetHandle );
}

//-------------------------------------------------------------------------------------------------
//      ���\�[�X�o���A�̐ݒ�.
//-------------------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------------------
//      �R�}���h�����s���ĉ�ʂɕ\�����܂�.
//-------------------------------------------------------------------------------------------------
void App::Present( u32 syncInterval )
{
    // �R�}���h���X�g�ւ̋L�^���I�����C�R�}���h���s.
    m_CmdList->Close();
    m_CmdQueue->ExecuteCommandLists( 1, CommandListCast( m_CmdList.GetAddress() ) );

    // �R�}���h�̎��s�̏I����ҋ@����
    m_Fence->Signal( 0 );
    m_Fence->SetEventOnCompletion( 1, m_EventHandle );
    m_CmdQueue->Signal( m_Fence.GetPtr(), 1 );
    WaitForSingleObject( m_EventHandle, INFINITE );

    // ��ʂɕ\������.
    m_SwapChain->Present( syncInterval, 0 );

    // �R�}���h���X�g�ƃR�}���h�A���P�[�^�����Z�b�g����.
    m_CmdAllocator->Reset();
    m_CmdList->Reset( m_CmdAllocator.GetPtr(), nullptr );
}

//-------------------------------------------------------------------------------------------------
//      �E�B���h�E�v���V�[�W���ł�.
//-------------------------------------------------------------------------------------------------
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

        case WM_SIZE:
            {
                if ( g_pApp != nullptr )
                {
                    u32 w = LOWORD( lp );
                    u32 h = HIWORD( lp );
                    g_pApp->OnResize( w, h );
                }
            }
    }

    return DefWindowProc( hWnd, uMsg, wp, lp );
}