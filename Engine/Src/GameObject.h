#pragma once

#include "iGameObject.h"

#include "Transform.h"
#include "MeshRenderer.h"

#include "Vertices.h"

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
	Transform *m_pTransform;
	MeshRenderer *m_pMeshRenderer;
};