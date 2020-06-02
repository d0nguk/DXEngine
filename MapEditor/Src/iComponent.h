#pragma once

#include "dx.h"

class iComponent
{
public:
	iComponent() {};
	virtual ~iComponent() {};

public:
	virtual BOOL Init() = 0;
	virtual void Update(float dTime = 0.0f) = 0;
	virtual void LateUpdate(float dTime = 0.0f) = 0;
	virtual void Release() = 0;
};