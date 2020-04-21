#pragma once

#include "stdafx.h"
#include "dx.h"

class Device
{
public:
	Device(HINSTANCE hInst);
	~Device();

#pragma region WINDOW
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
	void Update();
	void Render();
	void ReleaseDX();

private:
	HRESULT CreateDeviceAndSwapChain();
	HRESULT CreateRenderTarget();
	void SetViewport();

private:
	void ClearBackBuffer();
	void Flip();

private:
	ID3D11Device*				m_pDevice;
	ID3D11DeviceContext*		m_pDXDC;
	IDXGISwapChain*				m_pSwapChain;
	ID3D11RenderTargetView*		m_pRenderTarget;

	D3D_FEATURE_LEVEL			m_FeatureLevel;
#pragma endregion

};