#pragma once

#include "Peakforge/Renderer/Renderer.h"
#include "Peakforge/Renderer/Buffer.h"

#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace PF::Render::DX
{
	template<typename T>
	class DX11IndexBuffer : public IndexBuffer<T>
	{
	public:
		DX11IndexBuffer(T* indices, uint32_t num)
			: IndexBuffer<T>()
		{
			m_IndexCount = num;

			auto renderContext = (DX11Context*)Renderer::GetNativeRenderRef();
			m_D3DDevice = renderContext->GetD3DDevice();
			m_D3DContext = renderContext->GetD3DContext();

			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = sizeof(T) * num;
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = indices;
			HRESULT hr = m_D3DDevice->CreateBuffer(&desc, &data, m_IndexBuffer.GetAddressOf());
			PF_CORE_ASSERT(SUCCEEDED(hr), "Failed creating Indexies buffer");
		}

		void Bind() const override
		{
			m_D3DContext->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		}

		void Unbind() const override
		{

		}

		uint32_t GetCount() const override { return m_IndexCount; }

	private:
		uint32_t				m_IndexCount;
		ComPtr<ID3D11Buffer>	m_IndexBuffer;

		ID3D11Device* m_D3DDevice;
		ID3D11DeviceContext* m_D3DContext;
	};
}