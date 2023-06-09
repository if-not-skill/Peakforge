#include "pfpch.h"
#include "DX11Context.h"

#include "Platform/Windows/Window/WindowsWindow.h"
#include "Peakforge/Renderer/BufferMaker.h"
#include "ConstantBufferTypes.h"

#include <DirectXColors.h>
#include <WICTextureLoader.h>

#define SAMPLE_VERTEX_SHADER L"Engine/Shaders/VertexShader.cso"
#define SAMPLE_PIXEL_SHADER	 L"Engine/Shaders/PixelShader.cso"

#define SAMPLE_TEXTURE L"Engine/Textures/T_Sample.png"

namespace PF::Render::DX
{
	struct SimpleVertex
	{
		DirectX::XMFLOAT3 Pos;
	};

	DX11Context::DX11Context() :
		m_Window(nullptr),
		m_OutputWidth(800),
		m_OutputHeight(600),
		m_FeatureLevel(D3D_FEATURE_LEVEL_9_1)
	{
	}

	DX11Context::~DX11Context()
	{
		Shutdown();
	}

	bool DX11Context::Init(void* windowRef)
	{
		const auto windowsWindow = static_cast<WindowsWindow*>(windowRef);

		PF_CORE_ASSERT(windowsWindow, "ERROR initizile using WinAPI!");

		m_Window = windowsWindow->GetHWND();
		m_OutputWidth = windowsWindow->GetWidth();
		m_OutputHeight = windowsWindow->GetHeight();

		CreateDevice();
		CreateResources();

		ShowAdapterInfo();

		InitializeShaders();
		InitializeScene();
		InitializeRasterizer();

		InitializeSampler();
		InitializeTexture();

		LOG_CORE_INFO("DirectX Initialized");
		return true;
	}

	void DX11Context::Shutdown()
	{
		if (m_RenderTargetView) { m_RenderTargetView->Release(); m_RenderTargetView = nullptr; }
		if (m_SwapChain) { m_SwapChain->Release(); m_SwapChain = nullptr; }
		if (m_D3DContext) { m_D3DContext->Release(); m_D3DContext = nullptr; }
		if (m_D3DDevice) { m_D3DDevice->Release(); m_D3DDevice = nullptr; }

		LOG_CORE_INFO("DirectX Shitdowned");
	}

	void DX11Context::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		m_OutputWidth = std::max<uint32_t>(width, 1);
		m_OutputHeight = std::max<uint32_t>(height, 1);

