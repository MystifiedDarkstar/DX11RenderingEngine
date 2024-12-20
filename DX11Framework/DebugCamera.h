#pragma once

#include "BaseCamera.h"

class DebugCamera : public BaseCamera
{
public:
	DebugCamera() = default;
	DebugCamera(XMFLOAT3 Position, XMFLOAT3 LookTo, XMFLOAT3 Up, float WndWidth, float WndHeight, float NearDepth, float FarDepth);
	~DebugCamera();

	void Update(float DeltaTime) override;
	const XMFLOAT4X4* CreateViewMatrix() override;

	void AdjustPosition(XMVECTOR PosAdjustment);
	void AdjustRotation(XMVECTOR RotAdjustment);

	const XMVECTOR& GetUpVector() { return m_UpVec; }
	const XMVECTOR& GetDownVector() { return m_DownVec; }
	const XMVECTOR& GetForwardVector() { return m_ForwardVec; }
	const XMVECTOR& GetBackwardVector() { return m_BackwardVec; }
	const XMVECTOR& GetLeftVector() { return m_LeftVec; }
	const XMVECTOR& GetRightVector() { return m_RightVec; }

private:
	XMVECTOR m_CamRot = { 0,0,0 };

	XMVECTOR m_ForwardVec = {0,0,1.0f};
	XMVECTOR m_BackwardVec = {0, 0, -1.0f };
	XMVECTOR m_LeftVec = {-1, 0, 0 };
	XMVECTOR m_RightVec = {1, 0, 0 };
	XMVECTOR m_UpVec = {0, 1, 0};
	XMVECTOR m_DownVec = {0, -1, 0};

	const XMVECTOR m_DEFAULTFWDVECTOR = {0,0,1.0f};
	const XMVECTOR m_DEFAULTBACKVECTOR = {0,0,-1.0f};
	const XMVECTOR m_DEFAULTLEFTVECTOR = {-1.0f,0,0};
	const XMVECTOR m_DEFAULTRIGHTVECTOR = {1.0f,0,0};
	const XMVECTOR m_DEFAULTUPVECTOR = {0,1.0f, 0};
	const XMVECTOR m_DEFAULTDOWNVECTOR = {0,-1.0f,0};
};

