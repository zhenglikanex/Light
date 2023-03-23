cbuffer cbSceneData : register(b0)
{
	float4x4 projection_matrix;
	float4x4 view_matrix;
	float4x4 view_projection_matrix;
};

struct VertexIn
{
	float3 PosL  : POSITION;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	vout.PosH = mul(view_projection_matrix,float4(vin.PosL, 1.0f));
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return float4(1.0,1.0,0.0,1.0);
}
