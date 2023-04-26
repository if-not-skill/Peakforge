#include "pfpch.h"
#include "Renderer.h"
#include "RenderCommand.h"


namespace PF::Render
{
#ifdef PF_PLATFORM_WINDOWS
	Renderer::API Renderer::s_API = API::DirectX11;
#endif

	void Renderer::Init(void* windowRef)
	{
		RenderCommand::Init(windowRef);
	}

	void Renderer::Draw()
	{
		RenderCommand::Draw();
	}

	void Renderer::Clear()
	{
		RenderCommand::Clear();
	}

	void Renderer::SwapChain()
	{
		RenderCommand::SwapChain();
	}

	void Renderer::OnWindowResize(int width, int height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::OnSuspend()
	{
		RenderCommand::Suspend();
	}

	void Renderer::OnResume()
	{
		RenderCommand::Resume();
	}

	void* Renderer::GetNativeRenderRef()
	{
		return RenderCommand::GetNativeRenderRef();
	}
}
