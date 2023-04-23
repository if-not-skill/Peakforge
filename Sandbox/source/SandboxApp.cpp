#include <Peakforge.h>

class ExampleLayer : public PF::Layer
{
public:
	ExampleLayer()
		: Layer("Example Layer")
	{}

	void OnUpdate() override
	{
	// 	auto[x, y] = PF::Input::Input::GetMousePosition();
	// 	LOG_TRACE("MousePos: {0}, {1}", x, y);
	}
};

class Sandbox : public PF::Application
{
public:
	Sandbox() : Application("Sandbox")
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox() override
	{

	}
};

PF::Application* PF::CreateApplication()
{
	return new Sandbox();
}