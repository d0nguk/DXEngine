#pragma once

#ifdef FONT_EXPORT
#define FONT_EXPORT __declspec(dllexport)
#else
#define FONT_EXPORT __declspec(dllimport)
#endif

#include <d3d11.h>

#include <DirectXMath.h>
using namespace DirectX;

#include <SpriteBatch.h>
#include <SpriteFont.h>

#ifdef _DEBUG 
#pragma comment(lib, "DirectXTKd")
//#pragma comment(lib, "DirectXTK64d")
#else
#pragma comment(lib, "DirectXTK")
//#pragma comment(lib, "DiectXTK64")
#endif

namespace ENGINEDLL
{
	class FONT_EXPORT CFONT
	{
	private:
		CFONT() {}
		~CFONT() {}

	public:
		static BOOL Init(ID3D11Device* pDevice, const TCHAR* filename);
		static void Release();
		static void Begin();
		static void End();
		static void PrintInfo(int x, int y, XMFLOAT4 col, const TCHAR* msg, ...);

	private:
		static ID3D11Device*	m_pDevice;
		static SpriteBatch*		m_pBatch;
		static SpriteFont*		m_pFont;
		static bool				m_bInit;
	};
}

// copy /y "$(OutDir)Font.lib" "..\..\Test\Extern\Custom\Lib\"