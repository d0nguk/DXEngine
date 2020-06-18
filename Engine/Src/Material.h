#pragma once

#include "dx.h"
#include "Shader.h"

#include <vector>

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
	Shader	*pShader;
	std::vector<TEXTURE*> *pTex;

	//XMFLOAT4 vDiffuse;
	//XMFLOAT4 vAmbient;
	//XMFLOAT4 vSpecular;

	XMFLOAT4 vDiffuse;
	XMFLOAT4 vAmbient;
	XMFLOAT4 vSpecular;

	Material()
	{
		pTex = new std::vector<TEXTURE*>();
		pTex->clear();
	}

	~Material()
	{
		pShader = nullptr;
		
		auto iter = pTex->begin();
		for (; iter != pTex->end(); ++iter)
		{
			*iter = nullptr;
		}
		pTex->clear();
		delete pTex;
		pTex = nullptr;
	}
};