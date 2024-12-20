#pragma once
#include <DirectXMath.h>
#include <windows.h>

using namespace DirectX;

class DefCamera
{
	public:
		DefCamera() = default;
		DefCamera(XMFLOAT3 Position, XMFLOAT3 Fwd, XMFLOAT3 Up, float WndWidth, float WndHeight, float NearDepth, float FarDepth);
		~DefCamera();

		void Update(float DeltaTime);

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

		const XMFLOAT4X4* GetProj();
		const XMFLOAT4X4* GetView();

	private:
		XMFLOAT4X4 m_View;

		XMFLOAT3 m_EyePos;
		XMFLOAT3 m_FwdVector;
		XMFLOAT3 m_UpVector;

		float m_WindowWidth;
		float m_WindowHeight;
		float m_NearDepth;
		float m_FarDepth;

		XMFLOAT4X4 m_Projection;
		XMFLOAT4X4 m_View;

		float m_AspectRatio;
	};

