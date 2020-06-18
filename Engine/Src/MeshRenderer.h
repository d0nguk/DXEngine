#pragma once

#include "iComponent.h"
#include "MeshData.h"
#include "TextureLoader.h"
#include "Material.h"
#include "Shader.h"

class MeshRenderer : public iComponent
{
public:
	MeshRenderer();
	virtual ~MeshRenderer();

public:
	virtual BOOL Init() override;
	virtual void Update(float dTime = 0.0f) override;
	virtual void LateUpdate(float dTime = 0.0f) override;
	virtual void Release() override;

public:
	void Render();

public:
	void SetShader(Shader * pShader) { m_pShader = pShader; }
	const Shader* const GetShader() { return m_pShader; }
	
	void SetMesh(MeshData* pMesh)
	{
		m_pMesh = pMesh;
	}
	void SetMatrix(XMFLOAT4X4 & matrix)
	{
		m_pMaterial->pShader->SetMatrix(MATRIX::WORLD, matrix);
	}
	void SetTexture(const TCHAR* filename)
	{
		m_pTexture = TextureLoader::GetTexture(filename); 
	}
	void SetMaterial(Material * pMaterial)
	{
		m_pMaterial = pMaterial;
	}

private:
	void SetMaterial()
	{
		//m_pMaterial->pShader->SetMaterial();
	}

private:
	Shader		*m_pShader;
	TEXTURE		*m_pTexture;
	Material	*m_pMaterial;
	MeshData	*m_pMesh;
};