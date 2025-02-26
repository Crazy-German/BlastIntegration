#include "Texture.hlsli"
#include "Buffers/FrameBuffer.hlsli"
#include "Includes/defaultShaderIncludes.hlsli"
#include "Includes/GBufferShaderIncludes.hlsli"
#include "Buffers/ObjectBuffer.hlsli"

SamplerState defaultSampler : register(s0);
float SmoothTransition(float value, float edge0, float edge1)
{
    return smoothstep(edge0, edge1, value);
}
float4 SampleAlbedo(DefaultVStoPS pixel)
{
    float4 albedoTex = albedoTexture.Sample(defaultSampler, pixel.UV);
    float4 secondTex = secondAlbedoTexture.Sample(defaultSampler, pixel.UV);
    float4 thirdTex = thirdAlbedoTexture.Sample(defaultSampler, pixel.UV);
    float4 fourthTex = fourthAlbedoTexture.Sample(defaultSampler, pixel.UV);

    float rFactor = pixel.Color.r;
    float gFactor = pixel.Color.g;
    float bFactor = pixel.Color.b;

    float totalFactor = rFactor + gFactor + bFactor;
    totalFactor = max(totalFactor, 0.0001); // Prevent division by zero

    rFactor /= totalFactor;
    gFactor /= totalFactor;
    bFactor /= totalFactor;

    float4 lerpedColor = lerp(albedoTex, secondTex, rFactor);
    lerpedColor = lerp(lerpedColor, thirdTex, gFactor);
    lerpedColor = lerp(lerpedColor, fourthTex, bFactor);
    
    // Get the vertex color
    float3 color = pixel.Color.rgb;

    // Calculate the brightness of the color (luminance)
    float brightness = dot(color, float3(0.2989, 0.5870, 0.1140)); // Standard RGB luminance calculation

    // Use smoothstep to make the transition smoother for near-black colors
    float smoothBlack = SmoothTransition(brightness, 0.0, 0.1); // Smooth transition threshold

    float4 finalAlbedo = lerp(albedoTex, lerpedColor, smoothBlack);
    return float4(finalAlbedo.rgb, 1);
}

float SampleEmission(DefaultVStoPS pixel)
{
    float emission = effectsTexture.Sample(defaultSampler, pixel.UV).r;
    float secondEffect = secondEffectsTexture.Sample(defaultSampler, pixel.UV).r;
    float thirdEffect = thirdEffectsTexture.Sample(defaultSampler, pixel.UV).r;
    float fourthEffect = fourthEffectsTexture.Sample(defaultSampler, pixel.UV).r;

    float rFactor = pixel.Color.r;
    float gFactor = pixel.Color.g;
    float bFactor = pixel.Color.b;

    float totalFactor = rFactor + gFactor + bFactor;
    totalFactor = max(totalFactor, 0.0001); // Prevent division by zero

    rFactor /= totalFactor;
    gFactor /= totalFactor;
    bFactor /= totalFactor;

    float4 lerpedColor = lerp(emission, secondEffect, rFactor);
    lerpedColor = lerp(lerpedColor, thirdEffect, gFactor);
    lerpedColor = lerp(lerpedColor, fourthEffect, bFactor);
    
    // Get the vertex color
    float3 color = pixel.Color.rgb;

    // Calculate the brightness of the color (luminance)
    float brightness = dot(color, float3(0.f, 0.f, 0.f)); // Standard RGB luminance calculation

    // Use smoothstep to make the transition smoother for near-black colors
    float smoothBlack = SmoothTransition(brightness, 0.0, 0.1); // Smooth transition threshold

    float finalEmission = lerp(emission, lerpedColor, smoothBlack);
    return finalEmission;
}

