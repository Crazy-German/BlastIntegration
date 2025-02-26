#include "Buffers/FrameBuffer.hlsli"
#include "Includes/QuadShaderIncludes.hlsli"

SamplerState defaultClampSampler : register(s0);
Texture2D inputTexture : register(t1);

float4 main(QuadVStoPS pixel) : SV_TARGET
{
    float texelSize = 1.0f / FB_Resolution.y; // Vertical
    static const float GaussianKernel[5] = { 0.06136f, 0.24477f, 0.38774f, 0.24477f, 0.06136 };
    uint kernelSize = 5;
    float step = (((float) kernelSize - 1.0f) / 2.0f) * -1.0f;
    float3 result = 0;
    for (uint s = 0; s < kernelSize; ++s)
    {
        float2 uv = pixel.UV + float2(0.0f, texelSize * (step + (float) s)); // Vertical
        float3 color = inputTexture.Sample(defaultClampSampler, uv).rgb;
        result += color * GaussianKernel[s];
    }
    return float4(result, 1.0f);
}