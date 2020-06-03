#pragma once

#include "GameObject.h"

class Quad : public GameObject
{
public:
	Quad();
	virtual ~Quad();

public:
	virtual BOOL Init(XMFLOAT3 vPos = XMFLOAT3(0, 0, 0)) override;

public:
	BOOL CheckBoundary(XMFLOAT3 vPos);
};