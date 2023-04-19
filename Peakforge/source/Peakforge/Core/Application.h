#pragma once

namespace PF
{
	class Application 
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	Application* CreateApplication();
}