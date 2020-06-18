#pragma once

#include "dx.h"
#include "MeshData.h"
#include "Geometries.h"
#include "BufferData.h"

#include <map>

class MeshCreator
{
private:
	MeshCreator() {}
	~MeshCreator() {}

public:
	static void Init(ID3D11Device* pDevice);
	static void Release();
	static BOOL CreateMesh(const TCHAR* tag, Geometries * pGeo, BufferData * pBuff, UINT count = 1);
	static MeshData* LoadMesh(const TCHAR* tag);

private:
	static std::map<const TCHAR*, MeshData*> *m_Meshes;
	static ID3D11Device* m_pDevice;
	static bool m_bInit;
};