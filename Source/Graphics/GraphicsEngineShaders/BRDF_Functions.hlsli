#include "Buffers/LightBuffer.hlsli"
#include "PSFunctions.hlsli"
TextureCube EnvCubeTexture : register(t120);
Texture2D LUT : register(t127);
SamplerState LUTSampler : register(s14);
SamplerState defaultSampler : register(s0);

SamplerComparisonState shadowCmpSampler : register(s15);
Texture2D shadowMap : register(t100);

float3 Diffuse_BRDF(float3 anAlbedo)
{
    return anAlbedo / PI;
}

float NormalDistributionFunction_GGX(float aRoughness, float aNdotH)
{
    float a2 = pow(aRoughness, 4.f);
    return a2 / (PI * pow(1 + pow(aNdotH, 2.f) * (a2 - 1.f), 2.f));
}

float GeometricAttenuation_Smith_GGX(float aNdotX, float aK)
{
    return aNdotX / ((aNdotX * (1 - aK)) + aK);
}

float GeometricAttenuation_Schlick(float aNdotV, float aNdotL, float aRoughness)
{
    float k = pow(aRoughness + 1.f, 2) / 8.f;

    float attenuation = GeometricAttenuation_Smith_GGX(aNdotL, k) * GeometricAttenuation_Smith_GGX(aNdotV, k);
    
    return (attenuation);
}

float3 Fresnel_SphericalGaussianSchlick(float3 aToView, float3 aHalfAngle, float3 aSpecularColor)
{
    float VdotH = saturate(dot(aToView, aHalfAngle));
    float p = ((-5.55473f * VdotH) - 6.98316f) * VdotH;
    return aSpecularColor + ((float3(1.f, 1.f, 1.f) - aSpecularColor) * pow(2.f, p));
}

float3 Specular_BRDF(float aRoughness, float3 aNormal, float3 aHalfAngle, float3 aToView, float3 aToLight, float3 aSpecularColor)
{
    float normalDotLight = saturate(dot(aNormal, aToLight));
    float normalDotView = saturate(dot(aNormal, aToView));
    return (NormalDistributionFunction_GGX(aRoughness, saturate(dot(aNormal, aHalfAngle))) *
			Fresnel_SphericalGaussianSchlick(aToView, aHalfAngle, aSpecularColor) *
			GeometricAttenuation_Schlick(normalDotView, normalDotLight, aRoughness)) /
			max(4.f * normalDotLight * normalDotView, 0.01);
}

const float3 CalculateDiffuseIBL(float3 aPixelNormal, int aNumMips)
{
    return EnvCubeTexture.SampleLevel(defaultSampler, aPixelNormal, aNumMips).rgb;
}

const float3 CalculateSpecularIBL(float3 aSpecular, float3 aPixelNormal, float3 aToView, float aRoughmap, int aNumMips)
{
    const float3 R = reflect(-aToView, aPixelNormal);
    
    const float3 envColor = EnvCubeTexture.SampleLevel(defaultSampler, R, aRoughmap * aNumMips).rgb;
    
    float nDotV = saturate(dot(aPixelNormal, aToView));
    const float2 brdfLUT = LUT.Sample(LUTSampler, float2(nDotV, aRoughmap)).rg;
    const float3 iblSpecular = envColor * (aSpecular * brdfLUT.x + brdfLUT.y);
    return iblSpecular;
}

// Functions required for convoluting a cubemap.

float2 Hammersley(float i, float numSamples)
{
    uint b = uint(i);

    b = (b << 16u) | (b >> 16u);
    b = ((b & 0x55555555u) << 1u) | ((b & 0xAAAAAAAAu) >> 1u);
    b = ((b & 0x33333333u) << 2u) | ((b & 0xCCCCCCCCu) >> 2u);
    b = ((b & 0x0F0F0F0Fu) << 4u) | ((b & 0xF0F0F0F0u) >> 4u);
    b = ((b & 0x00FF00FFu) << 8u) | ((b & 0xFF00FF00u) >> 8u);

    const float radicalInverseVDC = float(b) * 2.3283064365386963e-10;
    return float2((i / numSamples), radicalInverseVDC);
}

