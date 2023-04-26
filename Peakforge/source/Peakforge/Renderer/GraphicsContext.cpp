#include "pfpch.h"
#include "GraphicsContext.h"

#include "Platform/Windows/Direct3D/DX11Context.h"
#include "Renderer.h"

namespace PF::Render
{
	GraphicsContext* GraphicsContext::Create()
	{
		Renderer::API api = Renderer::GetAPI();
		switch (api)
		{
		case Renderer::API::None: PF_CORE_ASSERT(false, "RernderAPI::None is currently not supported!"); return nullptr;

#ifdef PF_PLATFORM_WINDOWS
		case Renderer::API::DirectX11: return new DX::DX11Context();
#endif
		}

		PF_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}