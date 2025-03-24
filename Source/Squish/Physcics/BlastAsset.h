#pragma once

#include <unordered_map>
#include <vector>
#include <toolkit/NvBlastTkActor.h>

#include "shared/NvFoundation/NvCTypes.h"
#include "CommonUtilities/Vector2.hpp"
#include "CommonUtilities/Vector3.hpp"
#include "foundation/PxVec3.h"
#include "shared/NvFoundation/NvCTypes.h"
#include "toolkit/NvBlastTkEvent.h"

class TransformBase;
struct GeometryData;

namespace physx
{
	class PxGeometry;
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

static uint32_t GetIndex();

class BlastAsset : public Nv::Blast::TkEventListener
{
public:
    BlastAsset();
    BlastAsset(const BlastAsset& aAsset);
    ~BlastAsset();
    //Radial damage
    void Hit(physx::PxVec3 aWorldPosition, float aDamageVal, float aMinRadius, float aMaxRadius, uint32_t aIndex, physx::PxVec3 aImpulse);

    bool CreateAsset(const std::vector<CommonUtilities::Vector3f>& aPosData, const std::vector<CommonUtilities::Vector3f>& aNormData, const
                     std::vector<CommonUtilities::Vector2f>& aUvData, const std::vector<uint32_t>& aIndicies, unsigned aNrOfPieces = 4);

    BlastMesh GetRenderData();
	Nv::Blast::TkActor* GetActor();

    void receive(const Nv::Blast::TkEvent* events, uint32_t eventCount) override;
    void SetPosition(const CommonUtilities::Vector3f aPos);
private:
    void FinalizeAuthoring(int32_t defaultSupportDepth = -1);

    void Split(const Nv::Blast::TkSplitEvent& aSplit);

    Nv::Blast::FractureTool* myFractureTool;
    Nv::Blast::AuthoringResult* myAuthoringResult;

    const Nv::Blast::TkAsset* myAsset;
    Nv::Blast::TkActor* myActor;
    std::unordered_map<uint32_t,physx::PxRigidActor*> myPhysXActors;
    std::unordered_map<uint32_t,Nv::Blast::TkActor*> myBlastActors;
    Nv::Blast::TkFamily* myFamily;
    GeometryData** myGeometryData;
	std::unordered_map<uint32_t, physx::PxFixedJoint*> myPhysxJoints;
    TransformBase* myTransform;
    uint32_t myIndex;
};

void BlastLog(int type, const char* msg, const char* file, int line);
