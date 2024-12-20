#include "DebugCamera.h"

DebugCamera::DebugCamera(XMFLOAT3 Position, XMFLOAT3 LookTo, XMFLOAT3 Up, float WndWidth, float WndHeight, float NearDepth, float FarDepth) : 
    BaseCamera(Position, LookTo, Up, WndWidth, WndHeight, NearDepth, FarDepth, CT_Debug)
{

}

DebugCamera::~DebugCamera()
{
}

void DebugCamera::Update(float DeltaTime)
{
    const float cameraSpeed = 0.15f;
    const float rotationSpeed = 0.02f;

    if (GetAsyncKeyState(87) & 0x0001) // W
    {
        AdjustPosition(GetForwardVector() * cameraSpeed);
    }
    if (GetAsyncKeyState(83) & 0x0001) // S
    {
        AdjustPosition(GetBackwardVector() * cameraSpeed);
    }
    if (GetAsyncKeyState(65) & 0x0001) // A
    {
        AdjustPosition(GetLeftVector() * cameraSpeed);
    }
    if (GetAsyncKeyState(68) & 0x0001) // D
    {
        AdjustPosition(GetRightVector() * cameraSpeed);
    }
    if (GetAsyncKeyState(VK_SPACE) & 0x0001) // SPACE
    {
       AdjustPosition(GetUpVector() * cameraSpeed);
    }
    if (GetAsyncKeyState(90) & 0x0001)
    {
        AdjustPosition(GetDownVector() * cameraSpeed);
    }

    if (GetAsyncKeyState(81) & 0x0001)
    {
        XMVECTOR Temp = { 0.0f, -1 * rotationSpeed, 0.0f };
        AdjustRotation(Temp);
    }

    if (GetAsyncKeyState(69) & 0x0001)
    {
        XMVECTOR Temp = { 0.0f, 1 * rotationSpeed, 0.0f };
        AdjustRotation(Temp);
    }

    if (GetAsyncKeyState(82) & 0x0001)
    {
        XMVECTOR Temp = { -1 * rotationSpeed, 0.0f, 0.0f };
        AdjustRotation(Temp);
    }

    if (GetAsyncKeyState(70) & 0x001)
    {
        XMVECTOR Temp = { 1 * rotationSpeed, 0.0f, 0.0f };
        AdjustRotation(Temp);
    }
}

const XMFLOAT4X4* DebugCamera::CreateViewMatrix()
{
    XMVECTOR PosVector = XMLoadFloat3(&m_EyePos);

    XMMATRIX CamRotationMatrix = XMMatrixRotationRollPitchYawFromVector(m_CamRot);

    XMVECTOR CamTarget = XMVector3TransformCoord(m_DEFAULTFWDVECTOR, CamRotationMatrix);

    CamTarget += PosVector;

    XMVECTOR UpDir = XMVector3TransformCoord(m_DEFAULTUPVECTOR, CamRotationMatrix);

    //Create a view matrix in left hand coordinate system.
    XMStoreFloat4x4(&m_View, XMMatrixLookToLH(XMLoadFloat3(&m_EyePos), m_ForwardVec, m_UpVec)); 

    XMMATRIX VecRotationMatrix = XMMatrixRotationRollPitchYawFromVector(m_CamRot);

    m_ForwardVec = XMVector3TransformCoord(m_DEFAULTFWDVECTOR, VecRotationMatrix);
    m_BackwardVec = XMVector3TransformCoord(m_DEFAULTBACKVECTOR, VecRotationMatrix);
    m_LeftVec = XMVector3TransformCoord(m_DEFAULTLEFTVECTOR, VecRotationMatrix);
    m_RightVec = XMVector3TransformCoord(m_DEFAULTRIGHTVECTOR, VecRotationMatrix);
    m_UpVec = XMVector3TransformCoord(m_DEFAULTUPVECTOR, VecRotationMatrix);
    m_DownVec = XMVector3TransformCoord(m_DEFAULTDOWNVECTOR, VecRotationMatrix);

    return &m_View;
}

void DebugCamera::AdjustPosition(XMVECTOR PosAdjustment)
{
    XMFLOAT3 PosAdjust;
    XMStoreFloat3(&PosAdjust, PosAdjustment);
    m_EyePos.x += PosAdjust.x;
    m_EyePos.y += PosAdjust.y;
    m_EyePos.z += PosAdjust.z;
    CreateViewMatrix();
}

void DebugCamera::AdjustRotation(XMVECTOR RotAdjustment)
{
    m_CamRot += RotAdjustment;
    CreateViewMatrix();
}

