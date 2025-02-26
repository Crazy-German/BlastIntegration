#include "Texture.hlsli"
#include "Includes/DefaultShaderIncludes.hlsli"
#include "Buffers/MaterialBuffer.hlsli"

SamplerState DefaultSampler : register(s0);

float median(float3 aRGB)
{
    return max(min(aRGB.r, aRGB.g), min(max(aRGB.r, aRGB.g), aRGB.b));
}

float2 SafeNormalize(in float2 v)
{
    float len = length(v);
    len = (len > 0.0) ? 1.0 / len : 0.0;
    return v * len;
}

float screenPxRange(float2 uvs)
{
    const float screenPxRange = 2; //default for msdf-atlas-gen?
    //212 atlas size
    float2 unitRange = float2(screenPxRange, screenPxRange) / float2(256, 256);
    float2 screenTexSize = float2(1.0, 1.0) / fwidth(uvs);
    return max(0.5f * dot(unitRange, screenTexSize), 1.0f);
}

float4 main(UI_VS_to_PS input) : SV_Target
{
    float4 result;
    
    const float3 fontPx = fontTexture.Sample(DefaultSampler, input.uv).rgb;
    
    float sd = median(fontPx);
    float screenPxDist = screenPxRange(input.uv) * (sd - 0.5f);
    float opacity = clamp(screenPxDist + 0.5f, 0.0f, 1.0f);
    
    float3 bgColor = float3(0, 0, 0);
    float3 fgColor = float3(1, 1, 1);
    
    result.rgb = lerp(bgColor, fgColor, opacity);
    result.a = (result.r + result.g + result.b) * 0.5f;
    
    //float distanceAlphaMask = result.a;
    
    //const uint outlineMask = 1;
    //const float outlineMinValue0 = 0.05f;
    //const float outlineMinValue1 = 0.1f;
    //const float outlineMaxValue0 = 1.1f;
    //const float outlineMaxValue1 = 1.25f;
    
    
    //if(outlineMask)
    //{
    //    float opacityFactor = 0.0f;
    //    if ((distanceAlphaMask >= outlineMinValue0) && (distanceAlphaMask <= outlineMaxValue1))
    //    {
    //        opacityFactor = smoothstep(outlineMinValue0, outlineMinValue1, distanceAlphaMask);
    //    }
    //    else
    //    {
    //        opacityFactor = smoothstep(outlineMaxValue1, outlineMaxValue0, distanceAlphaMask);
    //    }
    //    result.rgb = lerp(MB_AlbedoTint.rgb, float3(0, 0, 0), opacityFactor);

    //}
    //if(result.a > 0.01f && result.a < 1.5f)
    //{
    //    result.a = 1;
    //    result.rgb = float3(0, 0, 0);
    //}
    
    result *= (MB_AlbedoTint, 1);
    result.rgb *= MB_AlbedoTint;
    result *= (MB_AlbedoTint, 1);
    
    return result;
}