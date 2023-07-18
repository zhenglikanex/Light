cbuffer cbMatrices : register(b0)
{
	float4x4 projection;
	float4x4 view;
};

Texture2D gEquirectangularMap : register(t0);
SamplerState gSamplerPointWarp : register(s0);

struct VertexIn
{
	float3 PosW  : POSITION;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float3 Pos : POSITION;
};

static const float2 invAtan = float2(0.1591, 0.3183);
float2 SampleSphericalMap(float3 v)
{
    float2 uv = float2(atan2(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

VertexOut VsMain(VertexIn vin)
{
	VertexOut vout;
	float4x4 view_projection_matrix = mul(projection,view);
	vout.PosH = mul(view_projection_matrix,float4(vin.PosW, 1.0f));
	vout.Pos = vin.PosW;
    return vout;
}

float4 PsMain(VertexOut pin) : SV_Target
{
	float2 uv = SampleSphericalMap(normalize(pin.Pos));
	float3 color = gEquirectangularMap.Sample(gSamplerPointWarp,uv).rgb;
    return float4(color,1.0f);
}
