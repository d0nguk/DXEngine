#define INPUT_EXPORT __declspec(dllexport)

#include "Input.h"

namespace ENGINEDLL
{
	IDirectInput8* CINPUT::m_pInput = nullptr;
	IDirectInputDevice8* CINPUT::m_pKeyboard = nullptr;
	IDirectInputDevice8* CINPUT::m_pMouse = nullptr;

	HWND CINPUT::m_hWnd;
	unsigned char CINPUT::m_prevKeyState[256] = { 0x00, };
	unsigned char CINPUT::m_keyState[256] = { 0x00, };
	CINPUT::KEYSTATE CINPUT::m_curKeyState[256] = { CINPUT::KEYSTATE::RELEASED, };

	unsigned char CINPUT::m_prevMouseState[4] = { 0x00, };
	CINPUT::KEYSTATE CINPUT::m_curMouseState[4] = { CINPUT::KEYSTATE::RELEASED, };
	DIMOUSESTATE CINPUT::m_mouseState;

	int	CINPUT::m_iWidth = 0;
	int	CINPUT::m_iHeight = 0;
	int	CINPUT::m_iMouseX = 0;
	int	CINPUT::m_iMouseY = 0;
	bool CINPUT::m_bInit = false;

	HRESULT CINPUT::Init(HINSTANCE hInst, HWND hWnd, int width, int height)
	{
		HRESULT hr = S_OK;

		m_iWidth = width;
		m_iHeight = height;

		m_hWnd = hWnd;

		hr = DirectInput8Create
		(
			hInst,
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			(void**)&m_pInput,
			nullptr
		);
		
		if (FAILED(hr))
			return hr;

		hr = InitKeyboard(hWnd);
		if (FAILED(hr))
			return hr;

		hr = InitMouse(hWnd);
		if (FAILED(hr))
			return hr;

		m_bInit = true;

		return hr;
	}

	void CINPUT::Update(float dTime)
	{
		Read();
		Process();
	}

	void CINPUT::Release()
	{
		if (m_pMouse != nullptr)
		{
			m_pMouse->Unacquire();
			m_pMouse->Release();
			m_pMouse = nullptr;
		}

		if (m_pKeyboard != nullptr)
		{
			m_pKeyboard->Unacquire();
			m_pKeyboard->Release();
			m_pKeyboard = nullptr;
		}

		m_bInit = false;
	}

	HRESULT CINPUT::InitKeyboard(HWND hWnd)
	{
		HRESULT hr = S_OK;

		hr = m_pInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr);
		if (FAILED(hr))
			return hr;

		hr = m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
		if (FAILED(hr))
			return hr;

		hr = m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		if (FAILED(hr))
			return hr;

		hr = m_pKeyboard->Acquire();
		if (FAILED(hr))
			return hr;

