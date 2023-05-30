#include "utils.hlsl"

cbuffer cbPerDrawConstants : register(b1)
{
    float4x4 cbModelMatrix;
}

cbuffer cbMaterialConstants : register(b0)
{
    float3 cbColor;
}

Texture2D gSourceMap : register(t0);
SamplerState gSamplerPointWrap : register(s0);

struct VertexIn
{
	float3 Pos  : POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

VertexOut VsMain(VertexIn vin)
{
	VertexOut vout;
	vout.PosH = mul(cbModelMatrix,float4(vin.Pos,1.0f));
    vout.TexCoord = vin.TexCoord;

    return vout;
}

float4 PsMain(VertexOut pin) : SV_Target
{
    float4 color = float4(gSourceMap.Sample(gSamplerPointWrap,GetTexcoord(pin.TexCoord)).rgb,1.0);
    return color;
}