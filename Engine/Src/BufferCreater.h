#pragma once

#include "dx.h"

#include <map>

class BufferCreater
{
private:
	BufferCreater() {}
	~BufferCreater() {}

public:
	static void Init(ID3D11Device* pDevice);
	static void Release();
	static HRESULT LoadBuffer(const TCHAR * filename, ID3D11Buffer** ppDesc, void * pData, UINT size, UINT flag);
private:
	static HRESULT CreateBuffer(ID3D11Buffer** ppDesc, void * pData, UINT size, UINT flag);

private:
	static std::map<const TCHAR*, ID3D11Buffer*> *m_Buffer;
	static ID3D11Device* m_pDevice;
};