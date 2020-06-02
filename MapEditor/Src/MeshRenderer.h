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
	
	void SetMesh(MeshData* pMesh) { m_pMesh = pMesh; }
	void SetVertex(ID3D11Buffer* pBuff) { m_pVertex = pBuff; }
	void SetMatrix(XMFLOAT4X4 & matrix) { m_pShader->SetMatrix(MATRIX::WORLD, matrix); }
	void SetTexture(const TCHAR* filename) { m_pTexture = TextureLoader::GetTexture(filename); }

	//void SetMaterial();

private:
	Shader			*m_pShader;
	MeshData		*m_pMesh;
	TEXTURE			*m_pTexture;
	ID3D11Buffer	*m_pVertex;
	// Material Ãß°¡
};