#include "pfpch.h"
#include "Renderer.h"
#include "RenderCommand.h"


namespace PF::Render
{
	void Renderer::Init(void* windowRef)
	{
		RenderCommand::Init(windowRef);
	}

	void Renderer::Shutdown()
	{
	}

	void Renderer::OnClear()
	{
		RenderCommand::Clear();
	}

	void Renderer::OnSwapChain()
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
