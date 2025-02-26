//Gilber-Johnson-Keerhit Algorithm
//Some form of EPA or check for how shapes collided will be added
#pragma once
#include "Collider.h"
#include "Simplex.h"

Vec3 Support(const Collider& aFirstCollider, const Collider& aSecondCollider, Vec3 aDirection); // returns a vertex on minkowski diffrence

bool Line(Simplex& aSimplex, Vec3& aDir);
bool Triangle(Simplex& aSimplex, Vec3& aDir);
bool Tetrahedron(Simplex& aSimplex, Vec3& aDir);

bool NextSimplex(Simplex& aSimplex, Vec3& aDirection);
bool SameDirection(const Vec3& aFirstDir, const Vec3& aSecondDir);

bool GJK(const Collider& aFirstCollider, const Collider& aSecondCollider);
bool GJK(const Collider& aFirstCollider, const Collider& aSecondCollider, Simplex& aSimplexOut);