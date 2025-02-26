#include "pch.h"
#include "Timer.h"

CommonUtilities::Timer::Timer()
	:myStartTime(std::chrono::high_resolution_clock::now())
{
	myPrevUpdateTime = myStartTime;
	myDeltaTime = 0;
	myTotalTime = 0;
}

void CommonUtilities::Timer::Update()
{
	myDeltaTime = std::chrono::duration_cast<std::chrono::duration<float>> (std::chrono::high_resolution_clock::now() - myPrevUpdateTime).count();
	myTotalTime = std::chrono::duration_cast<std::chrono::duration<double>> (std::chrono::high_resolution_clock::now() - myStartTime).count();
	myPrevUpdateTime = std::chrono::high_resolution_clock::now();
}

float CommonUtilities::Timer::GetDeltaTime() const
{
	return myDeltaTime;
}

double CommonUtilities::Timer::GetTotalTime() const
{
	return myTotalTime;
}

CommonUtilities::Timer& CommonUtilities::Timer::Get()
{
	static Timer myTimer;
	return myTimer;
}

