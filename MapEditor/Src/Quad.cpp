#include "Quad.h"
#include "MeshCreator.h"

Quad::Quad()
{
}

Quad::~Quad()
{
	Release();
}

BOOL Quad::Init(XMFLOAT3 vPos)
{
	m_pTransform = new Transform();
	//m_pTransform->Init();
	m_pTransform->Init(vPos, XMFLOAT3(0, 0, 0), XMFLOAT3(150, 150, 1));

	MeshData* pMesh = MeshCreator::LoadMesh(L"quad");
	if (pMesh == nullptr)
		return FALSE;

	m_pMeshRenderer = new MeshRenderer();
	m_pMeshRenderer->SetShader(g_pShader);
	m_pMeshRenderer->SetMesh(pMesh);
	//m_pMeshRenderer->SetTexture(L"..\\Data\\Textures\\drock.jpg");

	return TRUE;
}

BOOL Quad::CheckBoundary(XMFLOAT3 vPos)
{
	bool res = TRUE;

	float quadLeft = m_pTransform->Position.x - (m_pTransform->Scale.x * 0.5f);
	float quadRight = m_pTransform->Position.x + (m_pTransform->Scale.x * 0.5f);
	float quadTop = m_pTransform->Position.y - (m_pTransform->Scale.y * 0.5f);
	float quadBottom = m_pTransform->Position.y - (m_pTransform->Scale.y * 0.5f);

	if (quadLeft <= vPos.x && vPos.x <= quadRight &&
		quadBottom <= vPos.y && quadTop <= vPos.y)
		return TRUE;

	return FALSE;
}