float4 CalculateNormals(DefaultVStoPS pixel)
{
    const float2 normalMap = normalTexture.Sample(defaultSampler, pixel.UV).rg;
    const float2 secondNormalMap = secondNormalTexture.Sample(defaultSampler, pixel.UV).rg;
    const float2 thirdNormalMap = thirdNormalTexture.Sample(defaultSampler, pixel.UV).rg;
    const float2 fourthNormalMap = fourthNormalTexture.Sample(defaultSampler, pixel.UV).rg;
    float3 pixelNormal = float3(normalMap.r, normalMap.g, 0);
    float3 secondNormal = float3(secondNormalMap.r, secondNormalMap.g, 0);
    float3 thirdNormal = float3(thirdNormalMap.r, thirdNormalMap.g, 0);
    float3 fourthNormal = float3(fourthNormalMap.r, fourthNormalMap.g, 0);

    pixelNormal.r = 2 * pixelNormal.r - 1;
    pixelNormal.g = 2 * pixelNormal.g - 1;
    pixelNormal.b = sqrt(1 - saturate(pixelNormal.r * pixelNormal.r + pixelNormal.g * pixelNormal.g));
    pixelNormal = normalize(pixelNormal);

    secondNormal.r = 2 * secondNormal.r - 1;
    secondNormal.g = 2 * secondNormal.g - 1;
    secondNormal.b = sqrt(1 - saturate(secondNormal.r * secondNormal.r + secondNormal.g * secondNormal.g));
    secondNormal = normalize(secondNormal);

    thirdNormal.r = 2 * thirdNormal.r - 1;
    thirdNormal.g = 2 * thirdNormal.g - 1;
    thirdNormal.b = sqrt(1 - saturate(thirdNormal.r * thirdNormal.r + thirdNormal.g * thirdNormal.g));
    thirdNormal = normalize(thirdNormal);

    fourthNormal.r = 2 * fourthNormal.r - 1;
    fourthNormal.g = 2 * fourthNormal.g - 1;
    fourthNormal.b = sqrt(1 - saturate(fourthNormal.r * fourthNormal.r + fourthNormal.g * fourthNormal.g));
    fourthNormal = normalize(fourthNormal);

    float rFactor = pixel.Color.r;
    float gFactor = pixel.Color.g;
    float bFactor = pixel.Color.b;

    float totalFactor = rFactor + gFactor + bFactor;
    totalFactor = max(totalFactor, 0.0001); // Prevent division by zero

    rFactor /= totalFactor;
    gFactor /= totalFactor;
    bFactor /= totalFactor;

    float3 lerpedNormal = lerp(pixelNormal, secondNormal, rFactor);
    lerpedNormal = lerp(lerpedNormal, thirdNormal, gFactor);
    lerpedNormal = lerp(lerpedNormal, fourthNormal, bFactor);

	// Get the vertex color
    float3 color = pixel.Color;
    // Calculate the brightness of the color (luminance)
    float brightness = dot(color, float3(0.f, 0.f, 0.f)); // Standard RGB luminance calculation

    // Use smoothstep to make the transition smoother for near-black colors
    float smoothBlack = SmoothTransition(brightness, 0.0, 0.1); // Smooth transition threshold

    float3 finalNormal = lerp(pixelNormal, lerpedNormal, smoothBlack);

    const float3x3 TBN = float3x3(
	    normalize(pixel.Tangent),
		normalize(pixel.Binormal),
		normalize(pixel.Normal));

    return float4(mul(transpose(TBN), finalNormal), 1);
}

float4 SampleMaterials(DefaultVStoPS pixel)
{
    float3 material = materialTexture.Sample(defaultSampler, pixel.UV);
    float3 secondMaterial = secondMaterialTexture.Sample(defaultSampler, pixel.UV);
    float3 thirdMaterial = secondMaterialTexture.Sample(defaultSampler, pixel.UV);
    float3 fourthMaterial = secondMaterialTexture.Sample(defaultSampler, pixel.UV);

	float rFactor = pixel.Color.r;
    float gFactor = pixel.Color.g;
    float bFactor = pixel.Color.b;

    float totalFactor = rFactor + gFactor + bFactor;
    totalFactor = max(totalFactor, 0.0001); // Prevent division by zero

    rFactor /= totalFactor;
    gFactor /= totalFactor;
    bFactor /= totalFactor;

    float3 lerpedMaterial = lerp(material, secondMaterial, rFactor);
    lerpedMaterial = lerp(lerpedMaterial, thirdMaterial, gFactor);
    lerpedMaterial = lerp(lerpedMaterial, fourthMaterial, bFactor);

    // Get the vertex color
    float3 color = pixel.Color;
    // Calculate the brightness of the color (luminance)
    //float brightness = dot(color, float3(0.2989, 0.5870, 0.1140)); // Standard RGB luminance calculation
    float brightness = dot(color, float3(0.f, 0.f, 0.f));

    // Use smoothstep to make the transition smoother for near-black colors
    float smoothBlack = SmoothTransition(brightness, 0.0, 0.1); // Smooth transition threshold

    float3 finalMaterial = lerp(material, lerpedMaterial, smoothBlack);
    return float4(finalMaterial, 1);
}

GBufferOutput main(DefaultVStoPS pixel)
{
    GBufferOutput GBuffer;
    switch (FB_RenderPass)
    {
        case 6:
            GBuffer.Albedo = float4(pixel.Color.rgb, 1);
            break;
        case 7:
            GBuffer.Albedo = float4(pixel.Normal, 1);
            break;
    case 8:
            GBuffer.Albedo = float4(pixel.UV, 0, 1);
            break;
    case 9:
        default:
            GBuffer.Albedo = float4(SampleAlbedo(pixel).rgb, 1);
            break;
    }

    GBuffer.Material = SampleMaterials(pixel);
    GBuffer.WorldNormal = CalculateNormals(pixel);
    GBuffer.WorldPosition = float4(pixel.WorldPos, 1);
    GBuffer.Effects.r = SampleEmission(pixel);
    GBuffer.Effects.g = OB_IsHit;
    GBuffer.Effects.b = (pixel.Position.z/ pixel.Position.w) * 1000.0f * FB_FogMultiplier;
    return GBuffer;
}