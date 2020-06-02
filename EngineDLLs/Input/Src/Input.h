#pragma once

#ifdef INPUT_EXPORT
#define INPUT_EXPORT __declspec(dllexport)
#else
#define INPUT_EXPORT __declspec(dllimport)
#endif

#include <d3d11.h>
#include <dinput.h>

#pragma comment(lib, "dinput8")
#pragma comment(lib, "dxguid")

namespace ENGINEDLL
{
	class INPUT_EXPORT CINPUT
	{
	public:
		enum KEYSTATE
		{
			RELEASED,
			PRESSED,
			PRESSEDDOWN,
			PRESSEDUP,

			KEYSTATE_MAX
		};

	private:
		CINPUT() {}
		~CINPUT() {}

	public:
		static HRESULT Init(HINSTANCE hInst, HWND hWnd, int width, int height);
		static void Update(float dTime);
		static void Release();

		static HRESULT InitKeyboard(HWND hWnd);
		static HRESULT InitMouse(HWND hWnd);

	private:
		static HRESULT Read();
		static HRESULT ReadKeyboard();
		static HRESULT ReadMouse();
		static void Process();

	public:
		static bool GetKeyPressed(UINT key);
		static bool GetKeyPressedDown(UINT key);
		static bool GetKeyPressedUp(UINT key);

		static bool GetLButtonDown();
		static bool GetRButtonDown();
		static void GetMousePosition(int & x, int & y);
		static void GetRect(RECT & rect);

	private:
		static IDirectInput8*		m_pInput;
		static IDirectInputDevice8*	m_pKeyboard;
		static IDirectInputDevice8*	m_pMouse;

		static HWND					m_hWnd;

		static unsigned char		m_prevKeyState[256];	// Prev Frame Key State
		static unsigned char		m_keyState[256];		// Current Frame Key State
		static KEYSTATE				m_curKeyState[256];		// For Check KeyState
		
		static unsigned char		m_prevMouseState[4];	// Prev Frame Key State
		static KEYSTATE				m_curMouseState[4];	// For Check KeyState
		static DIMOUSESTATE			m_mouseState;

		static int					m_iWidth;
		static int					m_iHeight;
		static int					m_iMouseX;
		static int					m_iMouseY;

		static bool					m_bInit;
	};
}