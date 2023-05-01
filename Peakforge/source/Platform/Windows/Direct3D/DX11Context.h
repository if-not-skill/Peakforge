#pragma once

#include "Peakforge/Renderer/GraphicsContext.h"
#include "Peakforge/Renderer/Buffer.h"

#include "ConstantBuffer.h"
#include "Shaders.h"
#include "Vertex.h"

#include <d3d11.h>
#include <d3d11_1.h>

#include <wrl/client.h>


using Microsoft::WRL::ComPtr;

namespace PF::Render::DX
{
	class DX11Context final : public GraphicsContext
	{
	public:
		DX11Context();
		~DX11Context() override;

		// Inherited via GraphicsContext
		bool Init(void* windowRef) override;

		void Clear() override;
		void Draw() override;
		void SwapChain() override;

		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		void Suspend() override;
		void Resume() override;

		inline ID3D11Device* GetD3DDevice() const { return m_D3DDevice.Get(); }
		inline ID3D11DeviceContext* GetD3DContext() const { return m_D3DContext.Get(); }

	private:
		void Shutdown();

		void CreateDevice();
		void CreateResources();

		void CreateDepthStencilState();

		void OnDeviceLost();

		void ShowAdapterInfo() const;

	private:
		HWND								m_Window;
		int									m_OutputWidth;
		int									m_OutputHeight;

		D3D_FEATURE_LEVEL					m_FeatureLevel;
		ComPtr<ID3D11Device1>				m_D3DDevice;
		ComPtr<ID3D11DeviceContext1>		m_D3DContext;
		
		ComPtr<IDXGISwapChain1>				m_SwapChain;
		ComPtr<ID3D11RenderTargetView>		m_RenderTargetView;

		ComPtr<ID3D11DepthStencilView>		m_DepthStencilView;
		ComPtr<ID3D11Texture2D>				m_DepthStencilBuffer;
		ComPtr<ID3D11DepthStencilState>		m_DepthStencilState;

		ComPtr<IDXGIAdapter2>				m_Adapter;

	private:
		void InitializeShaders();
		void InitializeVertexShader();
		void InitializePixelShader();

		void InitializeScene();
		void InitializeRasterizer();
		void InitializeSampler();
		void InitializeTexture();

	private:
		VertexShader						m_VertexShader;
		PixelShader							m_PixelShader;

		VertexBuffer<Vertex>*				m_VertexBuffer;
		IndexBuffer<DWORD>*					m_IndexBuffer;
		ConstantBuffer<CB_VS_VertexShader>*	m_ConstantBuffer;


		ComPtr<ID3D11RasterizerState>		m_RasterizerState;

		ComPtr<ID3D11SamplerState>			m_SamplerState;
		ComPtr<ID3D11ShaderResourceView>	m_BaseTexture;
	};
}