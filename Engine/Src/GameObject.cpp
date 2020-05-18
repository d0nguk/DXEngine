#include "GameObject.h"
#include "Shader.h"
#include "Device.h"

#include "BufferCreator.h"
#include "MeshCreator.h"
#include "GeoLoader.h"

GameObject::GameObject() :
	m_pTransform(nullptr),
	m_pMeshRenderer(nullptr)
{

}

GameObject::~GameObject()
{
	Release();
}

BOOL GameObject::Init()
{
	//m_Transform.Init(XMFLOAT3(0,0,0), XMFLOAT3(0,0,0), XMFLOAT3(1,1,1));
	m_pTransform = new Transform();
	m_pTransform->Init();

	MeshData* pMesh = MeshCreator::LoadMesh(L"cube");
	if (pMesh == nullptr)
		return FALSE;

	m_pMeshRenderer = new MeshRenderer();
	m_pMeshRenderer->SetShader(g_pShader);
	m_pMeshRenderer->SetMesh(pMesh);

	return TRUE;
}

void GameObject::Update(float dTime)
{
	XMFLOAT3 dir = XMFLOAT3(0, 0, 0);
	if (CINPUT::GetKeyPressed(DIK_LEFT))
		dir.x -= 1.0f;
	if (CINPUT::GetKeyPressed(DIK_RIGHT))
		dir.x += 1.0f;
	if (CINPUT::GetKeyPressed(DIK_UP))
		dir.y += 1.0f;
	if (CINPUT::GetKeyPressed(DIK_DOWN))
		dir.y -= 1.0f;

	XMVECTOR v = XMLoadFloat3(&dir);
	XMStoreFloat3(&dir, XMVector3Normalize(v));

	dir.x *= dTime;
	dir.y *= dTime;
	dir.z *= dTime;
	m_pTransform->Translate(dir);

	m_pTransform->Rotate(XM_PIDIV4 * dTime);

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