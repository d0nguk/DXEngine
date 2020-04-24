#pragma once

#include "stdafx.h"

class iGameObject
{
public:
	iGameObject() {};
	virtual ~iGameObject() {};

public:
	virtual BOOL Init() = 0;
	virtual void Update(float dTime = 0.0f) = 0;
	virtual void LateUpdate(float dTime = 0.0f) = 0;
	virtual void Render(float dTime = 0.0f) = 0;
	virtual void Release() = 0;
};