#pragma once

#include "dx.h"
#include "Geometries.h"

#include <map>
#include <vector>

class GeoLoader
{
private:
	GeoLoader() {}
	~GeoLoader() {}

public:
	static void Init();
	static void Release();
	static BOOL LoadGeometry(const TCHAR* filename, const TCHAR* tag, bool bUseMaterials = false);
	static Geometries* LoadGeometry(const TCHAR* tag)
	{
		return m_GeosByTag->operator[](tag);
	}
private:
	static Geometries* LoadGeometryFromFile(const TCHAR* filename);
	static Geometries* LoadGeometryAndMaterialsFromFile(const TCHAR* filename, const TCHAR * tag);

public:
	static std::map<const TCHAR*, Geometries*> *m_Geos;
	static std::map<const TCHAR*, Geometries*> *m_GeosByTag;
	static bool m_bInit;
};