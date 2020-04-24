#include "BufferCreater.h"

std::map<const TCHAR*, ID3D11Buffer*> *BufferCreater::m_Buffer = nullptr;
ID3D11Device* BufferCreater::m_pDevice = nullptr;

void BufferCreater::Init(ID3D11Device* pDevice)
{
	m_Buffer = new std::map<const TCHAR*, ID3D11Buffer*>();
	m_Buffer->clear();

	m_pDevice = pDevice;
}

void BufferCreater::Release()
{
	if (m_Buffer != nullptr)
	{
		if (m_Buffer->size() > 0)
		{
			auto iter = m_Buffer->begin();

			for (; iter != m_Buffer->end(); ++iter)
			{
				if (iter->second != nullptr)
				{
					iter->second->Release();
					iter->second = nullptr;
				}
			}
		}

		m_Buffer->clear();
		delete m_Buffer;
		m_Buffer = nullptr;

		m_pDevice = nullptr;
	}
}

HRESULT BufferCreater::LoadBuffer(const TCHAR * filename, ID3D11Buffer ** ppDesc, void * pData, UINT size, UINT flag)
{
	HRESULT hr = S_OK;
	ID3D11Buffer* pBuff = nullptr;

	if(m_Buffer->size() > 0)
		pBuff = m_Buffer->at(filename);

	if (pBuff == nullptr)
	{
		hr = CreateBuffer(&pBuff, pData, size, flag);
		if (FAILED(hr))
			return hr;

		m_Buffer->insert(std::make_pair(filename, pBuff));
	}

	*ppDesc = pBuff;

	return hr;
}

HRESULT BufferCreater::CreateBuffer(ID3D11Buffer ** ppDesc, void * pData, UINT size, UINT flag)
{
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC bd;
	::memset(&bd, NULL, sizeof(D3D11_BUFFER_DESC));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = size;
	bd.BindFlags = flag;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA rd;
	::memset(&rd, NULL, sizeof(D3D11_SUBRESOURCE_DATA));

	rd.pSysMem = pData;

	hr = m_pDevice->CreateBuffer(&bd, &rd, ppDesc);
	if (FAILED(hr))
		return hr;

	return hr;
}