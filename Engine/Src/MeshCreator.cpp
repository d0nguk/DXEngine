#include "MeshCreator.h"
#include "BufferCreator.h"

std::map<const TCHAR*, MeshData*> *MeshCreator::m_Meshes = nullptr;
ID3D11Device* MeshCreator::m_pDevice = nullptr;
bool MeshCreator::m_bInit = false;

void MeshCreator::Init(ID3D11Device * pDevice)
{
	if (m_bInit)
		return;

	m_Meshes = new std::map<const TCHAR*, MeshData*>();
	m_Meshes->clear();

	m_pDevice = pDevice;

	m_bInit = true;
}

void MeshCreator::Release()
{
	if (m_Meshes != nullptr)
	{
		if (m_Meshes->size() > 0)
		{
			auto iter = m_Meshes->begin();

			for (; iter != m_Meshes->end(); ++iter)
			{
				if (iter->second != nullptr)
				{
					//iter->second->Release();
					delete iter->second;
					iter->second = nullptr;
				}
			}
		}

		m_Meshes->clear();
		delete m_Meshes;
		m_Meshes = nullptr;

		m_pDevice = nullptr;
	}

	m_bInit = false;
}

BOOL MeshCreator::CreateMesh(const TCHAR * tag, Geometries * pGeo, BufferData * pBuff)
{
	if (!m_bInit)
		return FALSE;

	MeshData *pData = new MeshData();

	pData->m_pVertices = pBuff->m_pVertices;
	pData->m_pIndices = pBuff->m_pIndices;
	pData->m_iVertexCnt = pGeo->vertexCnt;
	pData->m_iIndexCnt = 0; //pGeo->indexCnt;
	pData->m_iStartIndexLocation = 0;
	pData->m_iBaseVertexLocation = 0;
	pData->m_iStride = sizeof(pGeo->vertices[0]);

	m_Meshes->insert(std::make_pair(tag, pData));

	return TRUE;
}

MeshData* MeshCreator::LoadMesh(const TCHAR* tag)
{
	if (!m_bInit)
		return nullptr;

	MeshData *pData = nullptr;

	if (m_Meshes->size() > 0)
		pData = m_Meshes->at(tag);

	return pData;
}