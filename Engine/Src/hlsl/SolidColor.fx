struct cbMatrix
{
	float4x4 World;
	float4x4 View;
	float4x4 Proj;
	float4x4 MVP;
};

cbuffer Matrix : register(b0)
{
	cbMatrix _Matrix;
}

float4 VS_Main
(
	float4 pos : POSITION
) : SV_POSITION
{
	//pos.w = 1;
	//pos = mul(pos, _Matrix.MVP);

	return pos;
}

float4 PS_Main
(
	float4 pos : SV_POSITION
) : SV_Target
{
	//float4 col = float4(1,1,1,1);

	return 1.0f;
}