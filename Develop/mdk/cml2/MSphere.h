#pragma once

#include "RLib.h"
#include "MVector3.h"
#include "MBox.h"
#include <vector>

class MOBox;

enum CULL_STATE
{
	CS_NONE,
	CS_INSIDE,   // completely inside the frustum.
	CS_PARTIAL,  // partially inside and partially outside the frustum.
	CS_OUTSIDE   // completely outside the frustum
};

class CML2_API MSphere
{
public:
	float		radius;
	MVector3	center;
	MSphere() : radius(0),center(MVector3::ZERO)	{	}
	MSphere(const MVector3& _center, float _radius) : center(_center), radius(_radius)	{}
	
	explicit MSphere( const MBox* box )			// 박스를 포함하는 sphere
	{
		MVector3 radiusVec;
		center = 0.5f * (box->vmax + box->vmin);
		radiusVec = box->vmax - center;
		float len = radiusVec.Length();
		radius = len;
	}
	explicit MSphere( const std::vector<MVector3>* points )	// 점들을 포함하는 최소 sphere
	{
		assert(points->size() > 0);
		std::vector<MVector3>::const_iterator ptIt = points->begin();

		radius = 0.f;
		center = *ptIt++;

		while ( ptIt != points->end() )
		{
			const MVector3& tmp = *ptIt++;
			MVector3 cVec = tmp - center;
			float d = DotProduct( cVec, cVec );
			if ( d > radius*radius )
			{
				d = sqrtf(d);
				float r = 0.5f * (d+radius);
				float scale = (r-radius) / d;
				center = center + scale*cVec;
				radius = r;
			}
		}
	}


	bool Intersects(const MSphere& s) const
	{
		return ((s.center - center).Length() <= (s.radius + radius));
	}
	bool Intersects(const MVector3& v) const
	{
		return ((v - center).Length() <= radius);
	}

	bool isInPlane(const MPlane &plane)
	{
		float t = plane.GetDistanceTo(center); 
		if( t < -radius)
			return false;
		
		return true;
	}

	bool isInPlanes(const MPlane *pPlanes, int nNum)
	{
		for(int i=0; i<nNum;i++)
			if ( !isInPlane(pPlanes[i]))
				return false;

		return true;
	}

	MSphere Multiply(const MMatrix& m)
	{
		MSphere ret;
		m.TransformVect(center, ret.center);
		ret.radius = radius;
		return ret;
	}

	void makeOBB( MOBox *pOBB );

	/*!
	 * \brief
	 * 이 구가, 중심(x,y,z)이고 반지름 r인 구 안에 들어가냐 ?
	 */
	bool FitsIn(float x,float y,float z,float r) const;
	bool FitsIn(const MSphere& sphere) const;

	CULL_STATE	PlanesTest(const MPlane* pPlanes,int planesNum ) const;
	CULL_STATE	PlanesTest_M(const MPlane* pPlanes,int planesNum ,int& Masking) const;
	CULL_STATE	PlanesTest_MC(const MPlane* pPlanes,int planesNum ,int& Masking,int &DeterminantPlaneIndex) const;
};