#pragma once

#include "pfpch.h"
#include "Buffer.h"
#include "Renderer.h"

#ifdef PF_PLATFORM_WINDOWS
#include "Platform/Windows/Direct3D/DX11VertexBuffer.h"
#include "Platform/Windows/Direct3D/D3D11IndexBuffer.h"
#endif

namespace PF::Render
{
	template<typename T>
	static VertexBuffer<T>* CreateVertexBuffer(T* vertices, uint32_t num)
	{
		auto api = Renderer::GetAPI();
		switch (api)
		{
		case Renderer::API::None: PF_CORE_ASSERT(false, "RenderAPI: None is currently not supported!"); return nullptr;

#ifdef PF_PLATFORM_WINDOWS
		case Renderer::API::DirectX11: return new DX::DX11VertexBuffer<T>(vertices, num);
#endif
		}

		PF_CORE_ASSERT(false, "Unknown Render API");
		return nullptr;
	}

	template<typename T>
	static IndexBuffer<T>* CreateIndexBuffer(T* indices, uint32_t num)
	{
		auto api = Renderer::GetAPI();
		switch (api)
		{
		case Renderer::API::None: PF_CORE_ASSERT(false, "RenderAPI: None is currently not supported!"); return nullptr;

#ifdef PF_PLATFORM_WINDOWS
		case Renderer::API::DirectX11: return new DX::DX11IndexBuffer<T>(indices, num);
#endif
		}

		PF_CORE_ASSERT(false, "Unknown Render API");
		return nullptr;
	}
}