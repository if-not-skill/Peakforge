#include "pfpch.h"
#include "ImGuiLayer.h"
#include "Peakforge/Core/Application.h"

#include <imgui.h>
#include <imgui_internal.h>

#ifdef PF_PLATFORM_WINDOWS
#include "Platform/Windows/Window/WindowsWindow.h"
#include <backends/imgui_impl_win32.h>
#endif

#include "Peakforge/Renderer/Renderer.h"
#include "Platform/Windows/Direct3D/DXRenderAPI.h"
#include <backends/imgui_impl_dx11.h>

namespace PF::ImGUI
{
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	= default;

	void ImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;
		//io.ConfigViewportsNoDefaultParent = true;
		//io.ConfigDockingAlwaysTabBar = true;
		//io.ConfigDockingTransparentPayload = true;
		//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
		//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.
		
		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		bool success = false;

#ifdef PF_PLATFORM_WINDOWS
		const Application& app = Application::Get();
		const auto window = static_cast<WindowsWindow*>(app.GetWindow().GetNativeWindow());
		success = ImGui_ImplWin32_Init(window->GetHWND());

		if (!success)
		{
			LOG_CORE_ERROR("ImGuiLayer: ImGui_ImplWin32_Init error!");
			return;
		}
#endif

		const auto render = static_cast<Render::DX::Direct3DRendererAPI*>(Render::Renderer::GetNativeRenderRef());
		success = ImGui_ImplDX11_Init
		(
			render->GetD3DDevice(), 
			render->GetD3DContext()
		);

		if (!success)
		{
			LOG_CORE_ERROR("ImGuiLayer: ImGui_ImplDX11_Init error!");
		}

		LOG_CORE_INFO("ImGuiLayer: Initialized");
	}

	void ImGuiLayer::OnDetach()
	{
		// Cleanup
		ImGui_ImplDX11_Shutdown();

#ifdef PF_PLATFORM_WINDOWS
		ImGui_ImplWin32_Shutdown();
#endif

		ImGui::DestroyContext();

		LOG_CORE_INFO("ImGuiLayer: Detached");
	}

	void ImGuiLayer::OnImGuiRender()
	{
		static bool show = true;
		ImGui::ShowDemoWindow(&show);
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		const Application& app = Application::Get();
		const Window& window = app.GetWindow();
		io.DisplaySize = ImVec2(window.GetWidth(), window.GetHeight());

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
#ifdef PF_PLATFORM_WINDOWS
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
#endif
		}
	}
}
