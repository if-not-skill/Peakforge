#include "pfpch.h"
#include "DX11Context.h"

#include "Platform/Windows/Window/WindowsWindow.h"

#include <DirectXColors.h>

#define SAMPLE_VERTEX_SHADER L"Engine/Shaders/VertexShader.cso"
#define SAMPLE_PIXEL_SHADER	 L"Engine/Shaders/PixelShader.cso"

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
	= default;

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

		return true;
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

		// Set the viewport.
		D3D11_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<float>(m_OutputWidth), static_cast<float>(m_OutputHeight), 0.f, 1.f };
		m_D3DContext->RSSetViewports(1, &viewport);
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

		ComPtr<ID3D11Texture2D> depthStencil;
		ThrowIfFailed(m_D3DDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

		ThrowIfFailed(m_D3DDevice->CreateDepthStencilView(depthStencil.Get(), nullptr, m_DepthStencilView.ReleaseAndGetAddressOf()));

		// TODO: Initialize windows-size dependent objects here.
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
		
		LOG_CORE_INFO("DirectX11 Initialized:");
		LOG_CORE_INFO("\tRenderer: {0}", videoAdapterName);
		LOG_CORE_INFO("\tVideo Memory: {0} MB", adapterDescription.DedicatedVideoMemory / (1024 * 1024));
	}

	void DX11Context::InitializeShaders()
	{
		LOG_CORE_INFO("Shaders Initialization:");

		InitializeVertexShader();
		initializePixelShader();
	}

	void DX11Context::InitializeVertexShader()
	{
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{
				"POSITION",
				0,
				DXGI_FORMAT_R32G32_FLOAT,
				0,
				0,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			}
		};

		m_VertexShader.Initialize
		(
			m_D3DDevice,
			SAMPLE_VERTEX_SHADER,
			layout,
			ARRAYSIZE(layout)
		);
	}

	void DX11Context::initializePixelShader()
	{
		m_PixelShader.Initialize(m_D3DDevice, SAMPLE_PIXEL_SHADER);
	}
}
