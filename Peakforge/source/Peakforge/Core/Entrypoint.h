#pragma once

#if defined(PF_PLATFORM_WINDOWS)

extern PF::Application* PF::CreateApplication();

int main(int argc, char** argv)
{
	PF::Log::Init();
	LOG_CORE_INFO("Loggers Initialized!");

	const auto app = PF::CreateApplication();
	app->Run();
	delete app;

	return 0;
}

#endif