cbuffer LightBuffer : register(b4)
{
    
    struct DirLightData
    {
        float3 Color;
        float Intensity;
        float3 LightPos;
        bool Active;
        float3 LightDir;
        int Padding;
        float4x4 LightViewInv;
        float4x4 LightProj;
    } DirLight;
    struct PointLightData
    {
        float3 Color;
        float Intensity;
        float3 Lightpos;
        bool CastShadows;
        float4x4 LightViewInv;
        float4x4 LightProj;
        bool Active;
        float Range;
        float2 Padding;
    } PointLights[8];
    
    struct SpotLightData
    {
        float3 Color;
        float Intensity;
        float3 Lightpos;
        bool CastShadows;
        float3 LightDir;
        float InnerConeAngle;
        float4x4 LightViewInv;
        float4x4 LightProj;
        bool Active;
        float Range;
        float OuterConeAngle;
        float Padding;
    } SpotLights[8];
    
    int PointLightCount;
    int SpotLightCount;
    
    float2 L_Padding;

};