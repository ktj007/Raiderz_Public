#pragma once

#include "RLib.h"
#include "MTypes.h"
#include "MVector3.h"
#include "MPlane.h"
#include "MMatrix.h"
#include <float.h>
#include <vector>
#include <algorithm>

using namespace std;

class CML2_API MBox
{
public:
	union {
		struct {
			float minx,miny,minz,maxx,maxy,maxz;
		};
		struct {
			MVector3 vmin,vmax;
		};
		float m[2][3];
	};

	MBox() {}
	MBox(const MVector3& _vmin, const MVector3& _vmax) : vmin(_vmin), vmax(_vmax) {}
	MBox(float mx, float my, float mz,float Mx, float My, float Mz)
		: minx(mx), miny(my), minz(mz), maxx(Mx), maxy(My), maxz(Mz) {}

	explicit MBox(const std::vector<MBox>* boxes);
	explicit MBox(const std::vector<MVector3>* points) : vmin(1e33f, 1e33f, 1e33f), vmax(-1e33f, -1e33f, -1e33f)
	{
		for ( unsigned int i=0; i<points->size(); i++ )
			Add( (*points)[i] );
	}

#if (_MSC_VER >= 1900)
	MBox& operator= (const MBox& box) {
		memcpy(m, box.m, sizeof(m));
		return *this;
	}
#endif

	void Initialize() {
		vmin = MVector3(FLT_MAX,FLT_MAX,FLT_MAX);
		vmax = MVector3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	}

	void SetInfinity()
	{
		vmin = MVector3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
		vmax = MVector3(FLT_MAX,FLT_MAX,FLT_MAX);
	}

	bool IsValid() const
	{
		return (vmin.x <= vmax.x) && (vmin.y <= vmax.y) && (vmin.z <= vmax.z);
	}

	bool IsOut(const MVector3& v)
	{
		if( v.x < vmin.x || vmax.x < v.x )	return true;
		if( v.y < vmin.y || vmax.y < v.y )	return true;
		if( v.z < vmin.z || vmax.z < v.z )	return true;
		return false;
	}

	MVector3 Point(int i) const { return MVector3( (i&1)?vmin.x:vmax.x, (i&2)?vmin.y:vmax.y, (i&4)?vmin.z:vmax.z );  }

	MVector3 GetCenter() const { return .5f * (vmin+vmax); }

	bool Contains(const MVector3& point) const {
		return (minx<=point.x && point.x<=maxx &&
			miny<=point.y && point.y<=maxy &&
			minz<=point.z && point.z<=maxz );
	}

	// http://www.codercorner.com/RayAABB.cpp by Terdiman (optimized Woo);
	// 선분과 교차 검사
	bool ContainsLineSegment(const MVector3 &p, const MVector3 &q) const	// TODO: 이름이 intersect 가 어울릴듯
	{
		MVector3 boxExtents, diff, dir;
		float fAWdU[3];

		dir.x = 0.5f * (q.x - p.x);
		boxExtents.x = (maxx-minx)*0.5f;
		diff.x = (0.5f * (q.x + p.x)) - .5f*(maxx+minx);
		fAWdU[0] = fabsf(dir.x);
		if(fabsf(diff.x)>boxExtents.x + fAWdU[0])	return false;

		dir.y = 0.5f * (q.y - p.y);
		boxExtents.y = (maxy-miny)*0.5f;
		diff.y = (0.5f * (q.y + p.y)) - .5f*(maxy+miny);
		fAWdU[1] = fabsf(dir.y);
		if(fabsf(diff.y)>boxExtents.y + fAWdU[1])	return false;

		dir.z = 0.5f * (q.z - p.z);
		boxExtents.z = (maxz-minz)*.5f;
		diff.z = (0.5f * (q.z + p.z)) - .5f*(maxz+minz);
		fAWdU[2] = fabsf(dir.z);
		if(fabsf(diff.z)>boxExtents.z + fAWdU[2])	return false;

		float f;
		f = dir.y * diff.z - dir.z * diff.y;	if(fabsf(f)>boxExtents.y*fAWdU[2] + boxExtents.z*fAWdU[1])	return false;
		f = dir.z * diff.x - dir.x * diff.z;	if(fabsf(f)>boxExtents.x*fAWdU[2] + boxExtents.z*fAWdU[0])	return false;
		f = dir.x * diff.y - dir.y * diff.x;	if(fabsf(f)>boxExtents.x*fAWdU[1] + boxExtents.y*fAWdU[0])	return false;

		return true;
	}

