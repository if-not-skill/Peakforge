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
#include "Platform/Windows/Direct3D/DX11Context.h"
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

		const auto render = static_cast<Render::DX::DX11Context*>(Render::Renderer::GetNativeRenderRef());
		success = ImGui_ImplDX11_Init(render->GetD3DDevice(), render->GetD3DContext());
		PF_CORE_ASSERT(success, "ImGuiLayer: ImGui_ImplDX11_Init error!");
		LOG_CORE_INFO("ImGuiLayer: Initialized");
	}

	void ImGuiLayer::OnDetach()
	{
		// Cleanup
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		LOG_CORE_INFO("ImGuiLayer: Detached");
	}

	void ImGuiLayer::OnImGuiRender()
	{
		static bool show_demo_window = true;
		static bool show_another_window = false;

		ImGuiIO& io = ImGui::GetIO(); (void)io;

		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		
		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
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
