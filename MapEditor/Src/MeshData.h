#pragma once

#include "dx.h"

struct MeshData
{
	ID3D11Buffer* m_pVertices;
	ID3D11Buffer* m_pIndices;

	UINT m_iVertexCnt;
	UINT m_iIndexCnt;

	UINT m_iStartIndexLocation;
	UINT m_iBaseVertexLocation;

	UINT m_iStride;

	MeshData()
	{
		m_pVertices = nullptr;
		m_pIndices = nullptr;
	}
	~MeshData()
	{
		Release();
	}
	void Release()
	{
		m_pVertices = nullptr;
		m_pIndices = nullptr;
	}
};