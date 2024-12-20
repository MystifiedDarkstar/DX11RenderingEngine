#pragma once

#include <d3d11_4.h>
#include <DirectXMath.h>

struct Transform
{
public:
	DirectX::XMFLOAT3 Position = DirectX::XMFLOAT3(0,0,0);
	DirectX::XMFLOAT3 Rotation = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 Scale = DirectX::XMFLOAT3(1, 1, 1);
};
