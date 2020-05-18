#pragma once

#include <vector>
#include "vertices.h"

struct Geometries
{
	std::vector<POS_NRM_UV1> vertices;
	//std::vector<POS> vertices;
	//std::vector<INDEX32> indices;
	//POS * vertices;
	//DXGI_FORMAT IndexFormat;

	unsigned int vertexCnt;
	//unsigned int indexCnt;

	//unsigned int StartIndexLocation;
	//unsigned int BaseVertexLocation;

	Geometries()
	{
		vertices.clear();
		//indices.clear();

		vertexCnt = 0;
		//indexCnt = 0;
	}

	~Geometries()
	{
		//if (vertices != nullptr)
		//{
		//	delete[] vertices;
		//	vertices = nullptr;
		//}
	}
};
