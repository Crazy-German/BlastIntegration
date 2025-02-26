#pragma once
#include "Plane.hpp"
#include <vector>

namespace CommonUtilities
{
	template <class T>
	class PlaneVolume
	{
		public:
			// Default constructor: empty PlaneVolume.
			PlaneVolume();

			// Constructor taking a list of Plane that makes up the PlaneVolume.
			PlaneVolume(const std::vector<Plane<T>>& aPlaneList);

			// Add a Plane to the PlaneVolume.
			void AddPlane(const Plane<T>& aPlane);

			const std::vector<Plane<T>>& GetPlanes() const { return myConvexPolyhedron; }

			// Returns whether a point is inside the PlaneVolume: it is inside when the point is on the
			// plane or on the side the normal is pointing away from for all the planes in the PlaneVolume.
			bool IsInsideMeshCell(const Vector3<T>& aPosition) const;

			void RemoveAllPlanes();

		private:
			std::vector<Plane<T>> myConvexPolyhedron;

	};

	template<class T>
	inline PlaneVolume<T>::PlaneVolume()
	{
		myConvexPolyhedron = {};
	}

	template<class T>
	inline PlaneVolume<T>::PlaneVolume(const std::vector<Plane<T>>& aPlaneList)
	{
		myConvexPolyhedron = aPlaneList;
	}

	template<class T>
	inline void PlaneVolume<T>::AddPlane(const Plane<T>& aPlane)
	{
		myConvexPolyhedron.push_back(aPlane);
	}

	template<class T>
	inline bool PlaneVolume<T>::IsInsideMeshCell(const Vector3<T>& aPosition) const
	{
		for (int i = 0; i < myConvexPolyhedron.size(); i++)
		{
			if (!myConvexPolyhedron[i].IsInside(aPosition))
			{
				return false;
			}
		}
		return true;
	}
	template<class T>
	inline void PlaneVolume<T>::RemoveAllPlanes()
	{
		myConvexPolyhedron.clear();
	}
}