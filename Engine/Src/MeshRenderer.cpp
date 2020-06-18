#include "MeshRenderer.h"
#include "Device.h"
#include "GeoLoader.h"

MeshRenderer::MeshRenderer() :
	m_pShader(nullptr),
	m_pMesh(nullptr),
	m_pTexture(nullptr)
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

	MeshData* pMesh = m_pMesh;

	pDXDC->IASetVertexBuffers(0, 1, &pMesh->m_pVertices, &pMesh->m_iStride, &offset);
	if (pMesh->m_pIndices != nullptr)
		pDXDC->IASetIndexBuffer(pMesh->m_pIndices, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	pDXDC->IASetInputLayout(m_pMaterial->pShader->m_pLayout);
	pDXDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_pMaterial->pShader->SetShader();
	m_pMaterial->pShader->SetMaterial(m_pMaterial);
	m_pMaterial->pShader->Update();
	
	for (unsigned int i = 0; i < pMesh->vertexCount.size(); ++i)
	{
		pDXDC->PSSetShaderResources(0, 1, &m_pMaterial->pTex->operator[](i)->pTex);
		if (m_pMaterial->pTex->size() > 1)
		{
			UINT size = m_pMaterial->pTex->size();
			pDXDC->PSSetShaderResources(1, 1, &m_pMaterial->pTex->operator[](i + size*0.5f)->pTex);
		}
		pDXDC->Draw(pMesh->vertexCount[i], pMesh->startVertexLocation[i]);
	}
}