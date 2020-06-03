#pragma once

#include "iGameObject.h"

#include "Transform.h"
#include "MeshRenderer.h"
#include "Material.h"

#include "Vertices.h"

class GameObject : public iGameObject
{
public:
	GameObject();
	virtual ~GameObject();

public:
	virtual BOOL Init(XMFLOAT3 vPos = XMFLOAT3(0, 0, 0)) override;
	virtual void Update(float dTime = 0.0f) override;
	virtual void LateUpdate(float dTime = 0.0f) override;
	virtual void Render(float dTime = 0.0f) override;
	virtual void Release() override;

public:
	void SetPos(XMFLOAT3 vPos)
	{
		m_pTransform->Position = vPos;
	}
	XMFLOAT3 GetPos()
	{
		return m_pTransform->Position;
	}
	void SetTexture(const TCHAR * filename)
	{
		m_pMeshRenderer->SetTexture(filename);
	}

protected:
	Transform *m_pTransform;
	MeshRenderer *m_pMeshRenderer;
};