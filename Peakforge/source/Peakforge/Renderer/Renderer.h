#pragma once

namespace PF::Render
{
	class Renderer
	{
	public:
		enum class API
		{
			None = 0, DirectX11 = 1
		};

	public:
		static void Init(void* windowRef);

		static void Clear();
		static void Draw();
		static void SwapChain();

		static void OnWindowResize(int width, int height);

		static void OnSuspend();
		static void OnResume();

		static void* GetNativeRenderRef();

		static API GetAPI() { return s_API; }

	private:
		static API s_API;
	};

}