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

cbuffer cbMaterial : register(b2)
{
	float4 color;
}

struct VertexIn
{
	float3 PosL  : POSITION;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
};

VertexOut VsMain(VertexIn vin)
{
	VertexOut vout;
	
	float4x4 mvp = mul(view_projection_matrix,model_matrix);
	vout.PosH = mul(mvp,float4(vin.PosL, 1.0f));
    return vout;
}

float4 PsMain(VertexOut pin) : SV_Target
{
    return color;
}
