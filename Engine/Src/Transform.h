#pragma once

#include "iComponent.h"

class Transform : public iComponent
{
public:
	Transform();
	virtual ~Transform();

public:
	virtual BOOL Init() override;
	BOOL Init(XMFLOAT3 vPos, XMFLOAT3 vRot, XMFLOAT3 vScale);
	virtual void Update(float dTime = 0.0f) override;
	virtual void LateUpdate(float dTime = 0.0f) override;
	virtual void Release() override;

private:
	XMFLOAT4X4 m_mTrans, m_mRot, m_mScale, m_mTM;
	XMFLOAT3 m_vPos, m_vRot, m_vScale;
};