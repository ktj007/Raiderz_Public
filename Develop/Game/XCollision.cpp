#include "stdafx.h"
#include "XCollision.h"
#include <math.h>

float XCollision::SweepTest(const MSphere& sphere, const vec3& vec, const MPlane& pln, MPlane* out)
{
	float d1, d2;

	d1 = DotProduct(vec, pln.Normal());

	if (d1 < 0.0f) 
	{
		d2 = pln.GetDistanceTo(sphere.center) - sphere.radius;

		if (d2 >= 0 && d2 < -d1) 
		{
			float t = d2 / (-d1);

			if (out) 
			{
				*out = MPlane(pln.Normal(), pln.d-sphere.radius);
			}
			return t;
		}
	}
	return 1.0f;	
}

float XCollision::SweepTest(const MSphere& sphere, const vec3& vec, const MBox& bound, MPlane* out)
{
	float t, pt;
	float p, v;
	int i, j;

	for(i=0; i<3; i++) 
	{
		p = ((float*)&sphere.center.x)[i];
		v = vec.v[i];

		if (v < 0) 
		{
			//	|         <---|-----*
			//  b0'       p+v b1'   p
			if (p + v >= bound.m[1][i] + sphere.radius)
				return 1.0f;

			if (p < bound.m[1][i] + sphere.radius)
				continue;

			t = ((bound.m[1][i] + sphere.radius) - p) / v; // ((p - bound.bound[1][i] + sphere.radius)) / -v
		}	
		else 
		{
			//	*-----|--->         |
			//  p     b0' p+v       b1'
			if (p + v <= bound.m[0][i] - sphere.radius)
				return 1.0f;

			if (p > bound.m[0][i] - sphere.radius)
				continue;

			t = ((bound.m[0][i] - sphere.radius) - p) / v;
		}

//		t 지점이 다른 평면안에 있나 ?
//
		for(j=0; j<3; j++) 
		{
			if (i != j) 
			{
				pt = ((float*)&sphere.center.x)[j] + vec.v[j] * t; 

				if ((pt > bound.m[1][j] + sphere.radius) || (pt < bound.m[0][j] - sphere.radius )) 
					break;
			}
		}

		if (j == 3) 
		{
			if (out) 
			{
				for(j=0; j<3; j++) 
				{
					if (i == j) 
					{
						((float*)&out->a)[j] = vec.v[j] < 0 ? 1.0f : -1.0f;
					}
					else 
					{
						((float*)&out->a)[j] = 0.0f;
					}
				}

				out->d = -((float*)&sphere.center.x)[i] + vec.v[i] * t;
			}

			return t;
		}
	}
	return 1.0f;
}

float XCollision::_RayIntersect(const vec3& pivot, const vec3& pos, const vec3& vec, float radius)
{
	vec3 dv;
	float dvl;

	dv = pivot - pos;
	dvl = dv.Length();

//	겹쳐 있으면 충돌체크가 불가능하다
//
	if (dvl < radius) return 1.0f;

	float dotp = DotProduct(dv, vec);

//	도달 불가능
//
	if (dotp / dvl < dvl-radius) return 1.0f;

//	무한히 가도 닿을 수 없는 위치
//
	float vecl = vec.Length();
	float l2 = dotp / vecl;
	float db = (float) sqrt(dvl*dvl - l2*l2);

	if (db > radius) return 1.0f;

//	충돌 지점의 거리 구하기
//
	float dc = (float) sqrt(radius*radius - db*db);
	float dd = l2 - dc;

	if (dd < vecl)
		return dd / vecl;

	return 1.0f;
}

float XCollision::SweepTest(const MSphere& sphere, const vec3& vec, const MSphere& body, MPlane* out)
{
	vec3 dv, p;
	float radius = sphere.radius + body.radius, t;
	int i;

//	먼저 러프하게 aabb 체크

	for(i=0; i<3; i++) 
	{
		if (vec.v[i] > 0.0f) 
		{
			if (((float*)&sphere.center.x)[i] - radius > ((float*)&body.center.x)[i] || ((float*)&sphere.center.x)[i] + vec.v[i] + radius < ((float*)&body.center.x)[i])
				return 1.0f;
		}
		else 
		{
			if (((float*)&sphere.center.x)[i] + radius < ((float*)&body.center.x)[i] || ((float*)&sphere.center.x)[i] + vec.v[i] - radius > ((float*)&body.center.x)[i])
				return 1.0f;
		}
	}

//	충돌체크

	t = _RayIntersect(body.center, sphere.center, vec, radius);

	if (t < 1.0f) 
	{
		if (out) 
		{
			vec3 norm = Normalize((sphere.center + vec * t) - body.center);
			*out = MPlane(norm, body.center + norm * body.radius);
		}
		return t;
	}
	return 1.0f;
}

