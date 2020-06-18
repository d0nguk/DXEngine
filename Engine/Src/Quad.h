#pragma once

#include "GameObject.h"

class Quad : public GameObject
{
public:
	Quad();
	virtual ~Quad();

	virtual BOOL Init(XMFLOAT3 vPos = XMFLOAT3(0, 0, 0)) override;
	virtual void Update(float dTime = 0.0f) override;
	virtual void LateUpdate(float dTime = 0.0f) override;

public:
	XMFLOAT4 GetPlane()
	{
		XMVECTOR vPoint = XMVectorSet
		(
			m_pTransform->Position.x,
			m_pTransform->Position.y,
			m_pTransform->Position.z,
			0.0f
		);

		XMVECTOR vNormal = XMVectorSet(0, 0, -1, 0);

		XMVECTOR vPlane = XMPlaneFromPointNormal(vPoint, vNormal);
		XMFLOAT4 fPlane = XMFLOAT4(0, 0, 0, 0);
		XMStoreFloat4(&fPlane, vPlane);

		return fPlane;
	}
};