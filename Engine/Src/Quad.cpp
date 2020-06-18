#include "GeoLoader.h"
#include "MeshCreator.h"
#include "TextureLoader.h"

#include "Quad.h"

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
	m_pTransform->Init();
	m_pTransform->Position = vPos;
	//m_pTransform->Rotation.y = -XM_PIDIV4;
	m_pTransform->Scale = XMFLOAT3(2.5f, 2.5f, 2.5f);

	MeshData* pMesh = MeshCreator::LoadMesh(L"quad");

	if (pMesh == nullptr)
		return FALSE;

#pragma region SetMaterial
	m_pMaterial = new Material();
	m_pMaterial->pShader = Shader::GetShader(_SHADER::SHADER_TERRAIN);

	m_pMaterial->vDiffuse = XMFLOAT4(1, 1, 1, 1);
	m_pMaterial->vAmbient = XMFLOAT4(1, 1, 1, 1);
	m_pMaterial->vSpecular = XMFLOAT4(1, 1, 1, 100.0f);
#pragma endregion

	m_pMeshRenderer = new MeshRenderer();
	//m_pMeshRenderer->SetShader(Shader::g_pShaders[_SHADER::SHADER_DEFAULT]);
	m_pMeshRenderer->SetMesh(pMesh);
	m_pMeshRenderer->SetMaterial(m_pMaterial);

	XMStoreFloat4x4(&mReflect, XMMatrixIdentity());

	return TRUE;
}

void Quad::Update(float dTime)
{
	m_pTransform->Update(dTime);
}

void Quad::LateUpdate(float dTime)
{
	m_pTransform->LateUpdate(dTime);
}
