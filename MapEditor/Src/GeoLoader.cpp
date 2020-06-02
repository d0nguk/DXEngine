#include "GeoLoader.h"

#include "BufferCreator.h"
#include "MeshCreator.h"

std::map<const TCHAR*, Geometries*> *GeoLoader::m_Geos = nullptr;
bool GeoLoader::m_bInit = false;

void GeoLoader::Init()
{
	if (m_bInit)
		return;

	m_Geos = new std::map<const TCHAR*, Geometries*>();
	m_Geos->clear();

	m_bInit = true;
}

void GeoLoader::Release()
{
	if (m_Geos != nullptr)
	{
		if (m_Geos->size() > 0)
		{
			auto iter = m_Geos->begin();

			for (; iter != m_Geos->end(); ++iter)
			{
				if (iter->second != nullptr)
				{
					//iter->second->Release();
					delete iter->second;
					iter->second = nullptr;
				}
			}
		}

		m_Geos->clear();
		delete m_Geos;
		m_Geos = nullptr;
	}

	m_bInit = false;
}

BOOL GeoLoader::LoadGeometry(const TCHAR * filename, const TCHAR* tag)
{
	if (!m_bInit)
		return FALSE;

	Geometries* pGeo = nullptr;

	if (m_Geos->size() > 0)
		pGeo = m_Geos->operator[](filename);

	if (pGeo == nullptr)
	{
		pGeo = LoadGeometryFromFile(filename);

		if (pGeo == nullptr)
			return FALSE;

		m_Geos->operator[](filename) = pGeo;

		HRESULT hr = S_OK;
		BOOL res = TRUE;

		ID3D11Buffer* pBuff = nullptr;
		hr = BufferCreator::LoadBuffer
		(
			tag, &pBuff,
			&pGeo->vertices[0],
			pGeo->vertexCnt * sizeof(pGeo->vertices[0]),
			D3D11_BIND_VERTEX_BUFFER
		);
		if (FAILED(hr))
			return FALSE;

		BufferData *pBuffData = BufferCreator::LoadBuffer(tag);
		res = MeshCreator::CreateMesh(tag, pGeo, pBuffData);
		if (!res)
			return FALSE;
	}

	return TRUE;
}

Geometries * GeoLoader::LoadGeometryFromFile(const TCHAR * filename)
{
	FILE *fp = nullptr;
	if (_wfopen_s(&fp, filename, L"r") != 0)
		return nullptr;

	Geometries* pData = new Geometries();
	fscanf_s(fp, "%u", &pData->vertexCnt);

	//pData->vertices = new POS[pData->vertexCnt];
	for (unsigned int i = 0; i < pData->vertexCnt; ++i)
	{
		float x, y, z, nx, ny, nz, u1, v1;

		fscanf_s
		(
			fp, "%f %f %f %f %f %f %f %f",
			&x, &y, &z, &nx, &ny, &nz, &u1, &v1
		);

		pData->vertices.push_back(POS_NRM_UV1(x, y, z, nx, ny, nz, u1, v1));
		//pData->vertices.push_back(POS(x, y, z));
		//pData->vertices[i] = POS(x, y, z);
	}

	fclose(fp);

	return pData;
}
