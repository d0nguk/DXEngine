#pragma once

#include <DirectXMath.h>

using namespace DirectX;

enum MATRIX
{
	WORLD,
	VIEW,
	PROJ,
	MVP,

	M_MAX
};

struct cbMatrix
{
	XMMATRIX Matrix[M_MAX];

	cbMatrix()
	{
		Matrix[MATRIX::WORLD] = XMMatrixIdentity();
		Matrix[MATRIX::VIEW] = XMMatrixIdentity();
		Matrix[MATRIX::PROJ] = XMMatrixIdentity();
		Matrix[MATRIX::MVP] = XMMatrixIdentity();
	}
};