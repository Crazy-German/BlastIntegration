#include "Includes/DefaultShaderIncludes.hlsli"
#include "Texture.hlsli"
#include "BRDF_Functions.hlsli"
#include "Quad_VS.hlsl"

float3 YUVToRGB(float3 aYuv)
{
    // BT.601 coefs
    static const float3 yuvCoef_r = { 1.164f, 0.000f, 1.596f };
    static const float3 yuvCoef_g = { 1.164f, -0.392f, -0.813f };
    static const float3 yuvCoef_b = { 1.164f, 2.017f, 0.000f };
    aYuv -= float3(0.0625f, 0.5f, 0.5f);
    return saturate(float3(
    dot(aYuv, yuvCoef_r),
    dot(aYuv, yuvCoef_g),
    dot(aYuv, yuvCoef_b)
    ));
}


float4 main(QuadVStoPS input) : SV_TARGET
{
    float y = albedoTexture.Sample(defaultSampler, float2(input.UV.x, input.UV.y * 0.5f)).r;
    float u = albedoTexture.Sample(defaultSampler, float2(input.UV.x * 0.5f, 0.5f + input.UV.y * 0.25f)).r;
    float v = albedoTexture.Sample(defaultSampler, float2(input.UV.x * 0.5f, 0.75f + input.UV.y * 0.25f)).r;
    return float4(YUVToRGB(float3(y, u, v)), 1.0f);
}