#pragma once

namespace PF::Render
{
	class GraphicsContext 
	{
	public:
		GraphicsContext() = default;
		virtual ~GraphicsContext() = default;

		virtual bool Init(void* windowRef) = 0;

		virtual void Clear() = 0;
		virtual void Draw() = 0;
		virtual void SwapChain() = 0;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void Suspend() = 0;
		virtual void Resume() = 0;

		static GraphicsContext* Create();

	};
}