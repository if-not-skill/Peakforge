#include "pfpch.h"
#include "RenderAPI.h"

#include "Platform/Windows/Direct3D/DXRenderAPI.h"

namespace PF::Render
{
#ifdef PF_PLATFORM_WINDOWS
	RenderAPI::API RenderAPI::s_API = API::DirectX;
#endif

	RenderAPI* RenderAPI::Create()
	{
		switch (s_API)
		{
		case API::None:
		{
			PF_CORE_ASSERT(false, "RernderAPI::None is currently not supported!");
			return nullptr;
		}
		case API::DirectX:
		{
			return new DX::Direct3DRendererAPI();
		}
		}

		PF_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}