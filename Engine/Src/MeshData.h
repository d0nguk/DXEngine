#pragma once

#include "dx.h"
#include "Material.h"

struct MeshData
{
	ID3D11Buffer* m_pVertices;
	ID3D11Buffer* m_pIndices;

	std::vector<int> vertexCount;
	std::vector<int> startVertexLocation;

	UINT m_iVertexCnt;
	UINT m_iIndexCnt;

	UINT m_iStartIndexLocation;
	UINT m_iBaseVertexLocation;

	UINT m_iStride;

	TEXTURE* m_pTexture;

	bool m_bNext;

	MeshData()
	{
		m_pVertices = nullptr;
		m_pIndices = nullptr;
		m_pTexture = nullptr;
		m_bNext = false;
	}
	~MeshData()
	{
		Release();
	}
	void Release()
	{
		m_pVertices = nullptr;
		m_pIndices = nullptr;
		m_pTexture = nullptr;
	}
};