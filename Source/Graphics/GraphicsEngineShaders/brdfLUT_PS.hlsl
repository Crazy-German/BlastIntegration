#include "Includes/QuadShaderIncludes.hlsli"
#include "BRDF_Functions.hlsli"

float2 main(QuadVStoPS input) : SV_TARGET
{
    float2 integratedBRDF = IntegrateBRDF(input.UV.x, input.UV.y);
    return integratedBRDF;
}
