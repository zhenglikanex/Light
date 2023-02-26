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

struct VertexIn
{
	float3 PosL  : POSITION;
	float3 Normal : NORMAL;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float3 Color : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	float4x4 mvp = mul(gProjViewMatrix,gModelMatrix);
	vout.PosH = mul(mvp,float4(vin.PosL, 1.0f));
	vout.Color = vin.Normal;

    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return float4(1.0,1.0,1.0,1.0);
}
