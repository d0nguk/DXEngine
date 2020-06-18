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


typedef struct SharedBuffer
{
	XMFLOAT4X4		mView;
	XMFLOAT4X4		mProj;
	XMFLOAT4X4		mVP;
	XMFLOAT4		vTime;
} cbGlobal;

typedef struct DirectionalLight
{
	XMFLOAT4 vDiffuse;
	XMFLOAT4 vAmbient;
	XMFLOAT4 vSpecular;
	XMFLOAT4 vDirection;
} cbDIRECTIONAL;

typedef struct MaterialBuffer
{
	XMFLOAT4X4 mWorld;

	XMFLOAT4 vDiffuse;
	XMFLOAT4 vAmbient;
	XMFLOAT4 vSpecular;
} cbMaterial;