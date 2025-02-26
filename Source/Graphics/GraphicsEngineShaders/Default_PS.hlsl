#include "Buffers/MaterialBuffer.hlsli"
#include "Includes/DefaultShaderIncludes.hlsli"
#include "BRDF_Functions.hlsli"
#include "Buffers/Framebuffer.hlsli"
#include "Texture.hlsli"
#include "Buffers/DebugBuffer.hlsli"

float GetDirLightShadow(float4 aWorldPosition)
{
    
    float4 lightSpacePos = mul(DirLight.LightViewInv, float4(aWorldPosition.xyz, 1));
    lightSpacePos = mul(DirLight.LightProj, lightSpacePos);
    
    float3 lightSpaceUV = lightSpacePos.xyz / lightSpacePos.w;
    float shadowBias = 0.005f;
    float D = lightSpaceUV.z - shadowBias;
    
    float2 shadowUV = lightSpaceUV.xy * 0.5f + 0.5f;
    shadowUV.y = 1 - shadowUV.y;
    
    return shadowMap.SampleCmpLevelZero(shadowCmpSampler, shadowUV, D).r;
}

float CalculateLightAttenuation(float3 aPixelNormal, float3 aPixelPos, int aLightMode, int aLightIndex)
{
    float lightAttenuation = 0;
    switch (aLightMode)
    {
        case 0:
		{
                lightAttenuation = saturate(dot(aPixelNormal, normalize(-DirLight.LightDir.rgb)));

            }
            break;
        case
    1:
		{
				/* Point Light Code */
                float3 L = PointLights[aLightIndex].Lightpos.rgb - aPixelPos;
                float d = length(L);
        		// You require a very high Intensity for this, say 10000+.
                lightAttenuation = 1 / (d * d);
            }
            break;
        case
    2:
	{
				/* Spot Light Code */
                float3 L = SpotLights[aLightIndex].Lightpos.rgb - aPixelPos;
                float d = length(L);
                float spotCone = pow(saturate(dot(normalize(L), normalize(SpotLights[aLightIndex].LightDir.rgb))), SpotLights[aLightIndex].OuterConeAngle);
				// You require a very high Intensity for this, say 10000+.
                lightAttenuation = (1 / (d * d)) * spotCone;
            }
            break;
        default:
	    {
            }
            break;
    }
    return lightAttenuation;
}

