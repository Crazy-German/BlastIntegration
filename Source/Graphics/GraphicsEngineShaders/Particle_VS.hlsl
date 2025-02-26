#include "Buffers/ObjectBuffer.hlsli"
#include "Buffers/FrameBuffer.hlsli"

struct ParticleVertex
{
    float4 Position : POSITION;
    float4 Color : COLOR;
    float3 Velocity : VELOCITY;
    float Lifetime : LIFETIME;
    float Angle : ANGLE;
    float2 Size : SIZE;
    float4 ChannelMask : CHANNELMASK;
    uint2 Mirror : MIRROR;
};

struct ParticleVSToGS
{
    float4 Position : POSITION;
    float4 Color : COLOR;
    float3 Velocity : VELOCITY;
    float Lifetime : LIFETIME;
    float Angle : ANGLE;
    float2 Size : SIZE;
    float4 ChannelMask : CHANNELMASK;
    uint2 Mirror : MIRROR;
};


ParticleVSToGS main( ParticleVertex input)
{
    ParticleVSToGS result;
    
    float4 vertexPos = input.Position;
    //vertexPos.w = 1.0f;
    
    float4 vxWorldPos = mul(OB_World, vertexPos);
    
    //result.Position = vxWorldPos;
    
    float4 vxViewPos = mul(FB_InvView, vxWorldPos);
    //float4 vxProjPos = mul(FB_Projection, vxViewPos);
    
    
    result.Position = vxViewPos;
    result.Color = input.Color;
    result.Velocity = input.Velocity;
    result.Lifetime = input.Lifetime;
    result.Angle = input.Angle;
    result.Size = input.Size;
    result.ChannelMask = input.ChannelMask;
    result.Mirror = input.Mirror;
    return result;
}