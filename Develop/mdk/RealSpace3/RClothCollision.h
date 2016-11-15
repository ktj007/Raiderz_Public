#pragma once

#include "RTypes.h"

namespace rs3
{
	class RClothCollision
	{
	public:
		static bool PlaneConstraint(rs3::RVector &v,const rs3::RVector &n,float d);
		static bool SphereConstraint(const rs3::RVector &v0,rs3::RVector& v1,rs3::RVector& v2,const rs3::RVector& center,float r);
		static void CapsuleConstraint(const rs3::RVector &v0,rs3::RVector& v1,rs3::RVector& v2,const rs3::RVector &endPoint,
			const rs3::RVector &n,float length,float r);

		static bool SphereConstraint(const rs3::RVector &v0,rs3::RVector& v1,const rs3::RVector& center,float r);
		static void CapsuleConstraint(const rs3::RVector &v0,rs3::RVector& v1,const rs3::RVector &endPoint,
			const rs3::RVector &n,float length,float r);

		static inline void Clamp(float &val,float min,float max)
		{
			if( val < min )
			{
				val = min;
			}
			else if( val > max )
			{
				val = max;
			}
		}
	};
}