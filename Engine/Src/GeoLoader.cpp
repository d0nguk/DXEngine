#include "GeoLoader.h"

#include "BufferCreator.h"
#include "MeshCreator.h"
#include "TextureLoader.h"

std::map<const TCHAR*, Geometries*> *GeoLoader::m_Geos = nullptr;
std::map<const TCHAR*, Geometries*> *GeoLoader::m_GeosByTag = nullptr;
bool GeoLoader::m_bInit = false;

void GeoLoader::Init()
{
	if (m_bInit)
		return;

	m_Geos = new std::map<const TCHAR*, Geometries*>();
	m_Geos->clear();

	m_GeosByTag = new std::map<const TCHAR*, Geometries*>();
	m_GeosByTag->clear();

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
					delete iter->second;
					iter->second = nullptr;
				}
			}
		}

		m_Geos->clear();
		delete m_Geos;
		m_Geos = nullptr;
	}
	if (m_GeosByTag != nullptr)
	{
		if (m_GeosByTag->size() > 0)
		{
			auto iter = m_GeosByTag->begin();

			for (; iter != m_GeosByTag->end(); ++iter)
			{
				if (iter->second != nullptr)
				{
					iter->second = nullptr;
				}
			}
		}

		m_GeosByTag->clear();
		delete m_GeosByTag;
		m_GeosByTag = nullptr;
	}

	m_bInit = false;
}

BOOL GeoLoader::LoadGeometry(const TCHAR * filename, const TCHAR* tag, bool bUseMaterials)
{
	if (!m_bInit)
		return FALSE;

	Geometries* pGeo = nullptr;

	if (m_Geos->size() > 0)
		pGeo = m_Geos->operator[](filename);

	if (pGeo == nullptr)
	{
		if (!bUseMaterials)
		{
			pGeo = LoadGeometryFromFile(filename);

			if (pGeo == nullptr)
				return FALSE;

			m_Geos->operator[](filename) = pGeo;
			m_GeosByTag->operator[](tag) = pGeo;

			HRESULT hr = S_OK;
			BOOL res = TRUE;

			hr = BufferCreator::LoadBuffer
			(
				tag,
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
		else
		{
			pGeo = LoadGeometryAndMaterialsFromFile(filename, tag);

			if (pGeo == nullptr)
				return FALSE;

			m_Geos->operator[](filename) = pGeo;
			m_GeosByTag->operator[](tag) = pGeo;

			HRESULT hr = S_OK;
			BOOL res = TRUE;

			hr = BufferCreator::LoadBuffer
			(
				tag,
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

	pData->vertexCount.push_back(pData->vertexCnt);
	pData->startVertexLocation.push_back(0);

	fclose(fp);

	return pData;
}

Geometries * GeoLoader::LoadGeometryAndMaterialsFromFile(const TCHAR * filename, const TCHAR * tag)
{
	FILE *fp = nullptr;
	if (_wfopen_s(&fp, filename, L"r") != 0)
		return nullptr;

	char buff[1024] = { '\0' };
	UINT MaterialCount = 0;
	//fscanf_s(fp, "%u", &pData->vertexCnt);
	fscanf_s(fp, "%s %s %u",
		buff,
		1024,
		buff,
		1024,
		&MaterialCount
	);

	Geometries* pData = new Geometries;// [MaterialCount];

	int vertexCnt = 0;
	fscanf_s(fp, "%s %s %u",
		buff,
		1024,
		buff,
		1024,
		&vertexCnt
	);

	int vertexCount = 0;
	int startVertexLocation = 0;

	for (unsigned int i = 0; i < MaterialCount; ++i)
	{
		std::wstring file = L"..\\Data\\Textures\\";
		TCHAR name[1024] = { L'\0' };
		TCHAR wbuff[1024] = { L'\0' };
		//fscanf_s(fp, L"%s %s", wbuff, 1024, name, 1024);
		fwscanf_s(fp, L"%s %s", wbuff, 1024, name, 1024);

		file += tag;
		file += L"\\";
		file += name;
		
		pData->filename.push_back(file);

		pData->startVertexLocation.push_back(startVertexLocation);

		while (!feof(fp))
		{
			char cinput = '\0';

			fscanf_s(fp, "%c", &cinput, 1);
			if (cinput == 't')
			{
				pData[i].vertexCnt = pData[i].vertices.size();
				pData->vertexCount.push_back(vertexCount);
				startVertexLocation += vertexCount;
				vertexCount = 0;
				break;
			}
			else if (cinput == 'v')
			{
				float x, y, z, nx, ny, nz, u1, v1;

				fscanf_s
				(
					fp, "%c %f %f %f %f %f %f %f %f",
					&cinput, 1, &x, &y, &z, &nx, &ny, &nz, &u1, &v1
				);

				pData->vertices.push_back(POS_NRM_UV1(x, y, z, nx, ny, nz, u1, v1));
				++vertexCount;
			}
			else if (cinput == 'x')
			{
				pData->vertexCount.push_back(vertexCount);
				pData->vertexCnt = vertexCnt;
				break;
			}
		}
	}

	return pData;
}
