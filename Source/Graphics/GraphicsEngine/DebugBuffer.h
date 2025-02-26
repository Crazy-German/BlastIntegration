#pragma once
struct DebugBuffer
{
    float Exposure;
    int Tonemap;
    int BloomMode;
    int LuminanceMode;

	float CubemapInfluence;
    CU::Vector3f padding;
};