struct DefaultVertex
{
    float4 Position : POSITION;
    float4 Color : COLOR;
    uint4 BoneIDs : BONEIDS;
    float4 SkinWeights : SKINWEIGHTS;
    float2 UV : UV;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct DefaultVStoPS
{
    float4 Position     : SV_POSITION;
    float4 Color        : COLOR;
    float2 UV           : UV;
    float3 Normal       : NORMAL;
    float3 Tangent      : TANGENT;
    float3 Binormal     : BINORMAL;
    float3 WorldPos     : WORLDPOS;
    int DebugMode       : DEBUGMODE;
};

struct UI_VS_to_PS
{
    float4 position : SV_Position;
    float4 tint : COLOR;
    float2 uv : TEXCOORD;
    
};

struct SpriteVertex
{
    float4 position : WORLDPOS;
    float4 color : COLOR;
    float4 uv : TEXCOORD;
};