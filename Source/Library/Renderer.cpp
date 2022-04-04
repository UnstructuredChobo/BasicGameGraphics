#include "Renderer.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Renderer
      Summary:  Constructor
      Modifies: [m_driverType, m_featureLevel, m_d3dDevice, m_d3dDevice1,
                  m_immediateContext, m_immediateContext1, m_swapChain,
                  m_swapChain1, m_renderTargetView].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Renderer definition (remove the comment)
    --------------------------------------------------------------------*/
    Renderer::Renderer() :
        m_driverType(),
        m_featureLevel(),

        m_d3dDevice(nullptr),
        m_d3dDevice1(nullptr),

        m_immediateContext(nullptr),
        m_immediateContext1(nullptr),

        m_swapChain(nullptr),
        m_swapChain1(nullptr),

        m_vertexLayout(nullptr),
        m_pixelShader(nullptr),
        
        m_vertextBuffer(nullptr),
        m_vertexShader(nullptr),
        

        m_renderTargetView(nullptr)
        //m_indexBuffer(nullptr)
    {};
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Initialize
      Summary:  Creates Direct3D device and swap chain
      Args:     HWND hWnd
                  Handle to the window
      Modifies: [m_d3dDevice, m_featureLevel, m_immediateContext,
                  m_d3dDevice1, m_immediateContext1, m_swapChain1,
                  m_swapChain, m_renderTargetView].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Initialize definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::Initialize(_In_ HWND hWnd) {
        HRESULT hr = S_OK;

        D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        };
        UINT createDeviceFlags = 0;

