#include "GeoLoader.h"
#include "MeshCreator.h"
#include "TextureLoader.h"
#include "Camera.h"

#include "Character.h"

#include "Device.h"

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
	
	m_pTransform->Position.z = -5.0f;
	//m_pTransform->Rotation.y = -XM_PIDIV4;//XM_PIDIV2 + XM_PIDIV4;

	MeshData* pMesh = MeshCreator::LoadMesh(L"dwarf");

	if (pMesh == nullptr)
		return FALSE;

#pragma region SetMaterial
	m_pMaterial = new Material();
	m_pMaterial->pShader = Shader::GetShader(_SHADER::SHADER_DEFAULT);

	TEXTURE *pTex = nullptr;
	Geometries *pGeo = GeoLoader::LoadGeometry(L"dwarf");
	for (unsigned int i = 0; i < pGeo->filename.size(); ++i)
	{
		pTex = TextureLoader::GetTexture(pGeo->filename[i].c_str());
		m_pMaterial->pTex->push_back(pTex);
	}
	for (unsigned int i = 0; i < pGeo->filename.size(); ++i)
	{
		std::wstring wstr = pGeo->filename[i];
		wstr.pop_back();
		wstr.pop_back();
		wstr.pop_back();
		wstr.pop_back();
		wstr += L"_spec.BMP";

		pTex = TextureLoader::GetTexture(wstr.c_str());
		m_pMaterial->pTex->push_back(pTex);
	}

	m_pMaterial->vDiffuse = XMFLOAT4(1, 1, 1, 1);
	m_pMaterial->vAmbient = XMFLOAT4(1, 1, 1, 1);
	m_pMaterial->vSpecular = XMFLOAT4(1, 1, 1, 3.0f);
#pragma endregion

	m_pMeshRenderer = new MeshRenderer();
	//m_pMeshRenderer->SetShader(Shader::g_pShaders[_SHADER::SHADER_DEFAULT]);
	m_pMeshRenderer->SetMesh(pMesh);
	m_pMeshRenderer->SetMaterial(m_pMaterial);

	XMStoreFloat4x4(&mReflect, XMMatrixIdentity());

	return TRUE;
}

void Character::Update(float dTime)
{
	XMFLOAT3 dir = XMFLOAT3(0, 0, 0);
	if (CINPUT::GetKeyPressed(DIK_LEFT))
	{
		dir.x -= 1.0f;
		//dir.z -= 1.0f;
	}
	if (CINPUT::GetKeyPressed(DIK_RIGHT))
	{
		dir.x += 1.0f;
		//dir.z += 1.0f;
	}
	if (CINPUT::GetKeyPressed(DIK_UP))
	{
		//dir.x -= 1.0f;
		dir.z += 1.0f;
	}
	if (CINPUT::GetKeyPressed(DIK_DOWN))
	{
		//dir.x += 1.0f;
		dir.z -= 1.0f;
	}

	XMVECTOR v = XMLoadFloat3(&dir);
	v = XMVector3Normalize(v);
	XMStoreFloat3(&dir, v);
	//XMVECTOR forward = XMVectorSet(0, 0, -1, 0);
	//XMVECTOR cross = XMVector3Cross(forward, v);
	//XMVECTOR rot = XMVector3Dot(forward, v);

	//m_pTransform->Rotation.y = acos(XMVectorGetX(rot)) * (signbit(XMVectorGetY(cross)) ? -1.0f : 1.0f);

	dir.x *= dTime * 1.0f;
	dir.y *= dTime * 1.0f;
	dir.z *= dTime * 1.0f;
	m_pTransform->Translate(dir);

	//m_pTransform->Rotation = XMFLOAT3(0.0f, XM_PI, 0.0f);
	m_pTransform->Rotate(0.0f, XM_PIDIV4 * dTime, 0.0f);

	if (CINPUT::GetKeyPressed(DIK_Q))
		m_pTransform->Rotation.y -= XM_PIDIV4 * dTime;
	if (CINPUT::GetKeyPressed(DIK_E))
		m_pTransform->Rotation.y += XM_PIDIV4 * dTime;

	GameObject::Update(dTime);
}

void Character::LateUpdate(float dTime)
{
	GameObject::LateUpdate(dTime);
}
