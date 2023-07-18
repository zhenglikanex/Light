#include "utils.hlsl"

cbuffer cbMatrices : register(b0)
{
	float4x4 cbProjectionMatrix;
	float4x4 cbViewMatrix;
};

struct VertexIn
{
	float3 PosW  : POSITION;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float3 Pos : POSITION;
};

TextureCube gEnvironmentMap : register(t0);
SamplerState gSamplerPointWarp : register(s0);

VertexOut VsMain(VertexIn vin)
{
	VertexOut vout;
    float4x4 vp = mul(cbProjectionMatrix,cbViewMatrix);
	vout.PosH =  mul(vp,float4(vin.PosW,1.0));
    vout.PosH = vout.PosH.xyww;
    vout.Pos = normalize(vin.PosW);
    return vout;
}

float4 PsMain(VertexOut pin) : SV_Target
{
    float4 color = float4(gEnvironmentMap.Sample(gSamplerPointWarp,pin.Pos).rgb,1);
    return color;
}