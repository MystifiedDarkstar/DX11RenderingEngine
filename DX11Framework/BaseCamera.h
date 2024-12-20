#pragma once

#include <DirectXMath.h>
#include <windows.h>

#include "CameraTypes.h"

using namespace DirectX;

class BaseCamera
{
public:
	BaseCamera() = default;
	BaseCamera(XMFLOAT3 Position, XMFLOAT3 Fwd, XMFLOAT3 Up, float WndWidth, float WndHeight, float NearDepth, float FarDepth, CameraTypes Type);
	~BaseCamera();

	virtual void Update(float DeltaTime) {}

	CameraTypes GetType() { return m_Type; }

	XMFLOAT3 GetFwd();
	XMFLOAT3 GetEye();
	XMFLOAT3 GetUp();

	void SetFwd(XMFLOAT3 Fwd);
	void SetEye(XMFLOAT3 Eye);
	void SetUp(XMFLOAT3 Up);

	void SetNearDepth(float Near);
	void SetFarDepth(float Far);
	void SetWindowHeight(float WndHeight);
	void SetWindowWidth(float WndWidth);

	void SetActiveCamera();
	void DisableCamera();

	const XMFLOAT4X4* CreateProjectionMatrix();
	virtual const XMFLOAT4X4* CreateViewMatrix();

protected:
	XMFLOAT4X4 m_View;

	XMFLOAT3 m_EyePos;
	XMFLOAT3 m_FwdVector;
	XMFLOAT3 m_UpVector;

private:
	float m_WindowWidth;
	float m_WindowHeight;
	float m_NearDepth;
	float m_FarDepth;

	XMFLOAT4X4 m_Projection;

	float m_AspectRatio;

	bool m_IsActive;

	CameraTypes m_Type;
};

