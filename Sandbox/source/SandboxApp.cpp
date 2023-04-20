#include <Peakforge.h>

class ExampleLayer : public PF::Layer
{
public:
	ExampleLayer()
		: Layer("Example Layer")
	{}

	void OnUpdate() override
	{
		// LOG_TRACE("ExampleLayer: OnUpdate");
	}
};

class Sandbox : public PF::Application
{
public:
	Sandbox() 
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