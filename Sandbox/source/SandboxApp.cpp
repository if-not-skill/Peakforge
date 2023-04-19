#include <Peakforge.h>

class Sandbox : public PF::Application
{
public:
	Sandbox() 
	{

	}

	~Sandbox() override
	{

	}
};

PF::Application* PF::CreateApplication()
{
	return new Sandbox();
}