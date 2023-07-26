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
    float3 color = gEnvironmentMap.SampleLevel(gSamplerPointWarp,pin.Pos,5).rgb;

    color = color / (color + float3(1.0,1.0,1.0));
    float gamma = 1.0/2.2;
    color = pow(color, float3(gamma, gamma, gamma));

    return float4(color,1);
}