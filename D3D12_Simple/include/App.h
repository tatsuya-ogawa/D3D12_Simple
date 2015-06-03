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
#include <dxgi1_4.h>
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
    /* NOTHING */

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
    HINSTANCE           m_hInst;            //!< �C���X�^���X�n���h���ł�.
    HWND                m_hWnd;             //!< �E�B���h�E�n���h���ł�.
    UINT                m_BufferCount;      //!< �o�b�t�@���ł�.
    DXGI_FORMAT         m_SwapChainFormat;  //!< �X���b�v�`�F�C���̃t�H�[�}�b�g�ł�.
    D3D12_VIEWPORT      m_Viewport;         //!< �r���[�|�[�g�ł�.

    //=============================================================================================
    // protected methods.
    //=============================================================================================
    virtual bool OnInit         ();
    virtual void OnTerm         ();
    virtual void OnFrameMove    ();
    virtual void OnFrameRender  ();
    virtual void OnResize       ( u32 width, u32 height );

    void SetResourceBarrier( 
        ID3D12GraphicsCommandList*  pCmdList,
        ID3D12Resource*             pResource,
        D3D12_RESOURCE_STATES       stateBefore,
        D3D12_RESOURCE_STATES       stateAfter );
    void Present( u32 syncInterval );

private:
    //=============================================================================================
    // private variables.
    //=============================================================================================
    asdx::RefPtr<ID3D12Device>              m_Device;                   //!< �f�o�C�X�ł�.
    asdx::RefPtr<ID3D12CommandAllocator>    m_CmdAllocator;             //!< �R�}���h�A���P�[�^�ł�.
    asdx::RefPtr<ID3D12CommandQueue>        m_CmdQueue;                 //!< �R�}���h�L���[�ł�.
    asdx::RefPtr<ID3D12GraphicsCommandList> m_CmdList;                  //!< �R�}���h���X�g�ł�.
    asdx::RefPtr<IDXGIAdapter>              m_Adapter;                  //!< �A�_�v�^�[�ł�.
    asdx::RefPtr<IDXGIFactory3>             m_Factory;                  //!< DXGI�t�@�N�g���[�ł�.
    asdx::RefPtr<IDXGISwapChain>            m_SwapChain;                //!< �X���b�v�`�F�C���ł�.
    asdx::RefPtr<ID3D12DescriptorHeap>      m_DescriptorHeap;           //!< �f�X�N���v�^�[�q�[�v�ł�.
    asdx::RefPtr<ID3D12Resource>            m_ColorTarget;              //!< �J���[�^�[�Q�b�g�̃��\�[�X�ł�.
    asdx::RefPtr<ID3D12Fence>               m_Fence;                    //!< �t�F���X�ł�.
    D3D12_CPU_DESCRIPTOR_HANDLE             m_ColorTargetHandle;        //!< �J���[�^�[�Q�b�g�̃n���h���ł�.
    HANDLE                                  m_EventHandle;              //!< �C�x���g�n���h���ł�.

    //=============================================================================================
    // private methods.
    //=============================================================================================
    bool InitApp ();
    void TermApp ();
    bool InitWnd ();
    void TermWnd ();
    bool InitD3D ();
    void TermD3D ();
    void MainLoop();

    static LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp);
};
