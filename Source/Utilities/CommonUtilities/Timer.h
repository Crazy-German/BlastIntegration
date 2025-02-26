#pragma once
#include <chrono>

namespace CommonUtilities
{
	class Timer
	{
	public:
		Timer(const Timer& aTimer) = delete;
		Timer& operator=(const Timer& aTimer) = delete;
		void Update();
		float GetDeltaTime() const;
		double GetTotalTime() const;
		static Timer& Get();
	private:
		Timer();
		float myDeltaTime;
		double myTotalTime;
		const std::chrono::steady_clock::time_point myStartTime;
		std::chrono::steady_clock::time_point myPrevUpdateTime;
	};
}
