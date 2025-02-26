#include "GJK.h"

bool SameDirection(const Vec3& aFirstDir, const Vec3& aSecondDir)
{
	return aFirstDir.Dot(aSecondDir) > 0;
}

Vec3 Support(const Collider& aFirstCollider, const Collider& aSecondCollider, Vec3 aDirection)
{
	return aFirstCollider.FindFurthestPoint(aDirection) - aSecondCollider.FindFurthestPoint(-aDirection);
}

bool Line(Simplex& aSimplex, Vec3& aDir)
{
	Vec3 firstPoint = aSimplex[0];
	Vec3 secondPoint = aSimplex[1];

	Vec3 dif = secondPoint - firstPoint;
	Vec3 toOrigin = -firstPoint;
	if (SameDirection(dif, toOrigin))
	{
		aDir = (dif.Cross(toOrigin)).Cross(dif);
	}
	else
	{
		aSimplex = { firstPoint };
		aDir = toOrigin;
	}
	return false;
}

bool Triangle(Simplex& aSimplex, Vec3& aDir)
{
	Vec3 aPoint1 = aSimplex[0];
	Vec3 aPoint2 = aSimplex[1];
	Vec3 aPoint3 = aSimplex[2];

	Vec3 P1tP2 = aPoint2 - aPoint1;
	Vec3 P1tP3 = aPoint3 - aPoint1;
	Vec3 toOrigin = -aPoint1;

	Vec3 combined = P1tP2.Cross(P1tP3);

	if (SameDirection(combined.Cross(P1tP3), toOrigin))
	{
		if(SameDirection(P1tP3, toOrigin))
		{
			aSimplex = { aPoint1, aPoint3 };
			aDir = P1tP3.Cross(toOrigin).Cross(P1tP3);
		}
		else
		{
			return Line(aSimplex = { aPoint1, aPoint2 }, aDir);
		}
	}
	else
	{
		if(SameDirection(P1tP2.Cross(combined), toOrigin))
		{
			return Line(aSimplex = {aPoint1, aPoint2}, aDir);
		}
		else
		{
			if(SameDirection(combined, toOrigin))
			{
				aDir = combined;
			}
			else
			{
				aSimplex = { aPoint1, aPoint3, aPoint2 };
				aDir = -combined;
			}
		}
	}
	return false;
}

bool Tetrahedron(Simplex& aSimplex, Vec3& aDir)
{
	Vec3 aPoint1 = aSimplex[0];
	Vec3 aPoint2 = aSimplex[1];
	Vec3 aPoint3 = aSimplex[2];
	Vec3 aPoint4 = aSimplex[3];

	Vec3 P1tP2 = aPoint2 - aPoint1;
	Vec3 P1tP3 = aPoint3 - aPoint1;
	Vec3 P1tP4 = aPoint4 - aPoint1;
	Vec3 toOrigin = -aPoint1;

	Vec3 P1P2xP1P3 = P1tP2.Cross(P1tP3);
	Vec3 P1P3xP1P4 = P1tP3.Cross(P1tP4);
	Vec3 P1P4xP1P2 = P1tP4.Cross(P1tP2);
	if(SameDirection(P1P2xP1P3, toOrigin))
	{
		return Triangle(aSimplex = { aPoint1, aPoint2, aPoint3 }, aDir);
	}
	if(SameDirection(P1P3xP1P4, toOrigin))
	{
		return Triangle(aSimplex = { aPoint1, aPoint3, aPoint4 }, aDir);
	}
	if(SameDirection(P1P4xP1P2, toOrigin))
	{
		return Triangle(aSimplex = { aPoint1, aPoint4, aPoint2 }, aDir);
	}
	return true;
}

bool NextSimplex(Simplex& aSimplex, Vec3& aDirection)
{
	switch (aSimplex.size())
	{
	case 2:
	{
		return	Line(aSimplex, aDirection);
	}
	case 3:
	{
		return Triangle(aSimplex, aDirection);
	}
	case 4:
	{
		return Tetrahedron(aSimplex, aDirection);
	}
	default:
		break;
	}
	return false; // we have called this function when we are not supposed to
}

bool GJK(const Collider& aFirstCollider, const Collider& aSecondCollider)
{
	Vec3 support = Support(aFirstCollider, aSecondCollider, Vec3(1, 0, 0));

	Simplex collisionPoints;
	collisionPoints.push_fornt(support);
	Vec3 checkDirection = -support;
	while (true)
	{
		checkDirection.Normalize();
		support = Support(aFirstCollider, aSecondCollider, checkDirection);
		if (support.Dot(checkDirection) <= 0)
		{
			return false;
		}
		collisionPoints.push_fornt(support);
		if(NextSimplex(collisionPoints, checkDirection))
		{
			return true;
		}

	}
}

bool GJK(const Collider& aFirstCollider, const Collider& aSecondCollider, Simplex& aSimplexOut)
{
	Vec3 support = Support(aFirstCollider, aSecondCollider, Vec3(1, 0, 0));

	Simplex collisionPoints;
	collisionPoints.push_fornt(support);
	Vec3 checkDirection = -support;
	while (true)
	{
		support = Support(aFirstCollider, aSecondCollider, checkDirection);
		if (support.Dot(checkDirection) <= 0)
		{
			aSimplexOut = collisionPoints;
			return false;
		}
		collisionPoints.push_fornt(support);
		if(NextSimplex(collisionPoints, checkDirection))
		{
			aSimplexOut = collisionPoints;
			return true;
		}
	}
}
