#pragma once

#include <vector>
#include "vertices.h"

struct Geometries
{
	std::vector<POS_NRM_UV1> vertices;
	std::vector<int> vertexCount;
	std::vector<int> startVertexLocation;
	std::vector<std::wstring> filename;
	unsigned int vertexCnt;
	bool next;

	//std::vector<POS> vertices;
	//std::vector<INDEX32> indices;
	//POS * vertices;
	//DXGI_FORMAT IndexFormat;

	//unsigned int indexCnt;

	//unsigned int StartIndexLocation;
	//unsigned int BaseVertexLocation;

	Geometries()
	{
		vertices.clear();
		//indices.clear();
		vertexCount.clear();
		startVertexLocation.clear();
		filename.clear();

		vertexCnt = 0;
		//indexCnt = 0;

		next = false;
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
