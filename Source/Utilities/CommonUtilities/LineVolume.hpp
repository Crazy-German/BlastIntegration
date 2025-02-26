#pragma once
#include "Line.hpp"
#include <vector>

namespace CommonUtilities
{
	template <class T>
	class LineVolume
	{
		public:
			// Default constructor: empty LineVolume.
			LineVolume();

			// Constructor taking a list of Line that makes up the LineVolume.
			LineVolume(const std::vector<Line<T>>& aLineList);

			// Add a Line to the LineVolume.
			void AddLine(const Line<T>& aLine);

			// Returns whether a point is inside the LineVolume: it is inside when the point is
			// on the line or on the side the normal is pointing away from for all the lines in
			// the LineVolume.
			bool IsInsideMeshCell(const Vector2<T>& aPosition) const;

		private:
			std::vector<Line<T>> myConvexPolygon;
	};

	template<class T>
	inline LineVolume<T>::LineVolume()
	{
		myConvexPolygon = {};
	}

	template<class T>
	inline LineVolume<T>::LineVolume(const std::vector<Line<T>>& aLineList)
	{
		myConvexPolygon = aLineList;
	}

	template<class T>
	inline void LineVolume<T>::AddLine(const Line<T>& aLine)
	{
		myConvexPolygon.push_back(aLine);
	}

	template<class T>
	inline bool LineVolume<T>::IsInsideMeshCell(const Vector2<T>& aPosition) const
	{
		for (int i = 0; i < myConvexPolygon.size(); i++)
		{
			if (!myConvexPolygon[i].IsInsideMeshCell(aPosition))
			{
				return false;
			}
		}
		return true;
	}
}
