#include "pfpch.h"
#include "Application.h"

#include "Layer.h"
#include "Peakforge/Renderer/Renderer.h"

namespace PF
{
#define BIND_EVENT_FN(x, y) std::bind(x, y, std::placeholders::_1)

	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(&Application::OnEvent, this));

		Render::Renderer::Init(m_Window->GetNativeWindow());
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_Running)
		{
			for(const auto layer : m_LayerStack)
			{
				layer->OnUpdate();
			}

			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(&Application::OnWindowClose, this));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(&Application::OnWindowResize, this));

		dispatcher.Dispatch<AppActivateEvent>(BIND_EVENT_FN(&Application::OnAppActivate, this));
		dispatcher.Dispatch<AppDeactivateEvent>(BIND_EVENT_FN(&Application::OnAppDeactivate, this));
		dispatcher.Dispatch<AppSuspendingEvent>(BIND_EVENT_FN(&Application::OnAppSuspending, this));
		dispatcher.Dispatch<AppResumingEvent>(BIND_EVENT_FN(&Application::OnAppResuming, this));
		dispatcher.Dispatch<AppRenderEvent>(BIND_EVENT_FN(&Application::OnAppRender, this));

		for(auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled())
				break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;

		return false;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		Render::Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		return false;
	}

	bool Application::OnAppActivate(AppActivateEvent& e)
	{
		return false;
	}

	bool Application::OnAppDeactivate(AppDeactivateEvent& e)
	{
		return false;
	}

	bool Application::OnAppSuspending(AppSuspendingEvent& e)
	{
		Render::Renderer::OnSuspend();
		return false;
	}

	bool Application::OnAppResuming(AppResumingEvent& e)
	{
		Render::Renderer::OnResume();
		return false;
	}
	bool Application::OnAppRender(AppRenderEvent& e)
	{
		Render::Renderer::OnRender();
		return false;
	}
}