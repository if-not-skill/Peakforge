#pragma once

namespace PF::Render
{

	class Renderer
	{
	public:
		static void Init(void* windowRef);

		static void Clear();
		static void Draw();
		static void SwapChain();

		static void OnWindowResize(int width, int height);

		static void OnSuspend();
		static void OnResume();

		static void* GetNativeRenderRef();
	};

}