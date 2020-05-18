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
	static BOOL LoadGeometry(const TCHAR* filename, const TCHAR* tag);

private:
	static Geometries* LoadGeometryFromFile(const TCHAR* filename);

public:
	static std::map<const TCHAR*, Geometries*> *m_Geos;
	static bool m_bInit;
};