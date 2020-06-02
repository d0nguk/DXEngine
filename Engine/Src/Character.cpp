#include "dlls.h"
#include "MeshCreator.h"
#include "TextureLoader.h"
#include "Camera.h"

#include "Character.h"

Character::Character()
{
}

Character::~Character()
{
	Release();
}

BOOL Character::Init(XMFLOAT3 vPos)
{
	m_pTransform = new Transform();
	m_pTransform->Init();
	g_pCamera->SetTarget(m_pTransform);

	MeshData* pMesh = MeshCreator::LoadMesh(L"cube");
	if (pMesh == nullptr)
		return FALSE;

	m_pMeshRenderer = new MeshRenderer();
	m_pMeshRenderer->SetShader(g_pShader);
	m_pMeshRenderer->SetMesh(pMesh);
	m_pMeshRenderer->SetTexture(L"..\\Data\\Textures\\KakaoTalk_20200504_142017396.png");

	return TRUE;
}

void Character::Update(float dTime)
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
	v = XMVector3Normalize(v);
	XMStoreFloat3(&dir, v);
	//XMVECTOR forward = XMVectorSet(0, 0, -1, 0);
	//XMVECTOR cross = XMVector3Cross(forward, v);
	//XMVECTOR rot = XMVector3Dot(forward, v);

	//m_pTransform->Rotation.y = acos(XMVectorGetX(rot)) * (signbit(XMVectorGetY(cross)) ? -1.0f : 1.0f);

	dir.x *= dTime;
	dir.y *= dTime;
	dir.z *= dTime;
	m_pTransform->Translate(dir);

	GameObject::Update(dTime);
}

void Character::LateUpdate(float dTime)
{
	GameObject::LateUpdate(dTime);

	CFONT::Begin();
	CFONT::PrintInfo(300, 1, XMFLOAT4(1, 1, 1, 1), L"Character : %.2f, %.2f, %.2f",
		m_pTransform->Position.x,
		m_pTransform->Position.y,
		m_pTransform->Position.z
	);
	CFONT::End();
}