float3 ImportanceSampleGGX(float2 aXi, float3 aNormal, float aRoughness)
{
    const float roughnesSq = aRoughness * aRoughness;

    const float phi = 2.0f * PI * aXi.x;
    const float cosTheta = sqrt((1.0f - aXi.y) / (1.0f + (roughnesSq * roughnesSq - 1) * aXi.y));
    const float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

    float3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    const float3 up = abs(aNormal.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
    const float3 tangent = normalize(cross(up, aNormal));
    const float3 bitangent = cross(aNormal, tangent);

    return normalize(tangent * H.x + bitangent * H.y + aNormal * H.z);
}

float GeometricAttenuation_Schlick_GGX_IBL(float aNdotV, float aRoughness)
{
    // Note different k here when calculating G GGX for IBL!
    const float a = aRoughness;
    const float k = (a * a) / 2.0f;

    const float nominator = aNdotV;
    const float denominator = aNdotV * (1.0 - k) + k;

    return nominator / denominator;
}

float GeometricAttenuation_Smith_IBL(float3 aN, float3 aV, float3 aL, float aRoughness)
{
    const float NdotV = saturate(dot(aN, aV));
    const float NdotL = saturate(dot(aN, aL));
    
    const float GGX_NdotV = GeometricAttenuation_Schlick_GGX_IBL(NdotV, aRoughness);
    const float GGX_NdotL = GeometricAttenuation_Schlick_GGX_IBL(NdotL, aRoughness);

    return GGX_NdotL * GGX_NdotV;
}

// LUT creation function for caculating a BRDF lookup for the second half of the split-sum equation.
// Run this once with float2 result = IntegrateBRDF(uv.x, uv.y); Causes a LUT to be generated.

float2 IntegrateBRDF(float aNdotV, float aRoughness)
{
    float3 V;
    V.x = sqrt(1.0f - aNdotV * aNdotV);
    V.y = 0.0;
    V.z = aNdotV;

    float a = 0;
    float b = 0;

    const float3 N = float3(0, 0, 1);

    const uint NUM_SAMPLES = 1024u;
    for (uint i = 0u; i < NUM_SAMPLES; ++i)
    {
        const float2 xi = Hammersley(i, NUM_SAMPLES);
        const float3 H = ImportanceSampleGGX(xi, N, aRoughness);
        const float3 L = normalize(2.0 * dot(V, H) * H - V);

        const float NdotL = saturate(L.z);
        const float NdotH = saturate(H.z);
        const float VdotH = saturate(dot(V, H));
        const float NdotV = saturate(dot(N, V));

        if (NdotL > 0.0)
        {
            const float G = GeometricAttenuation_Smith_IBL(N, V, L, aRoughness);
            const float G_Vis = (G * VdotH) / (NdotH * NdotV);
            const float Fc = pow(1.0 - VdotH, 5.0);

            a += (1.0 - Fc) * G_Vis;
            b += Fc * G_Vis;
        }
    }

    return float2(a, b) / NUM_SAMPLES;
}

float CalculateShadow(float3 aWorldPos, float4x4 aLightView, float4x4 aLightProj)
{
    float4 lightSpacePos = mul((aLightView), float4(aWorldPos, 1));
    lightSpacePos = mul(aLightProj, lightSpacePos);

    float3 lightSpaceUVD = lightSpacePos.xyz / lightSpacePos.w;
    
    const float shadowBias = 0.00009f;
    float D = lightSpaceUVD.z - shadowBias;
    
    float2 shadowUV = { 0.f, 0.f };
    shadowUV.x = lightSpaceUVD.x * 0.5f + 0.5f;
    shadowUV.y = -lightSpaceUVD.y * 0.5f + 0.5f;
    return shadowMap.SampleCmpLevelZero(shadowCmpSampler, shadowUV, D).r;
}