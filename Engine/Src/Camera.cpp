#include "stdafx.h"
#include "Camera.h"
#include "Shader.h"

Camera::Camera()
{
	m_vEye = XMFLOAT3(0, 0, -5);
	m_vLookAt = XMFLOAT3(0, 0, 5);
	m_vUp = XMFLOAT3(0, 1, 0);

	m_fZNear = 1.0f;
	m_fZFar = 100.0f;

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
}

void Camera::LateUpdate()
{
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
}