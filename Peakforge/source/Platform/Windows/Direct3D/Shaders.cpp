#include "pfpch.h"
#include "Shaders.h"

namespace PF::Render::DX
{

#pragma region VertexShader
	VertexShader::VertexShader() {}

	void VertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device1>& device, std::wstring shaderPath, D3D11_INPUT_ELEMENT_DESC* inputDesc, UINT descSize)
	{
		HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), m_ShaderBuffer.GetAddressOf());
		PF_CORE_ASSERT(SUCCEEDED(hr), "Error: D3DReadFileToBlob");

		hr = device->CreateVertexShader
		(
			m_ShaderBuffer->GetBufferPointer(),
			m_ShaderBuffer->GetBufferSize(),
			nullptr,
			m_Shader.GetAddressOf()
		);
		PF_CORE_ASSERT(SUCCEEDED(hr), "Error: CreateVertexShader");

		hr = device->CreateInputLayout
		(
			inputDesc,
			descSize,
			m_ShaderBuffer->GetBufferPointer(),
			m_ShaderBuffer->GetBufferSize(),
			m_InputLayout.GetAddressOf()
		);
		PF_CORE_ASSERT(SUCCEEDED(hr), "Error: CreateInputLayout");
		LOG_CORE_TRACE("\tVertexShader Initialized");
	}
#pragma endregion

#pragma region PixelShader
	PixelShader::PixelShader() {}

	void PixelShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device1>& device, std::wstring shaderPath)
	{
		HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), m_ShaderBuffer.GetAddressOf());
		PF_CORE_ASSERT(SUCCEEDED(hr), "\tError: D3DReadFileToBlob");

		hr = device->CreatePixelShader
		(
			m_ShaderBuffer->GetBufferPointer(),
			m_ShaderBuffer->GetBufferSize(),
			NULL,
			m_Shader.GetAddressOf()
		);
		PF_CORE_ASSERT(SUCCEEDED(hr), "\tError: CreatePixelShader");

		LOG_CORE_TRACE("\tPixelShader Initialized");
	}
#pragma endregion
}