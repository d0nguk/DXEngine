#pragma once

#include "iGameObject.h"
#include "Transform.h"

struct POS
{
	float x, y, z;
};

class GameObject : public iGameObject
{
public:
	GameObject();
	virtual ~GameObject();

public:
	virtual BOOL Init() override;
	virtual void Update(float dTime = 0.0f) override;
	virtual void LateUpdate(float dTime = 0.0f) override;
	virtual void Render(float dTime = 0.0f) override;
	virtual void Release() override;

protected:
	Transform m_Transform;

	ID3D11Buffer* m_pVBuffer;
};