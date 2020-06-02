#pragma once

#include <Windows.h>
#include <DirectXMath.h>

#include "Transform.h"

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
	void LateUpdate(float dTime);
	void Resize(float width, float height, float fov);
	void SetTarget(Transform * pTransform) { m_pTarget = pTransform; }
	void Release();

private:
	void CreateViewMatrix();
	void CreateProjMatrix();

public:
	XMFLOAT3 ScreenToWorldPoint(XMFLOAT3 _vScreen);

private:
	XMFLOAT4X4 m_mView, m_mProj;
	XMFLOAT4X4 m_mViews[tagVIEW::VIEW_MAX], m_mProjs[tagVIEW::VIEW_MAX];

	Transform *m_pTarget;

	XMFLOAT3 m_vEye, m_vLookAt, m_vUp;
	float m_fWidth, m_fHeight;
	float m_fAspect, m_fFov;
	float m_fZNear, m_fZFar;

	float m_fOffsetX, m_fOffsetY, m_fOffsetZ;

	tagVIEW m_View;
};

extern Camera* g_pCamera;