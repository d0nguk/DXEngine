#pragma once

#include <unordered_map>

#include "dx.h"
#include "Material.h"

class TextureLoader
{
private:
	TextureLoader();
	~TextureLoader();

public:
	static BOOL Init(ID3D11Device* pDevice, ID3D11DeviceContext* pDXDC);
	static void Release();

public:
	static TEXTURE* GetTexture(const TCHAR * filename);

private:
	static HRESULT LoadTexture(const TCHAR * filename);
	static HRESULT LoadTextureDDS(const TCHAR* filename);
	static HRESULT LoadTexutreWIC(const TCHAR* filename);
private:
	static std::unordered_map<const TCHAR*, TEXTURE*> *m_texMap;

	static ID3D11Device* m_pDevice;
	static ID3D11DeviceContext* m_pDXDC;
	
	static bool m_bInit;
};