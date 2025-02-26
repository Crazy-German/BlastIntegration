#include "../Buffers/GBuffer.hlsli"
#include "../BRDF_Functions.hlsli"
#include "../Includes/QuadShaderIncludes.hlsli"
#include "../Buffers/Framebuffer.hlsli"
#include "../Buffers/DebugBuffer.hlsli"

float4 main(QuadVStoPS pixel) : SV_TARGET
{
    switch (FB_RenderPass)
    {
        case 0: 
            break;
        default:
            return 
            float4 (0, 0, 0, 1);
    }

    float3 albedo = GBufferAlbedo.Sample(defaultSampler, pixel.UV).rgb;
    float3 material = GBufferMaterial.Sample(defaultSampler, pixel.UV).rgb;
    float3 pixelNormal = GBufferNormal.Sample(defaultSampler, pixel.UV).rgb;

    float3 totalRadiance = float4(0, 0, 0, 0);
    [unroll]
    for (int index = 0; index < 8; index++)
    {
        [flatten]
        if (PointLights[index].Active == true)
        {
            const float occlusion = material.r;
            const float metalness = material.b;
            const float roughness = material.g;

            float3 worldPos = GBufferWorldPos.Sample(defaultSampler, pixel.UV).rgb;

            float3 L = PointLights[index].Lightpos.rgb - worldPos;
            float d = length(L);
	        // You require a very high Intensity for this, say 10000+.
            float lightAttenuation = 1 / (d * d);

            const float3 specularColor = lerp(float3(0.04f, 0.04f, 0.04f), albedo.rgb, metalness);
            const float3 diffuseColor = lerp(float3(0.00f, 0.00f, 0.00f), albedo.rgb, 1 - metalness);
            const float3 toView = normalize(FB_CameraPos - worldPos);
            const float3 lightDirection = normalize(PointLights[index].Lightpos.rgb - worldPos);
            const float3 halfAngle = normalize(lightDirection + toView);

            const int numMips = GetNumMips(EnvCubeTexture) - 1;
            const float3 diffuseIBL = CalculateDiffuseIBL(pixelNormal, numMips) * DB_CubemapInfluence;
            const float3 specularIBL = CalculateSpecularIBL(specularColor, pixelNormal, toView, roughness, numMips) * DB_CubemapInfluence;
            float3 ambientLight = (diffuseColor * diffuseIBL + specularIBL) * occlusion;
 
            float3 specularLight = Specular_BRDF(roughness, pixelNormal, halfAngle, toView, lightDirection, specularColor);
            float3 diffuseLight = Diffuse_BRDF(diffuseColor);
            diffuseLight *= 1 - specularLight;

            //float inShadow = CalculateShadow(worldPos, PointLights[index].LightViewInv, PointLights[index].LightProj);

	        // resulting color
            float3 radiance = ((diffuseLight + specularLight) * PointLights[index].Color * PointLights[index].Intensity * lightAttenuation/* * inShadow*/);
            totalRadiance += radiance;
        }
    }
    float emissionMask = GBufferFX.Sample(defaultSampler, pixel.UV).r;
    float3 emission = albedo * emissionMask;
    totalRadiance = totalRadiance + emission;
    return float4(totalRadiance, 1);
}