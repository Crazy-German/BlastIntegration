#include "../Buffers/GBuffer.hlsli"
#include "../BRDF_Functions.hlsli"
#include "../Includes/QuadShaderIncludes.hlsli"
#include "../Buffers/Framebuffer.hlsli"
#include "../Buffers/DebugBuffer.hlsli"
float4 main(QuadVStoPS pixel) : SV_TARGET
{
    float3 albedo = GBufferAlbedo.Sample(defaultSampler, pixel.UV).rgb;
    float3 material = GBufferMaterial.Sample(defaultSampler, pixel.UV).rgb;
    float3 pixelNormal = GBufferNormal.Sample(defaultSampler, pixel.UV).rgb;
    float emissionMask = GBufferFX.Sample(defaultSampler, pixel.UV).r;
    float3 emission = albedo * emissionMask;
    const float occlusion = material.r;
    const float metalness = material.b;
    const float roughness = material.g;

    switch(FB_RenderPass)
    {
    case 0: //game
            break;
    case 1: //albedo
            return float4(albedo, 1);
    case 2: //occlusion
            return float4(occlusion.rrr, 1);
    case 3: //metalness
            return float4(roughness.rrr, 1);
    case 4: //roughness
            return float4(metalness.rrr, 1);
    case 5: //normal
            return float4(pixelNormal, 1);
    case 6:
			//Vertex color (stored in albedo)
            return float4(albedo, 1);
    case 7:
			//Vertex normal (stored in Albedo)
            return float4(albedo, 1);
    case 8:
            return float4(albedo, 1);
    case 9:
            return float4(emission.r, 0, 0, 1);
    default:
            break;
    }
    float3 worldPos = GBufferWorldPos.Sample(defaultSampler, pixel.UV).rgb;

    float lightAttenuation = saturate(dot(pixelNormal, normalize(-DirLight.LightDir.rgb)));

    const float3 specularColor = lerp(float3(0.04f, 0.04f, 0.04f), albedo.rgb, metalness);
    const float3 diffuseColor = lerp(float3(0.00f, 0.00f, 0.00f), albedo.rgb, 1 - metalness);
    const float3 toView = normalize(FB_CameraPos - worldPos);
    const float3 lightDirection = normalize(DirLight.LightPos.rgb - worldPos);
    const float3 halfAngle = normalize(lightDirection + toView);

    const int numMips = GetNumMips(EnvCubeTexture) - 1;
    const float3 diffuseIBL = CalculateDiffuseIBL(pixelNormal, numMips) * DB_CubemapInfluence;
    const float3 specularIBL = CalculateSpecularIBL(specularColor, pixelNormal, toView, roughness, numMips) * DB_CubemapInfluence;
    float3 ambientLight = (diffuseColor * diffuseIBL + specularIBL) * occlusion;
 
    float3 specularLight = Specular_BRDF(roughness, pixelNormal, halfAngle, toView, lightDirection, specularColor);
    float3 diffuseLight = Diffuse_BRDF(diffuseColor);
    diffuseLight *= 1 - specularLight;

    float inShadow = CalculateShadow(worldPos, DirLight.LightViewInv, DirLight.LightProj);
	float3 radiance = ambientLight + ((diffuseLight + specularLight) * DirLight.Color * DirLight.Intensity * lightAttenuation * inShadow);
    radiance = radiance + emission;
    return float4(radiance, 1);
   
}