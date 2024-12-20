#pragma once

#include "BaseCamera.h"

class StaticCamera : public BaseCamera
{
public:
	StaticCamera() = default;
	StaticCamera(XMFLOAT3 Position, XMFLOAT3 LookAt, XMFLOAT3 Up, float WndWidth, float WndHeight, float NearDepth, float FarDepth);
	~StaticCamera();

	void Update(float DeltaTime) override;

	const XMFLOAT4X4* CreateViewMatrix() override;
};

