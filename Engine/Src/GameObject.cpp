#include "GameObject.h"
#include "BufferCreater.h"
#include "Shader.h"
#include "Device.h"

GameObject::GameObject()
	: m_Transform()
{

}

GameObject::~GameObject()
{
	Release();
}

BOOL GameObject::Init()
{
	//m_Transform.Init(XMFLOAT3(0,0,0), XMFLOAT3(0,0,0), XMFLOAT3(1,1,1));
	m_Transform.Init();

	POS data[] = 
	{
		{ -0.5f, -0.5f, 0.0f },
		{  0.0f,  0.5f, 0.0f }, 
		{  0.5f, -0.5f, 0.0f },
	};

	if (FAILED(BufferCreater::LoadBuffer(L"cube", &m_pVBuffer, data, 36, D3D11_BIND_VERTEX_BUFFER)))
		return FALSE;

	return TRUE;
}

void GameObject::Update(float dTime)
{
	m_Transform.Update(dTime);
}

void GameObject::LateUpdate(float dTime)
{
	m_Transform.LateUpdate(dTime);
}

void GameObject::Render(float dTime)
{
	UINT stride = sizeof(POS);
	UINT offset = 0;
	ID3D11DeviceContext* pDXDC = Device::GetDXDC();

	pDXDC->IASetVertexBuffers(0, 1, &m_pVBuffer, &stride, &offset);
	pDXDC->IASetInputLayout(g_pShader->m_pLayout);
	pDXDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_pShader->SetShader();

	pDXDC->Draw(3, 0);
}

void GameObject::Release()
{
	m_Transform.Release();

	m_pVBuffer = nullptr;
}