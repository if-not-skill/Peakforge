#pragma once

namespace PF::Render
{

	class Renderer
	{
	public:
		static void Init(void* windowRef);
		static void Shutdown();

		static void OnClear();
		static void OnSwapChain();

		static void OnWindowResize(int width, int height);

		static void OnSuspend();
		static void OnResume();

		static void* GetNativeRenderRef();
	};

}