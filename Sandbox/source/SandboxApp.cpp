#include <Peakforge.h>

class Sandbox : public PF::Core::Application
{
public:
	Sandbox() 
	{

	}

	~Sandbox() override
	{

	}
};

PF::Core::Application* PF::Core::CreateApplication()
{
	return new Sandbox();
}