#include "Includes/DefaultShaderIncludes.hlsli"
#include "Buffers/MiscBuffer.hlsli"

float4 main(UI_VS_to_PS input) : SV_TARGET
{
    float2 uv = input.uv;

    float2 rotUv = uv;
    rotUv -= 0.5f;

    float rotation = 0.0f;

    float s = sin(rotation);
    float c = cos(rotation);

    float2x2 rotMatrix = float2x2(-c, s, s, c);

    rotUv = mul(rotMatrix, rotUv);

    rotUv += 0.5;

    float2 newUv = rotUv * 2.0 - 1.0;

    float2 radUv;
    radUv.x = (atan2(newUv.x, newUv.y) * 0.15915494309189533576888376337251) + 0.5; 
    radUv.y = length(radUv);

    // TODO: Normalized time goes in here
    float remainingTime = MB_CooldownOneRemaining;

    float revealPercentage = 1 - remainingTime;

    float reveal = 1 - step(radUv.x, revealPercentage);

    float transparency = 0.25f;
    float transparentReveal = saturate(reveal - transparency);

    float3 outputColor = float3(0,0,0) * reveal;
    
    return float4(outputColor, transparentReveal);
}