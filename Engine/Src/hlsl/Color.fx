struct cbTime
{
	float x, y, z, w;
};

struct cbShared
{
	float4x4 View;
	float4x4 Proj;
	float4x4 VP;
	cbTime Time;
};

struct cbDirectional
{
	float4 vDiffuse;
	float4 vAmbient;
	float4 vSpecular;
	float4 vDirection;
};

struct cbMaterial
{
	float4x4 World;
	float4 vDiffuse;
	float4 vAmbient;
	float4 vSpecular;	// w : power
};

cbuffer Matrix : register(b0)
{
	cbShared Share;
}

cbuffer Directional : register(b1)
{
	cbDirectional Light;
}

cbuffer Material : register(b2)
{
	cbMaterial Mat;
}

float4 VS_Main
(
	float4 pos : POSITION
) : SV_POSITION
{
	pos.w = 1;
	pos = mul(pos, Mat.World);
	pos = mul(pos, Share.VP);

	return pos;
}

float4 PS_Main
(
	float4 pos : SV_POSITION
) : SV_Target
{
	return 1;// Mat.vDiffuse;
}