#include "lighting.hlsl"

struct Light
{
    float3 Direction;
    float padding1;
    float3 Color;  
    float padding2;
    float4x4 ViewProjectionMatrix;
};

#define kMaxLight 32

cbuffer cbSceneData : register(b0)
{
	float4x4 cbProjectionMatrix;
	float4x4 cbViewMatrix;
	float4x4 cbViewProjectionMatrix;
    float3 cbCameraPosition;
    int cbNumLight;
    Light cbLights[kMaxLight];
};

cbuffer cbPerDrawConstants : register(b1)
{
    float4x4 cbModelMatrix;
}

cbuffer cbMaterialConstants : register(b2)
{
    float3 cbAlbedoColor;            // 基础颜色
    float cbMetalness;          // 金属度 (0-1)
    float cbRoughness;          // 粗糙度(0-1)
}

Texture2D gShadowMap : register(t0);
SamplerState gSamplerPointWarp : register(s0);

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
    float4 pos = mul(cbModelMatrix,float4(vin.Position, 1.0f));
    vout.PosH = mul(cbViewProjectionMatrix,pos);
    vout.WorldPosition = pos;
    vout.Normal = vin.Normal;
    vout.TexCoord = vin.TexCoord;
    //vout.WorldNormalMatrix = mul((float3x3)cbModelMatrix,float3x3(vin.Tangent,vin.Binormal,vin.Normal));
    vout.WorldNormalMatrix = (float3x3)cbModelMatrix;

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

float IsShadow(Light light,float3 position,float cosTheta)
{   
    float4 posLight = mul(light.ViewProjectionMatrix,float4(position,1.0));
    float2 shadowUV = posLight.xy * 0.5 + 0.5;
    shadowUV.y = 1-shadowUV.y;
    float shadow = gShadowMap.Sample(gSamplerPointWarp,shadowUV).r;

    float bias = 5e-4 * tan(acos(cosTheta)); // cosTheta is dot( n,l ), clamped between 0 and 1
    bias = clamp(bias, 0.0f, 0.01f);
    return posLight.z > shadow + 0.0005 ? 0.5 : 1;
}

float3 Lighting(Light light,float3 worldPosition, float3 F0)
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

    float3 ks = F;
    float3 kd = float3(1.0,1.0,1.0) - ks;
    kd *= 1.0 - gParams.Metalness;
    
    result = kd * gParams.Albedo / kPI * Lradinace + specularBRDF * Lradinace * cosLi;
     
    result *= IsShadow(light,worldPosition,cosLi);

    return result;
}

float4 PsMain(VertexOut vsInput) : SV_Target
{
    gParams.Albedo = cbAlbedoColor;
    gParams.Metalness = cbMetalness;
    gParams.Roughness = cbRoughness;

    gParams.Normal = normalize(mul(vsInput.WorldNormalMatrix,vsInput.Normal));

    gParams.View = normalize(cbCameraPosition - vsInput.WorldPosition);
    gParams.NdotV = max(dot(gParams.Normal,gParams.View),0.0f);

    float3 F0 = lerp(kFdielectric,gParams.Albedo,gParams.Metalness);

    float3 color = float3(0,0,0);
    for(int i = 0;i<cbNumLight;++i)
    {
        color += Lighting(cbLights[i],vsInput.WorldPosition, F0);
    }

    color = color / (color + float3(1.0,1.0,1.0));
    color = pow(color, 1.0/2.2);  

    return float4(color,1.0f);
}