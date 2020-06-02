#pragma once

#include "stdafx.h"
#include "dx.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler")

#include "cbMatrix.h"

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
	SHADER_SOLIDCOLOR,
	SHADER_VERTEXCOLOR,
	SHADER_VERTEXNORMAL,
	SHADER_TEXTURE,
	SHADER_VERTEXNORMALTEXTURE,

	SHADER_MAX
} _SHADER;

struct LAYOUTS
{
	D3D11_INPUT_ELEMENT_DESC* pLayout;
	UINT numElements;
};

class Shader
{
public:
	Shader();
	virtual ~Shader();

public:
	HRESULT Create(const TCHAR* filename, _LAYOUT layout);
	static void Update();
	void Release();
	void SetShader();

private:
	HRESULT LoadShader(const TCHAR* filename, _LAYOUT layout);
	HRESULT CompileShader(const TCHAR* filename, const char* entry, const char* model, ID3DBlob** ppCode);
	HRESULT CreateInputLayout(_LAYOUT layout);

	HRESULT CreateConstantBuffer();
	HRESULT CreateConstantBuffer(UINT size, ID3D11Buffer** ppCB);
	HRESULT CreateDynamicConstantBuffer(UINT size, LPVOID pData, ID3D11Buffer** ppCB);
	HRESULT UpdateDynamicConstantBuffer(ID3D11Resource* pBuff, LPVOID pData, UINT size);
	void ReleaseConstantBuffer();

	int GetShaderError(const TCHAR* msg, HRESULT hr, ID3DBlob* pBlob,
		const TCHAR* filename, const char* EntryPoint, const char* ShaderModel);

public:
	void SetMatrix(MATRIX type, XMFLOAT4X4 & matrix);
	void SetTime(XMFLOAT4 & vTime);

public:
	ID3D11VertexShader*			m_pVS;
	ID3D11PixelShader*			m_pPS;

	ID3DBlob*					m_pVSCode;

	ID3D11InputLayout*			m_pLayout;

	ID3D11Buffer*				m_pCBMatrix;
	ID3D11Buffer*				m_pCBTime;

	cbMatrix					m_cbMatrix;
	XMFLOAT4					m_cbTime;
};

extern Shader* g_pShader;