	// origPt 에서 dir 방향으로 가는 반직선이 충돌하는지
	// from nvidia boundingbox.cpp, 리턴값 : 충돌여부, hitDist : 충돌 거리,
	bool Intersect(float* hitDist, const MVector3& origPt, const MVector3& dir)
	{
		MPlane sides[6] = { MPlane( 1, 0, 0,-vmin.x), MPlane(-1, 0, 0, vmax.x),
			MPlane( 0, 1, 0,-vmin.y), MPlane( 0,-1, 0, vmax.y),
			MPlane( 0, 0, 1,-vmin.z), MPlane( 0, 0,-1, vmax.z) };

		*hitDist = 0.f;  // safe initial value
		MVector3 hitPt = origPt;

		bool inside = false;

		for ( int i=0; (i<6) && !inside; i++ )
		{
			float cosTheta = sides[i].DotNormal( dir );
			float dist = sides[i].GetDistanceTo( origPt );

			//  if we're nearly intersecting, just punt and call it an intersection
			if ( ALMOST_ZERO(dist) ) return true;
			//  skip nearly (&actually) parallel rays
			if ( ALMOST_ZERO(cosTheta) ) continue;
			//  only interested in intersections along the ray, not before it.
			*hitDist = -dist / cosTheta;
			if ( *hitDist < 0.f ) continue;

			hitPt = (*hitDist)*dir + origPt;

			inside = true;

			for ( int j=0; (j<6) && inside; j++ )
			{
				if ( j==i )
					continue;
				float d = sides[j].GetDistanceTo( hitPt );

				inside = ((d + 0.00015) >= 0.f);
			}
		}

		return inside;        
	}

	bool ContainsAABB(const MBox &bbox)	const// TODO: 이름이 intersect 가 어울릴듯
	{
		// AABB vs AABB 충돌 검사
		if ( this->vmax.x > bbox.vmin.x && bbox.vmax.x > this->vmin.x )
			if ( this->vmax.y > bbox.vmin.y && bbox.vmax.y > this->vmin.y )
				if ( this->vmax.z > bbox.vmin.z && bbox.vmax.z > this->vmin.z )
					return true;
		return false;
	}

	bool ContainsPolygon(const MVector3 &tri0, const MVector3 &tri1, const MVector3 &tri2);

	bool isInPlane(const  MPlane &plane) const {		// 평면의 + 영역과 겹치는 부분이 있는지
		return (GetDistance(plane)>=0);
	}
	bool isInPlanes(const MPlane *planes, int nNum) const {
		for ( int i = 0; i<nNum; ++i )
			if ( !isInPlane(planes[i]))
				return false;

		return true;
	}

	bool isInViewFrustum(const  MPlane *planes) const {
		return isInPlane(planes[0]) && isInPlane(planes[1]) && isInPlane(planes[2]) && isInPlane(planes[3]) ;
	}
	bool isInViewFrustumWithNearZ(const  MPlane *planes) const {	// nearz plane 포함
		return isInPlane(planes[0]) && isInPlane(planes[1]) && isInPlane(planes[2]) && isInPlane(planes[3]) && isInPlane(planes[4]) ;
	}
	bool isInViewFrustumWithZ(const  MPlane *planes) const {	// near & farz plane 포함
		return isInPlane(planes[0]) && isInPlane(planes[1]) && isInPlane(planes[2]) && isInPlane(planes[3]) && isInPlane(planes[4])	&& isInPlane(planes[5]);
	}

