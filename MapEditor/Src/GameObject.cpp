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
	m_wTexname(nullptr)
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

	m_pMeshRenderer = new MeshRenderer();
	m_pMeshRenderer->SetShader(g_pShader);
	m_pMeshRenderer->SetMesh(pMesh);
	m_pMeshRenderer->SetTexture(texname[0]);

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
	ID3D11DeviceContext* pDXDC = Device::GetDXDC();

	XMFLOAT4X4 mTM = m_pTransform->GetTM();
	m_pMeshRenderer->SetMatrix(mTM);
	m_pMeshRenderer->Render();
}

void GameObject::Release()
{
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