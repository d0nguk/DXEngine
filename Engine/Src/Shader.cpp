#include "Shader.h"
#include "Device.h"

#pragma region LAYOUTS

#pragma region LAYOUT_POS
D3D11_INPUT_ELEMENT_DESC layout_POS[] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
};
UINT num_POS = ARRAYSIZE(layout_POS);
#pragma endregion

#pragma region LAYOUT_POS_COL
D3D11_INPUT_ELEMENT_DESC layout_POS_COL[] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
};
UINT num_POS_COL = ARRAYSIZE(layout_POS_COL);
#pragma endregion

#pragma region LAYOUT_POS_UV1
D3D11_INPUT_ELEMENT_DESC layout_POS_UV1[] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
};
UINT num_POS_UV1 = ARRAYSIZE(layout_POS_UV1);
#pragma endregion

#pragma region LAYOUT_POS_NRM
D3D11_INPUT_ELEMENT_DESC layout_POS_NRM[] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
};
UINT num_POS_NRM = ARRAYSIZE(layout_POS_NRM);
#pragma endregion

#pragma region LAYOUT_POS_NRM_UV1
D3D11_INPUT_ELEMENT_DESC layout_POS_NRM_UV1[] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},

	//{"POSITION", 0, DXGI_FORMAT_R32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//{"NORMAL",   0, DXGI_FORMAT_R32_FLOAT, 0, 4, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//{"TEXCOORD", 0, DXGI_FORMAT_R32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},
};
UINT num_POS_NRM_UV1 = ARRAYSIZE(layout_POS_NRM_UV1);
#pragma endregion

LAYOUTS g_Layout[_LAYOUT::LAYOUT_MAX] =
{
	{        layout_POS, num_POS},
	{    layout_POS_COL, num_POS_COL},
	{    layout_POS_NRM, num_POS_NRM},
	{    layout_POS_UV1, num_POS_UV1},
	{layout_POS_NRM_UV1, num_POS_NRM_UV1},
};

#pragma endregion

Shader* g_pShader = nullptr;

Shader::Shader()
{
	m_pVS = nullptr;
	m_pPS = nullptr;

	m_pVSCode = nullptr;

	m_pLayout = nullptr;
}

Shader::~Shader()
{
	Release();
}

HRESULT Shader::Create(const TCHAR * filename, _LAYOUT layout)
{
	HRESULT hr = S_OK;

	hr = LoadShader(filename, layout);
	if (FAILED(hr))
		return hr;
	hr = CreateInputLayout(layout);
	if (FAILED(hr))
		return hr;

	return hr;
}

void Shader::Update()
{

}

void Shader::Release()
{
	ReleaseConstantBuffer();

	if (m_pLayout != nullptr)
	{
		m_pLayout->Release();
		m_pLayout = nullptr;
	}
	if (m_pPS != nullptr)
	{
		m_pPS->Release();
		m_pPS = nullptr;
	}
	if (m_pVS != nullptr)
	{
		m_pVS->Release();
		m_pVS = nullptr;
	}
	if (m_pVSCode != nullptr)
	{
		m_pVSCode->Release();
		m_pVSCode = nullptr;
	}

	//SafeRelease(&m_pLayout);
	//SafeRelease(&m_pPS);
	//SafeRelease(&m_pVS);
	//SafeRelease(&m_pVSCode);
}

void Shader::SetShader()
{
	Device::GetDXDC()->VSSetShader(m_pVS, nullptr, 0);
	Device::GetDXDC()->PSSetShader(m_pPS, nullptr, 0);
}

HRESULT Shader::LoadShader(const TCHAR * filename, _LAYOUT layout)
{
	HRESULT hr = S_OK;

	ID3DBlob* pVSCode = nullptr;
	ID3D11Device* pDevice = Device::GetDevice();

	hr = CompileShader(filename, "VS_Main", "vs_5_0", &pVSCode);
	if (FAILED(hr))
		return hr;

	hr = pDevice->CreateVertexShader
	(
		pVSCode->GetBufferPointer(),
		pVSCode->GetBufferSize(),
		nullptr,
		&m_pVS
	);
	if (FAILED(hr))
	{
		pVSCode->Release();
		pVSCode = nullptr;
		return hr;
	}

	m_pVSCode = pVSCode;

	//===========================================================================

	ID3DBlob* pPSCode = nullptr;
	hr = CompileShader(filename, "PS_Main", "ps_5_0", &pPSCode);
	if (FAILED(hr))
		return hr;

	hr = pDevice->CreatePixelShader
	(
		pPSCode->GetBufferPointer(),
		pPSCode->GetBufferSize(),
		nullptr,
		&m_pPS
	);

	pPSCode->Release();
	pPSCode = nullptr;

	return hr;
}

