#pragma once

#include "stdafx.h"
#include "dx.h"

class iGameObject
{
public:
	iGameObject() {};
	virtual ~iGameObject() {};

public:
	virtual BOOL Init(XMFLOAT3 vPos = XMFLOAT3(0, 0, 0)) = 0;
	virtual void Update(float dTime = 0.0f) = 0;
	virtual void LateUpdate(float dTime = 0.0f) = 0;
	virtual void Render(float dTime = 0.0f) = 0;
	virtual void Release() = 0;
};