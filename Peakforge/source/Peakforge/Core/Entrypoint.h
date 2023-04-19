#pragma once

#if defined(PF_PLATFORM_WINDOWS)

extern PF::Core::Application* PF::Core::CreateApplication();

int main(int argc, char** argv)
{
	const auto app = PF::Core::CreateApplication();
	app->Run();
	delete app;

	return 0;
}

#endif