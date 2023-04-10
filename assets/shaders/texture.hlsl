cbuffer cbSceneData : register(b0)
{
	float4x4 projection_matrix;
	float4x4 view_matrix;
	float4x4 view_projection_matrix;
};

cbuffer cbModelMatrix : register(b1)
{
	float4x4 model_matrix;
}

cbuffer cbColor : register(b2)
{
	float4 color;
	float tiling_factor;
}

Texture2D color_map : register(t0);
SamplerState sampler_point_warp : register(s0);

struct VertexIn
{
	float3 PosL  : POSITION;
	float2 UV : TEXCOORD0;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float2 UV : TEXCOORD0;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	float4x4 mvp = mul(view_projection_matrix,model_matrix);
	vout.PosH = mul(mvp,float4(vin.PosL, 1.0f));
	vout.UV = vin.UV;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return color_map.SampleLevel(sampler_point_warp,pin.UV * tiling_factor,0) * color;
}
