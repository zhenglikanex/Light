cbuffer cbInput : register(b0)
{
	float4x4 projection;
	float4x4 view;
};

#define PI 3.1415926

TextureCube gEnvironmentMap : register(t0);
SamplerState gSamplerLinearWarp : register(s0);

struct VertexIn
{
	float3 PosW  : POSITION;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float3 Pos : POSITION;
};

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
	// The world vector acts as the normal of a tangent surface
    // from the origin, aligned to WorldPos. Given this normal, calculate all
    // incoming radiance of the environment. The result of this radiance
    // is the radiance of light coming from -Normal direction, which is what
    // we use in the PBR shader to sample irradiance.
    float3 N = normalize(pin.Pos);

    float3 irradiance = float3(0.0,0.0,0.0);   
    
    // tangent space calculation from origin point
    float3 up    = float3(0.0, 1.0, 0.0);
    float3 right = normalize(cross(up, N));
    up         = normalize(cross(N, right));
       
    float sampleDelta = 0.025;
    float nrSamples = 0.0;
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            float3 tangentSample = float3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; 

            irradiance += gEnvironmentMap.Sample(gSamplerLinearWarp,sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));
    
    return float4(irradiance,1.0);
}
