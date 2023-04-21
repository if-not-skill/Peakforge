#pragma once

namespace PF::Render
{

	class Renderer
	{
	public:
		static void Init(void* windowRef);
		static void Shutdown();

		static void OnWindowResize(int width, int height);

		static void OnRender();
		static void OnSuspend();
		static void OnResume();
	};

}