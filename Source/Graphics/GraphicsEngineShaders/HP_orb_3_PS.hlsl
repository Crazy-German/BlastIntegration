#include "UI_VS.hlsl"
#include "Buffers/MiscBuffer.hlsli"
#include "Texture.hlsli"

SamplerState DefaultSampler : register(s0);

float HeartPumpWave(float t)
{
    float frequency = 0.6; // How often the heart pumps (pulses per second)
    float amplitude = 0.2; // Intensity of the contraction (amplitude of the wave)
    float baseline = 0.9; // Baseline level (default heart size)
    // Heart pumping modeled as a sine wave, with periodic contraction/relaxation
    // Sinusoidal function modulates between contraction and relaxation
    return baseline + amplitude * (sin(t * frequency * 6.28318) + sin(t * (frequency * 2) * 6.28318)); // 6.28318 is 2*PI
	

}

float2 RotateUV(float2 uv, float angle)
{
    // Create the 2D rotation matrix
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    float2x2 rotationMatrix = float2x2(
        cosAngle, -sinAngle,
        sinAngle, cosAngle
    );

    // Rotate the UV coordinates
    return mul(uv, rotationMatrix);
}

float4 main(UI_VS_to_PS input) : SV_TARGET
{
	// Get UVs
    float2 uv = input.uv;

	// Spherif
    float2 cenUv = uv - 0.5f;
	// Spherify
    float2 expandedCenUv = cenUv * 2;
    float z = sqrt(1.0f - saturate(dot(expandedCenUv.xy, expandedCenUv.xy)));
    float2 spherifiedUv = expandedCenUv / (z + 1.0f);
    spherifiedUv *= 0.5f + 0.5f;
    float spherify = 1.0f;
    spherifiedUv = lerp(uv, spherifiedUv, spherify);

    float healthValue = MB_Health; //MB_Health;
	// Time
    float time = FB_TotalTime;
    float speedVeinX = time * 0.06f;
    float speedVeinY = time * 0.5f;
    float speedWave = time * 0.3f;

    float colorPump = HeartPumpWave(time);
    float uvPump = HeartPumpWave(time);
    spherifiedUv = RotateUV(lerp(uv, spherifiedUv, spherify) * (uvPump), 50);

	// Generate UV
	
    float2 sphereScrollVein1 = float2(spherifiedUv.x + speedVeinX, spherifiedUv.y + speedVeinY);
    float2 sphereScrollVein2 = float2(spherifiedUv.x + speedVeinX * 1.5, spherifiedUv.y + speedVeinY * 1.5);
    float2 sphereScrollVein3 = float2(spherifiedUv.x + speedVeinX * 2, spherifiedUv.y + speedVeinY * 3);
	
	
    float2 sphereScrollWave1 = float2(spherifiedUv.x, spherifiedUv.y = healthValue);
    float2 sphereScrollWave2 = float2(spherifiedUv.x, spherifiedUv.y = healthValue);

	// Samplings
    float alpha = pow(materialTexture.Sample(DefaultSampler, uv), 5).a;

    float VeinNoiseTex1 = materialTexture.Sample(DefaultSampler, sphereScrollVein1).r;
    float VeinNoiseTex2 = materialTexture.Sample(DefaultSampler, sphereScrollVein2).r;
    float VeinNoiseTex3 = materialTexture.Sample(DefaultSampler, sphereScrollVein3).r;
    float waveNoiseTex1 = materialTexture.Sample(DefaultSampler, sphereScrollWave1).g;
    float waveNoiseTex2 = materialTexture.Sample(DefaultSampler, sphereScrollWave2).g;
	
    float3 albedo = float3(colorPump / 2.2, 0, 0);

    float glassOrb = saturate((pow(materialTexture.Sample(DefaultSampler, uv), 5).b * 1));
	//float glassOrb = saturate(pow(materialTexture.Sample(defaultSampler, uv), 1.2f).b);
	// Radial mask
    float radialDistance = length(cenUv);
    radialDistance = 1 - radialDistance;
    radialDistance = saturate((radialDistance - 0.5) * 5 + 0.5);
    radialDistance = saturate(pow(radialDistance, 8));
    radialDistance *= alpha;
	
	// Health mask
    float healthMask = 1 - ((1 - uv.y) - healthValue) * 125.0;

    healthMask += (VeinNoiseTex1 * 2) * (VeinNoiseTex2 * 2) * VeinNoiseTex3 * 3;
    healthMask = saturate(healthMask);

	// // Gradient mask
    float gradientMask = smoothstep(0.0f, 3.5f, (1 - saturate(((1 - uv.y + 0.) - healthValue) * 7)));

	// // Line mask
    float lineGradient = 1 - smoothstep(0.1f, 1.1f, (1 - saturate(((1 - uv.y + (HeartPumpWave(time) * 0.05)) - healthValue) * 2)));
    float lineMask = healthMask * lineGradient;
	
	// // Color
    float3 baseColor = float3(0.6, 0, 0);

	// Less glowy, more wispy
    float3 liquid = (((VeinNoiseTex1)) * (albedo * 2)) * (baseColor * radialDistance) * (healthMask) * alpha;
    float3 lineGlow = saturate(baseColor * 4 * lineMask) * radialDistance;

    float3 output = saturate(liquid + glassOrb + lineGlow) * alpha;

    return float4(output, alpha);
}