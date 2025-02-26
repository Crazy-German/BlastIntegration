#include "Includes/QuadShaderIncludes.hlsli"
Texture2D inputTexture : register(t1);
SamplerState defaultClampSampler : register(s0);

float4 main(QuadVStoPS pixel) : SV_TARGET
{
    return inputTexture.Sample(defaultClampSampler, pixel.UV);
}