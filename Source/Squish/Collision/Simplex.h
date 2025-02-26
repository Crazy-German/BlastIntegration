#pragma once
#include <array>

#include "CommonUtilities/Vector3.hpp"

#define MIN(a, b) ((a)<(b)?(a):(b))

using Vec3 = CommonUtilities::Vector3<float>;
struct Simplex
{
private:
	std::array<Vec3, 4> myPoints;
	int mySize;
public:
	Simplex() : mySize(0) {};

	Simplex& operator=(std::initializer_list<Vec3> aList)
	{
		mySize = 0;
		for (const Vec3& point : aList)
		{
			myPoints[mySize++] = point;
		}
		return *this;
	}
	void push_fornt(Vec3 aPoint)
	{
		if (!isnan(aPoint.x))
		{
			myPoints = { aPoint, myPoints[0], myPoints[1] , myPoints[2] };
			mySize = MIN(mySize + 1, 4);
		}
	}
	Vec3& operator[](int i) { return myPoints[i]; }
	size_t size() const { return mySize; }

	auto begin() const { return myPoints.begin(); }
	auto end() const { return myPoints.end() - (4 - mySize); }
};
