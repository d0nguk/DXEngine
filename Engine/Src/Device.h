#pragma once

#include "stdafx.h"
#include "dx.h"
#include "dlls.h"
#include "Shader.h"
#include "GameObject.h"

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

public:
	static ID3D11Device* GetDevice() { return pDevice->m_pDevice; }
	static ID3D11DeviceContext* GetDXDC() { return pDevice->m_pDXDC; }

#pragma region RENDERING
private:
	void ClearBackBuffer();
	void Update();
	void Render();
	void PrintInfo();
	void Flip();
#pragma endregion

private:
	static Device* pDevice;

	ID3D11Device*				m_pDevice;
	ID3D11DeviceContext*		m_pDXDC;
	IDXGISwapChain*				m_pSwapChain;
	ID3D11RenderTargetView*		m_pRenderTarget;
	ID3D11Texture2D*			m_pDS;
	ID3D11DepthStencilView*		m_pDSView;

	DXGI_MODE_DESC				m_Mode;

	D3D_FEATURE_LEVEL			m_FeatureLevel;
#pragma endregion

#pragma region DATA
private:
	BOOL LoadData();
	void ReleaseData();

private:
	HRESULT LoadShader();
	BOOL LoadAddOn();
	BOOL LoadManager();
	void ReleaseShader();
	void ReleaseAddOn();
	void ReleaseManager();

private:
	Shader* m_pShader;
	iGameObject* m_Obj;
#pragma endregion
};