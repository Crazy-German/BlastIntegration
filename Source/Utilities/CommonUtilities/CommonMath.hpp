#pragma once
#include <random>

#define FLOAT_EQUAL(a, b) ((abs(a - b) < 0.00001) ? true : false)
#define PI 3.14159

namespace CommonUtilities
{
	template <typename T>
	struct Interval
	{
		T min;
		T max;
	};

	template <typename T>
	bool AxisOverlap(const Interval<T>& intervalShape1, const Interval<T>& intervalShape2);

	template<typename T>
	T Clamp(const T& n, const T& lower, const T& upper);

	template<typename T>
	static T RandomInterval(T aMin, T aMax);

	template <typename T>
	T Min(const T aFirstValue, const T aSecondValue)
	{
		if (aFirstValue < aSecondValue)
		{
			return aFirstValue;
		}
		else if (aSecondValue < aFirstValue)
		{
			return aSecondValue;
		}
		else
		{
			return 0;
		}

	}


	template <typename T>
	T Max(const T aFirstValue, const T aSecondValue)
	{
		if (aFirstValue > aSecondValue)
		{
			return aFirstValue;
		}
		else if (aSecondValue > aFirstValue)
		{
			return aSecondValue;
		}
		else
		{
			return 0;
		}
	}

	template <typename T>
	T Abs(const T aValue)
	{
		if (aValue < 0)
		{
			return (aValue * -1);
		}
		else
		{
			return aValue;
		}
	}
}

template<typename T>
bool CommonUtilities::AxisOverlap(const Interval<T>& intervalShape1, const Interval<T>& intervalShape2)
{
	return (intervalShape2.min <= intervalShape1.max) && (intervalShape1.min <= intervalShape2.max);
}

template<typename T>
T CommonUtilities::Clamp(const T& n, const T& lower, const T& upper)
{
	return max(lower, min(n, upper));
}

template <typename T>
static T CommonUtilities::RandomInterval(T aMin, T aMax)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<T> distr(aMin, aMax);
	return distr(gen);
}
namespace CU = CommonUtilities;
