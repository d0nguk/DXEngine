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
	class CFONT
	{
	private:
		CFONT() {}
		~CFONT() {}

	public:
		static FONT_EXPORT BOOL Init(ID3D11Device* pDevice, const wchar_t* filename);
		static FONT_EXPORT void Release();
		static FONT_EXPORT void Begin();
		static FONT_EXPORT void End();
		static FONT_EXPORT void PrintInfo(int x, int y, XMFLOAT4 col, const wchar_t* msg, ...);

	private:
		static ID3D11Device*	m_pDevice;
		static SpriteBatch*		m_pBatch;
		static SpriteFont*		m_pFont;
		static bool				m_bInit;
	};
}