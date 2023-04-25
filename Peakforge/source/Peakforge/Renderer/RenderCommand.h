#pragma once
#include "GraphicsContext.h"

namespace PF::Render
{
	class RenderCommand
	{
	public:
		static void Init(void* windowRef)
		{
			s_Context->Init(windowRef);
		}

		static void Clear()
		{
			s_Context->Clear();
		}

		static void Draw()
		{
			s_Context->Draw();
		}

		static void SwapChain()
		{
			s_Context->SwapChain();
		}

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_Context->SetViewport(x, y, width, height);
		}

		static void Suspend()
		{
			s_Context->Suspend();
		}

		static void Resume()
		{
			s_Context->Resume();
		}

		static void* GetNativeRenderRef()
		{
			return s_Context;
		}

	private:
		static GraphicsContext* s_Context;

	};
}
