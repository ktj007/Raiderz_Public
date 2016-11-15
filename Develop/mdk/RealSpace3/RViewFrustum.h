#pragma once

#include "RTypes.h"

#define IS_SPECIAL(F)  ((FLT_AS_DW(F) & 0x7f800000L)==0x7f800000L)

namespace rs3 {

#define RV_LEFT		0
#define RV_RIGHT	1
#define RV_TOP		2
#define RV_BOTTOM	3
#define RV_NEAR		4
#define RV_FAR		5
#define RV_NUM		6

///////////////////////////////////////////////////////////////////////////
//  PlaneIntersection
//    computes the point where three planes intersect
//    returns whether or not the point exists.
inline bool PlaneIntersection( RVector* intersectPt, const RPlane* p0, const RPlane* p1, const RPlane* p2 )
{
	RVector n0( p0->a, p0->b, p0->c );
	RVector n1( p1->a, p1->b, p1->c );
	RVector n2( p2->a, p2->b, p2->c );

	RVector n1_n2, n2_n0, n0_n1;  

	n1_n2 = CrossProduct( n1, n2 );
	n2_n0 = CrossProduct( n2, n0 );
	n0_n1 = CrossProduct( n0, n1 );

	float cosTheta = DotProduct( n0, n1_n2 );

	if ( ALMOST_ZERO(cosTheta) || IS_SPECIAL(cosTheta) )
		return false;

	float secTheta = 1.f / cosTheta;

	n1_n2 = n1_n2 * p0->d;
	n2_n0 = n2_n0 * p1->d;
	n0_n1 = n0_n1 * p2->d;

	*intersectPt = -(n1_n2 + n2_n0 + n0_n1) * secTheta;
	return true;
}

//  this function tests if the projection of a bounding sphere along the light direction intersects
//  the view frustum 

inline bool SweptSpherePlaneIntersect(float& t0, float& t1, const RPlane& plane, const RSphere& sphere, const RVector& sweepDir)
{
	float b_dot_n = plane.GetDistanceTo(sphere.center);
	float d_dot_n = plane.DotNormal(sweepDir);

	if (d_dot_n == 0.f)
	{
		if (b_dot_n <= sphere.radius)
		{
			//  effectively infinity
			t0 = 0.f;
			t1 = 1e32f;
			return true;
		}
		else
			return false;
	}
	else
	{
		float tmp0 = ( sphere.radius - b_dot_n) / d_dot_n;
		float tmp1 = (-sphere.radius - b_dot_n) / d_dot_n;
		t0 = min(tmp0, tmp1);
		t1 = max(tmp0, tmp1);
		return true;
	}
}



class RS_API RViewFrustum
{
	RPlane	m_planes[6];
	RVector	m_pntList[8];

public:
	RViewFrustum(void);
	RViewFrustum( const RMatrix& matrix );

	virtual ~RViewFrustum(void);

	const RPlane* GetPlanes() const;
	const RVector* GetPoints() const;
	void SetFromMatrix(const RMatrix &tm);
	void SetPlane(int nIndex, const RPlane &plane);
	void MakePoints();

