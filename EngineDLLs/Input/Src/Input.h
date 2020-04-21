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
	class CINPUT
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

	public:
		static INPUT_EXPORT HRESULT Init(HINSTANCE hInst, HWND hWnd, int width, int height);
		static INPUT_EXPORT void Update(float dTime);
		static INPUT_EXPORT void Release();

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

	private:
		static IDirectInput8*		m_pInput;
		static IDirectInputDevice8*	m_pKeyboard;
		static IDirectInputDevice8*	m_pMouse;

		static unsigned char		m_prevKeyState[256];	// Prev Frame Key State
		static unsigned char		m_keyState[256];		// Current Frame Key State
		static KEYSTATE				m_curKeyState[256];		// For Check KeyState
		static DIMOUSESTATE			m_mouseState;

		static int					m_iWidth;
		static int					m_iHeight;
		static int					m_iMouseX;
		static int					m_iMouseY;

		static bool					m_bInit;
	};
}