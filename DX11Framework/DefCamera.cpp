#include "DefCamera.h"

DefCamera::DefCamera(XMFLOAT3 Position, XMFLOAT3 Fwd, XMFLOAT3 Up, float WndWidth, float WndHeight, float NearDepth, float FarDepth) :
    m_EyePos(Position), m_FwdVector(Fwd), m_UpVector(Up), 
    m_WindowWidth(WndWidth), m_WindowHeight(WndHeight),
    m_NearDepth(NearDepth), m_FarDepth(FarDepth)
{
    m_AspectRatio = m_WindowWidth / m_WindowHeight; // create aspect  ratio
    GetProj();
    GetView();
}

DefCamera::~DefCamera()
{
}

void DefCamera::Update(float DeltaTime)
{

}

XMFLOAT3 DefCamera::GetFwd()
{
    return m_FwdVector;
}

XMFLOAT3 DefCamera::GetEye()
{
    return m_EyePos;
}

XMFLOAT3 DefCamera::GetUp()
{
    return m_UpVector;
}

void DefCamera::SetFwd(XMFLOAT3 Fwd)
{
    m_FwdVector = Fwd;
}

void DefCamera::SetEye(XMFLOAT3 Eye)
{
    m_EyePos = Eye;
}

void DefCamera::SetUp(XMFLOAT3 Up)
{
    m_UpVector = Up;
}

void DefCamera::SetNearDepth(float Near)
{
    m_NearDepth = Near;
}

void DefCamera::SetFarDepth(float Far)
{
    m_FarDepth = Far;
}

void DefCamera::SetWindowHeight(float WndHeight)
{
    m_WindowHeight = WndHeight;
}

void DefCamera::SetWindowWidth(float WndWidth)
{
    m_WindowWidth = WndWidth;
}

const XMFLOAT4X4* DefCamera::GetProj()
{
    m_AspectRatio = m_WindowWidth / m_WindowHeight;
    // Create a projection matrix in Left Hand Coordiante system.
    XMMATRIX perspective = XMMatrixPerspectiveFovLH(XMConvertToRadians(90), m_AspectRatio, m_NearDepth, m_FarDepth);
    XMStoreFloat4x4(&m_Projection, perspective);
    return &m_Projection;
}

const XMFLOAT4X4* DefCamera::GetView()
{
    //Create a view matrix in left hand coordinate system.
    XMStoreFloat4x4(&m_View, XMMatrixLookAtLH(XMLoadFloat3(&m_EyePos), XMLoadFloat3(&m_FwdVector), XMLoadFloat3(&m_UpVector)));
    return &m_View;
}
