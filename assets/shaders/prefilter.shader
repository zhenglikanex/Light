cbuffer cbInput : register(b0)
{
	float4x4 projection;
	float4x4 view;
    float roughness;
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

float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

float2 Hammersley(uint i,uint N)
{
    return float2(float(i)/float(N),RadicalInverse_VdC(i));
}

float3 ImportanceSampleGGX(float2 Xi,float3 N,float roughness)
{
    float a = roughness * roughness;

    // 构建球坐标
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1 - cosTheta * cosTheta);

    // 从球坐标构建笛卡尔坐标(切线空间)
    float3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // 从切线空间转换到世界空间
    float3 up        = abs(N.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    float3 tangent   = normalize(cross(up, N));
    float3 bitangent = cross(N, tangent);

    float3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

float4 PsMain(VertexOut pin) : SV_Target
{
	float3 N = normalize(pin.Pos);
    float3 R = N;
    float3 V = R;

    uint samplerCount = 1024;
    float totalWeight = 0.0;
    float3 prefilteredColor = float3(0,0,0);
    for(uint i = 0;i<samplerCount;++i)
    {
        float2 Xi = Hammersley(i, samplerCount);
        float3 H  = ImportanceSampleGGX(Xi, N, roughness);
        float3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if(NdotL > 0.0)
        {
            prefilteredColor += gEnvironmentMap.Sample(gSamplerLinearWarp, L).rgb * NdotL;
            totalWeight      += NdotL;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;
    return float4(prefilteredColor,1.0);
}
