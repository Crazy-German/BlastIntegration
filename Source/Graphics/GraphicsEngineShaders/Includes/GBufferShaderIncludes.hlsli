struct GBufferOutput
{
    float4 Albedo           : SV_Target0;
    float4 WorldNormal      : SV_Target1;
    float4 Material         : SV_Target2;
    float4 WorldPosition    : SV_Target3;
    float4 Effects          : SV_Target4;
};