float XCollision::SweepTest(const MSphere& sphere, const vec3& vec, const MCapsule&capsule, MPlane* out)
{
	//	먼저 러프하게 aabb 체크해야 좋다 !!
	// 이 함수는 캡슐 맞는 충돌이 계산이 안됩니다. 되는건... 실런더 모양 이므로 캡슐의 top과 bottom를 radius만큼 더하거나 빼야 합니다.
	// 캡슐 기울기를 생각해서 해야 합니다.
//	캡슐 조정 -----------------------------------------
	MCapsule calCapsule = capsule;
	vec3 vVelTop = (calCapsule.top - calCapsule.bottom).Normalize();
	vec3 vVelBottom = (calCapsule.bottom - calCapsule.top).Normalize();

	calCapsule.top += vVelTop * calCapsule.radius;
	calCapsule.bottom += vVelBottom * calCapsule.radius;
	calCapsule.CalcHeight();

//	충돌 불가능한 위치 체크 -----------------------------------------
	vec3 pivot = calCapsule.top;
	vec3 lerp = calCapsule.bottom - calCapsule.top;
	float t = 1.0f;

	const float d1 = DotProduct(lerp, vec);
	const float d2 = DotProduct(lerp, sphere.center - pivot);

	if (d1 < 0) 
	{
		// 1
		if (d2 < 0)
		{
			float dst = pivot.DistanceTo(sphere.center);
			if (dst >= sphere.radius+calCapsule.radius)
			{
				return 1.0f;
			}
		}
	}	
	else 
	{
		if (d2 > 0)
		{
			// 2
			float dst = pivot.DistanceTo(sphere.center);
			if (dst > (sphere.radius+calCapsule.radius+calCapsule.height))
			{
				return 1.0f;
			}
		}
	}



//	calCapsule.pivot 성분을 제거하여 calCapsule.pivot 을 지나고 calCapsule.pivot 과 직각인 평면위의 성분으로 만든다 --------
	vec3 lerpN = Normalize(lerp);

	vec3 p1 = sphere.center - (DotProduct(sphere.center - pivot, lerpN)) * lerpN;
	vec3 v1 = vec - (DotProduct(lerpN, vec)) * lerpN;

	//	p1, v1 을 이용하여 구의 직선의 충돌 체크를 한다
	//
	t = _RayIntersect(pivot, p1, v1, sphere.radius + calCapsule.radius);

	const float SAFE_GAP = 0.05f;
	if (t < SAFE_GAP) t = 0.0f;

	if (t < 1.0f) 
	{
		//		유효한 위치인가 ?
		//
		vec3 p = sphere.center + vec * t;
		float v = DotProduct(lerpN, p - pivot);

		vec3 pln_pos;

		if (v > 0 && v < calCapsule.height) 
		{
			if (out) 
			{
				vec3 norm = Normalize((p1 + v1 * t) - pivot);
				*out = MPlane(norm, p - norm * sphere.radius);
			}

			return t;
		}
	}


//	뚜껑과 비교 -----------------------------------
	if (d1 > 0)	// 윗뚜껑
	{ 
		if (d2 < 0) 
		{
			t = _RayIntersect(pivot, sphere.center, vec, sphere.radius + calCapsule.radius);

			if (t < 1.0f) 
			{
				if (out) 
				{
					vec3 norm = Normalize(sphere.center + vec * t - pivot);
					*out = MPlane(norm, pivot + norm * calCapsule.radius);
				}
				return t;
			}
		}
	}
	else 
	{
		if (d2 > calCapsule.height) 
		{
			t = _RayIntersect(calCapsule.bottom, sphere.center, vec, sphere.radius + calCapsule.radius);

			if (t < 1.0f) 
			{
				if (out) 
				{
					vec3 norm = Normalize(sphere.center + vec * t - calCapsule.bottom);
					*out = MPlane(norm, calCapsule.bottom + norm * calCapsule.radius);
				}
				return t;
			}
		}	
	}

	return 1.0f;
}

bool XCollision::ContactTest(const MBox& aabb1, const MBox& aabb2)
{
	for(int i=0; i<3; i++)
	{
		if (aabb1.m[0][i] > aabb2.m[1][i] || aabb1.m[1][i] < aabb2.m[0][i])
			return false;
	}
	return true;
}

float IntersectTriTest(const MSphere& sphere, const vec3& vec, const vec3& p1, const vec3& p2, const vec3& p3, MPlane* out)
{
//	sphere.radius 가 0인 것만 취급한다.
//
	vec3 vu = p2 - p1;
	vec3 vv = p3 - p1;
	vec3 norm = CrossProduct(vu, vv);

//	같은 방향이면 무효
//
	float d1 = DotProduct(vec, norm);

	if (d1 >= 0.0f)
		return 1.0f;

//	충돌지점은
//
	MPlane pln = MPlane(norm, p1);
	float d2 = pln.GetDistanceTo(sphere.center);

//	평면상으로는 충돌했다.
//
	if (d2 > 0 && d1 < d2) 
	{
		float t = d1 / d2;
		vec3 p = sphere.center + vec * t;
		float u = DotProduct(vu, p);

		if (u < 0.0f || u > 1.0f) return 1.0f;

		float v = DotProduct(vv, p);

		if (v < 0.0f || u + v > 1.0f) return 1.0f;

		if (out) *out = pln;

		return t;
	}

	return 1.0f;
}