#include "Device.h"

#include "BufferCreator.h"
#include "MeshCreator.h"
#include "GeoLoader.h"
#include "TextureLoader.h"

#include <filesystem>
#include <iostream>
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
	m_FeatureLevel(D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0),
	m_pCursor(nullptr)
{
	m_Mode.Width = 1600;
	m_Mode.Height = 900;
	m_Mode.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	m_iAF = 4;
	m_iAA = 4;

	m_renderMode = RENDERMODE::EDITMAP;
	m_texIndex = 0;
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
	}

	return res;
}

BOOL Device::Run()
{
	//GameObject *pTmpasdmfpasgd = new GameObject();
	//pTmpasdmfpasgd->Init();

	if (!MessagePump())
		return FALSE;

	return TRUE;
}

void Device::Release()
{
	if (m_pDevice == nullptr)
		return;

	SaveMap();

	auto i = texname.begin();
	for (; i != texname.end(); ++i)
	{
		delete (*i);
		*i = nullptr;
	}
	texname.clear();

	if (m_pCursor != nullptr)
	{
		delete m_pCursor;
		m_pCursor = nullptr;
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
	if (tempObj != nullptr)
	{
		if (tempObj->size() > 0)
		{
			auto iter = tempObj->begin();

			for (; iter != tempObj->end(); ++iter)
			{
				if (*iter != nullptr)
				{
					delete (*iter);
					(*iter) = nullptr;
				}
			}
		}

		tempObj->clear();
		delete tempObj;
		tempObj = nullptr;
	}

	if (m_pTexList != nullptr)
	{
		if (m_pTexList->size() > 0)
		{
			auto iter = m_pTexList->begin();

			for (; iter != m_pTexList->end(); ++iter)
			{
				if (*iter != nullptr)
				{
					delete (*iter);
					(*iter) = nullptr;
				}
			}
		}

		m_pTexList->clear();
		delete m_pTexList;
		m_pTexList = nullptr;
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
			PrintInfo();
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
	td.Format = DXGI_FORMAT_D32_FLOAT;
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

	rd.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;

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

	// ZWrite On, ZTest On
	dd.DepthEnable = TRUE;
	dd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dd.DepthFunc = D3D11_COMPARISON_LESS;
	dd.StencilEnable = FALSE;

	hr = m_pDevice->CreateDepthStencilState(&dd, &m_pDSState[_DEPTHSTENCIL::DS_WRITE_ON_TEST_ON]);
	if (FAILED(hr))
		return hr;

	dd.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;

	hr = m_pDevice->CreateDepthStencilState(&dd, &m_pDSState[_DEPTHSTENCIL::DS_WRITE_ON_TEST_OFF]);
	if (FAILED(hr))
		return hr;

	m_pDXDC->OMSetDepthStencilState(m_pDSState[_DEPTHSTENCIL::DS_WRITE_ON_TEST_ON], 0);

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

void Device::ReleaseStateObject()
{
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

void Device::ClearBackBuffer()
{
	//XMFLOAT4 col = XMFLOAT4(0, 0.125f, 0.3f, 1);
	//XMFLOAT4 col = XMFLOAT4(1.0f, 0.8f, 1.0f, 1);
	XMFLOAT4 col = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	m_pDXDC->ClearRenderTargetView(m_pRenderTarget, (float*)&col);
	m_pDXDC->ClearDepthStencilView(m_pDSView, D3D11_CLEAR_DEPTH, 1.0f, 0);
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

	g_pShader->SetTime(m_vTime);

	//m_pCamera->Update();

	if (CINPUT::GetKeyPressedDown(DIK_TAB))
		m_renderMode = (RENDERMODE)(m_renderMode ^ RENDERMODE::SELECTTEX);
	else
	{
		if (CINPUT::GetKeyPressed(DIK_LALT))
			m_renderMode = RENDERMODE(m_renderMode | RENDERMODE::ROTATECAM);
		else if (CINPUT::GetKeyPressedUp(DIK_LALT))
			m_renderMode = RENDERMODE(m_renderMode & !(RENDERMODE::ROTATECAM));
	}

	if (CINPUT::GetKeyPressedDown(DIK_R))
	{
		auto iter = m_pTerrain->begin();
		for (; iter != m_pTerrain->end(); ++iter)
		{
			delete *iter;
			*iter = nullptr;
		}

		m_pTerrain->clear();
		delete m_pTerrain;
		m_pTerrain = nullptr;
		m_pTerrain = new std::vector<iGameObject*>();
	}

	int mx = 0, my = 0;
	CINPUT::GetMousePosition(mx, my);
	XMFLOAT3 vWorld = XMFLOAT3(0, 0, 0);

	switch (m_renderMode)
	{
	case RENDERMODE::EDITMAP:
		#pragma region EDITMAP
		g_pCamera->Set3D();
		vWorld = g_pCamera->ScreenToWorldPoint(XMFLOAT3(float(mx), float(my), 0.0f));
		vWorld.x = roundf(vWorld.x);
		vWorld.z = roundf(vWorld.z);
		((GameObject*)m_pCursor)->SetPos(vWorld);

		if (CINPUT::GetKeyPressed(DIK_LSHIFT))
		{
			static float sx = 0.0f, sz = 0.0f;
			static float ex = 0.0f, ez = 0.0f;

			if (CINPUT::GetMouseButtonDown(0))
			{
				sx = vWorld.x;
				sz = vWorld.z;
			}
			else if (CINPUT::GetMouseButtonUp(0))
			{
				auto iter = tempObj->begin();
				for (; iter != tempObj->end(); ++iter)
				{
					GameObject *pL = (GameObject*)(*iter);
					auto iter2 = m_pTerrain->begin();
					bool bExist = false;
					for (; iter2 != m_pTerrain->end(); ++iter2)
					{
						GameObject *pR = (GameObject*)(*iter2);
						
						if (pL->GetPos().x == pR->GetPos().x && pL->GetPos().z == pR->GetPos().z)
						{
							bExist = true;
						}
					}
					if (bExist)
					{
						if ((*iter) != nullptr)
						{
							delete *iter;
							*iter = nullptr;
						}
					}
					else
						m_pTerrain->push_back(*iter);					
				}
				tempObj->clear();
			}
			else if (CINPUT::GetMouseButton(0))
			{
				if (ex != vWorld.x || ez != vWorld.z)
				{
					auto iter = tempObj->begin();
					for (; iter != tempObj->end(); ++iter)
					{
						if ((*iter) != nullptr)
						{
							delete *iter;
							*iter = nullptr;
						}
					}
					tempObj->clear();

					ex = vWorld.x;
					ez = vWorld.z;

					float left = 0.0f, top = 0.0f, right = 0.0f, bottom = 0.0f;
					left = sx < ex ? sx : ex;
					top = sz < ez ? sz : ez;
					right = sx < ex ? ex : sx;
					bottom = sz < ez ? ez : sz;

					for (int i = left; i <= right; ++i)
					{
						for (int j = top; j <= bottom; ++j)
						{
							iGameObject *tmp = new GameObject();
							tmp->Init(XMFLOAT3(i, 0.0f, j));
							((GameObject*)tmp)->SetTexture(texname[m_texIndex]);
							tempObj->push_back(tmp);
						}
					}
				}
			}
		}
		else if (CINPUT::GetKeyPressedUp(DIK_LSHIFT))
		{
			auto iter = tempObj->begin();
			for (; iter != tempObj->end(); ++iter)
			{
				if ((*iter) != nullptr)
				{
					delete *iter;
					*iter = nullptr;
				}
			}
			tempObj->clear();
		}
		else
		{
			if (CINPUT::GetMouseButton(0))
			{
				XMVECTOR vL = XMLoadFloat3(&vWorld);
				XMVECTOR vR = XMVectorZero();

				auto iter = m_pTerrain->begin();

				for (; iter != m_pTerrain->end(); ++iter)
				{
					GameObject* pObj = (GameObject*)*iter;

					vR = XMLoadFloat3(&pObj->GetPos());

					if (XMVector3Equal(vL, vR))
						break;
				}

				if (iter == m_pTerrain->end())
				{
					iGameObject *tmp = new GameObject();
					tmp->Init(vWorld);
					((GameObject*)tmp)->SetTexture(texname[m_texIndex]);

					m_pTerrain->push_back(tmp);
				}
			}
		}

		if (CINPUT::GetMouseButton(1))
		{
			XMVECTOR vL = XMLoadFloat3(&vWorld);
			XMVECTOR vR = XMVectorZero();

			auto iter = m_pTerrain->begin();

			for (; iter != m_pTerrain->end(); ++iter)
			{
				GameObject* pObj = (GameObject*)*iter;

				vR = XMLoadFloat3(&pObj->GetPos());

				if (XMVector3Equal(vL, vR))
				{
					iGameObject *tmp = *iter;
					m_pTerrain->erase(iter);
					delete tmp;
					tmp = nullptr;

					break;
				}
			}
		}

		if (CINPUT::GetKeyPressed(DIK_LCONTROL))
		{
			if (CINPUT::GetKeyPressedDown(DIK_Z))
			{
				if (m_pTerrain->size() > 0)
				{
					iGameObject* tmp = m_pTerrain->operator[](m_pTerrain->size() - 1);
					m_pTerrain->pop_back();
					delete tmp;
					tmp = nullptr;
				}
			}
			else if (CINPUT::GetKeyPressedDown(DIK_S))
			{
				SaveMap();
			}
		}
		#pragma endregion
		break;

	case RENDERMODE::SELECTTEX:
		#pragma region SELECTTEX
		g_pCamera->Set2D();
		vWorld = g_pCamera->ScreenToWorldPoint(XMFLOAT3(float(mx), float(my), 0.0f));

		if (CINPUT::GetMouseButtonDown(0))
		{
			for (unsigned int i = 0; i < texname.size() - 1; ++i)
			{
				if (m_pTexList->operator[](i)->CheckBoundary(vWorld))
				{
					m_texIndex = i;
					break;
				}
			}
		}

		#pragma endregion
		break;

	case RENDERMODE::ROTATECAM:
		break;
	}

	((GameObject*)m_pCursor)->SetTexture(texname[m_texIndex]);
	m_pCursor->Update(m_vTime.x);
	for (iGameObject* Obj : *m_pTerrain)
	{
		Obj->Update(m_vTime.x);
	}

	for (Quad* Obj : *m_pTexList)
	{
		Obj->Update(m_vTime.x);
	}

	for (iGameObject* Obj : *tempObj)
	{
		Obj->Update(m_vTime.x);
	}

	//SaveMap();
}

void Device::LateUpdate()
{	
	g_pCamera->LateUpdate(m_vTime.x);

	g_pCamera->Set3D();
	m_pCursor->LateUpdate(m_vTime.x);
	for (iGameObject* Obj : *m_pTerrain)
	{
		Obj->LateUpdate(m_vTime.x);
	}

	g_pCamera->Set2D();
	for (Quad* Obj : *m_pTexList)
	{
		Obj->LateUpdate(m_vTime.x);
	}
	for (iGameObject* Obj : *tempObj)
	{
		Obj->LateUpdate(m_vTime.x);
	}

}

void Device::Render()
{
	g_pCamera->Set3D();
	
	m_pCursor->Render();
	for (iGameObject* Obj : *m_pTerrain)
	{
		Obj->Render();
	}

	if ((m_renderMode & RENDERMODE::SELECTTEX) == RENDERMODE::SELECTTEX)
	{
		g_pCamera->Set2D();
		for (Quad* Obj : *m_pTexList)
		{
			Obj->Render();
		}
	}
	for (iGameObject* Obj : *tempObj)
	{
		Obj->Render();
	}

}

void Device::PrintInfo()
{
	CFONT::Begin();

	int y = -13;
	CFONT::PrintInfo(1, y += 14, XMFLOAT4(1, 1, 1, 1), L"Delta Time : %.6f(s)", m_vTime.x);
	CFONT::PrintInfo(1, y += 14, XMFLOAT4(1, 1, 1, 1), L"Play Time : %.2f(s)", m_vTime.w);
	CFONT::PrintInfo(1, y += 14, XMFLOAT4(1, 1, 1, 1), L"Object Count : %d", m_pTerrain->size());

	CFONT::PrintInfo(1, y += 14, XMFLOAT4(1, 1, 1, 1), L"Render Mode : %s",
		m_renderMode == RENDERMODE::EDITMAP ? L"Edit Map" :
		(m_renderMode == RENDERMODE::SELECTTEX ? L"Select Texture" : L"Rotate Cam")
	);

	CFONT::PrintInfo(1, y += 14, XMFLOAT4(1, 1, 1, 1), L"Texture Index : %d", m_texIndex);

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

	m_pTexList = new std::vector<Quad*>();
	m_pTexList->clear();

	tempObj = new std::vector<iGameObject*>();
	tempObj->clear();

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

	m_pCursor = new GameObject();
	m_pCursor->Init();

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

	m_pShader = new Shader();
	hr = m_pShader->Create(L"..\\Src\\hlsl\\SolidColor_pnu.fx", _LAYOUT::L_POS_NRM_UV1);
	if (FAILED(hr))
		return hr;

	g_pShader = m_pShader;

	return hr;
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
	
	res = CFONT::Init(m_pDevice, L"..\\..\\Extern\\Font\\±¼¸²9k.sfont");
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

	GeoLoader::Init();
	GeoLoader::LoadGeometry(L"..\\Data\\Models\\cube.txt", L"cube");
	GeoLoader::LoadGeometry(L"..\\Data\\Models\\sphere.txt", L"sphere");
	GeoLoader::LoadGeometry(L"..\\Data\\Models\\quad.txt", L"quad");

	TextureLoader::Init(m_pDevice, m_pDXDC);
	TEXTURE* pTex = nullptr;
	Quad* pQuad = nullptr;
	texname.clear();

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
	wchar_t* pStr = nullptr;
	filename = "..\\Data\\Textures\\magenta.jpg";
	int strSize = MultiByteToWideChar(CP_ACP, 0, filename.c_str(), -1, NULL, NULL);
	pStr = new WCHAR[strSize];
	MultiByteToWideChar(CP_ACP, 0, filename.c_str(), strlen(filename.c_str()) + 1, pStr, strSize);
	texname.push_back(pStr);

	int posOffset = 0;
	for (unsigned int i = 0; i < texname.size(); ++i)
	{
		if (!wcscmp(texname[i], L"..\\Data\\Textures\\magenta.jpg"))
			continue;

		pTex = TextureLoader::GetTexture(texname[i]);
		pQuad = new Quad();
		pQuad->Init();
		pQuad->SetTexture(texname[i]);

		m_pTexList->push_back(pQuad);

		m_pTexList->operator[](posOffset)->SetPos(XMFLOAT3(m_Mode.Width * 0.5f - 100, m_Mode.Height * 0.5f - 100 - (posOffset * 175), 0));
		++posOffset;
	}

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
	if (m_pShader != nullptr)
	{
		delete m_pShader;
		m_pShader = nullptr;
	}

	g_pShader = nullptr;
}

void Device::ReleaseAddOn()
{
	CTIMER::Release();
	CINPUT::Release();
	CFONT::Release();
}

void Device::SaveMap()
{
	std::ofstream file;
	std::string enter = "\n";
	char buff[1024] = { '\0' };

	file.open("..\\..\\Extern\\Data\\Maps\\Map.txt", std::ios_base::out);

	if (!file.is_open())
	{
		// Error
	}

	sprintf_s(buff, 1024, "%d", texname.size() - 1);
	file.write(buff, strlen(buff));
	file.write(enter.c_str(), enter.length());
	for (unsigned int i = 0; i < texname.size() - 1; ++i)
	{
		char *tmp = WCharToChar(texname[i]);
		std::string str = "";

		str = tmp + enter;
		file.write(str.c_str(), str.length());
		
		free(tmp);
		tmp = nullptr;
	}
	file.write(enter.c_str(), enter.length());

	sprintf_s(buff, 1024, "%d", m_pTerrain->size());
	file.write(buff, strlen(buff));
	file.write(enter.c_str(), enter.length());
	for (unsigned int i = 0; i < m_pTerrain->size(); ++i)
	{
		std::string str = "";

		char pos[1024] = { '\0', };
		GameObject* pObj = (GameObject*)(m_pTerrain->operator[](i));
		XMFLOAT3 vPos = pObj->GetPos();
		sprintf_s(pos, 1024, "%d %d %d ",
			int(vPos.x),
			int(vPos.y - 1.0f),
			int(vPos.z)
		);

		char* sL = WCharToChar(pObj->GetTextureName());
		char* sR = nullptr;
		str = sL;

		for (unsigned int i = 0; i < texname.size(); ++i)
		{
			sR = WCharToChar(texname[i]);

			if (!str.compare(sR))
			{
				sprintf_s(buff, 1024, "%d", i);
				free(sR);
				sR = nullptr;
				break;
			}

			free(sR);
			sR = nullptr;
		}

		str = pos;
		str += buff;

		file.write(str.c_str(), str.length());
		file.write(enter.c_str(), enter.length());

		free(sL);
		sL = nullptr;
	}

	file.close();
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

		XMFLOAT3 vPos = XMFLOAT3(float(x), float(y+1), float(z));
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

void Device::ReleaseManager()
{
	TextureLoader::Release();
	GeoLoader::Release();
	MeshCreator::Release();
	BufferCreator::Release();
}