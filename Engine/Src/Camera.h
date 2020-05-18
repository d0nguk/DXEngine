#pragma once

#include <Windows.h>
#include <DirectXMath.h>

using namespace DirectX;

enum tagVIEW
{
	VIEW2D,
	VIEW3D,

	VIEW_MAX
};

class Camera
{
public:
	Camera();
	~Camera();

public:
	BOOL Init(float width, float height, float fov);
	void Update(float dTime);
	void LateUpdate();
	void Resize(float width, float height, float fov);
	void Release();

private:
	void CreateViewMatrix();
	void CreateProjMatrix();

private:
	XMFLOAT4X4 m_mView, m_mProj;
	XMFLOAT4X4 m_mViews[tagVIEW::VIEW_MAX], m_mProjs[tagVIEW::VIEW_MAX];

	XMFLOAT3 m_vEye, m_vLookAt, m_vUp;
	float m_fWidth, m_fHeight;
	float m_fAspect, m_fFov;
	float m_fZNear, m_fZFar;

	tagVIEW m_View;
};