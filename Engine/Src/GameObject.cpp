#include "GameObject.h"
#include "Shader.h"
#include "Device.h"

#include "BufferCreator.h"
#include "MeshCreator.h"
#include "GeoLoader.h"
#include "TextureLoader.h"

GameObject::GameObject() :
	m_pTransform(nullptr),
	m_pMeshRenderer(nullptr),
	m_wTexname(nullptr),
	m_pMaterial(nullptr)
{

}

GameObject::~GameObject()
{
	Release();
}

BOOL GameObject::Init(XMFLOAT3 vPos)
{
	m_pTransform = new Transform();
	//m_pTransform->Init();
	m_pTransform->Init(vPos, XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));

	MeshData* pMesh = MeshCreator::LoadMesh(L"cube");
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
	m_pMeshRenderer->SetMesh(pMesh);
	m_pMeshRenderer->SetMaterial(m_pMaterial);

	XMStoreFloat4x4(&mReflect, XMMatrixIdentity());

	return TRUE;
}

void GameObject::Update(float dTime)
{
	m_pTransform->Update(dTime);
}

void GameObject::LateUpdate(float dTime)
{
	m_pTransform->LateUpdate(dTime);
}

void GameObject::Render(float dTime)
{
	XMFLOAT4X4 mTM = m_pTransform->GetTM();
	XMMATRIX mTMR = XMLoadFloat4x4(&mTM);
	mTMR = mTMR * XMLoadFloat4x4(&mReflect);
	XMStoreFloat4x4(&mTM, mTMR);
	m_pMeshRenderer->SetMatrix(mTM);
	m_pMeshRenderer->Render();
}

void GameObject::Release()
{
	if (m_pMaterial != nullptr)
	{
		delete m_pMaterial;
		m_pMaterial = nullptr;
	}
	if (m_pTransform != nullptr)
	{
		delete m_pTransform;
		m_pTransform = nullptr;
	}
	if (m_pMeshRenderer != nullptr)
	{
		delete m_pMeshRenderer;
		m_pMeshRenderer = nullptr;
	}
}