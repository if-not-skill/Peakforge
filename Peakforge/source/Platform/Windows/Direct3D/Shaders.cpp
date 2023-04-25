#include "pfpch.h"
#include "Shaders.h"

bool VertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device1>& device, std::wstring shaderPath)
{
	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), m_ShaderBuffer.GetAddressOf());
	if (FAILED(hr)) return false;

	hr = device->CreateVertexShader
	(
		m_ShaderBuffer->GetBufferPointer(), 
		m_ShaderBuffer->GetBufferSize(),
		nullptr, 
		m_Shader.GetAddressOf()
	);
	
	return SUCCEEDED(hr);
}
