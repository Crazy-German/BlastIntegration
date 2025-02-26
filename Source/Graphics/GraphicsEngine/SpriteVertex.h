#pragma once
#include "CommonUtilities/Vector.hpp"
#include <vector>
#include "RHIStructs.h"

struct SpriteVertex
{
    SpriteVertex(float aX, float aY, float aU, float aV)
    {
        position = CU::Vector4f(aX, aY, 1, 1);
        tint = CU::Vector4f(1, 1, 1, 1);
        uv = CU::Vector2<float>(aU, aV);
    }

    CU::Vector4f position;
    CU::Vector4f tint;
    CU::Vector2<float> uv;

    static const std::vector<VertexElementDesc> InputLayerDefinition;
};

