#include "BaseCamera.h"

BaseCamera::BaseCamera(XMFLOAT3 Position, XMFLOAT3 Fwd, XMFLOAT3 Up, float WndWidth, float WndHeight, float NearDepth, float FarDepth, CameraTypes Type) :
    m_EyePos(Position), m_FwdVector(Fwd), m_UpVector(Up),
    m_WindowWidth(WndWidth), m_WindowHeight(WndHeight),
    m_NearDepth(NearDepth), m_FarDepth(FarDepth), m_Type(Type)
{
    m_AspectRatio = m_WindowWidth / m_WindowHeight; // create aspect  ratio
    CreateProjectionMatrix();
    CreateViewMatrix();
}

BaseCamera::~BaseCamera()
{

}

XMFLOAT3 BaseCamera::GetFwd()
{
    return m_FwdVector;
}

XMFLOAT3 BaseCamera::GetEye()
{
    return m_EyePos;
}

XMFLOAT3 BaseCamera::GetUp()
{
    return m_UpVector;
}

void BaseCamera::SetFwd(XMFLOAT3 Fwd)
{
    //Set Fwd vector then recreate view matrix since a value has changed within the matrix.
    m_FwdVector = Fwd;
    CreateViewMatrix();
}

void BaseCamera::SetEye(XMFLOAT3 Eye)
{
    //Set camera position then recreate view matrix since a value has changed within the matrix.
    m_EyePos = Eye;
    CreateViewMatrix();
}

void BaseCamera::SetUp(XMFLOAT3 Up)
{
    //Set up vector then recreate view matrix since a value has changed within the matrix.
    m_UpVector = Up;
    CreateViewMatrix();
}

void BaseCamera::SetNearDepth(float Near)
{
    // Set Near clipping depth then recreate projection matrix since a value within the matrix has changed.
    m_NearDepth = Near;
    CreateProjectionMatrix();
}

void BaseCamera::SetFarDepth(float Far)
{
    // Set the far clipping depth then recreate projection matrix since a value within the matrix has changed.
    m_FarDepth = Far;
    CreateProjectionMatrix();
}

void BaseCamera::SetWindowHeight(float WndHeight)
{
    // Set window height and then recreate projection matrix since a value has changed.
    m_WindowHeight = WndHeight;
    CreateProjectionMatrix();
}

void BaseCamera::SetWindowWidth(float WndWidth)
{
    // Set window width and then recreate projection matrix since a value has changed.
    m_WindowWidth = WndWidth;
    CreateProjectionMatrix();
}

void BaseCamera::SetActiveCamera()
{
    m_IsActive = true;
}

void BaseCamera::DisableCamera()
{
    m_IsActive = false;
}

const XMFLOAT4X4* BaseCamera::CreateProjectionMatrix()
{
    // Create a projection matrix in Left Hand Coordiante system.
    XMMATRIX perspective = XMMatrixPerspectiveFovLH(XMConvertToRadians(90), m_AspectRatio, m_NearDepth, m_FarDepth);
    XMStoreFloat4x4(&m_Projection, perspective);
    return &m_Projection;
}

const XMFLOAT4X4* BaseCamera::CreateViewMatrix()
{
    //Create a view matrix in left hand coordinate system.
    return &m_View;
}

