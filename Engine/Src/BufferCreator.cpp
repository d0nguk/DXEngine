#include "BufferCreator.h"
#include "Vertices.h"
#include <vector>
#include "Device.h"
#include "Geometries.h"

std::map<const TCHAR*, BufferData*> *BufferCreator::m_Buffers = nullptr;
ID3D11Device* BufferCreator::m_pDevice = nullptr;
bool BufferCreator::m_bInit = false;

void BufferCreator::Init(ID3D11Device* pDevice)
{
	if (m_bInit)
		return;

	m_Buffers = new std::map<const TCHAR*, BufferData*>();
	m_Buffers->clear();

	m_pDevice = pDevice;

	m_bInit = true;
}

void BufferCreator::Release()
{
	if (m_Buffers != nullptr)
	{
		if (m_Buffers->size() > 0)
		{
			auto iter = m_Buffers->begin();

			for (; iter != m_Buffers->end(); ++iter)
			{
				if (iter->second != nullptr)
				{
					//iter->second->Release();
					//if(iter->second->m_bNext)
					//	delete[] iter->second;
					//else
						delete iter->second;

					iter->second = nullptr;
				}
			}
		}

		m_Buffers->clear();
		delete m_Buffers;
		m_Buffers = nullptr;

		m_pDevice = nullptr;
	}

	m_bInit = false;
}

HRESULT BufferCreator::LoadBuffer(const TCHAR * tag, void * _pData, UINT size, UINT flag)
{
	if (!m_bInit)
		return E_FAIL;

	HRESULT hr = S_OK;
	ID3D11Buffer* pBuff = nullptr;
	BufferData * pData = nullptr;

	if (m_Buffers->size() > 0)
		pData = m_Buffers->operator[](tag);

	if (pData == nullptr)
	{
		pData = new BufferData();
		m_Buffers->operator[](tag) = pData;
	}

	if (flag == D3D11_BIND_VERTEX_BUFFER)
	{
		if (pData->m_pVertices == nullptr)
		{
			hr = CreateBuffer(&pBuff, _pData, size, flag);
			if (FAILED(hr))
				return hr;

			pData->m_pVertices = pBuff;
		}
	}
	else if (flag == D3D11_BIND_INDEX_BUFFER)
	{
		if (pData->m_pIndices == nullptr)
		{
			hr = CreateBuffer(&pBuff, _pData, size, flag);
			if (FAILED(hr))
				return hr;

			pData->m_pIndices = pBuff;
		}
	}

	return hr;
}

HRESULT BufferCreator::LoadBufferWithMaterials(const TCHAR * tag, void * _pData, UINT flag, UINT count)
{
	if (!m_bInit)
		return E_FAIL;

	HRESULT hr = S_OK;
	ID3D11Buffer* pBuff = nullptr;
	BufferData * pData = nullptr;

	if (m_Buffers->size() > 0)
		pData = m_Buffers->operator[](tag);

	if (pData == nullptr)
	{
		pData = new BufferData[count]();
		m_Buffers->operator[](tag) = pData;
	}

	BufferData * pRes = pData;
	if (flag == D3D11_BIND_VERTEX_BUFFER)
	{
		Geometries * pGeo = (Geometries*)(_pData);

		while (1)
		{
			if (pRes->m_pVertices == nullptr)
			{
				hr = CreateBuffer(&pBuff, (void*)&pGeo->vertices[0], pGeo->vertexCnt * sizeof(pGeo->vertices[0]), flag);
				if (FAILED(hr))
					return hr;

				pRes->m_pVertices = pBuff;
			}

			if (pGeo->next)
			{
				pRes->m_bNext = true;
				++pGeo;
				++pRes;
			}
			else
				break;
		}
	}
	else if (flag == D3D11_BIND_INDEX_BUFFER)
	{
		if (pData->m_pIndices == nullptr)
		{
			hr = CreateBuffer(&pBuff, _pData, 0, flag);
			if (FAILED(hr))
				return hr;

			pData->m_pIndices = pBuff;
		}
	}

	return hr;
}

BufferData * BufferCreator::LoadBuffer(const TCHAR * tag)
{
	if (!m_bInit)
		return nullptr;

	BufferData *pData = nullptr;

	if (m_Buffers->size() > 0)
		pData = m_Buffers->operator[](tag);

	return pData;
}

HRESULT BufferCreator::CreateBuffer(ID3D11Buffer ** ppDesc, void * pData, UINT size, UINT flag)
{
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC bd;
	::memset(&bd, NULL, sizeof(D3D11_BUFFER_DESC));

	POS_NRM_UV1* data = (POS_NRM_UV1*)pData;

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = size;
	bd.BindFlags = flag;
	bd.CPUAccessFlags = 0;
	bd.StructureByteStride = sizeof(POS);

	D3D11_SUBRESOURCE_DATA rd;
	::memset(&rd, NULL, sizeof(D3D11_SUBRESOURCE_DATA));

	rd.pSysMem = pData;
	
	hr = m_pDevice->CreateBuffer(&bd, &rd, ppDesc);
	if (FAILED(hr))
		return hr;

	return hr;
}