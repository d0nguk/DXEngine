#include "Device.h"

#include "BufferCreator.h"
#include "MeshCreator.h"
#include "GeoLoader.h"
#include "TextureLoader.h"

#include "Character.h"
#include "Quad.h"

#include <filesystem>
#include <istream>
#include <fstream>

Device* Device::pDevice = nullptr;
std::vector<const TCHAR*> texname;

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
	m_Mode.Width = 1600;
	m_Mode.Height = 900;
	m_Mode.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	m_iAF = 4;
	m_iAA = 4;
}

Device::~Device()
{
	Release();
}

BOOL Device::Init()
{
	BOOL res = TRUE;

	if (pDevice == nullptr)
	{
		res = InitWindow();
		if (!res)
			return res;

		if (FAILED(InitDX()))
			return FALSE;

		pDevice = this;

		res = LoadData();
		if (!res)
			return res;

		m_pObj = new Character();
		m_pObj->Init();
		
		m_pMirror = new Quad();
		m_pMirror->Init(XMFLOAT3(0.0f, 0.0f, 0.0f));
		((Quad*)m_pMirror)->SetTexture(L"..\\Data\\Textures\\mirror.jpg");
	}

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
	if (m_pDevice == nullptr)
		return;

	auto i = texname.begin();
	for (; i != texname.end(); ++i)
	{
		delete (*i);
		*i = nullptr;
	}
	texname.clear();

	if (m_pObj != nullptr)
	{
		delete m_pObj;
		m_pObj = nullptr;
	}
	if (m_pMirror != nullptr)
	{
		delete m_pMirror;
		m_pMirror = nullptr;
	}
	if (m_pTerrain != nullptr)
	{
		if (m_pTerrain->size() > 0)
		{
			auto iter = m_pTerrain->begin();

			for (; iter != m_pTerrain->end(); ++iter)
			{
				if (*iter != nullptr)
				{
					delete (*iter);
					(*iter) = nullptr;
				}
			}
		}

		m_pTerrain->clear();
		delete m_pTerrain;
		m_pTerrain = nullptr;
	}

	ReleaseData();
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

	RECT rc, screen;
	memset(&rc, NULL, sizeof(RECT));
	memset(&screen, NULL, sizeof(RECT));
	GetWindowRect(GetDesktopWindow(), &screen);

	HWND hWnd = ::CreateWindow
	(
		m_wClassName,
		m_wWindowName,
		WS_POPUP | WS_VISIBLE,
		((screen.right - screen.left) - m_Mode.Width) * 0.5f,
		((screen.bottom - screen.top) - m_Mode.Height) * 0.5f,
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

	ResizeWindow(m_Mode.Width, m_Mode.Height);

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
			Update();
			LateUpdate();
			Render();
			//PrintInfo();
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

	hr = CreateDepthStencilView();
	if (FAILED(hr))
		return hr;

	m_pDXDC->OMSetRenderTargets
	(
		1,
		&m_pRenderTarget,
		m_pDSView
	);

	SetViewport();

	hr = CreateStateObject();
	if (FAILED(hr))
		return hr;
	
	return hr;
}

void Device::ReleaseDX()
{
	if(m_pDXDC != nullptr)
		m_pDXDC->ClearState();

	ReleaseStateObject();

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
	sd.SampleDesc.Count = m_iAA;
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
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.SampleDesc.Count = m_iAA;
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

HRESULT Device::CreateStateObject()
{
	HRESULT hr = S_OK;

	hr = CreateRasterState();
	if (FAILED(hr))
		return hr;

	hr = CreateDepthStencilState();
	if (FAILED(hr))
		return hr;

	hr = CreateSamplerState();
	if (FAILED(hr))
		return hr;

	hr = CreateBlendState();
	if (FAILED(hr))
		return hr;

	return hr;
}

HRESULT Device::CreateRasterState()
{
	HRESULT hr = S_OK;

	D3D11_RASTERIZER_DESC rd;
	::memset(&rd, NULL, sizeof(D3D11_RASTERIZER_DESC));

	rd.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	rd.FrontCounterClockwise = FALSE;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0;
	rd.SlopeScaledDepthBias = 0;
	rd.DepthClipEnable = FALSE;
	rd.ScissorEnable = FALSE;
	rd.MultisampleEnable = TRUE;
	rd.AntialiasedLineEnable = FALSE;

	hr = m_pDevice->CreateRasterizerState(&rd, &m_pRState[_RASTER::RS_SOLID_BACK]);
	if (FAILED(hr))
		return hr;

	//rd.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
	rd.FrontCounterClockwise = TRUE;
	rd.DepthClipEnable = TRUE;
	hr = m_pDevice->CreateRasterizerState(&rd, &m_pRState[_RASTER::RS_SOLID_FRONT]);
	if (FAILED(hr))
		return hr;

	rd.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	rd.FrontCounterClockwise = FALSE;
	rd.DepthClipEnable = FALSE;

	hr = m_pDevice->CreateRasterizerState(&rd, &m_pRState[_RASTER::RS_WIREFRAME_NONE]);
	if (FAILED(hr))
		return hr;

	m_pDXDC->RSSetState(m_pRState[_RASTER::RS_SOLID_BACK]);

	return hr;
}

HRESULT Device::CreateDepthStencilState()
{
	HRESULT hr = S_OK;

	D3D11_DEPTH_STENCIL_DESC dd;
	::memset(&dd, NULL, sizeof(D3D11_DEPTH_STENCIL_DESC));

	// ZWrite Off
	dd.DepthEnable = FALSE;								// Default : TRUE  
	dd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;		// Default : D3D11_DEPTH_WRITE_MASK_ALL
	dd.DepthFunc = D3D11_COMPARISON_LESS;				// Default : D3D11_COMPARISON_LESS
	dd.StencilEnable = FALSE;							// Default : FALSE
	dd.StencilReadMask = 0xff;							// Default : 0xff
	dd.StencilWriteMask = 0xff;							// Default : 0xff
	// dd.FrontFace, dd.BackFace
	/*
	dd.FrontFace와 dd.BackFace의 자료형은 D3D11_DEPTH_STENCILOP_DESC
	해당 구조체는
	D3D11_STENCIL_OP		StencilFailOp		( Default : D3D11_STENCIL_OP_KEEP )
	D3D11_STENCIL_OP		StencilDepthFailOp	( Default : D3D11_STENCIL_OP_KEEP )
	D3D11_STENCIL_OP		StencilPassOp		( Default : D3D11_STENCIL_OP_KEEP )
	D3D11_COMPARISON_FUNC 	StencilFunc			( Default : D3D11_COMPARISON_LESS )
	로 구성되어 있다
	*/
	hr = m_pDevice->CreateDepthStencilState(&dd, &m_pDSState[_DEPTHSTENCIL::DEPTH_WRITE_OFF]);
	if (FAILED(hr))
		return hr;

	// Z Write On, Z Test Off
	dd.DepthEnable = TRUE;
	dd.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
	hr = m_pDevice->CreateDepthStencilState(&dd, &m_pDSState[_DEPTHSTENCIL::DEPTH_WRITE_ON_TEST_OFF]);
	if (FAILED(hr))
		return hr;

	// Z Write On, Z Test On
	dd.DepthFunc = D3D11_COMPARISON_LESS;
	hr = m_pDevice->CreateDepthStencilState(&dd, &m_pDSState[_DEPTHSTENCIL::DEPTH_WRITE_ON_TEST_ON]);
	if (FAILED(hr))
		return hr;

	// Stencil Mirror
	dd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dd.StencilEnable = TRUE;
	dd.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	dd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	dd.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_REPLACE;
	dd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dd.BackFace = dd.FrontFace;
	hr = m_pDevice->CreateDepthStencilState(&dd, &m_pDSState[_DEPTHSTENCIL::STENCIL_MIRROR]);
	if (FAILED(hr))
		return hr;

	// Stencil Reflection
	dd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dd.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	dd.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	dd.BackFace = dd.FrontFace;
	hr = m_pDevice->CreateDepthStencilState(&dd, &m_pDSState[_DEPTHSTENCIL::STENCIL_REFLECTION]);
	if (FAILED(hr))
		return hr;

	m_pDXDC->OMSetDepthStencilState(m_pDSState[_DEPTHSTENCIL::DEPTH_WRITE_ON_TEST_ON], 0);

	return hr;
}

HRESULT Device::CreateSamplerState()
{
	HRESULT hr = S_OK;

	D3D11_SAMPLER_DESC sd;
	::memset(&sd, NULL, sizeof(D3D11_SAMPLER_DESC));

	sd.Filter = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC;
	sd.MaxAnisotropy = m_iAF;

	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;
	sd.MipLODBias = 0;

	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd.BorderColor[0] = 1;
	sd.BorderColor[1] = 1;
	sd.BorderColor[2] = 1;
	sd.BorderColor[3] = 1;

	sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	hr = m_pDevice->CreateSamplerState(&sd, &m_pSState[_SAMPLER::SS_CLAMP]);
	if (FAILED(hr))
		return hr;

	sd.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;

	hr = m_pDevice->CreateSamplerState(&sd, &m_pSState[_SAMPLER::SS_REPEAT]);
	if (FAILED(hr))
		return hr;

	m_pDXDC->PSSetSamplers(0, 1, &m_pSState[_SAMPLER::SS_REPEAT]);

	return hr;
}

HRESULT Device::CreateBlendState()
{
	HRESULT hr = S_OK;

	D3D11_BLEND_DESC bd;
	::memset(&bd, NULL, sizeof(D3D11_BLEND_DESC));

	bd.AlphaToCoverageEnable = false;
	bd.IndependentBlendEnable = false;

	// Render Target은 최대 8개까지 세팅이 가능하기 때문에
	// D3D11_BLEND_DESC 안에는 D3D11_RENDER_TARGET_BLEND_DESC 자료형 변수가 8개짜리 배열로 있습니다
	// 따라서 각각의 렌더타겟에 대해 설정해줄 수 있게 됩니다
	bd.RenderTarget[0].BlendEnable = TRUE;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = m_pDevice->CreateBlendState(&bd, &m_pBState[_BLEND::BS_DEFAULT]);
	if (FAILED(hr))
		return hr;

	bd.RenderTarget[0].RenderTargetWriteMask = 0;
	hr = m_pDevice->CreateBlendState(&bd, &m_pBState[_BLEND::BS_NORENDER]);
	if (FAILED(hr))
		return hr;

	float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pDXDC->OMSetBlendState(m_pBState[_BLEND::BS_DEFAULT], blendFactors, 0xffffffff);

	return hr;
}

void Device::ReleaseStateObject()
{
	ReleaseBlendState();
	ReleaseSamplerState();
	ReleaseDepthStencilState();
	ReleaseRasterState();
}

void Device::ReleaseRasterState()
{
	for (int i = 0; i < _RASTER::RS_MAX; ++i)
	{
		if (m_pRState[i] != nullptr)
		{
			m_pRState[i]->Release();
			m_pRState[i] = nullptr;
		}
	}
}

void Device::ReleaseDepthStencilState()
{
	for (int i = 0; i < _DEPTHSTENCIL::DS_MAX; ++i)
	{
		if (m_pDSState[i] != nullptr)
		{
			m_pDSState[i]->Release();
			m_pDSState[i] = nullptr;
		}
	}
}

void Device::ReleaseSamplerState()
{
	for (int i = 0; i < _SAMPLER::SS_MAX; ++i)
	{
		if (m_pSState[i] != nullptr)
		{
			m_pSState[i]->Release();
			m_pSState[i] = nullptr;
		}
	}
}

void Device::ReleaseBlendState()
{
	for (int i = 0; i < _BLEND::BS_MAX; ++i)
	{
		if (m_pBState[i] != nullptr)
		{
			m_pBState[i]->Release();
			m_pBState[i] = nullptr;
		}
	}
}

void Device::ClearBackBuffer()
{
	//XMFLOAT4 col = XMFLOAT4(0, 0.125f, 0.3f, 1);
	//XMFLOAT4 col = XMFLOAT4(1.0f, 0.8f, 1.0f, 1);
	XMFLOAT4 col = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	m_pDXDC->ClearRenderTargetView(m_pRenderTarget, (float*)&col);
	m_pDXDC->ClearDepthStencilView(m_pDSView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Device::Update()
{
	CTIMER::Update();
	m_vTime = XMFLOAT4
	(
		CTIMER::DeltaTime(),
		CTIMER::UnscaledDeltaTime(),
		CTIMER::Time(),
		CTIMER::UnscaledTime()
	);

	CINPUT::Update(m_vTime.y);

	if (CINPUT::GetKeyPressed(DIK_EQUALS))
		CTIMER::SpeedUp();
	if (CINPUT::GetKeyPressed(DIK_MINUS))
		CTIMER::SpeedDown();

	//Shader::g_pCurrent->SetTime(m_vTime);

	//m_pCamera->Update();
	m_pObj->Update(m_vTime.x);
	m_pMirror->Update(m_vTime.x);
	for (auto* Obj : *m_pTerrain)
	{
		Obj->Update(m_vTime.x);
	}
}

void Device::LateUpdate()
{
	m_pCamera->LateUpdate(m_vTime.x);
	m_pObj->LateUpdate(m_vTime.x);
	m_pMirror->LateUpdate(m_vTime.x);
	for (auto* Obj : *m_pTerrain)
	{
		Obj->LateUpdate(m_vTime.x);
	}
}

void Device::Render()
{
#pragma region AASASGASGGASDASGD
	/*
	// Object Rendering
	XMMATRIX mMirror = XMMatrixIdentity();
	XMFLOAT4X4 fMirror;
	XMStoreFloat4x4(&fMirror, mMirror);

	//m_pDXDC->OMSetDepthStencilState(m_pDSState[_DEPTHSTENCIL::DEPTH_WRITE_ON_TEST_ON], 1);
	//for (auto* Obj : *m_pTerrain)
	//{
	//	((GameObject*)Obj)->SetReflect(fMirror);
	//	Obj->Render();
	//}
	((GameObject*)m_pObj)->SetReflect(fMirror);
	m_pObj->Render();

	// Mirror Write
	float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//m_pDXDC->OMSetBlendState(m_pBState[_BLEND::BS_NORENDER], blendFactors, 0xffffffff);
	//m_pDXDC->OMSetDepthStencilState(m_pDSState[_DEPTHSTENCIL::STENCIL_MIRROR], 1);
	//m_pMirror->Render();

	// Make Mirror Matrix
	XMVECTOR vPlane = XMLoadFloat4(&((Quad*)m_pMirror)->GetPlane());
	mMirror = XMMatrixReflect(vPlane);
	XMStoreFloat4x4(&fMirror, mMirror);

	// Reflect Object Render
	m_pDXDC->RSSetState(m_pRState[_RASTER::RS_SOLID_FRONT]);
	m_pDXDC->OMSetBlendState(m_pBState[_BLEND::BS_DEFAULT], blendFactors, 0xffffffff);
	m_pDXDC->OMSetDepthStencilState(m_pDSState[_DEPTHSTENCIL::DEPTH_WRITE_ON_TEST_ON], 0);
	//m_pDXDC->OMSetDepthStencilState(m_pDSState[_DEPTHSTENCIL::STENCIL_REFLECTION], 1);
	//for (auto* Obj : *m_pTerrain)
	//{
	//	((GameObject*)Obj)->SetReflect(fMirror);
	//	Obj->Render();
	//}
	((GameObject*)m_pObj)->SetReflect(fMirror);
	m_pObj->Render();

	// Mirror Render
	//m_pDXDC->RSSetState(m_pRState[_RASTER::RS_SOLID_BACK]);
	//m_pDXDC->OMSetDepthStencilState(m_pDSState[_DEPTHSTENCIL::DEPTH_WRITE_ON_TEST_ON], 0);
	//m_pMirror->Render();
	*/
#pragma endregion

	//for (auto* Obj : *m_pTerrain)
	//{
	//	Obj->Render();
	//}
	XMMATRIX mMirror = XMMatrixIdentity();
	XMFLOAT4X4 fMirror;
	XMStoreFloat4x4(&fMirror, mMirror);

	((GameObject*)m_pObj)->SetReflect(fMirror);
	m_pObj->Render();

	XMVECTOR vPlane = XMLoadFloat4(&((Quad*)m_pMirror)->GetPlane());
	mMirror = XMMatrixReflect(vPlane);
	XMStoreFloat4x4(&fMirror, mMirror);

	((GameObject*)m_pObj)->SetReflect(fMirror);
	m_pObj->Render();

	/*
	-> Render Object
	-> Set Blend [BS_NORENDER]
	-> Set Depth [STENCIL_MIRROR]
	-> Mirror Write
	-> Set Raster [RS_SOLID_FRONT]
	-> Set Depth [STENCIL_REFLECTION]
	-> Render Reflected Object
	-> Set Depth [DEPTH_WRITE_ON_TEST_ON]
	-> Set Blend [BS_DEFAULT]
	-> Render Mirror
	*/
}

void Device::PrintInfo()
{
	CFONT::Begin();

	int y = -13;
	CFONT::PrintInfo(1, y += 14, XMFLOAT4(1, 1, 1, 1), L"Delta Time : %.6f(s)", m_vTime.x);
	CFONT::PrintInfo(1, y += 14, XMFLOAT4(1, 1, 1, 1), L"Play Time : %.2f(s)", m_vTime.w);
	CFONT::PrintInfo(1, y += 28, XMFLOAT4(1, 1, 1, 1), L"Directional Light : (1.0f, -1.0f, 3.0f)");

	CFONT::End();
}

void Device::Flip()
{
	m_pSwapChain->Present(0, 0);
}

BOOL Device::LoadData()
{
	BOOL res = TRUE;

	m_pTerrain = new std::vector<iGameObject*>();
	m_pTerrain->clear();

	if (FAILED(LoadShader()))
		return FALSE;

	res = LoadCamera();
	if (!res)
		return res;

	res = LoadAddOn();
	if (!res)
		return res;

	res = LoadManager();
	if (!res)
		return res;

	LoadMap();

	return res;
}

void Device::ReleaseData()
{
	ReleaseManager();
	ReleaseAddOn();
	ReleaseCamera();
	ReleaseShader();
}

HRESULT Device::LoadShader()
{
	HRESULT hr = S_OK;

	Shader *pShader = nullptr;
	_LAYOUT layoutTag = _LAYOUT::LAYOUT_MAX;
	_SHADER shaderTag = _SHADER::SHADER_MAX;
	
	pShader = new Shader();
	layoutTag = _LAYOUT::L_POS;
	shaderTag = _SHADER::SHADER_ERROR;
	hr = pShader->Create(L"..\\Src\\hlsl\\Error.fx", layoutTag);
	if (FAILED(hr))
		return hr;
	//Shader::g_pShaders[shaderTag] = pShader;
	Shader::AddShader(pShader, shaderTag);

	pShader = new Shader();
	layoutTag = _LAYOUT::L_POS_NRM_UV1;
	shaderTag = _SHADER::SHADER_DEFAULT;
	hr = pShader->Create(L"..\\Src\\hlsl\\Character.fx", layoutTag);
	//if (FAILED(hr))
	//	return hr;
	//Shader::g_pShaders[shaderTag] = pShader;
	Shader::AddShader(pShader, shaderTag);

	pShader = new Shader();
	shaderTag = _SHADER::SHADER_TERRAIN;
	hr = pShader->Create(L"..\\Src\\hlsl\\Terrain.fx", layoutTag);
	//if (FAILED(hr))
	//	return hr;
	//Shader::g_pShaders[shaderTag] = pShader;
	Shader::AddShader(pShader, shaderTag);

	pShader = new Shader();
	layoutTag = _LAYOUT::L_POS;
	shaderTag = _SHADER::SHADER_COLOR;
	hr = pShader->Create(L"..\\Src\\hlsl\\Color.fx", layoutTag);
	//if (FAILED(hr))
	//	return hr;
	//Shader::g_pShaders[shaderTag] = pShader;
	Shader::AddShader(pShader, shaderTag);

	return S_OK;
}

BOOL Device::LoadCamera()
{
	BOOL res = TRUE;

	m_pCamera = new Camera();
	res = m_pCamera->Init(float(m_Mode.Width), float(m_Mode.Height), XM_PIDIV4);
	if (!res)
		return FALSE;

	g_pCamera = m_pCamera;

	return TRUE;
}

BOOL Device::LoadAddOn()
{
	BOOL res = TRUE;
	
	res = CFONT::Init(m_pDevice, L"..\\..\\Extern\\Font\\굴림9k.sfont");
	if (!res)
		return res;

	HRESULT hr = S_OK;
	hr = CINPUT::Init(m_hInst, m_hWnd, m_Mode.Width, m_Mode.Height);
	if (FAILED(hr))
		return FALSE;

	CTIMER::Init();

	return res;
}

BOOL Device::LoadManager()
{
	BOOL res = TRUE;

	BufferCreator::Init(m_pDevice);
	MeshCreator::Init(m_pDevice);

	TextureLoader::Init(m_pDevice, m_pDXDC);

	namespace fs = std::experimental::filesystem;
	fs::path _path = "..\\Data\\Textures\\";
	if (!fs::is_directory(_path))
		return FALSE;

	std::string filename = "";

	for (const auto& entry : fs::directory_iterator{ _path })
	{
		if (fs::is_regular_file(entry.status()))
		{
			filename = entry.path().string();

			if (!filename.compare("..\\Data\\Textures\\magenta.jpg"))
				continue;

			wchar_t* pStr;
			int strSize = MultiByteToWideChar(CP_ACP, 0, filename.c_str(), -1, NULL, NULL);
			pStr = new WCHAR[strSize];
			MultiByteToWideChar(CP_ACP, 0, filename.c_str(), strlen(filename.c_str()) + 1, pStr, strSize);

			texname.push_back(pStr);
		}
	}
	wchar_t* pStr;
	filename = "..\\Data\\Textures\\magenta.jpg";
	int strSize = MultiByteToWideChar(CP_ACP, 0, filename.c_str(), -1, NULL, NULL);
	pStr = new WCHAR[strSize];
	MultiByteToWideChar(CP_ACP, 0, filename.c_str(), strlen(filename.c_str()) + 1, pStr, strSize);
	texname.push_back(pStr);

	GeoLoader::Init();
	GeoLoader::LoadGeometry(L"..\\Data\\Models\\cube.txt", L"cube");
	GeoLoader::LoadGeometry(L"..\\Data\\Models\\quad.txt", L"quad");
	GeoLoader::LoadGeometry(L"..\\Data\\Models\\dwarf.txt", L"dwarf", true);

	return res;
}

void Device::ReleaseCamera()
{
	if (m_pCamera != nullptr)
	{
		delete m_pCamera;
		m_pCamera = nullptr;
		g_pCamera = nullptr;
	}
}

void Device::ReleaseShader()
{
	Shader::ReleaseShaders();
}

void Device::ReleaseAddOn()
{
	CTIMER::Release();
	CINPUT::Release();
	CFONT::Release();
}

void Device::ReleaseManager()
{
	TextureLoader::Release();
	MeshCreator::Release();
	BufferCreator::Release();
	GeoLoader::Release();
}

void Device::LoadMap()
{
	std::vector<std::string> tex;
	std::ifstream file;

	file.open("..\\..\\Extern\\Data\\Maps\\Map.txt", std::ios_base::in);

	if (!file.is_open())
	{
		file.close();

		return;
	}

	int texCount = 0;
	file >> texCount;

	for (int i = 0; i < texCount; ++i)
	{
		std::string str = "";

		file >> str;

		tex.push_back(str);
	}

	int objCount = 0;
	file >> objCount;
	for (int i = 0; i < objCount; ++i)
	{
		int x = 0, y = 0, z = 0, index = 0;
		file >> x >> y >> z >> index;

		XMFLOAT3 vPos = XMFLOAT3(float(x), float(y), float(z));
		iGameObject *pObj = new GameObject();
		pObj->Init(vPos);
		TCHAR *tmp = CharToWChar(tex[index].c_str());
		bool bFind = false;
		for (unsigned int j = 0; j < texname.size(); ++j)
		{
			if (!wcscmp(tmp, texname[j]))
			{
				((GameObject*)pObj)->SetTexture(texname[j]);
				bFind = true;
				break;
			}
		}

		if (!bFind)
		{
			const TCHAR* magenta = L"..\\Data\\Textures\\magenta.jpg";
			for (unsigned int j = 0; j < texname.size(); ++j)
			{
				if (!wcscmp(magenta, texname[j]))
				{
					((GameObject*)pObj)->SetTexture(texname[j]);
					break;
				}
			}
		}

		m_pTerrain->push_back(pObj);

		free(tmp);
		tmp = nullptr;
	}
}