#pragma once

#include "GameObject.h"

class Character : public GameObject
{
public:
	Character();
	virtual ~Character();

	virtual BOOL Init(XMFLOAT3 vPos = XMFLOAT3(0, 0, 0)) override;
	virtual void Update(float dTime = 0.0f) override;
	virtual void LateUpdate(float dTime = 0.0f) override;
};