#pragma once

#include "stdafx.h"
#include "dx.h"
#include "dlls.h"
#include "Shader.h"
#include "Camera.h"

#include "Character.h"
#include "Quad.h"

#include <vector>

enum RENDERMODE
{
	EDITMAP = 0,
	SELECTTEX = 1 << 0,
	ROTATECAM = 1 << 1,

	MODE_MAX = 3
};

class Device
{
public:
	Device(HINSTANCE hInst);
	~Device();

#pragma region WINDOWS
public:
	BOOL Init();
	BOOL Run();
	void Release();

private:
	BOOL InitWindow();
	void ResizeWindow(int width, int height);
	BOOL MessagePump();
	void ReleaseWindow();

public:
	static LRESULT MsgProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

private:
	HINSTANCE m_hInst;
	HWND m_hWnd;

	const TCHAR* m_wClassName;
	const TCHAR* m_wWindowName;
#pragma endregion

#pragma region DX
private:
	HRESULT InitDX();
	void ReleaseDX();

private:
	HRESULT CreateDeviceAndSwapChain();
	HRESULT CreateRenderTarget();
	HRESULT CreateDepthStencilView();
	void SetViewport();

private:
	HRESULT CreateStateObject();
	HRESULT CreateRasterState();
	HRESULT CreateDepthStencilState();
	HRESULT CreateSamplerState();

	void ReleaseStateObject();
	void ReleaseRasterState();
	void ReleaseDepthStencilState();
	void ReleaseSamplerState();

public:
	static ID3D11Device* GetDevice() { return pDevice->m_pDevice; }
	static ID3D11DeviceContext* GetDXDC() { return pDevice->m_pDXDC; }

#pragma region RENDERING
private:
	void ClearBackBuffer();
	void Update();
	void LateUpdate();
	void Render();
	void PrintInfo();
	void Flip();
#pragma endregion

private:
	static Device* pDevice;

public:
	typedef enum tagRasterizerState
	{
		RS_SOLID_BACK,
		RS_WIREFRAME_NONE,

		RS_MAX
	} _RASTER;
	typedef enum tagDepthStencilState
	{
		DS_WRITE_ON_TEST_ON,
		DS_WRITE_ON_TEST_OFF,

		DS_MAX
	} _DEPTHSTENCIL;
	typedef enum tagSamplerState
	{
		SS_CLAMP,
		SS_REPEAT,

		SS_MAX
	} _SAMPLER;

private:
	ID3D11Device				*m_pDevice;
	ID3D11DeviceContext			*m_pDXDC;
	IDXGISwapChain				*m_pSwapChain;
	ID3D11RenderTargetView		*m_pRenderTarget;
	ID3D11Texture2D				*m_pDS;
	ID3D11DepthStencilView		*m_pDSView;

	ID3D11RasterizerState		*m_pRState[RS_MAX];
	ID3D11DepthStencilState		*m_pDSState[DS_MAX];
	ID3D11SamplerState			*m_pSState[SS_MAX];

	DXGI_MODE_DESC				m_Mode;

	D3D_FEATURE_LEVEL			m_FeatureLevel;

	UINT						m_iAA;
	UINT						m_iAF;
#pragma endregion

#pragma region DATA
private:
	BOOL LoadData();
	void ReleaseData();

private:
	HRESULT LoadShader();
	BOOL LoadCamera();
	BOOL LoadAddOn();
	BOOL LoadManager();
	void ReleaseCamera();
	void ReleaseShader();
	void ReleaseAddOn();
	void ReleaseManager();

private:
	void SaveMap();
	void LoadMap();

	wchar_t* CharToWChar(const char* pstrSrc)
	{
		int nLen = strlen(pstrSrc) + 1;
		wchar_t* pwstr = (LPWSTR)malloc(sizeof(wchar_t)* nLen);
		//mbstowcs(pwstr, pstrSrc, nLen);
		mbstowcs_s(nullptr, pwstr, nLen, pstrSrc, nLen);
		return pwstr;
	}
	char* WCharToChar(const wchar_t* pwstrSrc)
	{
//#if !defined _DEBUG
//		int len = 0;
//		len = (wcslen(pwstrSrc) + 1) * 2;
//		char* pstr = (char*)malloc(sizeof(char) * len);
//		WideCharToMultiByte(949, 0, pwstrSrc, -1, pstr, len, NULL, NULL);
//#else
		int nLen = wcslen(pwstrSrc);
		char* pstr = (char*)malloc(sizeof(char) * nLen + 1);
		//wcstombs(pstr, pwstrSrc, nLen + 1);
		wcstombs_s(nullptr, pstr, nLen + 1, pwstrSrc, nLen + 1);
//#endif
		return pstr;
	}


private:
	XMFLOAT4		m_vTime;

	std::vector<iGameObject*> *m_pTerrain;
	std::vector<Quad*> *m_pTexList;
	std::vector<iGameObject*> *tempObj;

	iGameObject	*m_pCursor;

	Camera			*m_pCamera;
	Shader			*m_pShader;

	RENDERMODE		m_renderMode;
	UINT			m_texIndex;
#pragma endregion
};

extern std::vector<const TCHAR*> texname;