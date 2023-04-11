cbuffer cbSceneData : register(b0)
{
	float4x4 projection_matrix;
	float4x4 view_matrix;
	float4x4 view_projection_matrix;
};

Texture2D color_map[] : register(t0);
SamplerState sampler_point_warp : register(s0);

struct VertexIn
{
	float3 PosW  : POSITION;
	float2 UV : TEXCOORD0;
	float4 Color : COLOR;
	nointerpolation float TexIndex : COLOR1;
	float TilingFactor : COLOR2;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float2 UV : TEXCOORD0;
	float4 Color : COLOR;
	nointerpolation float TexIndex : COLOR1;
	float TilingFactor : COLOR2;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	vout.PosH = mul(view_projection_matrix,float4(vin.PosW, 1.0f));
	vout.UV = vin.UV;
	vout.Color = vin.Color;
	vout.TexIndex = vin.TexIndex;
	vout.TilingFactor = vin.TilingFactor;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return color_map[int(pin.TexIndex)].Sample(sampler_point_warp,pin.UV * pin.TilingFactor) * pin.Color;
}
