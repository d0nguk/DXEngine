#pragma once

#include "dx.h"
#include "Shader.h"

struct TEXTURE
{
	ID3D11ShaderResourceView* pTex;
	D3D11_TEXTURE2D_DESC TexDesc;

	~TEXTURE()
	{
		if (pTex != nullptr)
		{
			pTex->Release();
			pTex = nullptr;
		}
	}
};

struct Material
{
	TEXTURE *pTex;
	Shader	*pShader;
};