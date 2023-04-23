#include "pfpch.h"
#include "RenderCommand.h"

namespace PF::Render
{
	GraphicsContext* RenderCommand::s_Context = GraphicsContext::Create();
}