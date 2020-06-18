#include "stdafx.h"
#include "TextureLoader.h"
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>

bool TextureLoader::m_bInit = false;
ID3D11Device* TextureLoader::m_pDevice = nullptr;
ID3D11DeviceContext* TextureLoader::m_pDXDC = nullptr;
std::unordered_map<const TCHAR*, TEXTURE*> *TextureLoader::m_texMap;

TextureLoader::TextureLoader()
{

}

TextureLoader::~TextureLoader()
{

}

BOOL TextureLoader::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pDXDC)
{
	if (m_bInit)
		return TRUE;

	m_texMap = new std::unordered_map<const TCHAR*, TEXTURE*>();
	m_texMap->clear();

	m_pDevice = pDevice;
	m_pDXDC = pDXDC;

	m_bInit = true;

	return TRUE;
}

void TextureLoader::Release()
{
	if (m_texMap != nullptr)
	{
		if (m_texMap->size() > 0)
		{
			auto iter = m_texMap->begin();

			for (; iter != m_texMap->end(); ++iter)
			{
				if (iter->second != nullptr)
				{
					//iter->second->Release();
					delete iter->second;
					iter->second = nullptr;
					::memset((void*)iter->first, NULL, sizeof(const TCHAR*));
				}
			}
		}

		m_texMap->clear();
		delete m_texMap;
		m_texMap = nullptr;

		m_pDevice = nullptr;
		m_pDXDC = nullptr;
	}

	m_bInit = false;
}

HRESULT TextureLoader::LoadTexture(const TCHAR * filename)
{
	HRESULT hr = S_OK;
	
	hr = LoadTextureDDS(filename);
	if (FAILED(hr))
	{
		hr = LoadTexutreWIC(filename);
		if (FAILED(hr))
			return hr;
	}

	return hr;
}

TEXTURE * TextureLoader::GetTexture(const TCHAR * filename)
{
	if (!m_bInit)
		return nullptr;

	TEXTURE* pTex = nullptr;
	pTex = m_texMap->operator[](filename);

	if (pTex == nullptr)
	{
		HRESULT hr = LoadTexture(filename);
		if (FAILED(hr))
			return nullptr;
		else
			pTex = m_texMap->operator[](filename);
	}

	return pTex;
}

HRESULT TextureLoader::LoadTextureDDS(const TCHAR * filename)
{
	HRESULT hr = E_FAIL;

	

	return hr;
}

HRESULT TextureLoader::LoadTexutreWIC(const TCHAR * filename)
{
	HRESULT hr = S_OK;

	TEXTURE* pTexture = new TEXTURE();
	::memset(pTexture, NULL, sizeof(TEXTURE));

	//hr = DirectX::CreateWICTextureFromFile
	//(
	//	m_pDevice,
	//	filename,
	//	nullptr,
	//	&pTexture->pTex
	//);

	hr = DirectX::CreateWICTextureFromFileEx
	(
		m_pDevice,
		m_pDXDC,
		filename,
		0,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		0,
		D3D11_RESOURCE_MISC_GENERATE_MIPS,
		WIC_LOADER_DEFAULT,
		nullptr,
		&pTexture->pTex
	);

	if (FAILED(hr))
	{
		delete pTexture;
		pTexture = nullptr;

		return hr;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC dc;
	::memset(&dc, NULL, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	pTexture->pTex->GetDesc(&dc);

	ID3D11Resource *pRes = nullptr;
	pTexture->pTex->GetResource(&pRes);
	ID3D11Texture2D *pTex = nullptr;
	pRes->QueryInterface<ID3D11Texture2D>(&pTex);
	if (pTex)
	{
		pTex->GetDesc(&pTexture->TexDesc);
	}

	pTex->Release();
	pRes->Release();

	//m_texMap.insert(std::make_pair(filename, pTexture));
	m_texMap->operator[](filename) = pTexture;

	return hr;
}