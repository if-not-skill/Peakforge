#pragma once

namespace PF::Render
{
	class GraphicsContext 
	{
		enum class API
		{
			None = 0, DirectX11 = 1
		};

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

		static API GetAPI() { return s_API; }
		static GraphicsContext* Create();

	private:
		static API s_API;

	};
}