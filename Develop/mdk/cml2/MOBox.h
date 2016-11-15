#ifndef _MOBOX_H_
#define _MOBOX_H_

/**
  Oriented Bounding Box
  : 미완성 클래스 ...
 */

#include "MTypes.h"
#include "MVector3.h"
#include <float.h>
#include <vector>

class MMatrix;
class MBox;	// AABB
class CML2_API MOBox	// OBB
{
public:
	MOBox()
		: vRange(MVector3::ZERO)
	{
	}

	MOBox(const MMatrix &_transform);
	MOBox(const MVector3 &_xaxis, const MVector3 &_yaxis, const MVector3 &_zaxis, const MVector3 &_range, const MVector3 &_position);


	~MOBox()
	{
	}

	bool Intersect(const MBox &aabb);
	bool Intersect(MOBox &rhs);
	static bool intersect(MOBox &a, MOBox &b);
	bool IsOut(MVector3& v);

	MVector3 vAxis[3];
	MVector3 vRange;
	MVector3 vPosition;
};

#endif