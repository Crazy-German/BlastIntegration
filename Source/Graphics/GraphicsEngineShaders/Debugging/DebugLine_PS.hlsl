#include "../Includes/DefaultShaderIncludes.hlsli"

float4 main(DefaultVStoPS pixel) : SV_TARGET
{
    float4 result;
    result.rgba = pixel.Color;
	return result;
}