		CreateResources();
	}

	void DX11Context::Suspend()
	{
		LOG_CORE_WARN("Suspend");
	}

	void DX11Context::Resume()
	{
		LOG_CORE_WARN("Resume");
	}

	void DX11Context::Clear()
	{
		// Clear the views.
		m_D3DContext->ClearRenderTargetView(m_RenderTargetView.Get(), DirectX::Colors::CornflowerBlue);
		m_D3DContext->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		m_D3DContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());
		
		CreateDepthStencilState();

		// Set the viewport.
		D3D11_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<float>(m_OutputWidth), static_cast<float>(m_OutputHeight), 0.f, 1.f };
		m_D3DContext->RSSetViewports(1, &viewport);
	}

	void DX11Context::Draw()
	{
		m_D3DContext->IASetInputLayout(m_VertexShader.GetInputLayout());
		m_D3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_D3DContext->RSSetState(m_RasterizerState.Get());
		m_D3DContext->OMSetDepthStencilState(m_DepthStencilState.Get(), 0);
		m_D3DContext->PSSetSamplers(0, 1, m_SamplerState.GetAddressOf());

		m_D3DContext->VSSetShader(m_VertexShader.GetShader(), NULL, 0);
		m_D3DContext->PSSetShader(m_PixelShader.GetShader(), NULL, 0);
		
		m_ConstantBuffer->data.yOffset -= 0.001f;
		m_ConstantBuffer->Bind();
		m_D3DContext->VSSetConstantBuffers(0, 1, m_ConstantBuffer->GetAddressOf());

		m_D3DContext->PSSetShaderResources(0, 1, m_BaseTexture.GetAddressOf());
		m_D3DContext->DrawIndexed(m_IndexBuffer->GetCount(), 0, 0);
	}

	void DX11Context::SwapChain()
	{
		// The first argument instructs DXGI to block until VSync, putting the application
		// to sleep until the next VSync. This ensures we don't waste any cycles rendering
		// frames that will never be displayed to the screen.
		HRESULT hr = m_SwapChain->Present(1, 0);

		// If the device was reset we must completely reinitialize the renderer.
		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			OnDeviceLost();
		}
		else
		{
			ThrowIfFailed(hr);
		}
	}

	void DX11Context::CreateDevice()
	{
		UINT creationFlags = 0;

#ifdef PF_DEBUG
		//creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		constexpr D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};

		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> context;

		ThrowIfFailed(D3D11CreateDevice
		(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			creationFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			device.ReleaseAndGetAddressOf(),
			&m_FeatureLevel,
			context.ReleaseAndGetAddressOf()
		));


		ComPtr<ID3D11Debug> d3dDebug;
		if (SUCCEEDED(device.As(&d3dDebug)))
		{
			ComPtr<ID3D11InfoQueue> d3dInfoQueue;
			if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
			{
#ifndef PF_DEBUG
				d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
				d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
				D3D11_MESSAGE_ID hide[] =
				{
					D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
					// TODO: Add more message IDs here as needed.
				};
				D3D11_INFO_QUEUE_FILTER filter = {};
				filter.DenyList.NumIDs = ARRAYSIZE(hide);
				filter.DenyList.pIDList = hide;
				d3dInfoQueue->AddStorageFilterEntries(&filter);
			}
		}

		ThrowIfFailed(device.As(&m_D3DDevice));
		ThrowIfFailed(context.As(&m_D3DContext));
	}

	void DX11Context::CreateResources()
	{
		// Clear the previous window size specific context.
		m_D3DContext->OMSetRenderTargets(0, nullptr, nullptr);
		m_RenderTargetView.Reset();
		m_DepthStencilView.Reset();
		m_D3DContext->Flush();

		const UINT backBufferWidth = static_cast<UINT>(m_OutputWidth);
		const UINT backBufferHeight = static_cast<UINT>(m_OutputHeight);
		const DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
		const DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		constexpr UINT backBufferCount = 2;

		// If the swap chain already exists, resize it, otherwise create one.
		if (m_SwapChain)
		{
			HRESULT hr = m_SwapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

			if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
			{
				// If the device was removed for any reason, a new device and swap chain will need to be created.
				OnDeviceLost();

				// Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
				// and correctly set up the new device.
				return;
			}

			ThrowIfFailed(hr);
		}
		else
		{
			// First, retrieve the underlying DXGI Device from the D3D Device.
			ComPtr<IDXGIDevice1> dxgiDevice;
			ThrowIfFailed(m_D3DDevice.As(&dxgiDevice));

			// Identify the physical adapter (GPU or card) this device is running on.
			ComPtr<IDXGIAdapter> dxgiAdapter;
			ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

			ThrowIfFailed(dxgiAdapter.As(&m_Adapter));

			// And obtain the factory object that created it.
			ComPtr<IDXGIFactory2> dxgiFactory;
			ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

			// Create a descriptor for the swap chain.
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
			swapChainDesc.Width = backBufferWidth;
			swapChainDesc.Height = backBufferHeight;
			swapChainDesc.Height = backBufferHeight;
			swapChainDesc.Format = backBufferFormat;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = backBufferCount;

			DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
			fsSwapChainDesc.Windowed = TRUE;

			// Create a SwapChain from a Win32 window.
			ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
				m_D3DDevice.Get(),
				m_Window,
				&swapChainDesc,
				&fsSwapChainDesc,
				nullptr,
				m_SwapChain.ReleaseAndGetAddressOf()
			));

			// This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
			// ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_Window, DXGI_MWA_NO_ALT_ENTER));
		}

		// Obtain the backbuffer for this window which will be the final 3D rendertarget.
		ComPtr<ID3D11Texture2D> backBuffer;
		ThrowIfFailed(m_SwapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

		// Create a view interface on the rendertarget to use on bind.
		ThrowIfFailed(m_D3DDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_RenderTargetView.ReleaseAndGetAddressOf()));

		// Allocate a 2-D surface as the depth/stencil buffer and
		// create a DepthStencil view on this surface to use on bind.
		CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);
		
		// 
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = m_OutputWidth;
		desc.Height = m_OutputHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		HRESULT hr = m_D3DDevice->CreateTexture2D(&desc, NULL, m_DepthStencilBuffer.GetAddressOf());
		PF_CORE_ASSERT(SUCCEEDED(hr), "Failed CreateTexture2D");

		hr = m_D3DDevice->CreateDepthStencilView(m_DepthStencilBuffer.Get(), NULL, m_DepthStencilView.GetAddressOf());
		PF_CORE_ASSERT(SUCCEEDED(hr), "Failed CreateDepthStencilView");

		// TODO: Initialize windows-size dependent objects here.
	}

	void DX11Context::CreateDepthStencilState()
	{
		// Create depth stencil state
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

		HRESULT hr = m_D3DDevice->CreateDepthStencilState(&desc, m_DepthStencilState.GetAddressOf());
		PF_CORE_ASSERT(SUCCEEDED(hr), "Failed CreateDepthStencilState");
	}

	void DX11Context::OnDeviceLost()
	{
		m_DepthStencilView.Reset();
		m_RenderTargetView.Reset();
		m_SwapChain.Reset();
		m_D3DContext.Reset();
		m_D3DDevice.Reset();
		m_Adapter.Reset();

		CreateDevice();

		CreateResources();
	}

	void DX11Context::ShowAdapterInfo() const
	{
		auto adapterDescription = DXGI_ADAPTER_DESC2();
		m_Adapter->GetDesc2(&adapterDescription);
		
		auto temp = std::wstring(adapterDescription.Description);
		std::string videoAdapterName(temp.begin(), temp.end());
		
		LOG_CORE_TRACE("VideoAdapter Initialized:");
		LOG_CORE_TRACE("\tRenderer: {0}", videoAdapterName);
		LOG_CORE_TRACE("\tVideo Memory: {0} MB", adapterDescription.DedicatedVideoMemory / (1024 * 1024));
	}

	void DX11Context::InitializeShaders()
	{
		LOG_CORE_TRACE("Shaders Initialization:");

		InitializeVertexShader();
		InitializePixelShader();
	}

	void DX11Context::InitializeVertexShader()
	{
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			//{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		m_VertexShader.Initialize
		(
			m_D3DDevice,
			SAMPLE_VERTEX_SHADER,
			layout,
			ARRAYSIZE(layout)
		);
	}

	void DX11Context::InitializePixelShader()
	{
		m_PixelShader.Initialize(m_D3DDevice, SAMPLE_PIXEL_SHADER);
	}

	void DX11Context::InitializeScene()
	{
		// first triangle
		Vertex vertexes[] =
		{
			Vertex(-0.5f, -0.5f, 1.0f, 0.0f, 1.0f),
			Vertex(-0.5f, 0.5f, 1.0f, 0.0f, 0.0f),
			Vertex(0.5f, 0.5f, 1.0f, 1.0f, 0.0f),
			Vertex(0.5f, -0.5f, 1.0f, 1.0f, 1.0f),
		};
		m_VertexBuffer = CreateVertexBuffer<Vertex>(vertexes, ARRAYSIZE(vertexes));

		DWORD indices[] =
		{
			0, 1, 2,
			0, 2, 3
		};
		m_IndexBuffer = CreateIndexBuffer<DWORD>(indices, ARRAYSIZE(indices));
		
		m_ConstantBuffer = new ConstantBuffer<CB_VS_VertexShader>();

		m_VertexBuffer->Bind();
		m_IndexBuffer->Bind();
		m_ConstantBuffer->Bind();
		m_ConstantBuffer->data.xOffset = 0.f;
		
		LOG_CORE_TRACE("Scene Initialized");
	}

	void DX11Context::InitializeRasterizer()
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
		
		const HRESULT hr = m_D3DDevice->CreateRasterizerState(&desc, m_RasterizerState.GetAddressOf());
		PF_CORE_ASSERT(SUCCEEDED(hr), "Failed CreateRasterizerState");
		LOG_CORE_TRACE("Rasterizer Initialized");
	}

	void DX11Context::InitializeSampler()
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;

		HRESULT hr = m_D3DDevice->CreateSamplerState(&desc, m_SamplerState.GetAddressOf());
		PF_CORE_ASSERT(SUCCEEDED(hr), "Failed CreateSamplerState");
		LOG_CORE_TRACE("Sampler Initialized");
	}

	void DX11Context::InitializeTexture()
	{
		const HRESULT hr = DirectX::CreateWICTextureFromFile(m_D3DDevice.Get(), SAMPLE_TEXTURE, nullptr, m_BaseTexture.GetAddressOf());
		if (FAILED(hr)) LOG_CORE_WARN("Failed CreateWICTextureFromFile");
	}
}
