#pragma once

#include <vector>
#include <toolkit/NvBlastTkActor.h>

#include "NvCTypes.h"
#include "CommonUtilities/Vector2.hpp"
#include "CommonUtilities/Vector3.hpp"
#include "shared/NvFoundation/NvCTypes.h"

namespace Nv::Blast
{
	struct AuthoringResult;
	class FractureTool;
}

namespace physx
{
	class PxConvexMeshGeometry;
}

namespace Squish
{
	class RigidBody;
}

struct BlastMesh
{
    std::vector<NvcVec3> pos;
    std::vector<NvcVec3> norm;
    std::vector<NvcVec2> uv;
    std::vector<uint32_t> indicies;
};

class BlastAsset
{
public:
    BlastAsset();
    BlastAsset(const BlastAsset& aAsset);
    ~BlastAsset();

    void Hit();
    bool CreateAsset(const std::vector<CommonUtilities::Vector3f>& aPosData, const std::vector<CommonUtilities::Vector3f>& aNormData, const
                     std::vector<CommonUtilities::Vector2f>& aUvData, const std::vector<uint32_t>& aIndicies, unsigned aNrOfPieces = 4);

    BlastMesh GetRenderData();
private:
    void FinalizeAuthoring(int32_t defaultSupportDepth = -1);
    physx::PxConvexMeshGeometry CreateChunkShape();
    Nv::Blast::FractureTool* myFractureTool;
    Nv::Blast::AuthoringResult* myAuthoringResult;
    Nv::Blast::TkAsset* myAsset;
    Nv::Blast::TkActor* myActor;
};

void BlastLog(int type, const char* msg, const char* file, int line);
