#include "pfpch.h"
#include "GraphicsContext.h"

#include "Platform/Windows/Direct3D/DX11Context.h"

namespace PF::Render
{
#ifdef PF_PLATFORM_WINDOWS
	GraphicsContext::API GraphicsContext::s_API = API::DirectX11;
#endif

	GraphicsContext* GraphicsContext::Create()
	{
		switch (s_API)
		{
		case API::None:
		{
			PF_CORE_ASSERT(false, "RernderAPI::None is currently not supported!");
			return nullptr;
		}
		case API::DirectX11:
		{
			return new DX::DX11Context();
		}
		}

		PF_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}