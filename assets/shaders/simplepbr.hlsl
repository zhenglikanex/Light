struct Light
{
    float3 Direction;
    float padding1;
    float3 Color;  
    float padding2;
};

cbuffer cbSceneData : register(b0)
{
	float4x4 cbProjectionMatrix;
	float4x4 cbViewMatrix;
	float4x4 cbViewProjectionMatrix;
    float3 cbCameraPosition;
    float padding1;
    Light light;
};

cbuffer cbPerDrawConstants : register(b1)
{
    float4x4 cbModelMatrix;
}

cbuffer cbMaterialConstants : register(b2)
{
    float3 cbAlbedo;        // 基础颜色
    float cbMetalness;          // 金属度 (0-1)
    float cbRoughness;          // 粗糙度(0-1)
}

Texture2D color_map2 : register(t1);
Texture2D color_map : register(t0);
SamplerState sampler_point_warp : register(s0);

static const float kPI = 3.1415926;
static const float kEpsilon = 0.00001;

static const float3 kFdielectric = 0.04;

struct PbrParams
{
    float3 Albedo;
    float Metalness;
    float Roughness;

    float3 Normal;
    float3 View;
    float  NdotV;
};

static PbrParams gParams;

struct VertexIn
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL; 
    float2 TexCoord : TEXCOORD0;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 WorldPosition : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
    float3x3 WorldNormalMatrix : COLOR0;
};

VertexOut VsMain(VertexIn vin)
{
    VertexOut vout;
    vout.PosH = mul(cbViewProjectionMatrix, float4(vin.Position, 1.0f));
    vout.WorldPosition = (float3)mul(cbViewMatrix,float4(vin.Position,1.0f));
    vout.Normal = vin.Normal;
    vout.TexCoord = vin.TexCoord;
    vout.WorldNormalMatrix = mul((float3x3)cbModelMatrix,float3x3(vin.Tangent,vin.Binormal,vin.Normal));

    return vout;
}

// Shlick's approximation of the Fresnel factor.
float3 fresnelSchlick(float3 F0, float cosTheta)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2
float ndfGGX(float cosLh, float roughness)
{
	float alpha = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (kPI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float NdotV, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(NdotV, k);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}


float3 Lighting(float3 F0)
{
    float3 result = 0.0;

    float3 Li = -light.Direction;
    float3 Lradinace = light.Color;
    float3 Lh = normalize(Li + gParams.View);

    float cosLi = max(0,dot(gParams.Normal,Li));
    float cosLh = max(0,dot(gParams.Normal,Lh));

    float3 F = fresnelSchlick(F0,max(0.0,dot(Lh,gParams.View)));
    float D = ndfGGX(cosLh,gParams.Roughness);
    float G = GeometrySmith(gParams.Normal,gParams.View,Li,gParams.Roughness);

    // Cook-TorranceBRDF
    float3 specularBRDF = F * D * G / max(kEpsilon,4.0 * cosLi * gParams.NdotV);

    result = specularBRDF * Lradinace * cosLi;

    return result;
}

float4 PsMain(VertexOut vsInput) : SV_Target
{
    gParams.Albedo = cbAlbedo;
    gParams.Metalness = cbMetalness;
    gParams.Roughness = cbRoughness;

    gParams.Normal = normalize(vsInput.Normal);

    gParams.View = normalize(cbCameraPosition - vsInput.WorldPosition);
    gParams.NdotV = max(dot(gParams.Normal,gParams.View),0.0f);

    float3 F0 = lerp(kFdielectric,gParams.Albedo,gParams.Metalness);

    float3 color = Lighting(F0);
    return float4(color,1.0f);
}