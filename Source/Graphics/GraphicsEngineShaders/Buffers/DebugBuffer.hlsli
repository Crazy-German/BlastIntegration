cbuffer DebugBuffer : register(b10)
{
    float DB_Exposure;
    int DB_Tonemap;
    int DB_BloomMode;
    int DB_LuminanceMode;

    float DB_CubemapInfluence;
    float3 DB_Padding;
};