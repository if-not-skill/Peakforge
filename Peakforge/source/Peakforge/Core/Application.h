#pragma once

#include "Window.h"
#include "LayerStack.h"
#include "Peakforge/Events/ApplicationEvent.h"

#include "Peakforge/ImGui/ImGuiLayer.h"

namespace PF
{
	class Layer;

	class Application 
	{
	public:
		Application(const std::string& title = "Peakforge");
		virtual ~Application();

		void Run();
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		static inline Application& Get() { return *s_Instance; }
		inline Window& GetWindow() const { return *m_Window; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		bool OnAppActivate(AppActivateEvent& e);
		bool OnAppDeactivate(AppDeactivateEvent& e);
		bool OnAppSuspending(AppSuspendingEvent& e);
		bool OnAppResuming(AppResumingEvent& e);

	private:
		std::unique_ptr<Window> m_Window;

		bool m_Running = true;

		LayerStack m_LayerStack;
		ImGUI::ImGuiLayer* m_ImGuiLayer;

	private:
		static Application* s_Instance;
	};

	Application* CreateApplication();
}