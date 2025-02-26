#include "Buffers/FrameBuffer.hlsli"
#include "Includes/DefaultShaderIncludes.hlsli"
#include "BRDF_Functions.hlsli"


float4 main(DefaultVStoPS input) : SV_TARGET
{
    float3 toView = float3(normalize(FB_CameraPos.xyz - input.WorldPos.xyz));
    
    toView = float3(toView.x, -toView.y, toView.z);
    
    float4 cubeView = EnvCubeTexture.Sample(defaultSampler, toView);
    
    return cubeView;
}