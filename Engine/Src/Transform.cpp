#include "Transform.h"

Transform::Transform() :
	m_vPos(Position),
	m_vRot(Rotation),
	m_vScale(Scale)
{

}

Transform::~Transform()
{
	Release();
}

BOOL Transform::Init()
{
	Position = XMFLOAT3(0, 0, 0);
	Rotation = XMFLOAT3(0, 0, 0);
	Scale = XMFLOAT3(1, 1, 1);

	return TRUE;
}

BOOL Transform::Init(XMFLOAT3 vPos, XMFLOAT3 vRot, XMFLOAT3 vScale)
{
	Position = vPos;
	Rotation = vRot;
	Scale = vScale;

	return TRUE;
}

void Transform::Update(float dTime)
{

}

void Transform::LateUpdate(float dTime)
{
	XMMATRIX mTrans, mRot, mScale, mTM;

	mTrans = XMMatrixTranslation
	(
		m_vPos.x,
		m_vPos.y,
		m_vPos.z
	);

	mRot = XMMatrixRotationRollPitchYaw
	(
		m_vRot.x,
		m_vRot.y,
		m_vRot.z
	);

	mScale = XMMatrixScaling
	(
		m_vScale.x,
		m_vScale.y,
		m_vScale.z
	);

	mTM = mScale * mRot * mTrans;

	XMStoreFloat4x4(&m_mTrans, mTrans);
	XMStoreFloat4x4(&m_mRot, mRot);
	XMStoreFloat4x4(&m_mScale, mScale);
	XMStoreFloat4x4(&m_mTM, mTM);
}

void Transform::Release()
{

}