	float GetDistance(const MVector3 &point) {	// 한점에서 바운딩박스의 최단거리
		MVector3 closest; // 가장가까운점
		if( minx<=point.x && point.x<=maxx ) closest.x=point.x;
		else closest.x = fabs(point.x-minx) < fabs(point.x-maxx) ? minx : maxx;
		if( miny<=point.y && point.y<=maxy ) closest.y=point.y;
		else closest.y = fabs(point.y-miny) < fabs(point.y-maxy) ? miny : maxy;
		if( minz<=point.z && point.z<=maxz ) closest.z=point.z;
		else closest.z = fabs(point.z-minz) < fabs(point.z-maxz) ? minz : maxz;
		return Length(closest-point);
	}

	float GetDistance(const MPlane &plane)	const {	// 평면에서 boundingbox와의 최대거리
		float a,b,c;
		a=(plane.a>0)?m[1][0]:m[0][0];
		b=(plane.b>0)?m[1][1]:m[0][1];
		c=(plane.c>0)?m[1][2]:m[0][2];
		return plane.a*a+plane.b*b+plane.c*c+plane.d;
	}

	float GetDistanceReverse(const MPlane &plane)	const {	// 평면에서 boundingbox와의 최대거리 - 역방향
		float a,b,c;
		a=(-plane.a>0)?m[1][0]:m[0][0];
		b=(-plane.b>0)?m[1][1]:m[0][1];
		c=(-plane.c>0)?m[1][2]:m[0][2];
		return -plane.a*a + -plane.b*b + -plane.c*c + -plane.d;
	}

	float GetDistanceClosest(const MPlane &plane)	const {	// 평면에서 boundingbox와의 최소거리
		float a,b,c;
		a=(plane.a<0)?m[1][0]:m[0][0];
		b=(plane.b<0)?m[1][1]:m[0][1];
		c=(plane.c<0)?m[1][2]:m[0][2];
		return plane.a*a+plane.b*b+plane.c*c+plane.d;
	}

	float GetRadius(void) const
	{
		return (vmin - vmax).Length() / 2.0f;
	}

	void Add(const MVector3 &kMin, const MVector3 &kMax)
	{
		if ( vmin.x > kMin.x )	vmin.x = kMin.x;
		if ( vmin.y > kMin.y )	vmin.y = kMin.y;
		if ( vmin.z > kMin.z )	vmin.z = kMin.z;
		if ( vmax.x < kMax.x )	vmax.x = kMax.x;
		if ( vmax.y < kMax.y )	vmax.y = kMax.y;
		if ( vmax.z < kMax.z )	vmax.z = kMax.z;

		return;
	}

	void Add(const MBox &kAABBox)
	{
		if ( vmin.x > kAABBox.vmin.x )	vmin.x = kAABBox.vmin.x;
		if ( vmin.y > kAABBox.vmin.y )	vmin.y = kAABBox.vmin.y;
		if ( vmin.z > kAABBox.vmin.z )	vmin.z = kAABBox.vmin.z;
		if ( vmax.x < kAABBox.vmax.x )	vmax.x = kAABBox.vmax.x;
		if ( vmax.y < kAABBox.vmax.y )	vmax.y = kAABBox.vmax.y;
		if ( vmax.z < kAABBox.vmax.z )	vmax.z = kAABBox.vmax.z;
	}

