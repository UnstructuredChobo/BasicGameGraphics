﻿#include "Game/Game.h"
#include "Common.h"
#include <wrl.h>
#include <iostream>

namespace library
{
    /*--------------------------------------------------------------------
      Global Variables
    --------------------------------------------------------------------*/


    HINSTANCE               g_hInst = nullptr;
    HWND                    g_hWnd = nullptr;
    D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
    D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
    D3D11_TEXTURE2D_DESC    m_bbDesc;
    D3D11_VIEWPORT          m_viewport;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
    Microsoft::WRL::ComPtr<ID3D11Device> g_pd3dDevice;
    //Microsoft::WRL::ComPtr<ID3D11Device1> g_pd3dDevice1;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> g_pImmediateContext;
    //Microsoft::WRL::ComPtr<ID3D11DeviceContext1> g_pImmediateContext1;
    Microsoft::WRL::ComPtr<IDXGISwapChain> g_pSwapChain;
    //Microsoft::WRL::ComPtr<IDXGISwapChain1> g_pSwapChain1;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> g_pRenderTargetView;
    /*--------------------------------------------------------------------
      Forward declarations
    --------------------------------------------------------------------*/

    /*F+F+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      Function: WindowProc
      Summary:  Defines the behavior of the window—its appearance, how
                it interacts with the user, and so forth
      Args:     HWND hWnd
                  Handle to the window
                UINT uMsg
                  Message code
                WPARAM wParam
                  Additional data that pertains to the message
                LPARAM lParam
                  Additional data that pertains to the message
      Returns:  LRESULT
                  Integer value that your program returns to Windows
    -----------------------------------------------------------------F-F*/
    LRESULT CALLBACK WindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

    LRESULT CALLBACK WindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        PAINTSTRUCT ps;
        HDC hdc;

        switch (uMsg)
        {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;



        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }

        return 0;
    }

    //--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
    HRESULT InitWindow(_In_ HINSTANCE hInstance, _In_ INT nCmdShow)
    {
        // Register class
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WindowProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = L"TutorialWindowClass";
        wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
        if (!RegisterClassEx(&wcex))
            return E_FAIL;

        // Create window
        g_hInst = hInstance;
        RECT rc = { 0, 0, 800, 600 };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
        g_hWnd = CreateWindow(L"TutorialWindowClass", L"Direct3D 11 Tutorial 1: Direct3D 11 Basics",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
            nullptr);
        if (!g_hWnd)
            return E_FAIL;

        ShowWindow(g_hWnd, nCmdShow);

        return S_OK;
    }

    //--------------------------------------------------------------------------------------
    // Create Direct3D device and swap chain
    //--------------------------------------------------------------------------------------
    HRESULT InitDevice()
    {
        HRESULT hr = S_OK;

        D3D_FEATURE_LEVEL levels[] = {
            D3D_FEATURE_LEVEL_9_1,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_11_1
        };

        // This flag adds support for surfaces with a color-channel ordering different
        // from the API default. It is required for compatibility with Direct2D.
        UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(DEBUG) || defined(_DEBUG)
        deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        DXGI_SWAP_CHAIN_DESC desc;
        ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
        desc.Windowed = TRUE;
        desc.BufferCount = 2;
        desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.SampleDesc.Count = 1;      //multisampling setting
        desc.SampleDesc.Quality = 0;    //vendor-specific flag
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        desc.OutputWindow = g_hWnd;

        Microsoft::WRL::ComPtr<ID3D11Device> device;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
        Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
        

        hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            deviceFlags,
            levels,
            ARRAYSIZE(levels),
            D3D11_SDK_VERSION,
            &desc,
            swapChain.GetAddressOf(),
            device.GetAddressOf(),
            &g_featureLevel,
            context.GetAddressOf()
        );

        device.As(&g_pd3dDevice);
        context.As(&g_pImmediateContext);
        swapChain.As(&g_pSwapChain);

        // Configure the back buffer and viewport.
        hr = g_pSwapChain->GetBuffer(
            0,
            __uuidof(ID3D11Texture2D),
            (void**)&pBackBuffer);

        pBackBuffer->GetDesc(&m_bbDesc);

        ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
        m_viewport.Height = (float)m_bbDesc.Height;
        m_viewport.Width = (float)m_bbDesc.Width;
        m_viewport.MinDepth = 0;
        m_viewport.MaxDepth = 1;


        g_pImmediateContext->RSSetViewports(
            1,
            &m_viewport
        );

        hr = g_pd3dDevice->CreateRenderTargetView(
            pBackBuffer.Get(),
            nullptr,
            g_pRenderTargetView.GetAddressOf()
        );
        
        return hr;


    }


    //--------------------------------------------------------------------------------------
    // Render the frame
    //--------------------------------------------------------------------------------------
    void Render()
    {
        float ClearColor[4] = { 0.0f, 0.125f, 0.6f, 1.0f }; // RGBA
        g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView.Get(), ClearColor);

        g_pSwapChain->Present(0, 0);
    }


    //--------------------------------------------------------------------------------------
    // Clean up the objects we've created
    //--------------------------------------------------------------------------------------
    void CleanupDevice()
    {
        //std::cout << "hello";
        if (g_pImmediateContext) g_pImmediateContext->ClearState();

        if (g_pRenderTargetView) g_pRenderTargetView->Release();
        //std::cout << "hello";
        if (pBackBuffer) pBackBuffer->Release();
        if (g_pSwapChain) g_pSwapChain->Release();
        
        if (g_pImmediateContext) g_pImmediateContext->Release();
        
        if (g_pd3dDevice) g_pd3dDevice->Release();
        
    }

}