	bool Test(const RVector& vecPoint) const;
	bool TestBox(const RBox& aabb) const;
	bool TestSphere(const RSphere& sphere) const;
	bool TestSphereCorrect(const RSphere& sphere) const;
	bool TestSphereSwept(const RSphere& sphere,const RVector3 sweepDir) const;
};

inline RViewFrustum::RViewFrustum( const RMatrix& matrix )
{
	SetFromMatrix(matrix);
}

inline const RPlane* RViewFrustum::GetPlanes() const
{
	return m_planes;
}

inline const RVector* RViewFrustum::GetPoints() const
{
	return m_pntList;
}

inline void RViewFrustum::SetPlane(int nIndex, const RPlane &plane)
{
	m_planes[nIndex] = plane;
}

inline void RViewFrustum::MakePoints()
{
	for(int i=0;i<8;i++)
	{
		const RPlane& p0 = (i&1)?m_planes[4] : m_planes[5];
		const RPlane& p1 = (i&2)?m_planes[2] : m_planes[3];
		const RPlane& p2 = (i&4)?m_planes[0] : m_planes[1];

		PlaneIntersection( &m_pntList[i], &p0, &p1, &p2 );
	}
}


inline void RViewFrustum::SetFromMatrix(const RMatrix &tm)
{
	m_planes[RV_LEFT]	.SetPlane( tm._14+tm._11, tm._24+tm._21, tm._34+tm._31, tm._44+tm._41 );
	m_planes[RV_RIGHT]	.SetPlane( tm._14-tm._11, tm._24-tm._21, tm._34-tm._31, tm._44-tm._41 );
	m_planes[RV_TOP]	.SetPlane( tm._14-tm._12, tm._24-tm._22, tm._34-tm._32, tm._44-tm._42 );
	m_planes[RV_BOTTOM] .SetPlane( tm._14+tm._12, tm._24+tm._22, tm._34+tm._32, tm._44+tm._42 );
	m_planes[RV_NEAR]	.SetPlane( tm._14+tm._13, tm._24+tm._23, tm._34+tm._33, tm._44+tm._43 ); // from ati sdk
	m_planes[RV_FAR]	.SetPlane( tm._14-tm._13, tm._24-tm._23, tm._34-tm._33, tm._44-tm._43 );
	for(int i=0;i<6;i++)
		m_planes[i].Normalize();

	MakePoints();
}

// test point
inline bool RViewFrustum::Test(const RVector& vecPoint) const
{
	for(int i=0;i<6;i++)
	{
		if(m_planes[i].GetDistanceTo(vecPoint) < 0)
			return false;
	}
	return true;
}

inline bool RViewFrustum::TestBox(const RBox& aabb) const
{
	return aabb.isInViewFrustumWithZ(m_planes);
}

inline bool RViewFrustum::TestSphere(const RSphere& sphere) const
{
	for(int i=0;i<6;i++)
	{
		float t = m_planes[i].GetDistanceTo(sphere.center); 
		if(t < -sphere.radius)
			return false;
	}
	return true;
}


inline bool RViewFrustum::TestSphereCorrect(const RSphere& sphere) const
{
	for(int i=0;i<6;i++)
	{
		if(m_planes[i].GetDistanceTo(sphere.center) < -sphere.radius)
			return false;
	}

	RVector a, b;

	for (int i=0; i<7; i+=2)
	{
		a = m_pntList[i];
		b = m_pntList[i+1];

		if (MMath::GetDistancePointLineSegment(sphere.center, a, b) < sphere.radius)
			return true;
	}

	a = m_pntList[0];
	b = m_pntList[4];
	if (MMath::GetDistancePointLineSegment(sphere.center, a, b) < sphere.radius)
		return true;

	a = m_pntList[2];
	b = m_pntList[6];
	if (MMath::GetDistancePointLineSegment(sphere.center, a, b) < sphere.radius)
		return true;

	a = m_pntList[0];
	b = m_pntList[2];
	if (MMath::GetDistancePointLineSegment(sphere.center, a, b) < sphere.radius)
		return true;

	a = m_pntList[4];
	b = m_pntList[6];
	if (MMath::GetDistancePointLineSegment(sphere.center, a, b) < sphere.radius)
		return true;

	a = m_pntList[1];
	b = m_pntList[3];
	if (MMath::GetDistancePointLineSegment(sphere.center, a, b) < sphere.radius)
		return true;

	a = m_pntList[3];
	b = m_pntList[7];
	if (MMath::GetDistancePointLineSegment(sphere.center, a, b) < sphere.radius)
		return true;

	a = m_pntList[7];
	b = m_pntList[5];
	if (MMath::GetDistancePointLineSegment(sphere.center, a, b) < sphere.radius)
		return true;

	a = m_pntList[5];
	b = m_pntList[1];
	if (MMath::GetDistancePointLineSegment(sphere.center, a, b) < sphere.radius)
		return true;

	return false;
}

inline bool RViewFrustum::TestSphereSwept(const RSphere& sphere,const RVector3 sweepDir) const
{
	//  algorithm -- get all 12 intersection points of the swept sphere with the view frustum
	//  for all points >0, displace sphere along the sweep driection.  if the displaced sphere
	//  is inside the frustum, return TRUE.  else, return FALSE
	float displacements[12];
	int cnt = 0;
	float a, b;
	bool inFrustum = false;

	for (int i=0; i<6; i++)
	{
		if (SweptSpherePlaneIntersect(a, b, m_planes[i], sphere, sweepDir))
		{
			if (a>=0.f)
				displacements[cnt++] = a;
			if (b>=0.f)
				displacements[cnt++] = b;
		}
	}

	for (int i=0; i<cnt; i++)
	{
		RSphere displacedSphere(sphere);
		displacedSphere.center += sweepDir*displacements[i];
		displacedSphere.radius *= 1.1f;
		inFrustum |= TestSphere(displacedSphere);
	}
	return inFrustum;
}

}