#ifdef _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        this->m_featureLevel = D3D_FEATURE_LEVEL_11_0;
        this->m_driverType = D3D_DRIVER_TYPE_NULL;
        ComPtr<ID3D11Device> device;
        ComPtr<ID3D11DeviceContext> context;
        ComPtr<IDXGISwapChain> swapChain;

        hr = D3D11CreateDevice(
            nullptr,                    // Specify nullptr to use the default adapter.
            D3D_DRIVER_TYPE_HARDWARE,   // Create a device using the hardware graphics driver.
            0,                          // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
            createDeviceFlags,                // Set debug and Direct2D compatibility flags.
            levels,                     // List of feature levels this app can support.
            ARRAYSIZE(levels),          // Size of the list above.
            D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
            &device,                    // Returns the Direct3D device created.
            &m_featureLevel,            // Returns feature level of device created.
            &context                    // Returns the device immediate context.
        );

        if (FAILED(hr)) return (hr);
        device.As(&m_d3dDevice);
        context.As(&m_immediateContext);
        //����ü�� ��ũ����

        DXGI_SWAP_CHAIN_DESC desc = {

        .BufferDesc = {
            .Format = DXGI_FORMAT_B8G8R8A8_UNORM,
        },

        .SampleDesc = {
            .Count = 1,
            .Quality = 0,
        },
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 1,
        .OutputWindow = hWnd,
        .Windowed = TRUE,


        };
        
        Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;
        m_d3dDevice.As(&dxgiDevice);
        Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
        Microsoft::WRL::ComPtr<IDXGIFactory> factory;

        hr = dxgiDevice->GetAdapter(&adapter);

        if (SUCCEEDED(hr))
        {
            adapter->GetParent(IID_PPV_ARGS(&factory));
            hr = factory->CreateSwapChain(
                m_d3dDevice.Get(),
                &desc,
                m_swapChain.GetAddressOf()

            );
        }
        /*
        //m_bbDesc.Width = 1;
        //m_bbDesc.Height = 1;
        CD3D11_TEXTURE2D_DESC depthStencilDesc(
            DXGI_FORMAT_D24_UNORM_S8_UINT,
            static_cast<UINT> (800),
            static_cast<UINT> (600),
            1, // This depth stencil view has only one texture.
            1, // Use a single mipmap level.
            D3D11_BIND_DEPTH_STENCIL
        );

        m_d3dDevice->CreateTexture2D(
            &depthStencilDesc,
            nullptr,
            m_pDepthStencil.GetAddressOf()
        );

        CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
        if (&depthStencilViewDesc == NULL) {
            return 0;
        }
        //���� ���ٽ�
        hr = m_d3dDevice->CreateDepthStencilView(
            m_pDepthStencil.Get(),
            &depthStencilViewDesc,
            m_pDepthStencilView.GetAddressOf()
        );
        //assert(hr);
        */
        D3D11_TEXTURE2D_DESC    m_bbDesc;
        D3D11_VIEWPORT          m_viewport = {
        .Width = (float)600,
        .Height = (float)800,
        .MinDepth = 0,
        .MaxDepth = 1,
        };
        ComPtr<ID3D11Texture2D> pBackBuffer;

        hr = m_swapChain->GetBuffer(
            0,
            __uuidof(ID3D11Texture2D),
            (void**)pBackBuffer.GetAddressOf());

        pBackBuffer->GetDesc(&m_bbDesc);

        hr = m_d3dDevice->CreateRenderTargetView(
            pBackBuffer.Get(),
            nullptr,
            m_renderTargetView.GetAddressOf()
        );
        m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);
        m_immediateContext->RSSetViewports(
            1,
            &m_viewport
        );

        //CompileFromFile Ptr
        ComPtr<ID3DBlob> vs_blob_ptr = nullptr;
        ComPtr<ID3DBlob> ps_blob_ptr = nullptr;
        
        //Compile Shaders
        hr = compileShaderFromFile(
            L"..//Library//Color.fxh",
            "VS",
            "vs_5_0",
            &vs_blob_ptr);
        if (FAILED(hr)) return (hr);
        
        
        hr = device->CreateVertexShader(vs_blob_ptr->GetBufferPointer(),
            vs_blob_ptr->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());

        if (FAILED(hr)) return (hr);


        D3D11_INPUT_ELEMENT_DESC layout[] = {
            {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0}
        };
        UINT numElements = ARRAYSIZE(layout);
        
        hr = device->CreateInputLayout(layout, numElements, vs_blob_ptr->GetBufferPointer(),
            vs_blob_ptr->GetBufferSize(), m_vertexLayout.GetAddressOf());

        if (FAILED(hr)) return (hr);



        hr = compileShaderFromFile(
            L"..//Library//Color.fxh",
            "PS",
            "ps_5_0",
            &ps_blob_ptr);

        hr = m_d3dDevice->CreatePixelShader(ps_blob_ptr->GetBufferPointer(), ps_blob_ptr->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf());

        //Describe out buffer
        D3D11_BUFFER_DESC bufferDesc = {
        .ByteWidth = (sizeof(Vertex) * 3),
        .Usage = D3D11_USAGE_DYNAMIC,
        .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
        .MiscFlags = 0,
        .StructureByteStride = 0
        };

        Vertex vertices[] = {
            {XMFLOAT3(0.0f, 0.0f, 0.5f)},
            {XMFLOAT3(0.5f,-0.5f, 0.5f)},
            {XMFLOAT3(-0.5f,-0.5f, 0.5f) },

        };

        D3D11_SUBRESOURCE_DATA subData = {
        .pSysMem = vertices,
        .SysMemPitch = 0,
        .SysMemSlicePitch = 0,
        };

        hr = m_d3dDevice->CreateBuffer(&bufferDesc, &subData, m_vertextBuffer.GetAddressOf());
        if (FAILED(hr)) return (hr);


        /*
        D3D11_BUFFER_DESC indexDesc;
        indexDesc.Usage = D3D11_USAGE_DYNAMIC;
        indexDesc.ByteWidth = sizeof(Vertex) * 36;
        indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        indexDesc.MiscFlags = 0;
        indexDesc.StructureByteStride = 0;

        //indexbuffer subresource
        UINT idxVertexID[] = {
            0,1,3, 1,2,3, 1,5,2, 5,6,2, 5,4,6, 4,7,6,
            4,5,0, 5,1,0, 4,0,7, 0,3,7, 3,2,7, 2,6,7
        };
        //define idxbuffer subresource
        D3D11_SUBRESOURCE_DATA idxsubData;
        idxsubData.pSysMem = idxVertexID;
        idxsubData.SysMemPitch = 0;
        idxsubData.SysMemSlicePitch = 0;
        
        
        //make indexbuffer 
        hr = device->CreateBuffer(&indexDesc, &idxsubData, m_indexBuffer.GetAddressOf());
        if (FAILED(hr)) return (hr);
        */
        

        //set buffer
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        m_immediateContext->IASetVertexBuffers(0, 1, m_vertextBuffer.GetAddressOf(), &stride, &offset);
        //m_immediateContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
        m_immediateContext->IASetInputLayout(m_vertexLayout.Get());


        //set topology
        m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


        
        return hr;

    }
 
    void Renderer::Render() {

        //set Shaders
        float ClearColor[4] = { 0.0f, 0.0f, 0.6f, 1.0f };
        m_immediateContext->ClearRenderTargetView(m_renderTargetView.Get(), ClearColor);
        m_immediateContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
        m_immediateContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
        m_immediateContext->Draw(3, 0);
        m_swapChain->Present(1, 0);
        

        
    }

    /*
    void Renderer::Resize(UINT width, UINT height) {
        HRESULT hr = S_OK;
        hr = m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, 0);
        //Failed(hr);
    }
    */


    HRESULT Renderer::compileShaderFromFile(PCWSTR filename, PCSTR pEntry , PCSTR pShaderModel, ID3DBlob** compiledShader) {

        DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
        shaderFlags |= D3D10_SHADER_DEBUG;
        shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
        ComPtr<ID3DBlob> blob = nullptr;
        
        HRESULT hr = D3DCompileFromFile(filename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, pEntry, pShaderModel, NULL, 0, compiledShader,blob.GetAddressOf());
 
        if (FAILED(hr)) return(hr);
        return hr;
    }
}