#pragma once
#include "RenderAPI.h"

namespace PF::Render
{
	class RenderCommand
	{
	public:
		static void Init(void* windowRef)
		{
			s_RenderAPI->Init(windowRef);
		}

		static void Render()
		{
			s_RenderAPI->Render();
		}

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RenderAPI->SetViewport(x, y, width, height);
		}

		static void Suspend()
		{
			s_RenderAPI->Suspend();
		}

		static void Resume()
		{
			s_RenderAPI->Resume();
		}

	private:
		static RenderAPI* s_RenderAPI;

	};
}
