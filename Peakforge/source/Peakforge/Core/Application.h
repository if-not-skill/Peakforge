#pragma once

#include "Window.h"
#include "LayerStack.h"
#include "Peakforge/Events/ApplicationEvent.h"

#include "Peakforge/Renderer/RenderAPI.h"

namespace PF
{
	class Layer;

	class Application 
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		bool OnAppActivate(AppActivateEvent& e);
		bool OnAppDeactivate(AppDeactivateEvent& e);
		bool OnAppSuspending(AppSuspendingEvent& e);
		bool OnAppResuming(AppResumingEvent& e);
		bool OnAppRender(AppRenderEvent& e);

	private:
		std::unique_ptr<Window> m_Window;

		bool m_Running = true;

		LayerStack m_LayerStack;
	};

	Application* CreateApplication();
}