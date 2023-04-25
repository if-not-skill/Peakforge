#pragma once

#include <DirectXMath.h>

namespace PF::Render::DX
{
	struct Vertex
	{
		Vertex() = default;
		Vertex(float x, float y, float z, float r, float g, float b) : 
			pos(x, y, z),
			color(r, g, b)
		{}

		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
	};
}