#include "pfpch.h"
#include "Application.h"

namespace PF::Core 
{
	Application::Application()
	{
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		Log::Log::Init();
		LOG_CORE_INFO("Loggers Initialized!");

		while (true) 
		{

		}
	}
}