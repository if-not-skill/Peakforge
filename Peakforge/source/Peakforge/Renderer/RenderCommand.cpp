#include "pfpch.h"
#include "RenderCommand.h"

namespace PF::Render
{
	RenderAPI* RenderCommand::s_RenderAPI = RenderAPI::Create();
}