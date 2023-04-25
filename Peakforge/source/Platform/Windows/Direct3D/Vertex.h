#pragma once

#include <DirectXMath.h>

namespace PF::Render::DX
{
	struct Vertex
	{
		Vertex() = default;
		Vertex(float x, float y) : pos(x, y) {}

		DirectX::XMFLOAT2 pos;
	};
}