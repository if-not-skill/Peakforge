#pragma once

#include <d3d11.h>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <string>

class VertexShader
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device1>& device, std::wstring shaderPath);

	inline ID3D11VertexShader* GetShader() const { return m_Shader.Get(); }
	inline ID3D10Blob* GetShaderBuffer() const { return m_ShaderBuffer.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_Shader = nullptr;
	Microsoft::WRL::ComPtr<ID3D10Blob> m_ShaderBuffer = nullptr;
};
