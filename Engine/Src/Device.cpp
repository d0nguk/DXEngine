#include "Device.h"

LRESULT CALLBACK gMsgProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	return Device::MsgProc(hWnd, iMsg, wParam, lParam);
}

Device::Device(HINSTANCE hInst)
	: 
	m_hInst(hInst),
	m_wClassName(L"CLASS"),
	m_wWindowName(L"WINDOW"),
	m_FeatureLevel(D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0)
{
	m_Mode.Width = 800;
	m_Mode.Height = 600;
	m_Mode.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
}

Device::~Device()
{
	Release();
}

BOOL Device::Init()
{
	BOOL res = TRUE;

	res = InitWindow();
	if (!res)
		return res;

	if (FAILED(InitDX()))
		return FALSE;

	return res;
}

BOOL Device::Run()
{
	if (!MessagePump())
		return FALSE;

	return TRUE;
}

void Device::Release()
{
	ReleaseDX();
	ReleaseWindow();
}

BOOL Device::InitWindow()
{
	WNDCLASSEX wc;

	wc.cbClsExtra = 0;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(m_hInst, (LPCTSTR)IDI_APPLICATION);
	wc.hIconSm = LoadIcon(m_hInst, (LPCTSTR)IDI_APPLICATION);
	wc.hInstance = m_hInst;
	wc.lpfnWndProc = (WNDPROC)gMsgProc;
	wc.lpszClassName = m_wClassName;
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	::RegisterClassEx(&wc);

	RECT rc;
	memset(&rc, NULL, sizeof(RECT));

	HWND hWnd = ::CreateWindow
	(
		m_wClassName,
		m_wWindowName,
		WS_OVERLAPPEDWINDOW,
		0, 0,
		m_Mode.Width, m_Mode.Height,
		GetDesktopWindow(),
		NULL,
		wc.hInstance,
		NULL
	);

	if (!hWnd)
		return FALSE;

	m_hWnd = hWnd;

	::ShowWindow(hWnd, SW_SHOWDEFAULT);
	::UpdateWindow(hWnd);

	ResizeWindow(800, 600);

	return TRUE;
}

void Device::ResizeWindow(int width, int height)
{
	RECT oldrc;
	DWORD style = (DWORD) ::GetWindowLong(m_hWnd, GWL_STYLE);
	DWORD exstyle = (DWORD) ::GetWindowLong(m_hWnd, GWL_EXSTYLE);

	::GetWindowRect(m_hWnd, &oldrc);

	RECT newrc;
	newrc.left = 0;
	newrc.top = 0;
	newrc.right = width;
	newrc.bottom = height;

	::AdjustWindowRectEx(&newrc, style, NULL, exstyle);

	int newWidth = newrc.right - newrc.left;
	int newHeight = newrc.bottom - newrc.top;

	::SetWindowPos
	(
		m_hWnd,
		HWND_NOTOPMOST,
		oldrc.left,
		oldrc.top,
		newWidth,
		newHeight,
		SWP_SHOWWINDOW
	);
}

BOOL Device::MessagePump()
{
	MSG msg;
	::memset(&msg, NULL, sizeof(MSG));
	
	while (1)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			// Rendering
			ClearBackBuffer();
			//Update();
			//Render();
			Flip();
		}
	}

	return FALSE;
}

void Device::ReleaseWindow()
{
	UnregisterClass(m_wClassName, m_hInst);
}

LRESULT Device::MsgProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			SendMessage(hWnd, WM_DESTROY, wParam, lParam);
			return 0;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

HRESULT Device::InitDX()
{
	HRESULT hr = S_OK;

	hr = CreateDeviceAndSwapChain();
	if (FAILED(hr))
		return hr;

	hr = CreateRenderTarget();
	if (FAILED(hr))
		return hr;

	m_pDXDC->OMSetRenderTargets
	(
		1,
		&m_pRenderTarget,
		m_pDSView
	);

	SetViewport();

	CFONT::Init(m_pDevice, L"..\\..\\Extern\\Font\\±¼¸²9k.sfont");
	OutputDebugStringW(L"ASDF");
	
	return hr;
}

void Device::ReleaseDX()
{
	CFONT::Release();

	if(m_pDXDC != nullptr)
		m_pDXDC->ClearState();

	if (m_pRenderTarget != nullptr)
	{
		m_pRenderTarget->Release();
		m_pRenderTarget = nullptr;
	}
	if (m_pSwapChain != nullptr)
	{
		m_pSwapChain->Release();
		m_pSwapChain = nullptr;
	}
	if (m_pDXDC != nullptr)
	{
		m_pDXDC->Release();
		m_pDXDC = nullptr;
	}
	if (m_pDevice != nullptr)
	{
		m_pDevice->Release();
		m_pDevice = nullptr;
	}
}

HRESULT Device::CreateDeviceAndSwapChain()
{
	HRESULT hr = S_OK;

	DXGI_SWAP_CHAIN_DESC sd;
	::memset(&sd, NULL, sizeof(DXGI_SWAP_CHAIN_DESC));

	sd.Windowed = TRUE;
	sd.OutputWindow = m_hWnd;
	sd.BufferCount = 1;
	sd.BufferDesc.Width = m_Mode.Width;
	sd.BufferDesc.Height = m_Mode.Height;
	sd.BufferDesc.Format = m_Mode.Format;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Numerator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	hr = D3D11CreateDeviceAndSwapChain
	(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		&m_FeatureLevel,
		1,
		D3D11_SDK_VERSION,
		&sd,
		&m_pSwapChain,
		&m_pDevice,
		NULL,
		&m_pDXDC
	);

	return hr;
}

HRESULT Device::CreateRenderTarget()
{
	HRESULT hr = S_OK;

	ID3D11Texture2D *pBackBuffer;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return hr;

	hr = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTarget);

	if (pBackBuffer != nullptr)
	{
		pBackBuffer->Release();
		pBackBuffer = nullptr;
	}

	return hr;
}

HRESULT Device::CreateDepthStencilView()
{
	HRESULT hr = S_OK;

	D3D11_TEXTURE2D_DESC td;
	::memset(&td, NULL, sizeof(D3D11_TEXTURE2D_DESC));

	td.Width = m_Mode.Width;
	td.Height = m_Mode.Height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D32_FLOAT;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	hr = m_pDevice->CreateTexture2D(&td, NULL, &m_pDS);
	if (FAILED(hr))
		return hr;

	D3D11_DEPTH_STENCIL_VIEW_DESC dd;
	::memset(&dd, NULL, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	dd.Format = td.Format;
	dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	dd.Texture2D.MipSlice = 0;

	hr = m_pDevice->CreateDepthStencilView(m_pDS, &dd, &m_pDSView);
	if (FAILED(hr))
		return hr;

	return hr;
}

void Device::SetViewport()
{
	D3D11_VIEWPORT vp;

	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = (float)m_Mode.Width;
	vp.Height = (float)m_Mode.Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	m_pDXDC->RSSetViewports(1, &vp);
}

void Device::ClearBackBuffer()
{
	XMFLOAT4 col = XMFLOAT4(0, 0.125f, 0.3f, 1);

	m_pDXDC->ClearRenderTargetView(m_pRenderTarget, (float*)&col);
	m_pDXDC->ClearDepthStencilView(m_pDSView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Device::Update()
{

}

void Device::Render()
{

}

void Device::Flip()
{
	m_pSwapChain->Present(0, 0);
}
