#include "MeshRenderer.h"
#include "Device.h"

MeshRenderer::MeshRenderer() :
	m_pShader(nullptr),
	m_pMesh(nullptr)
{

}

MeshRenderer::~MeshRenderer()
{
	Release();
}

BOOL MeshRenderer::Init()
{
	return TRUE;
}

void MeshRenderer::Update(float dTime)
{

}

void MeshRenderer::LateUpdate(float dTime)
{

}

void MeshRenderer::Release()
{
	m_pTexture = nullptr;
	m_pShader = nullptr;
	m_pMesh = nullptr;
}

void MeshRenderer::Render()
{
	ID3D11DeviceContext * pDXDC = Device::GetDXDC();

	UINT offset = 0;

	pDXDC->IASetVertexBuffers(0, 1, &m_pMesh->m_pVertices, &m_pMesh->m_iStride, &offset);
	if (m_pMesh->m_pIndices != nullptr)
		pDXDC->IASetIndexBuffer(m_pMesh->m_pIndices, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	pDXDC->IASetInputLayout(m_pShader->m_pLayout);
	pDXDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_pShader->SetShader();

	pDXDC->PSSetShaderResources(0, 1, &m_pTexture->pTex);

	pDXDC->Draw(m_pMesh->m_iVertexCnt, 0);
}