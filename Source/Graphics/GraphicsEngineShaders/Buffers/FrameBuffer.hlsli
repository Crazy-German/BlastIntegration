cbuffer FrameBuffer : register(b0)
{
    float4x4 FB_InvView;
    float4x4 FB_Projection;

    float3 FB_CameraPos;
    int FB_RenderPass;

    float2 FB_Resolution;
    float FB_TotalTime;
    float FB_DeltaTime;

    float FB_FogMultiplier;
    float3 padding;
}