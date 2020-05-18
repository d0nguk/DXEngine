#pragma once

#include "dx.h"

struct BufferData
{
	ID3D11Buffer* m_pVertices;
	ID3D11Buffer* m_pIndices;

	BufferData()
	{
		m_pVertices = nullptr;
		m_pIndices = nullptr;
	}
	~BufferData()
	{
		Release();
	}
	void Release()
	{
		if (m_pVertices != nullptr)
		{
			m_pVertices->Release();
			m_pVertices = nullptr;
		}
		if (m_pIndices != nullptr)
		{
			m_pIndices->Release();
			m_pIndices = nullptr;
		}
	}
};