HRESULT Shader::CompileShader(const TCHAR* filename, const char* entry, const char* model, ID3DBlob** ppCode)
{
	HRESULT hr = S_OK;
	ID3DBlob* pError = nullptr;

	UINT Flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
	//UINT Flags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;

#ifdef _DEBUG
	Flags |= D3DCOMPILE_DEBUG;
#endif

	hr = D3DCompileFromFile
	(
		filename,
		nullptr,
		nullptr,		// hlsl 내에서 include시 해당부분 수정
		entry,
		model,
		Flags,
		0,
		ppCode,
		&pError
	);

	if (FAILED(hr))
		GetShaderError(L"Shader 컴파일 실패", hr, pError, filename, entry, model);

	if(pError != nullptr)
		pError->Release();
	pError = nullptr;

	return hr;
}

HRESULT Shader::CreateInputLayout(_LAYOUT layout)
{
	HRESULT hr = S_OK;

	D3D11_INPUT_ELEMENT_DESC* pLayout = g_Layout[layout].pLayout;

	hr = Device::GetDevice()->CreateInputLayout
	(
		pLayout,
		g_Layout[layout].numElements,
		m_pVSCode->GetBufferPointer(),
		m_pVSCode->GetBufferSize(),
		&m_pLayout
	);

	if (FAILED(hr))
		return hr;

	//m_pLayouts[layout] = m_pLayout;

	return hr;
}

HRESULT Shader::CreateConstantBuffer()
{
	HRESULT hr = S_OK;

	return hr;
}

HRESULT Shader::CreateConstantBuffer(UINT size, ID3D11Buffer ** ppCB)
{
	HRESULT hr = S_OK;
	ID3D11Buffer* pCB = nullptr;

	D3D11_BUFFER_DESC bd;
	::memset(&bd, NULL, sizeof(D3D11_BUFFER_DESC));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = Device::GetDevice()->CreateBuffer(&bd, nullptr, &pCB);
	if (FAILED(hr))
		return hr;

	*ppCB = pCB;

	return hr;
}

HRESULT Shader::CreateDynamicConstantBuffer(UINT size, LPVOID pData, ID3D11Buffer ** ppCB)
{
	HRESULT hr = S_OK;
	ID3D11Buffer* pCB = nullptr;

	D3D11_BUFFER_DESC bd;
	::memset(&bd, NULL, sizeof(D3D11_BUFFER_DESC));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA sd;
	sd.pSysMem = pData;

	hr = Device::GetDevice()->CreateBuffer(&bd, &sd, &pCB);
	if (FAILED(hr))
		return hr;

	*ppCB = pCB;

	return hr;
}

HRESULT Shader::UpdateDynamicConstantBuffer(ID3D11Resource * pBuff, LPVOID pData, UINT size)
{
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE ms;
	::memset(&ms, NULL, sizeof(D3D11_MAPPED_SUBRESOURCE));

	hr = Device::GetDXDC()->Map(pBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	if (FAILED(hr))
		return hr;

	memcpy(ms.pData, pData, size);

	Device::GetDXDC()->Unmap(pBuff, 0);

	return hr;
}

void Shader::ReleaseConstantBuffer()
{

}

int Shader::GetShaderError(const TCHAR * msg, HRESULT hr, ID3DBlob * pBlob, const TCHAR * filename, const char * EntryPoint, const char * ShaderModel)
{
	TCHAR errw[4096] = L"";
	//::mbstowcs(errw, (char*)pBlob->GetBufferPointer(), pBlob->GetBufferSize());
	::mbstowcs_s(nullptr, errw, 4096, (char*)pBlob->GetBufferPointer(), pBlob->GetBufferSize());

	TCHAR herr[1024] = L"아래의 오류를 확인하십시오.";

	TCHAR errmsg[4096];
	swprintf_s(errmsg, 4096,
		L"%s \n에러코드(0x%08X) : %s \n\n%s",
		msg, hr, herr, errw
	);

	MessageBox(NULL, errmsg, L"Shader Compile Error", MB_OK);

	return 0;
}