#pragma once
#include <chrono>
#include <iostream>

namespace CommonUtilities
{
	class ScopedTimer
	{
	public:
		//Declare inside a scope. Will cout the spent time when destroyed.
		ScopedTimer();
		//Declare inside a scope. Will cout the debugname + the spent time when destroyed.
		ScopedTimer(const char* aDebugName);
		~ScopedTimer();

	private:
		std::chrono::high_resolution_clock::time_point myStartTime;
		const char* myDebugName;
	};

	//ScopedTimer::ScopedTimer()
	//{
	//	myStartTime = std::chrono::high_resolution_clock::now();
	//}
	//
	//ScopedTimer::ScopedTimer(const char* aDebugName)
	//{
	//	myStartTime = std::chrono::high_resolution_clock::now();
	//}
	//
	//ScopedTimer::~ScopedTimer()
	//{
	//	std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
	//	float timing = std::chrono::duration_cast<std::chrono::duration<float>>(now - myStartTime).count();
	//}
}
