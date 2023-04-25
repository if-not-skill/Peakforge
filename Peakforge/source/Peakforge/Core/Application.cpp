#include "pfpch.h"
#include "Application.h"

#include "Layer.h"
#include "Peakforge/Renderer/Renderer.h"

namespace PF
{
#define BIND_EVENT_FN(x, y) std::bind(x, y, std::placeholders::_1)
	
	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& title)
	{
		PF_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		WindowProps props;
		props.Title = title;
		props.Width = 800;
		props.Height = 600;
		
		m_Window = std::unique_ptr<Window>(Window::Create(props));
		m_Window->SetEventCallback(BIND_EVENT_FN(&Application::OnEvent, this));

		Render::Renderer::Init(m_Window->GetNativeWindow());
		
		m_ImGuiLayer = new ImGUI::ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application() {}

	void Application::Run()
	{
		while (m_Running)
		{
			m_Window->OnUpdate();

			if (!m_Running)
				break;

			Render::Renderer::Clear();

			Render::Renderer::Draw();

			for(auto layer : m_LayerStack)
				layer->OnUpdate();

			m_ImGuiLayer->Begin();
			for (auto layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			Render::Renderer::SwapChain();
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
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
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
}