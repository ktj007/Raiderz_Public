#ifndef _XCOLLISION_H_
#define _XCOLLISION_H_

#include "MMath.h"


class XCollision
{
private:
	static float _RayIntersect(const vec3& pivot, const vec3& pos, const vec3& vec, float radius);
	static float _SphereToSphere(const vec3& vSphereCenter1, const vec3& vSph);

public:
	static float SweepTest(const MSphere& sphere, const vec3& vec, const MPlane& pln, MPlane* out = NULL);
	static float SweepTest(const MSphere& sphere, const vec3& vec, const MBox& bound, MPlane* out = NULL);
	static float SweepTest(const MSphere& sphere, const vec3& vec, const MSphere& body, MPlane* out = NULL);
	static float SweepTest(const MSphere& sphere, const vec3& vec, const MCapsule& capsule, MPlane* out = NULL);

	static float IntersectTriTest(const MSphere& sphere, const vec3& vec, const vec3& p1, const vec3& p2, const vec3& p3, MPlane* out=NULL);
	static bool ContactTest(const MBox& ab1, const MBox& ab2);
};




#endif	// _XCOLLISION_H_