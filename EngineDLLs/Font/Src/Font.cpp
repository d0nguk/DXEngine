#include "Font.h"
#include <stdio.h>
#include <tchar.h>
#include <stdarg.h>
#include <SimpleMath.h>

namespace ENGINEDLL
{
	ID3D11Device* CFONT::m_pDevice = nullptr;
	SpriteBatch* CFONT::m_pBatch = nullptr;
	SpriteFont* CFONT::m_pFont = nullptr;
	bool CFONT::m_bInit = false;

	BOOL CFONT::Init(ID3D11Device * pDevice, const wchar_t* filename)
	{
		BOOL res = TRUE;

		if (!m_bInit)
		{
			m_pDevice = pDevice;
			ID3D11DeviceContext* pDXDC = nullptr;
			m_pDevice->GetImmediateContext(&pDXDC);

			m_pBatch = new SpriteBatch(pDXDC);

			try
			{
				m_pFont = new SpriteFont(m_pDevice, filename);
				m_pFont->SetLineSpacing(14.0f);
				m_pFont->SetDefaultCharacter('_');
			}
			catch (std::exception& e)
			{
				e.what();
				res = FALSE;
			}

			pDXDC->Release();
			pDXDC = nullptr;

			m_bInit = res;
		}

		return res;
	}

	void CFONT::Release()
	{
		if (m_pFont != nullptr)
		{
			delete m_pFont;
			m_pFont = nullptr;
		}

		if (m_pBatch != nullptr)
		{
			delete m_pBatch;
			m_pBatch = nullptr;
		}

		m_bInit = false;
	}

	void CFONT::Begin()
	{
		if (m_bInit)
		{
			UINT mask = 0xFFFFFFFF;

			ID3D11BlendState* pBlend = nullptr;
			ID3D11SamplerState* pSampler = nullptr;
			ID3D11DepthStencilState* pDepth = nullptr;
			ID3D11RasterizerState* pRaster = nullptr;

			ID3D11DeviceContext* pDXDC = nullptr;
			m_pDevice->GetImmediateContext(&pDXDC);

			pDXDC->OMGetBlendState(&pBlend, NULL, &mask);
			pDXDC->PSGetSamplers(0, 1, &pSampler);
			pDXDC->OMGetDepthStencilState(&pDepth, 0);
			pDXDC->RSGetState(&pRaster);

			m_pBatch->Begin
			(
				DirectX::SpriteSortMode::SpriteSortMode_Deferred,
				pBlend, pSampler, pDepth, pRaster
			);
		}
	}

	void CFONT::End()
	{
		if(m_bInit)
			m_pBatch->End();
	}

	void CFONT::PrintInfo(int x, int y, XMFLOAT4 col, const wchar_t * msg, ...)
	{
		if (m_bInit)
		{
			wchar_t buff[2048] = L"";
			va_list vl;
			int len = 0;

			va_start(vl, msg);
			len = _vscwprintf(msg, vl) + 1;
			//vswprintf_s(buff, msg, vl);
			vswprintf_s(buff, len, msg, vl);
			va_end(vl);

			m_pFont->DrawString
			(
				m_pBatch,
				buff,
				SimpleMath::Vector2((float)x, (float)y),
				SimpleMath::Color(col)
			);
		}
	}
}
