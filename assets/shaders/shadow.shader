#include "utils.hlsl"

struct Light
{
    float3 Direction;
    float padding1;
    float3 Color;  
    float padding2;
};

cbuffer cbSceneData : register(b0)
{
	float4x4 cbProjectionMatrix;
	float4x4 cbViewMatrix;
	float4x4 cbViewProjectionMatrix;
    float3 cbCameraPosition;
    float padding1;
    Light light;
};

cbuffer cbPerDrawConstants : register(b1)
{
    float4x4 cbModelMatrix;
}

struct VertexIn
{
	float3 Pos  : POSITION;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
};

VertexOut VsMain(VertexIn vin)
{
	VertexOut vout;
	float4 worldPosition = mul(cbModelMatrix,float4(vin.Pos,1.0f));
	vout.PosH = mul(cbViewProjectionMatrix,worldPosition);
    return vout;
}

void PsMain(VertexOut pin)
{
    return;
}