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

struct VSOutput
{
	float4 pos : SV_POSITION;
	float4 viewDirection : VIEW;
	float4 col : NORMAL;
	float2 uv : TEXCOORD0;
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

VSOutput VS_Main
(
	float3 pos : POSITION,
	float3 nrm : NORMAL,
	float2 uv : TEXCOORD0
)
{
	float4 _pos = float4(pos, 1);
	_pos = mul(_pos, Mat.World);
	_pos = mul(_pos, Share.VP);

	float4 view = float4(pos, 0);
	view = mul(view, Mat.World);
	view = mul(view, Share.View);

	VSOutput o = (VSOutput)0;

	o.pos = _pos;
	o.viewDirection = view;
	o.col = float4(nrm, 0);
	o.uv = uv;

	return o;
}

SamplerState _sampler;
Texture2D Tex : register(t0);

float Directional(float4 N, float4 L)
{
	N = mul(N, Mat.World);
	N = normalize(N);
	L = normalize(-L);
	float4 NDotL = dot(N, L) * 0.5f + 0.5f;

	float4 Diffuse = Light.vDiffuse * NDotL * Mat.vDiffuse;
	float4 Ambient = Light.vAmbient * Mat.vAmbient;

	return Diffuse + Ambient;
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
	float4 L = Light.vDirection;
	float4 V = view; V.w = 0;
	float4 D = Directional(N, L);

	float4 diff = Tex.Sample(_sampler, uv);
	diff = pow(diff, 0.5f);
	diff = diff * D;
	diff.a = Mat.vDiffuse.a;

	return diff;
}