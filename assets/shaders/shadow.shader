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
	float3 PosW : POSITIONT;
	float3 Pos2 : Color0;
};

VertexOut VsMain(VertexIn vin)
{
	VertexOut vout;
	float4 worldPosition = mul(cbModelMatrix,float4(vin.Pos,1.0f));
	vout.PosW = worldPosition;
	vout.PosH = mul(cbViewProjectionMatrix,worldPosition);
	vout.Pos2 = vout.PosH;
    return vout;
}

float4 PsMain(VertexOut pin) : SV_Target
{
    float4 pos = mul(cbViewProjectionMatrix,float4(pin.PosW,1.0));
	pos.xyz /= pos.w;

	return float4(pos.z,0,0,0);
}