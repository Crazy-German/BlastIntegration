#include "ScopedTimer.h"

CommonUtilities::ScopedTimer::ScopedTimer()
{
	myStartTime = std::chrono::high_resolution_clock::now();
	myDebugName = "";
}

CommonUtilities::ScopedTimer::ScopedTimer(const char* aDebugName)
{
	myStartTime = std::chrono::high_resolution_clock::now();
	myDebugName = aDebugName;
}

CommonUtilities::ScopedTimer::~ScopedTimer()
{
	std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
	double timing = std::chrono::duration_cast<std::chrono::duration<double>>(now - myStartTime).count();

	std::cout << myDebugName << " time spent: " << timing << "\n";
}
