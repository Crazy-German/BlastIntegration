#pragma once
#include "lowlevel/NvBlast.h"

#include <vector>
#include <extensions/authoringCommon/NvBlastExtAuthoringTypes.h>

struct BlastMesh
{
    std::vector<Nvcvec>
};

class BlastAsset
{
public:
    BlastAsset();
    BlastAsset(const BlastAsset& aAsset);
    ~BlastAsset();

    void Hit();
    bool CreateAsset();
private:
    void AllocateFamily();
    void CreateActor();
    NvBlastAsset* myAsset;
    NvBlastActor* myFirstActor;
    NvBlastFamily* myFamily;
    NvBlastLog myLog;
};

void BlastLog(int type, const char* msg, const char* file, int line);