float4 main(DefaultVStoPS pixel) : SV_TARGET
{
    float4 result;
    result.r = 0;
    result.g = 0;
    result.b = 0;
    result.a = 0;
    
    float3 kA = 0.0f;
    
    //Create TBN Matrix
    const float3x3 TBN = float3x3
    (
    normalize(pixel.Tangent),
    normalize(pixel.Binormal),
    normalize(pixel.Normal)
    );
    
    //scale UV
    const float2 scaledUV = pixel.UV;
    
    //Sample pixelNormal from normalMap
    const float2 normalSample = normalTexture.Sample(defaultSampler, scaledUV).rg;
    float3 pixelNormal = float3(normalSample, 0);
    pixelNormal = 2.0f * pixelNormal - 1.0f;
    pixelNormal.z = sqrt(1 - saturate(dot(pixelNormal.xy, pixelNormal.xy)));
    pixelNormal = normalize(pixelNormal);
   // pixelNormal.xy *= M_NormalStrength;
    pixelNormal = normalize(mul(pixelNormal, TBN));

    
    //Sample materialTexture
    const float3 materialMap = materialTexture.Sample(defaultSampler, scaledUV).rgb;
    const float metalMap = materialMap.b;
    const float roughMap = materialMap.g;
    const float occlusionMap = materialMap.r;

    //Sample albedoTexture
    const float4 albedoMap = albedoTexture.Sample(defaultSampler, scaledUV);
    
    //Perfect Specular surface color
    const float3 specularColor = lerp((float3) 0.04f, albedoMap.rgb, metalMap);
    //Perfect Diffuse surface color
    const float3 diffuseColor = lerp((float3) 0.00f, albedoMap.rgb, 1 - metalMap);
 
    float3 toView = normalize(FB_CameraPos.xyz - pixel.WorldPos.xyz);
    
    //DIRECTIONAL LIGHT
    float3 directlightRadiance = 0.0f;
    float3 toDirLight = normalize(DirLight.LightPos.xyz - pixel.WorldPos.xyz);
    float3 halfAngle = normalize((toDirLight + toView));
        
        //Direct lighting
    float3 kS = Specular_BRDF(roughMap, pixelNormal, halfAngle, toView, toDirLight, specularColor);
        
    float3 kD = Diffuse_BRDF(diffuseColor);
        
    kD *= (1.0f - kS);
    const float3 lightColorAndIntensity = DirLight.Color * DirLight.Intensity;
        
        
    float3 directlightDirectRadiance = (kD + kS) * lightColorAndIntensity * saturate(dot(pixelNormal, toDirLight));
    float dirLightAttenuation = saturate(dot(pixelNormal, normalize(-DirLight.LightDir.rgb)));
        
        //Indirect lighting
    float2 integratedBRDF = IntegrateBRDF(pixel.UV.x, pixel.UV.y);
        
    const float3 diffuseIBL = CalculateDiffuseIBL(pixelNormal, GetNumMips(EnvCubeTexture)) * DB_CubemapInfluence;
    const float3 specularIBL = CalculateSpecularIBL(specularColor, pixelNormal, toView, roughMap, GetNumMips(EnvCubeTexture)) * DB_CubemapInfluence;
        
    kA = (diffuseColor * diffuseIBL + specularIBL) * occlusionMap;
        

    float shadow = GetDirLightShadow(float4(pixel.WorldPos, 0));
        [flatten]
    if (DirLight.Active == false)
    {
        directlightDirectRadiance = 0.0f;
        
    }
    directlightRadiance = directlightDirectRadiance * shadow + kA;
    
    //POINTLIGHTS
    float3 pointLightTotalRadiance = 0.0f;
    [unroll]
    for (uint pointIndex = 0; pointIndex < 4; ++pointIndex)
    {
        float pointDistance = length(PointLights[pointIndex].Lightpos - pixel.WorldPos.xyz);
        [flatten]
        if (PointLights[pointIndex].Active == true && pointDistance < PointLights[pointIndex].Range)
        {
            float3 toPointLight = normalize(PointLights[pointIndex].Lightpos - pixel.WorldPos.xyz);
            float3 halfAnglePoint = normalize((toPointLight + toView));
        
            //DirectLighting
            const float3 kS = Specular_BRDF(roughMap, pixelNormal, halfAnglePoint, toView, toPointLight, specularColor);
        
            float3 kD = Diffuse_BRDF(diffuseColor);
        
            //kD *= (1.0f - kS);
            const float3 lightColorAndIntensity = PointLights[pointIndex].Color * PointLights[pointIndex].Intensity;
        
            const float3 pointlightDirectRadiance = (kD + kS) * lightColorAndIntensity * saturate(dot(pixelNormal, toPointLight));
        
            //Indirect lighting
            float2 integratedBRDF = IntegrateBRDF(pixel.UV.x, pixel.UV.y);
        
            const float3 diffuseIBL = CalculateDiffuseIBL(pixelNormal, GetNumMips(EnvCubeTexture)) * DB_CubemapInfluence;
            const float3 specularIBL = CalculateSpecularIBL(specularColor, pixelNormal, toView, roughMap, GetNumMips(EnvCubeTexture)) * DB_CubemapInfluence;
        
            const float3 kA = (diffuseColor * diffuseIBL + specularIBL) * occlusionMap;
        
            //Attenuation
            float pointLightRange = max(PointLights[pointIndex].Range, 0.001f);
            float pointAttenuation = 1 - pow(pointDistance * (1.0f / pointLightRange), 2);
            
            pointLightTotalRadiance += pointlightDirectRadiance * pointAttenuation;

        }
        
    }
     //SPOTLIGHTS
    float3 spotLightTotalRadiance = 0.0f;
    [unroll]
    for (uint spotIndex = 0; spotIndex < 4; ++spotIndex)
    {
        float spotDistance = length(SpotLights[spotIndex].Lightpos - pixel.WorldPos.xyz);
        [flatten]
        if (SpotLights[spotIndex].Active == true && spotDistance < SpotLights[spotIndex].Range)
        {
            float3 toSpotLight = normalize(SpotLights[spotIndex].Lightpos - pixel.WorldPos.xyz);
            float3 halfAngleSpot = normalize((toSpotLight + toView));
        
            //DirectLighting
            const float3 kS = Specular_BRDF(roughMap, pixelNormal, halfAngleSpot, toView, toSpotLight, specularColor);
        
            float3 kD = Diffuse_BRDF(diffuseColor);
        
            //kD = (1.0f - kS);
            const float3 lightColorAndIntensity = SpotLights[spotIndex].Color * SpotLights[spotIndex].Intensity;
        
            const float3 spotlightDirectRadiance = (kD + kS) * lightColorAndIntensity * saturate(dot(pixelNormal, toSpotLight));
        
            //Indirect lighting
            float2 integratedBRDF = IntegrateBRDF(pixel.UV.x, pixel.UV.y);
        
            const float3 diffuseIBL = CalculateDiffuseIBL(pixelNormal, GetNumMips(EnvCubeTexture)) * DB_CubemapInfluence;
            const float3 specularIBL = CalculateSpecularIBL(specularColor, pixelNormal, toView, roughMap, GetNumMips(EnvCubeTexture)) * DB_CubemapInfluence;
        
            const float3 kA = (diffuseColor * diffuseIBL + specularIBL) * occlusionMap;
            
            
            //Attenuation
            float3 spotLightReverseDir = SpotLights[spotIndex].LightDir * -1;
            float spotLightRange = max(SpotLights[spotIndex].Range, 0.001f);
            float distanceAttenuationSpotLight = 1 - pow(spotDistance * (1.0f / spotLightRange), 2);
            float angleAttenuation = pow(dot(spotLightReverseDir, toSpotLight) - sin(SpotLights[spotIndex].OuterConeAngle) / (cos(SpotLights[spotIndex].InnerConeAngle) - cos(SpotLights[spotIndex].OuterConeAngle)), 2);
            float spotAttenuation = mul(distanceAttenuationSpotLight, angleAttenuation);
            
            spotLightTotalRadiance += spotlightDirectRadiance * spotAttenuation;

        }
       
    
    }
    
    //Assemble total light
    float3 radiance = (directlightRadiance + pointLightTotalRadiance + spotLightTotalRadiance);
    float emissionMask = effectsTexture.Sample(defaultSampler, scaledUV).r;
    float3 emission = albedoMap.rgb * emissionMask;
    
    result.rgb = radiance + emission;
    result.a = 1.0f;
    return result;

    };

