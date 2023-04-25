#pragma once

#include <DirectXMath.h>

namespace PF::Render::DX
{
	struct Vertex
	{
		Vertex() = default;
		Vertex(float x, float y, float z, float u, float v) : /*, float r, float g, float b*/ 
			pos(x, y, z),
			texCoord(u, v)
			//color(r, g, b)
		{}

		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 texCoord;
		//DirectX::XMFLOAT3 color;
	};
}