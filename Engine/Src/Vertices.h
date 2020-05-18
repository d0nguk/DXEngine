#pragma once

struct POS
{
	float x, y, z;

	POS() :
		x(0.0f),
		y(0.0f),
		z(0.0f)
	{

	}
	POS(float _x, float _y, float _z) :
		x(_x),
		y(_y),
		z(_z)
	{

	}
};

struct POS_NRM_UV1
{
	float x, y, z;
	float nx, ny, nz;
	float u1, v1;

	POS_NRM_UV1() :
		x(0.0f),
		y(0.0f),
		z(0.0f),
		nx(0.0f),
		ny(0.0f),
		nz(0.0f),
		u1(0.0f),
		v1(0.0f)
	{

	}
	POS_NRM_UV1
	(
		float _x, float _y, float _z,
		float _nx, float _ny, float _nz,
		float _u1, float _v1
	) :
		 x(_x),
		 y(_y),
		 z(_z),
		nx(_nx),
		ny(_ny),
		nz(_nz),
		u1(_u1),
		v1(_v1)
	{

	}
};

struct INDEX16
{
	unsigned int v1, v2, v3;
};

struct INDEX32
{
	unsigned short v1, v2, v3;
};