#include "Includes/DefaultShaderIncludes.hlsli"
#include "Texture.hlsli"
#include "BRDF_Functions.hlsli"
//#include "UI_VS.hlsl"


float4 main(UI_VS_to_PS input) : SV_TARGET
{
    const float4 color = albedoTexture.Sample(defaultSampler, input.uv);
    //return float4(input.uv.x, input.uv.y, 0, 1);
    float3 skibidiColor = pow(abs(color.xyz), 1.0 / 2.2);
    return float4(skibidiColor.xyz, color.a) * input.tint;
}