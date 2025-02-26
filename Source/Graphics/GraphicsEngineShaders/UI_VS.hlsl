#include "Includes/DefaultShaderIncludes.hlsli"
#include "Buffers/FrameBuffer.hlsli"


UI_VS_to_PS main(SpriteVertex vertex)
{
    UI_VS_to_PS result;
    float2 NDC_Position = vertex.position.xy / FB_Resolution;
    NDC_Position *= 2.0f;
    NDC_Position.x = NDC_Position.x - 1.0f;
    NDC_Position.y = 1.0f - NDC_Position.y;
    const float w = 1.0f; // No Scale on screen
    
    //NDC_Position.y = NDC_Position.y * 2.f;
    result.position = float4(NDC_Position, vertex.position.z, w);
    result.uv = vertex.uv.xy;
    result.tint = vertex.color;
    return result;
}