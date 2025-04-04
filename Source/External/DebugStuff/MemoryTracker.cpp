#define _CRT_SECURE_NO_WARNINGS
#include "MemoryTracker.h"

#include "StackTrace.h"
#include <algorithm>
#include <array>
#include <mutex>
#include <unordered_set>
#include <unordered_map>
#include <debugapi.h>

using namespace Tga;

#ifdef _DEBUG

struct AllocationEntry
{
	size_t mySize = 0;
	StackTrace myStackTrace;
};

static thread_local bool tIsAllocationInProgress = false;
static std::mutex locAllocationMapMutex;
static std::unordered_map<long, AllocationEntry> locAllocationMap;
static int locTotalAllocationCount = 0;
static std::unordered_map<StackTrace, int> loStackTraceToAllocationCount;
static MemoryTrackingSettings locMemoryTrackingSettings;

// from DBGINT.H:
#define nNoMansLandSize 4

typedef struct _CrtMemBlockHeader
{
	struct _CrtMemBlockHeader* pBlockHeaderNext;
	struct _CrtMemBlockHeader* pBlockHeaderPrev;
	char* szFileName;
	int                         nLine;
#ifdef _WIN64
	/* These items are reversed on Win64 to eliminate gaps in the struct
	 * and ensure that sizeof(struct)%16 == 0, so 16-byte alignment is
	 * maintained in the debug heap.
	 */
	int                         nBlockUse;
	size_t                      nDataSize;
#else  /* _WIN64 */
	size_t                      nDataSize;
	int                         nBlockUse;
#endif  /* _WIN64 */
	long                        lRequest;
	unsigned char               gap[nNoMansLandSize];
	/* followed by:
	 *  unsigned char           data[nDataSize];
	 *  unsigned char           anotherGap[nNoMansLandSize];
	 */
} _CrtMemBlockHeader;
// end DBGINT.H:

int AllocHook(
	int allocType,
	void* userData,
	size_t size,
	int blockType,
	long requestNumber,
	const unsigned char*,
	int)
{
	if (blockType == _CRT_BLOCK || tIsAllocationInProgress)
		return true;

	// Do not track allocations by the memory tracker itself
	tIsAllocationInProgress = true;

	std::lock_guard<std::mutex> guard(locAllocationMapMutex);

	bool alloc = false;
	bool free = false;
	switch (allocType)
	{
	case _HOOK_ALLOC:
	{
		alloc = true;
		break;
	}
	case _HOOK_REALLOC:
	{
		alloc = true;
		free = true;
		break;
	}
	case _HOOK_FREE:
	{
		free = true;
		break;
	}
	}

	if (alloc)
	{
		AllocationEntry& entry = locAllocationMap[requestNumber];

		if (locMemoryTrackingSettings.myShouldStoreStackTraces)
		{	
			entry.myStackTrace = StackTrace::CaptureStackTrace(1);
			loStackTraceToAllocationCount[entry.myStackTrace]++;
		}

		entry.mySize = size;
		locTotalAllocationCount++;
	}

	if (free)
	{
		// read request number from header
		size_t headerSize = sizeof(_CrtMemBlockHeader);
		_CrtMemBlockHeader* pHead;
		size_t ptr = (size_t)userData - headerSize;
		pHead = (_CrtMemBlockHeader*)(ptr);

		long freeRequestNumber = pHead->lRequest;

		locAllocationMap.erase(freeRequestNumber);
	}

	tIsAllocationInProgress = false;
	return true;
}

void Tga::StartMemoryTracking(const MemoryTrackingSettings& trackingSettings)
{
	locMemoryTrackingSettings = trackingSettings;
	_CrtSetAllocHook(&AllocHook);
}

void PrintTopLeaks(std::unordered_map<StackTrace, int>& map)
{
	std::vector<std::pair<StackTrace, int>> pairs;

	for (const auto& p : map)
	{
		pairs.push_back(p);
	}

	std::sort(pairs.begin(), pairs.end(), [](const auto& a, const auto& b) {return a.second > b.second; });

	OutputDebugStringA("================================================================================\n");
	int i = 0;
	for (const auto& p : pairs)
	{
		if (i >= 10)
			break;

		char buffer[100];
		sprintf_s(buffer, "Count: %d\n, Stack Trace:", p.second);
		OutputDebugStringA(buffer);
		p.first.Print();

		OutputDebugStringA("================================================================================\n");

		i++;
	}
}

void Tga::StopMemoryTrackingAndPrint()
{
	std::lock_guard<std::mutex> guard(locAllocationMapMutex);

	_CrtSetAllocHook(nullptr);

	if (locMemoryTrackingSettings.myShouldTrackAllAllocations)
	{
		OutputDebugStringA("================================================================================\n");
		char buffer[100];
		sprintf_s(buffer, "== Total Allocation Count: %d\n", locTotalAllocationCount);
		OutputDebugStringA(buffer);

		if (locMemoryTrackingSettings.myShouldStoreStackTraces)
		{
			OutputDebugStringA("== Top 10 Allocations: \n");
			PrintTopLeaks(loStackTraceToAllocationCount);
		}
		else
		{
			OutputDebugStringA("================================================================================\n");
		}
	}

	if (locAllocationMap.size() == 0)
	{
		OutputDebugStringA("================================================================================\n");
		OutputDebugStringA("== No memory leaks found! \n");
		OutputDebugStringA("================================================================================\n");
	}
	else
	{
		OutputDebugStringA("================================================================================\n");
		char buffer[100];
		sprintf_s(buffer, "== Total Number of Memory Leaks: %d\n", (int)locAllocationMap.size());
		OutputDebugStringA(buffer);

		if (locMemoryTrackingSettings.myShouldStoreStackTraces)
		{
			OutputDebugStringA("================================================================================\n");
			std::unordered_map<StackTrace, int> stackTraceToLeakCountMap;

			for (const auto& p : locAllocationMap)
			{
				stackTraceToLeakCountMap[p.second.myStackTrace]++;
			}

			OutputDebugStringA("== Top 10 Leaks: \n");
			PrintTopLeaks(stackTraceToLeakCountMap);
		}
		else
		{
			OutputDebugStringA("================================================================================\n");
			OutputDebugStringA("== For more details enable DebugFeature::MemoryTrackingStackTraces\n");
			OutputDebugStringA("== in EngineCreateParameters::myActivateDebugSystems\n");
			OutputDebugStringA("== when calling Tga::Engine::GetInstance()->Start(...)\n");
			OutputDebugStringA("================================================================================\n");
		}
	}

	locAllocationMap.clear();
}

#else // #ifdef _DEBUG

void Tga::StartMemoryTracking(const MemoryTrackingSettings&) {}
void Tga::StopMemoryTrackingAndPrint() {}

#endif // #ifdef _DEBUG