#pragma once

#include "Peakforge/Renderer/Renderer.h"
#include "Peakforge/Renderer/Buffer.h"

#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace PF::Render::DX
{
	template<typename T>
	class DX11VertexBuffer : public VertexBuffer<T>
	{
	public:
		DX11VertexBuffer(T* vertices, uint32_t num)
			: VertexBuffer<T>()
		{
			auto renderContext = (DX11Context*)Renderer::GetNativeRenderRef();
			m_D3DDevice = renderContext->GetD3DDevice();
			m_D3DContext = renderContext->GetD3DContext();

			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));

			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = sizeof(T) * num;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA data;
			ZeroMemory(&data, sizeof(data));
			data.pSysMem = vertices;

			HRESULT hr = m_D3DDevice->CreateBuffer(&desc, &data, m_VertexBuffer.GetAddressOf());
			PF_CORE_ASSERT(SUCCEEDED(hr), "Error CreateBuffer");
		}

		~DX11VertexBuffer()
		{

		}

		void Bind() const override
		{
			UINT stride = sizeof(Vertex);
			UINT offset = 0;
			m_D3DContext->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &stride, &offset);
		}

		void Unbind() const override
		{

		}

	private:
		ComPtr<ID3D11Buffer>	m_VertexBuffer;

		ID3D11Device*			m_D3DDevice;
		ID3D11DeviceContext*	m_D3DContext;
	};
}