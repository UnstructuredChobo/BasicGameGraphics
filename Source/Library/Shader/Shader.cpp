#include "Shader.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Shader::Shader
      Summary:  Constructor
      Args:     PCWSTR pszFileName
                  Name of the file that contains the shader code
              PCSTR pszEntryPoint
                  Name of the shader entry point functino where shader
                  execution begins
              PCSTR pszShaderModel
                  Specifies the shader target or set of shader features
                  to compile against
      Modifies: [m_pszFileName, m_pszEntryPoint, m_pszShaderModel].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Shader::Shader definition (remove the comment)
    --------------------------------------------------------------------*/
    Shader::Shader(_In_ PCWSTR pszFileName, _In_ PCSTR pszEntryPoint, _In_ PCSTR pszShaderModel) : 
        m_pszFileName(pszFileName),
        m_pszEntryPoint(pszEntryPoint),
        m_pszShaderModel(pszShaderModel) {
    
    };
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Shader::GetFileName
      Summary:  Returns the shader file name
      Returns:  PCWSTR
                  Shader file name
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Shader::GetFileName definition (remove the comment)
    --------------------------------------------------------------------*/
    PCWSTR Shader::GetFileName() const {
        return m_pszFileName;
    };
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Shader::compile
      Summary:  Compiles the given shader file
      Args:     ID3DBlob** ppOutBlob
                  Receives a pointer to the ID3DBlob interface that you
                  can use to access the compiled code
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Shader::compile(_Outptr_ ID3DBlob **ppOutBlob) {
        ComPtr<ID3DBlob> errorBlob = nullptr;
        DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
        shaderFlags |= D3D10_SHADER_DEBUG;
        shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

        HRESULT hr = D3DCompileFromFile(
            m_pszFileName, 
            nullptr,
            nullptr, 
            m_pszEntryPoint, 
            m_pszShaderModel, 
            shaderFlags,
            0u, 
            ppOutBlob, 
            errorBlob.GetAddressOf());
        //OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        if (FAILED(hr))
        {
            WCHAR szMessage[256];
            swprintf_s(
                szMessage,
                L"Shader.cpp",
                m_pszFileName
            );
            MessageBox(
                nullptr,
                szMessage,
                L"Error",
                MB_OK
            );
            return hr;
        }
        return S_OK;
    }
}