	void Add(const MVector3 &kPoint)
	{
#if 1
		__asm
		{
			mov		eax,[kPoint]
			mov		ecx,[this]

			movss	xmm3,[eax]MVector3.x
			movss	xmm4,[eax]MVector3.y
			movss	xmm5,[eax]MVector3.z

			movss	xmm0,[ecx]MBox.vmin.x
			movss	xmm1,[ecx]MBox.vmin.y
			movss	xmm2,[ecx]MBox.vmin.z
			minss	xmm0,xmm3
			minss	xmm1,xmm4
			minss	xmm2,xmm5
			movss	[ecx]MBox.vmin.x,xmm0
			movss	[ecx]MBox.vmin.y,xmm1
			movss	[ecx]MBox.vmin.z,xmm2

			movss	xmm0,[ecx]MBox.vmax.x
			movss	xmm1,[ecx]MBox.vmax.y
			movss	xmm2,[ecx]MBox.vmax.z
			maxss	xmm0,xmm3
			maxss	xmm1,xmm4
			maxss	xmm2,xmm5
			movss	[ecx]MBox.vmax.x,xmm0
			movss	[ecx]MBox.vmax.y,xmm1
			movss	[ecx]MBox.vmax.z,xmm2
		}
#else
		if ( vmin.x > kPoint.x )	vmin.x = kPoint.x;
		if ( vmin.y > kPoint.y )	vmin.y = kPoint.y;
		if ( vmin.z > kPoint.z )	vmin.z = kPoint.z;
		if ( vmax.x < kPoint.x )	vmax.x = kPoint.x;
		if ( vmax.y < kPoint.y )	vmax.y = kPoint.y;
		if ( vmax.z < kPoint.z )	vmax.z = kPoint.z;
#endif

	}

	MBox And(const MBox &bbox)
	{
		MBox andBBox;

		for ( int i = 0; i<3; ++i )
		{
			andBBox.m[0][i] = (std::max)(bbox.m[0][i],this->m[0][i]);
			andBBox.m[1][i] = (std::min)(bbox.m[1][i],this->m[1][i]);
		}

		return andBBox;
	}

	void GetPoints(MVector3 *pPoints) const	// Points 크기는 반드시 8개 이상
	{
		pPoints[0] = vmin;
		pPoints[1] = MVector3(vmin.x, vmin.y, vmax.z);
		pPoints[2] = MVector3(vmin.x, vmax.y, vmin.z);
		pPoints[3] = MVector3(vmin.x, vmax.y, vmax.z);
		pPoints[4] = MVector3(vmax.x, vmin.y, vmin.z);
		pPoints[5] = MVector3(vmax.x, vmin.y, vmax.z);
		pPoints[6] = MVector3(vmax.x, vmax.y, vmin.z);
		pPoints[7] = vmax;
	}

	MVector3 GetCenter()
	{
		return (vmin + ( vmax - vmin )*0.5f);
	}

	bool IsEqual(const MBox &rhs) const
	{
		if( vmin.IsEqual(rhs.vmin) && vmax.IsEqual(rhs.vmax) ) return true;
		return false;
	}

#if (_MSC_VER >= 1900)
	// Modern Visual C++ already defines INFINITY and causes name clash.
	static const MBox _INFINITY;
	static const MBox _INVALID;
#else
	static const MBox INFINITY;
	static const MBox INVALID;
#endif
};

inline MBox::MBox(const std::vector<MBox>* boxes)
{
	Initialize();
	for(unsigned int i=0;i<boxes->size();i++)
		Add((*boxes)[i]);
}

// from nvidia, 박스를 트랜스폼하면, 커집니다.
//  Transform an axis-aligned bounding box by the specified matrix, and compute a new axis-aligned bounding box
inline void TransformBox( MBox* result, const MBox& src, const MMatrix& matrix )
{
	MVector3 pts[8];
	for ( int i=0; i<8; i++ )
		pts[i] = src.Point(i);

	result->vmin = MVector3(3.3e33f, 3.3e33f, 3.3e33f);
	result->vmax = MVector3(-3.3e33f, -3.3e33f, -3.3e33f);

	for (int i=0; i<8; i++)
	{
		MVector3 tmp = pts[i];
		matrix.TransformVectHomogeneous(tmp);
		result->Add(tmp);
	}
}