#pragma once

#include <unordered_map>
#include <vector>
#include <toolkit/NvBlastTkActor.h>

#include "NvCTypes.h"
#include "CommonUtilities/Vector2.hpp"
#include "CommonUtilities/Vector3.hpp"
#include "shared/NvFoundation/NvCTypes.h"
#include "toolkit/NvBlastTkEvent.h"

namespace physx
{
	class PxFixedJoint;
	class PxRigidActor;
	class PxConvexMeshGeometry;
}

namespace Nv::Blast
{
	struct AuthoringResult;
	class FractureTool;
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

class BlastAsset : public Nv::Blast::TkEventListener
{
public:
    BlastAsset();
    BlastAsset(const BlastAsset& aAsset);
    ~BlastAsset();

    void Hit();
    bool CreateAsset(const std::vector<CommonUtilities::Vector3f>& aPosData, const std::vector<CommonUtilities::Vector3f>& aNormData, const
                     std::vector<CommonUtilities::Vector2f>& aUvData, const std::vector<uint32_t>& aIndicies, unsigned aNrOfPieces = 4);

    BlastMesh GetRenderData();
	Nv::Blast::TkActor* GetActor();

    void receive(const Nv::Blast::TkEvent* events, uint32_t eventCount) override;
private:
    void FinalizeAuthoring(int32_t defaultSupportDepth = -1);

    void Split(const Nv::Blast::TkSplitEvent& aSplit);

    Nv::Blast::FractureTool* myFractureTool;
    Nv::Blast::AuthoringResult* myAuthoringResult;

    const Nv::Blast::TkAsset* myAsset;
    Nv::Blast::TkActor* myActor;
    std::unordered_map<uint32_t,physx::PxRigidActor*> myPhysXActors;
	std::unordered_map<uint32_t, physx::PxFixedJoint*> myPhysxJoints;
};

void BlastLog(int type, const char* msg, const char* file, int line);
