#pragma once

#include "dx.h"
#include "BufferData.h"

#include <map>

class BufferCreator
{
private:
	BufferCreator() {}
	~BufferCreator() {}

public:
	static void Init(ID3D11Device* pDevice);
	static void Release();
	static HRESULT LoadBuffer(const TCHAR * tag, void * _pData, UINT size, UINT flag);
	static HRESULT LoadBufferWithMaterials(const TCHAR * tag, void * _pData, UINT flag, UINT count);
	static BufferData* LoadBuffer(const TCHAR* tag);
private:
	static HRESULT CreateBuffer(ID3D11Buffer** ppDesc, void * pData, UINT size, UINT flag);

private:
	static std::map<const TCHAR*, BufferData*> *m_Buffers;
	static ID3D11Device* m_pDevice;
	static bool m_bInit;
};