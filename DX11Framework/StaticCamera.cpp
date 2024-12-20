#include "StaticCamera.h"

StaticCamera::StaticCamera(XMFLOAT3 Position, XMFLOAT3 LookAt, XMFLOAT3 Up, float WndWidth, float WndHeight, float NearDepth, float FarDepth) : 
	BaseCamera(Position, LookAt, Up, WndWidth, WndHeight, NearDepth, FarDepth, CT_Static)
{

}

StaticCamera::~StaticCamera()
{

}

void StaticCamera::Update(float DeltaTime)
{
}

const XMFLOAT4X4* StaticCamera::CreateViewMatrix()
{
	//Create a view matrix in left hand coordinate system.
    XMStoreFloat4x4(&m_View, XMMatrixLookAtLH(XMLoadFloat3(&m_EyePos), XMLoadFloat3(&m_FwdVector), XMLoadFloat3(&m_UpVector)));
	return &m_View;
}
