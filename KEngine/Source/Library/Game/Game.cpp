﻿#include "Game/Game.h"
#include "Common.h"
#include <wrl.h>
#include <iostream>
// DirectX11 Init 순서
// Device -> SwapChain -> SwapChain DESC -> Depth Stencil & DESC -> Backbuffer -> RTV -> Viewport -> TargetView
namespace library
{
    /*--------------------------------------------------------------------
      Global Variables
    --------------------------------------------------------------------*/
    HINSTANCE               g_hInst = 0;
    HWND                    g_hWnd = 0;
    D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
    D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
    D3D11_TEXTURE2D_DESC    m_bbDesc ;
    D3D11_VIEWPORT          m_viewport;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer = 0;
    Microsoft::WRL::ComPtr<ID3D11Device> g_pd3dDevice = 0;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> g_pImmediateContext = 0;
    Microsoft::WRL::ComPtr<IDXGISwapChain> g_pSwapChain = 0;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> g_pRenderTargetView = 0;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>         g_pDepthStencil = 0;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  g_pDepthStencilView = 0;

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

        case WM_CLOSE:
            DestroyWindow(hWnd);
            return 0;
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
        wcex.lpszClassName = L"LAB1";
        wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
        if (!RegisterClassEx(&wcex))
            return E_FAIL;


        g_hInst = hInstance;
        RECT rc = { 0, 0, 800, 600 };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
        g_hWnd = CreateWindow(L"LAB1", L"Game Graphics Programming Lab 01: Direct3D 11 Basics",
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
            D3D_FEATURE_LEVEL_11_1,
        };
        UINT createDeviceFlags = 0;

#ifdef _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        Microsoft::WRL::ComPtr<ID3D11Device> device;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
        Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
        
        hr = D3D11CreateDevice(
            nullptr,                    // Specify nullptr to use the default adapter.
            D3D_DRIVER_TYPE_HARDWARE,   // Create a device using the hardware graphics driver.
            0,                          // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
            createDeviceFlags,                // Set debug and Direct2D compatibility flags.
            levels,                     // List of feature levels this app can support.
            ARRAYSIZE(levels),          // Size of the list above.
            D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
            &device,                    // Returns the Direct3D device created.
            &g_featureLevel,            // Returns feature level of device created.
            &context                    // Returns the device immediate context.
        );

        if (FAILED(hr))
        {
        }

        device.As(&g_pd3dDevice);
        context.As(&g_pImmediateContext);

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

        // Create the DXGI device object to use in other factories, such as Direct2D.
        Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;
        g_pd3dDevice.As(&dxgiDevice);

        // Create swap chain.
        Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
        Microsoft::WRL::ComPtr<IDXGIFactory> factory;

        hr = dxgiDevice->GetAdapter(&adapter);

        if (SUCCEEDED(hr))
        {
            adapter->GetParent(IID_PPV_ARGS(&factory));

            hr = factory->CreateSwapChain(
                g_pd3dDevice.Get(),
                &desc,
                &g_pSwapChain
            );
        }

        CD3D11_TEXTURE2D_DESC depthStencilDesc(
            DXGI_FORMAT_D24_UNORM_S8_UINT,
            static_cast<UINT> (m_bbDesc.Width),
            static_cast<UINT> (m_bbDesc.Height),
            1, // This depth stencil view has only one texture.
            1, // Use a single mipmap level.
            D3D11_BIND_DEPTH_STENCIL
        );

        g_pd3dDevice->CreateTexture2D(
            &depthStencilDesc,
            nullptr,
            &g_pDepthStencil
        );

        CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);

        g_pd3dDevice->CreateDepthStencilView(
            g_pDepthStencil.Get(),
            &depthStencilViewDesc,
            &g_pDepthStencilView
        );

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
        float ClearColor[4] = { 0.0f, 0.0f, 0.6f, 1.0f };
        g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView.Get(), ClearColor);

        g_pSwapChain->Present(0, 0);
    }


    //--------------------------------------------------------------------------------------
    // Clean up the objects we've created
    //--------------------------------------------------------------------------------------
    void CleanupDevice()
    {

        g_pImmediateContext.Reset();
        pBackBuffer.Reset();
        g_pSwapChain.Reset();
        g_pImmediateContext.Reset();
        g_pd3dDevice.Reset();
    }

}