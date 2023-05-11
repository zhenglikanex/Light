cbuffer cbPerFrameConstants : register(b0)
{
	float4x4 gProjMatrix;
	float4x4 gViewMatrix;
	float4x4 gProjViewMatrix;
};

cbuffer cbPerDrawConstants : register(b1)
{
	float4x4 gModelMatrix;
};

cbuffer cbConstansView : register(b2)
{
	float4 gColor2;
};

Texture2D gDiffuseMap : register(t0);

SamplerState gSamplerLinearWrap : register(s0);

struct VertexIn
{
	float3 PosL  : POSITION;
	float3 Normal : NORMAL;
	float2 UV : TEXCOORD;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float3 Color : COLOR;
	float2 UV : TEXCOORD;
};

VertexOut VsMain(VertexIn vin)
{
	VertexOut vout;
	
	float4x4 mvp = mul(gProjViewMatrix,gModelMatrix);
	vout.PosH = mul(mvp,float4(vin.PosL, 1.0f));
	vout.Color = vin.Normal;
	vout.UV = vin.UV;
    
    return vout;
}

float4 PsMain(VertexOut pin) : SV_Target
{
	float4 color = gDiffuseMap.SampleLevel(gSamplerLinearWrap,pin.UV,0);
    return color;
}
