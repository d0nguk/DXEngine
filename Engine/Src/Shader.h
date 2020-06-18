#pragma once

#include "stdafx.h"
#include "dx.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler")

#include "cbBuffers.h"

typedef enum tagLAYOUT
{
	L_POS,
	L_POS_COL,
	L_POS_NRM,
	L_POS_UV1,
	L_POS_NRM_UV1,

	LAYOUT_MAX
} _LAYOUT;

typedef enum tagSHADER
{
	SHADER_DEFAULT,
	SHADER_COLOR,
	SHADER_TERRAIN,
	SHADER_ERROR,

	SHADER_MAX
} _SHADER;

struct LAYOUTS
{
	D3D11_INPUT_ELEMENT_DESC* pLayout;
	UINT numElements;
};

struct Material;

class Shader
{
public:
	Shader();
	virtual ~Shader();

public:
	HRESULT Create(const TCHAR* filename, _LAYOUT layout);
	void Update();
	void Release();
	void SetShader();

protected:
	HRESULT LoadShader(const TCHAR* filename, _LAYOUT layout);
	HRESULT CompileShader(const TCHAR* filename, const char* entry, const char* model, ID3DBlob** ppCode);
	HRESULT CreateInputLayout(_LAYOUT layout);

	virtual HRESULT CreateConstantBuffer();
	HRESULT CreateConstantBuffer(UINT size, ID3D11Buffer** ppCB);
	HRESULT CreateDynamicConstantBuffer(UINT size, LPVOID pData, ID3D11Buffer** ppCB);
	HRESULT UpdateDynamicConstantBuffer(ID3D11Resource* pBuff, LPVOID pData, UINT size);
	void ReleaseConstantBuffer();

	int GetShaderError(const TCHAR* msg, HRESULT hr, ID3DBlob* pBlob,
		const TCHAR* filename, const char* EntryPoint, const char* ShaderModel);

public:
	void SetMatrix(MATRIX type, XMFLOAT4X4 & matrix);
	void SetTime(XMFLOAT4 & vTime);
	void SetMaterial(Material * mat);

public:
	static Shader *g_pCurrent;
private:
	static Shader *g_pError;
private:
	static Shader *g_pShaders[_SHADER::SHADER_MAX];
public:
	static void AddShader(Shader * pShader, _SHADER _tag)
	{
		g_pShaders[_tag] = pShader;
		if (_tag == _SHADER::SHADER_ERROR)
			g_pError = pShader;
	}
	static Shader * GetShader(_SHADER _tag)
	{
		return g_pShaders[_tag]->m_bInit ? g_pShaders[_tag] : g_pError;
	}
	static void ReleaseShaders()
	{
		for (int i = 0; i < _SHADER::SHADER_MAX; ++i)
		{
			if (g_pShaders[i] != nullptr)
			{
				delete g_pShaders[i];
				g_pShaders[i] = nullptr;
			}
		}

		g_pCurrent = nullptr;
	}
public:
	ID3D11VertexShader*			m_pVS;
	ID3D11PixelShader*			m_pPS;

	ID3DBlob*					m_pVSCode;

	ID3D11InputLayout*			m_pLayout;

	ID3D11Buffer				*m_pCBShared;
	ID3D11Buffer				*m_pCBDirecional;
	ID3D11Buffer				*m_pCBMaterial;

	cbGlobal					m_cbShared;
	cbDIRECTIONAL				m_cbDirectional;
	cbMaterial					m_cbMaterial;

private:
	bool						m_bInit;
};