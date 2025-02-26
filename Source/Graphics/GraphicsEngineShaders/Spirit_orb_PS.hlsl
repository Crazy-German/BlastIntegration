#include "UI_VS.hlsl"
#include "Texture.hlsli"
#include "Buffers/MiscBuffer.hlsli"

SamplerState DefaultSampler : register(s0);

float4 main(UI_VS_to_PS input) : SV_TARGET
{
	// Get UVs
	float2 uv = input.uv;

	// Spherify
    float2 cenUv = uv - 0.5f;

	// Spherify
	float2 expandedCenUv = cenUv * 2;
	float z = sqrt(1.0f - saturate(dot(expandedCenUv.xy, expandedCenUv.xy)));
	float2 spherifiedUv = expandedCenUv / (z + 1.0f);
	spherifiedUv *= 0.5f + 0.5f;
	float spherify = 1;
	spherifiedUv = lerp(uv, spherifiedUv, spherify);

	// Time
	float time = FB_TotalTime;
	float speed = 0.2f;

	// Generate UVs
	float2 sphereScrollUv1 = float2(spherifiedUv.x, spherifiedUv.y  + time * speed * 1.2141f) * 1.2523f;
	float2 sphereScrollUv2 = float2(spherifiedUv.x + time * speed * 0.1849f, spherifiedUv.y  + time * speed * 0.5849f) * 0.6724f;
	float2 sphereScrollUv3 = float2(spherifiedUv.x + time * speed * 0.1234f, spherifiedUv.y + time * speed * 0.5425f) * 0.3145f;
	float2 sphereScrollUv4 = float2(spherifiedUv.x + time * speed * 0.1934f, spherifiedUv.y + time * speed * 0.6125f) * 0.4634f;
	
	// Samplings
	float alpha = pow(materialTexture.Sample(DefaultSampler, uv), 2).a;

	float liquidNoiseTex1 = materialTexture.Sample(DefaultSampler, sphereScrollUv1).r;
	float liquidNoiseTex2 = materialTexture.Sample(DefaultSampler, sphereScrollUv2).r;
	float liquidNoiseTex3 = materialTexture.Sample(DefaultSampler, sphereScrollUv3).g;
	float liquidNoiseTex4 = materialTexture.Sample(DefaultSampler, sphereScrollUv4).g;
	
	float3 albedo = pow(albedoTexture.Sample(DefaultSampler, sphereScrollUv3), 2.2f).rgb;

	float glassOrb = saturate(pow(materialTexture.Sample(DefaultSampler, uv), 2.2f).b);

	// Radial mask
    float radialDistance = length(cenUv);
	radialDistance = 1 - radialDistance;
	radialDistance = saturate((radialDistance - 0.5f) * 6.4f + 0.5f);
	radialDistance = saturate(pow(radialDistance, 6.0f));
	radialDistance *= alpha;
		
	// Health mask
	float healthValue = MB_Spirit;
	float healthMask = 1 - ((1 - uv.y) - healthValue) * 100.0f;

	healthMask += liquidNoiseTex4 * liquidNoiseTex3 * 3.0f;
	healthMask = saturate(healthMask);

	// // Gradient mask
	float gradientMask = smoothstep(0.0f, 2.9f, (1 - saturate(((1 - uv.y) - healthValue) * 4.8f))) * (liquidNoiseTex2 * 0.39f);

	// // Line mask
	float lineGradient =  1 - smoothstep(0.1f, 1.0f, 1 - saturate(((1 - uv.y + 0.02) - healthValue) * 4));
	float lineMask = healthMask * lineGradient * 1.2f;

	float3 liquid = saturate(((((liquidNoiseTex1 * liquidNoiseTex2 * 2.0f) + 
					(liquidNoiseTex3 * liquidNoiseTex4 * liquidNoiseTex3 * 1.0f)) *
					(albedo * 2.0f)) * radialDistance + (albedo * 0.4f)) * (gradientMask + healthMask)) * 
					saturate(alpha); 

	float3 lineGlow = saturate(3.4f * lineMask * pow(float3(0.5f,0.5f,0.25f), 0.5));

	float3 output = saturate(liquid + lineGlow) * alpha;

	output = saturate(output + glassOrb * pow(float3(0.5f,0.5f,0.25f), 0.25f) * alpha);

	return float4(output, alpha);
}