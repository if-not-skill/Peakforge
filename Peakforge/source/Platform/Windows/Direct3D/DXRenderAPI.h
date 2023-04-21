#pragma once

#include "Peakforge/Renderer/RenderAPI.h"

#include <d3d11.h>
#include <d3d11_1.h>

#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace PF::Render::DX
{
	class Direct3DRendererAPI final : public RenderAPI
	{
	public:
		Direct3DRendererAPI();
		~Direct3DRendererAPI() override;

		// Inherited via RenderAPI
		bool Init(void* windowRef) override;
		void Render() override;

		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		void Suspend() override;
		void Resume() override;

	private:
		void Clear();
		void Present();

		void CreateDevice();
		void CreateResources();

		void OnDeviceLost();

		void ShowAdapterInfo() const;

	private:
		HWND							m_Window;
		int								m_OutputWidth;
		int								m_OutputHeight;

		D3D_FEATURE_LEVEL				m_FeatureLevel;
		ComPtr<ID3D11Device1>			m_D3DDevice;
		ComPtr<ID3D11DeviceContext1>	m_D3DContext;
		
		ComPtr<IDXGISwapChain1>         m_SwapChain;
		ComPtr<ID3D11RenderTargetView>  m_RenderTargetView;
		ComPtr<ID3D11DepthStencilView>  m_DepthStencilView;

		ComPtr<IDXGIAdapter>			m_Adapter;

	};
}