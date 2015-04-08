//-------------------------------------------------------------------------------------------------
// File : App.h
// Desc : Application Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

#pragma once

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <Windows.h>
#include <d3d12.h>
#include <asdxTypedef.h>
#include <asdxRef.h>


//-------------------------------------------------------------------------------------------------
// Linker
//-------------------------------------------------------------------------------------------------
#pragma comment( lib, "d3d12.lib" )
#pragma comment( lib, "d3dcompiler.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "comctl32.lib" )


///////////////////////////////////////////////////////////////////////////////////////////////////
// App class
///////////////////////////////////////////////////////////////////////////////////////////////////
class App
{
    //=============================================================================================
    // list of friend classes and methods.
    //=============================================================================================
    /* NOTHING */

public:
    //=============================================================================================
    // public variables.
    //=============================================================================================


    //=============================================================================================
    // public methods.
    //=============================================================================================
    App();
    virtual ~App();
    void Run();


protected:
    //=============================================================================================
    // protected variables.
    //=============================================================================================
    HINSTANCE           m_hInst;
    HWND                m_hWnd;
    D3D_DRIVER_TYPE     m_DriverType;
    D3D_FEATURE_LEVEL   m_FeatureLevel;
    DXGI_FORMAT         m_SwapChainFormat;
    DXGI_FORMAT         m_DepthStencilFormat;



    //=============================================================================================
    // protected methods.
    //=============================================================================================
    virtual bool OnInit();
    virtual void OnTerm();
    virtual void OnFrameMove();
    virtual void OnFrameRender();
    virtual void OnResize();

    void SetResourceBarrier( ID3D12GraphicsCommandList* pCmdList, ID3D12Resource* pResource, UINT stateBefore, UINT stateAfter );

private:
    //=============================================================================================
    // private variables.
    //=============================================================================================
    asdx::RefPtr<ID3D12Device>              m_Device;
    asdx::RefPtr<ID3D12CommandAllocator>    m_CmdAllocator;
    asdx::RefPtr<ID3D12CommandQueue>        m_CmdQueue;
    asdx::RefPtr<ID3D12GraphicsCommandList> m_GfxCmdList;
    asdx::RefPtr<IDXGIFactory>              m_Factory;
    asdx::RefPtr<IDXGISwapChain>            m_SwapChain;
    asdx::RefPtr<ID3D12DescriptorHeap>      m_DescriptorHeap;
    asdx::RefPtr<ID3D12RootSignature>       m_RootSignature;
    asdx::RefPtr<ID3D12Resource>            m_ColorTarget;
    asdx::RefPtr<ID3D12Resource>            m_DepthStencilTarget;
    D3D12_VIEWPORT                          m_Viewport;
    D3D12_CPU_DESCRIPTOR_HANDLE             m_ColorTargetHandle;

    //=============================================================================================
    // private methods.
    //=============================================================================================
    bool InitApp();
    void TermApp();
    bool InitWnd();
    void TermWnd();
    bool InitD3D();
    void TermD3D();
    void MainLoop();

    static LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp);
};
