#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "Peakforge/Renderer/Renderer.h"
#include "ConstantBufferTypes.h"

namespace PF::Render::DX
{
	template<class T>
	class ConstantBuffer
	{
	private:
		ConstantBuffer(const ConstantBuffer<T>& rhs) = default;

	public:
		T data;

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_Buffer;
		ID3D11DeviceContext* m_Context;

	public:
		ConstantBuffer()
		{
			const auto renderContext = static_cast<DX11Context*>(Renderer::GetNativeRenderRef());
			auto device = renderContext->GetD3DDevice();
			m_Context = renderContext->GetD3DContext();


			D3D11_BUFFER_DESC desc;
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			desc.ByteWidth = sizeof(T) + (16 - (sizeof(T) % 16));
			desc.StructureByteStride = 0;

			const HRESULT hr = device->CreateBuffer(&desc, nullptr, m_Buffer.GetAddressOf());
			PF_CORE_ASSERT(SUCCEEDED(hr), "Failed initialized Constant Buffer");
		}

		~ConstantBuffer() = default;

		void Bind()
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			HRESULT hr = m_Context->Map(Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			CopyMemory(mappedResource.pData, &data, sizeof(data));
			m_Context->Unmap(m_Buffer.Get(), 0);
		}

		inline ID3D11Buffer* Get() const { return m_Buffer.Get(); }
		inline ID3D11Buffer* const* GetAddressOf() const { return m_Buffer.GetAddressOf(); }

	};
}