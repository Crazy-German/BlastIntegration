#include "BRDF_Functions.hlsli" // Included beacuse defaultsampler
#include "Includes/QuadShaderIncludes.hlsli"
#include "Buffers/DebugBuffer.hlsli"
Texture2D intermediateBuffer : register(t126);
// Pick one of these at a time. Keep in mind that ACES and Lottes need to be
// corrected after output but not the Unreal one!

float3 Tonemap_ACES(float3 aColor)
{
    // Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return (aColor * (a * aColor + b)) / (aColor * (c * aColor + d) + e);
}

float3 Tonemap_UnrealEngine(float3 aColor)
{
    // Unreal 3, Documentation: "Color Grading"
	// Adapted to be close to Tonemap_ACES, with similar range
	// Gamma 2.2 correction is baked in, don't use with sRGB conversion!
    return aColor / (aColor + 0.155) * 1.019;
}


float3 Tonemap_Lottes(float3 aColor)
{
	// Lottes 2016, "Advanced Techniques and Optimization of HDR Color Pipelines"
    const float a = 1.6;
    const float d = 0.977;
    const float hdrMax = 8.0;
    const float midIn = 0.18;
    const float midOut = 0.267;

	// Can be precomputed
    const float b =
		(-pow(midIn, a) + pow(hdrMax, a) * midOut) /
		((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);
    const float c =
		(pow(hdrMax, a * d) * pow(midIn, a) - pow(hdrMax, a) * pow(midIn, a * d) * midOut) /
		((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);

    return pow(aColor, a) / (pow(aColor, a * d) * b + c);
}

float3 LinearToGamma(float3 aColor)
{
    return pow(abs(aColor), 1.0 / 2.2);
}

float4 main(QuadVStoPS pixel) : SV_TARGET
{

    float3 color = intermediateBuffer.Sample(defaultSampler, pixel.UV).rgb;

    //color = float3(1, 1, 1) - exp(color * DB_Exposure);   Exposure code, could maybe be used in P7

    switch (DB_Tonemap)
    {
        case 0:
            color = Tonemap_ACES(color);
            color = LinearToGamma(color);
            return float4(color, 1);
        case 1:
            color = Tonemap_Lottes(color);
            color = LinearToGamma(color);
            return float4(color, 1);
        case 2:
            color = Tonemap_UnrealEngine(color);
            return float4(color, 1);
        case 3:
            color = LinearToGamma(color);
            return float4(color, 1);
        case 4:
            return float4(color, 1);
    }
    return 1;
}