#include "Includes/DefaultShaderIncludes.hlsli"
#include "BRDF_Functions.hlsli"
#include "Buffers/ObjectBuffer.hlsli"
#include "Buffers/FrameBuffer.hlsli"

UI_VS_to_PS main(SpriteVertex vertex) 
{
    UI_VS_to_PS result;
    
    float4 skibidiposition = mul(OB_World, vertex.position);
    float2 NDC_Position = skibidiposition.xy / FB_Resolution;
    NDC_Position *= 2.0f;
    NDC_Position.x = NDC_Position.x - 1.0f;
    NDC_Position.y = 1.0f - NDC_Position.y;
    const float w = 1.0f; // No Scale on screen
    
    //NDC_Position.y = NDC_Position.y * 2.f;
    result.position = float4(NDC_Position, vertex.position.z, w);
    
    //float4 tempPosition = float4(NDC_Position, vertex.position.z, w);
    //float4 fentcart = float4(OB_World._11_12_13_14);
    //result.position = (fentcart + tempPosition);
    result.uv = vertex.uv.xy;
    result.tint = vertex.color;
    return result;
}
