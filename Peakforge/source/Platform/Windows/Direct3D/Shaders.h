#pragma once

#include "Peakforge/Core/Base.h"

#include <d3d11.h>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <d3dcompiler.h>

namespace PF::Render::DX
{
	class VertexShader
	{
	public:
		VertexShader();

		void Initialize(Microsoft::WRL::ComPtr<ID3D11Device1>& device, std::wstring shaderPath, D3D11_INPUT_ELEMENT_DESC* inputDesc, UINT descSize);

		inline ID3D11VertexShader* GetShader() const { return m_Shader.Get(); }
		inline ID3D10Blob* GetShaderBuffer() const { return m_ShaderBuffer.Get(); }
		inline ID3D11InputLayout* GetInputLayout() const { return m_InputLayout.Get(); }

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_Shader;
		Microsoft::WRL::ComPtr<ID3D10Blob>			m_ShaderBuffer;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_InputLayout;
	};

	class PixelShader
	{
	public:
		PixelShader();

		void Initialize(Microsoft::WRL::ComPtr<ID3D11Device1>& device, std::wstring shaderPath);

		inline ID3D11PixelShader* GetShader() const { return m_Shader.Get(); }
		inline ID3D10Blob* GetShaderBuffer() const { return m_ShaderBuffer.Get(); }

	private:
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_Shader;
		Microsoft::WRL::ComPtr<ID3D10Blob>			m_ShaderBuffer;
	};
}
