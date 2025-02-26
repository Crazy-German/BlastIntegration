#pragma once
#include "Vector2.hpp"

namespace CommonUtilities
{
	template <class T>
	class Line
	{
		public:
			// Default constructor: there is no line, the normal is the zero vector.
			Line();

			// Copy constructor.
			Line(const Line <T>& aLine);

			// Constructor that takes two points that define the line, the direction is aPoint1 - aPoint0.
			Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);

			// Init the line with two points, the same as the constructor above.
			void InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);

			// Init the line with a point and a direction.
			void InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T>& aDirection);

			// Returns whether a point is inside the line: it is inside when the point is on the line or on the side the normal is pointing away from.
			bool IsInsideMeshCell(const Vector2<T>& aPosition) const;

			// Returns the direction of the line.
			const Vector2<T>& GetDirection() const;

			// Returns the normal of the line, which is (-direction.y, direction.x).
			const Vector2<T> GetNormal() const;

		private:
			Vector2<T> myPoint;
			Vector2<T> myDirection;
	};


	template<class T>
	inline Line<T>::Line()
	{
		myPoint = Vector2<T>();
		myDirection = Vector2<T>();
	}

	template<class T>
	inline Line<T>::Line(const Line<T>& aLine)
	{
		myPoint = aLine.myPoint;
		myDirection = aLine.myDirection;
	}

	template<class T>
	inline Line<T>::Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
	{
		myPoint = aPoint0;
		myDirection = aPoint1 - aPoint0;
	}

	template<class T>
	inline void Line<T>::InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
	{
		myPoint = aPoint0;
		myDirection = aPoint1 - aPoint0;
	}

	template<class T>
	inline void Line<T>::InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T>& aDirection)
	{
		myPoint = aPoint;
		myDirection = aDirection;
	}

	template<class T>
	inline bool Line<T>::IsInsideMeshCell(const Vector2<T>& aPosition) const
	{
		if (GetNormal().Dot(aPosition - myPoint) <= 0)
		{
			return true;
		}
		return false;
	}

	template<class T>
	inline const Vector2<T>& Line<T>::GetDirection() const
	{
		return myDirection;
	}

	template<class T>
	inline const Vector2<T> Line<T>::GetNormal() const
	{
		return Vector2<T>(-myDirection.y, myDirection.x);
	}

}