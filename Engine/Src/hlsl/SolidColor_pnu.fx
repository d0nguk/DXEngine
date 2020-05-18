struct cbMatrix
{
	float4x4 World;
	float4x4 View;
	float4x4 Proj;
	float4x4 MVP;
};

struct cbTime
{
	float4 Time;
};

struct VSOutput
{
	float4 pos : SV_POSITION;
	float4 col : NORMAL;
	float2 uv : TEXCOORD0;
};

cbuffer Matrix : register(b0)
{
	cbMatrix _Matrix;
}

cbuffer Time : register(b1)
{
	cbTime _Time;
}

VSOutput VS_Main
(
	float3 pos : POSITION,
	float3 nrm : NORMAL,
	float2 uv : TEXCOORD0
)
{
	float4 _pos = float4(pos, 1);
	_pos = mul(_pos, _Matrix.MVP);

	VSOutput o = (VSOutput)0;

	o.pos = _pos;
	o.col = float4(nrm, 1);
	o.uv = uv;

	return o;
}

float4 PS_Main
(
	float4 pos : SV_POSITION,
	float4 col : NORMAL,
	float2 uv : TEXCOORD0
) : SV_Target
{
	//float4 col = float4(1, 1, 1, 1);
	//return col;
	
	//return _Time.Time * 0.1f;
	
	return float4(uv,1,1);
}