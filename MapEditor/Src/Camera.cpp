#include "stdafx.h"
#include "Camera.h"
#include "Shader.h"
#include "Device.h"

Camera* g_pCamera = nullptr;

Camera::Camera() :
	m_pTarget(nullptr)
{
	m_fOffsetX =  25.0f;
	m_fOffsetY =  25.0f;
	m_fOffsetZ = -25.0f;

	m_vEye = XMFLOAT3(m_fOffsetX, m_fOffsetY, m_fOffsetZ);
	m_vLookAt = XMFLOAT3(0, 0, 0);
	m_vUp = XMFLOAT3(0, 1, 0);

	m_fZNear = 1.0f;
	m_fZFar = 1000.0f;

	m_fViewY = 0.0f;

	m_View = VIEW3D;
}

Camera::~Camera()
{
	Release();
}

BOOL Camera::Init(float width, float height, float fov)
{
	Resize(width, height, fov);

	g_pShader->SetMatrix(MATRIX::VIEW, m_mViews[m_View]);
	g_pShader->SetMatrix(MATRIX::PROJ, m_mProjs[m_View]);

	return TRUE;
}

void Camera::Update(float dTime)
{
	// 카메라 이동
	//if (m_pTarget != nullptr)
	//{
	//	m_vEye.x = m_pTarget->Position.x;
	//	m_vEye.y = m_pTarget->Position.y;
	//	m_vEye.z = m_pTarget->Position.z;

	//	m_vLookAt = m_vEye;
	//}
	//else
	//{
	//	m_vEye.x = 0.0f;
	//	m_vEye.y = 0.0f;
	//	m_vEye.z = 0.0f;
	//}
	//
	//m_vEye.x += m_fOffsetX;
	//m_vEye.y += m_fOffsetY;
	//m_vEye.z += m_fOffsetZ;

	if (!CINPUT::GetKeyPressed(DIK_LCONTROL))
	{
		if (CINPUT::GetKeyPressed(DIK_W))
			m_vLookAt.y += dTime;
		if (CINPUT::GetKeyPressed(DIK_S))
			m_vLookAt.y -= dTime;
		if (CINPUT::GetKeyPressed(DIK_A))
			m_vLookAt.x -= dTime;
		if (CINPUT::GetKeyPressed(DIK_D))
			m_vLookAt.x += dTime;
	}

	m_vEye.x = m_vLookAt.x + m_fOffsetX;
	m_vEye.y = m_vLookAt.y + m_fOffsetY;
	m_vEye.z = m_vLookAt.z + m_fOffsetZ;

	if (CINPUT::GetKeyPressedDown(DIK_Q))
		m_fViewY -= 1.0f;
	if (CINPUT::GetKeyPressedDown(DIK_E))
		m_fViewY += 1.0f;
}

void Camera::LateUpdate(float dTime)
{
	Update(dTime);

	CreateViewMatrix();
	CreateProjMatrix();

	g_pShader->SetMatrix(MATRIX::VIEW, m_mViews[m_View]);
	g_pShader->SetMatrix(MATRIX::PROJ, m_mProjs[m_View]);
}

void Camera::Resize(float width, float height, float fov)
{
	m_fWidth = width;
	m_fHeight = height;

	m_fFov = fov;

	m_fAspect = width / height;

	CreateViewMatrix();
	CreateProjMatrix();

	g_pShader->SetMatrix(MATRIX::VIEW, m_mViews[m_View]);
	g_pShader->SetMatrix(MATRIX::PROJ, m_mProjs[m_View]);
}

void Camera::Release()
{

}

void Camera::CreateViewMatrix()
{
	XMMATRIX mView;
	::memset(&mView, 0, sizeof(XMMATRIX));

	XMVECTOR vEye, vLookAt, vUp;
	vEye = XMLoadFloat3(&m_vEye);
	vLookAt = XMLoadFloat3(&m_vLookAt);
	vUp = XMLoadFloat3(&m_vUp);

	mView = XMMatrixLookAtLH(vEye, vLookAt, vUp);

	XMStoreFloat4x4(&m_mViews[tagVIEW::VIEW2D], XMMatrixIdentity());
	XMStoreFloat4x4(&m_mViews[tagVIEW::VIEW3D], mView);

	m_mView = m_mViews[m_View];
}

void Camera::CreateProjMatrix()
{
	XMMATRIX mPers;
	XMMATRIX mOrtho;
	::memset(&mPers, 0, sizeof(XMMATRIX));
	::memset(&mOrtho, 0, sizeof(XMMATRIX));

	mPers = XMMatrixPerspectiveFovLH(m_fFov, m_fAspect, m_fZNear, m_fZFar);
	mOrtho = XMMatrixOrthographicLH(m_fWidth, m_fHeight, m_fZNear, m_fZFar);

	XMStoreFloat4x4(&m_mProjs[tagVIEW::VIEW2D], mOrtho);
	XMStoreFloat4x4(&m_mProjs[tagVIEW::VIEW3D], mPers);

	m_mProj = m_mProjs[m_View];
}

XMFLOAT3 Camera::ScreenToWorldPoint(XMFLOAT3 _vScreen)
{
	XMFLOAT3 res;

	switch (m_View)
	{
	case VIEW2D:
		res = ScreenToWorldPoint2D(_vScreen);
		break;

	case VIEW3D:
		res = ScreenToWorldPoint3D(_vScreen);
		break;
	}

	return res;
}

XMFLOAT3 Camera::ScreenToWorldPoint2D(XMFLOAT3 _vScreen)
{
	XMFLOAT3 fWorld = XMFLOAT3(0.0f, 0.0f, 0.0f);
	fWorld.x = _vScreen.x - (m_fWidth * 0.5f);
	fWorld.y = _vScreen.y - (m_fHeight * 0.5f);
	fWorld.y = -fWorld.y;

	return fWorld;
}

XMFLOAT3 Camera::ScreenToWorldPoint3D(XMFLOAT3 _vScreen)
{
	XMFLOAT3 fWorld = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMVECTOR vScreen = XMVectorZero();

	vScreen = XMVectorSetX(vScreen, (((2.0f*_vScreen.x) / m_fWidth) - 1.0f)); // Convert To -1 <= x <= 1
	vScreen = XMVectorSetY(vScreen, (((-2.0f*_vScreen.y) / m_fHeight) + 1.0f)); // Convert To -1 <= y <= 1
	vScreen = XMVectorSetZ(vScreen, 1.0f);

	//XMMATRIX InverseProj = XMMatrixInverse(&XMMatrixDeterminant(XMLoadFloat4x4(&m_mProj)), XMLoadFloat4x4(&m_mProj));
	//vScreen = XMVector3TransformCoord(vScreen, InverseProj);
	vScreen = XMVectorSetX(vScreen, XMVectorGetX(vScreen) / m_mProj._11);
	vScreen = XMVectorSetY(vScreen, XMVectorGetY(vScreen) / m_mProj._22);

	XMMATRIX InverseView = XMMatrixInverse(NULL, XMLoadFloat4x4(&m_mView));
	vScreen = XMVector3TransformCoord(vScreen, InverseView);
	vScreen -= XMLoadFloat3(&m_vEye);

	XMVECTOR dir = XMVector3Normalize(vScreen);
	float len = (m_fViewY - m_vEye.y) / XMVectorGetY(dir);

	XMStoreFloat3(&fWorld, dir);
	fWorld.x = m_vEye.x + fWorld.x * len;
	fWorld.y = m_fViewY;// m_vEye.y + fWorld.y * len;
	fWorld.z = m_vEye.z + fWorld.z * len;

	return fWorld;
}
