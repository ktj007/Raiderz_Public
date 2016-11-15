#ifndef _MCAPSULE_H
#define _MCAPSULE_H

#include "RLib.h"
#include "MVector3.h"
#include "MMatrix.h"
#include "MOBox.h"

class CML2_API MCapsule
{
public:
		MVector3	bottom;
		MVector3	top;
		float		height;
		float		radius;

		MCapsule() : bottom(MVector3::ZERO), top(MVector3::ZERO), height(0), radius(0) {}
		MCapsule(const MVector3& b, const MVector3& t, float r) : bottom(b), top(t), radius(r) { height = bottom.DistanceTo(top); }
		MCapsule(const MVector3& b, const MVector3& dir, float r, float h) : bottom(b), height(h), radius(r) { top = bottom + dir * h; }

		bool		CollisionSphere(const MVector3& center, float rad) const;
		bool		CollisionCapsule(const MCapsule& capsule) const;
		bool		CollisionSegment(const MVector3& a, const MVector3& b) const;	//a-b ¼±ºÐ°ú Ä¸½¶ÀÇ Ãæµ¹.
		float		GetDistanceToSegment(const MVector3& a, const MVector3& b) const;
		bool		IntersectRay(const MVector3& rayOrigin, const MVector3& rayDir, float &outDistance) const;

		MCapsule	Multiply(const MMatrix& m) const;
		void		Scale(float f);

		void		SetHeight(float h);
		void		CalcHeight();

		MVector3	GetDir() const;
		MVector3	GetCenter() const;

		void makeOBB(MOBox *pOBB); 
};


#endif	// _MCAPSULE_H