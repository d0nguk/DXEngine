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
	float4 viewDirection : VIEW;
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

	float4 view = float4(pos, 0);
	view = mul(view, _Matrix.World);
	view = mul(view, _Matrix.View);

	VSOutput o = (VSOutput)0;

	o.pos = _pos;
	o.viewDirection = view;
	o.col = float4(nrm, 0);
	o.uv = uv;

	return o;
}

SamplerState _sampler;
Texture2D Tex : register(t0);

float4 Directional(float4 N, float4 L)
{
	N = mul(N, _Matrix.World);
	N = normalize(N);
	L = normalize(-L);

	float4 _col = max(0, dot(N, L));// *0.5f + 0.5f;
	_col.a = 1;

	return _col;
}

float4 Specular(float4 N, float4 V, float4 L)
{


	return 1;
}

float4 PS_Main
(
	float4 pos : SV_POSITION,
	float4 view : VIEW,
	float4 col : NORMAL,
	float2 uv : TEXCOORD0
) : SV_Target
{	
	float4 N = col;
	float4 L = float4(-1, -1, 1, 0);
	float4 V = view; V.w = 0;
	float4 D = Directional(N, L);
	float4 S = Specular(N, view, L);

	float4 diff = Tex.Sample(_sampler, uv);//float2(uv.x * 3.0f + _Time.Time.z, uv.y * 3.0f));

	return diff;
}