		return hr;
	}

	HRESULT CINPUT::InitMouse(HWND hWnd)
	{
		HRESULT hr = S_OK;

		::memset(&m_mouseState, NULL, sizeof(DIMOUSESTATE));

		hr = m_pInput->CreateDevice(GUID_SysMouse, &m_pMouse, NULL);
		if (FAILED(hr))
			return hr;

		hr = m_pMouse->SetDataFormat(&c_dfDIMouse);
		if (FAILED(hr))
			return hr;

		hr = m_pMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		if (FAILED(hr))
			return hr;

		hr = m_pMouse->Acquire();
		if (FAILED(hr))
			return hr;

		RECT rc;
		GetWindowRect(m_hWnd, &rc);
		m_iMouseX = (int)((rc.right - rc.left) * 0.5f + rc.left);
		m_iMouseY = (int)((rc.bottom - rc.top) * 0.5f + rc.top);
		SetCursorPos(m_iMouseX, m_iMouseY);

		return hr;
	}

	HRESULT CINPUT::Read()
	{
		HRESULT hr = S_OK;

		hr = ReadKeyboard();
		if (FAILED(hr))
			return hr;

		hr = ReadMouse();
		if (FAILED(hr))
			return hr;

		return hr;
	}

	HRESULT CINPUT::ReadKeyboard()
	{
		HRESULT hr = S_OK;
		hr = m_pKeyboard->GetDeviceState(sizeof(m_keyState), (void*)&m_keyState);

		if (FAILED(hr))
		{
			if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
				m_pKeyboard->Acquire();
			else
				return hr;
		}

		for (int i = 0; i < 256; ++i)
		{
			// Case 1 : Before : X // After : X
			if (m_prevKeyState[i] == 0x00 && m_keyState[i] == 0x00)
			{
				m_curKeyState[i] = KEYSTATE::RELEASED;
			}
			// Case 2 : Before : X // After : O
			else if (m_prevKeyState[i] == 0x00 && m_keyState[i] == 0x80)
			{
				m_curKeyState[i] = KEYSTATE::PRESSEDDOWN;
			}
			// Case 3 : Before : O // After : X
			else if (m_prevKeyState[i] == 0x80 && m_keyState[i] == 0x00)
			{
				m_curKeyState[i] = KEYSTATE::PRESSEDUP;
			}
			// Case 4 : Before : O // After : O
			else if (m_prevKeyState[i] == 0x80 && m_keyState[i] == 0x80)
			{
				m_curKeyState[i] = KEYSTATE::PRESSED;
			}

			m_prevKeyState[i] = m_keyState[i];
		}

		return hr;
	}

	HRESULT CINPUT::ReadMouse()
	{
		HRESULT hr = S_OK;
		hr = m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), (void*)&m_mouseState);

		if (FAILED(hr))
		{
			if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
				m_pKeyboard->Acquire();
			else
				return hr;
		}

		for (int i = 0; i < 4; ++i)
		{
			// Case 1 : Before : X // After : X
			if (m_prevMouseState[i] == 0x00 && m_mouseState.rgbButtons[i] == 0x00)
			{
				m_curMouseState[i] = KEYSTATE::RELEASED;
			}
			// Case 2 : Before : X // After : O
			else if (m_prevMouseState[i] == 0x00 && m_mouseState.rgbButtons[i] == 0x80)
			{
				m_curMouseState[i] = KEYSTATE::PRESSEDDOWN;
			}
			// Case 3 : Before : O // After : X
			else if (m_prevMouseState[i] == 0x80 && m_mouseState.rgbButtons[i] == 0x00)
			{
				m_curMouseState[i] = KEYSTATE::PRESSEDUP;
			}
			// Case 4 : Before : O // After : O
			else if (m_prevMouseState[i] == 0x80 && m_mouseState.rgbButtons[i] == 0x80)
			{
				m_curMouseState[i] = KEYSTATE::PRESSED;
			}

			m_prevMouseState[i] = m_mouseState.rgbButtons[i];
		}

		return hr;
	}

	void CINPUT::Process()
	{
		m_iMouseX += m_mouseState.lX;
		m_iMouseY += m_mouseState.lY;

		if (m_iMouseX < 0)
			m_iMouseX = 0;
		if (m_iMouseX > m_iWidth)
			m_iMouseX = m_iWidth;

		if (m_iMouseY < 0)
			m_iMouseY = 0;
		if (m_iMouseY > m_iHeight)
			m_iMouseY = m_iHeight;

		//SetCursorPos(m_iMouseX, m_iMouseY);
	}

	bool CINPUT::GetKeyPressed(UINT key)
	{
		return m_curKeyState[key] == KEYSTATE::PRESSED;
	}

	bool CINPUT::GetKeyPressedDown(UINT key)
	{
		return m_curKeyState[key] == KEYSTATE::PRESSEDDOWN;
	}

	bool CINPUT::GetKeyPressedUp(UINT key)
	{
		return m_curKeyState[key] == KEYSTATE::PRESSEDUP;
	}

	bool CINPUT::GetLButtonDown()
	{
		if (m_curMouseState[0] == KEYSTATE::PRESSED)
			return true;

		return false;
	}

	bool CINPUT::GetRButtonDown()
	{
		if (m_curMouseState[1] == KEYSTATE::PRESSEDDOWN)
			return true;

		return false;
	}

	void CINPUT::GetMousePosition(int & x, int & y)
	{
		RECT window;// , client;
		GetWindowRect(m_hWnd, &window);

		POINT p;
		GetCursorPos(&p);

		x = p.x - window.left;
		y = p.y - window.top;

		if (x < 0)
			x = 0;
		else if (x > m_iWidth)
			x = m_iWidth;

		if (y < 0)
			y = 0;
		else if (y > m_iHeight)
			y = m_iHeight;
	}

	void CINPUT::GetRect(RECT & rect)
	{
		//GetClientRect(m_hWnd, &rect);
		GetWindowRect(m_hWnd, &rect);
	}
}