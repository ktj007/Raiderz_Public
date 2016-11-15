#include "stdafx.h"
#include "MTypes.h"
#include "MMath.h"

const float MMath::PI = 3.1415926535f;
const float MMath::TWOPI = 6.283185307f;
const float MMath::HALFPI = 1.570796326794895f;
const float MMath::EPSILON = 0.000001f;
const float MMath::TOLER = 0.001f;


float MMath::GetDistanceSegment( const MVector3 &A, const MVector3 &B,const MVector3 &C,const MVector3 &D, bool* bParallel /*= NULL*/ )
{
	// new code, by pok
	//////////////////////////////////////////////////////////////////////////
	//
	// from Essential Mathematics for Games & Interactive Applications, 489~495
	//
	//////////////////////////////////////////////////////////////////////////

	MVector3 w0 = A - C;
	MVector3 u = B - A;
	MVector3 v = D - C;

	//
	//(w0 종점)						(wc 종점)
	// A-----------u vector, s_c--------B
	// |								|
	// w0 vector					wc vector
	// |								|
	// C-----------v vector, t_c--------D
	//
	// wc : [A,B], [C,D] 선분위의 최단거리를 잇는 선분
	// s_c : [A,B] 상에서 A를 시작점, u 백터를 방향백터로 할때 wc의 종점이 있게하는 상수
	// t_c : [C,D] 상에서 C를 시작점, v 백터를 방향백터로 할때 wc의 시작점이 있게하는 상수
	// t_c * v + wc = w0 + s_c * u
	//	i.e) wc = w0 + s_c*u - t_c*v
	// 양변에, u, v를 dot
	//	i.e) u dot ( wc ) = u dot (w0 + s_c*u - t_c*v) ( = u dot w0 + s_c * (u dot u) - t_c * (u dot v) )
	//	i.e) v dot ( wc ) = v dot (w0 + s_c*u - t_c*v) ( = v dot w0 + s_c * (v dot u) - t_c * (v dot v) )
	// wc는 최단을 이으므로 u와 v에 모두 수직
	//	i.e) wc dot u = 0, wc dot v = 0
	//	i.e) 0 = u dot w0 + s_c * (u dot u) - t_c * (u dot v) = v dot w0 + s_c * (v dot u) - t_c * (v dot v)

	// temp a,b,c,d,e
	float a = u.DotProduct(u);
	float b = u.DotProduct(v);
	float c = v.DotProduct(v);
	float d = u.DotProduct(w0);
	float e = v.DotProduct(w0);

	// parameters to compute s_c, t_c
	float sn, sd, tn, td;

	// denon이 0 이면 u, v가 평행
	//	i.e) | u dot v | == ||u||*||v||
	float denom = a*c - b*b;
	if( MMath::IsZero(denom, FLT_EPSILON) )
	{
		// 평행하는 경우
		if(bParallel)
			*bParallel = true;

		// clamp s_c to 0
		sd = td = c;
		sn = 0.0f;
		tn = e;
	}
	else
	{
		// 평행하지 않는 경우
		if(bParallel)
			*bParallel = false;

		// clamp s_c within [0,1]
		sd = td = denom;
		sn = b*e - c*d;
		tn = a*e - b*d;

		// clamp s_c to 0
		if (sn < 0.0f)
		{
			sn = 0.0f;
			tn = e;
			td = c;
		}
		// clamp s_c to 1
		else if (sn > sd)
		{
			sn = sd;
			tn = e + b;
			td = c;
		}
	}

	float t_c, s_c;
	// clamp t_c within [0,1]
	// clamp t_c to 0
	if (tn < 0.0f)
	{
		t_c = 0.0f;
		// clamp s_c to 0
		if ( -d < 0.0f )
		{
			s_c = 0.0f;
		}
		// clamp s_c to 1
		else if ( -d > a )
		{
			s_c = 1.0f;
		}
		else
		{
			s_c = -d/a;
		}
	}
	// clamp t_c to 1
	else if (tn > td)
	{
		t_c = 1.0f;
		// clamp s_c to 0
		if ( (-d+b) < 0.0f )
		{
			s_c = 0.0f;
		}
		// clamp s_c to 1
		else if ( (-d+b) > a )
		{
			s_c = 1.0f;
		}
		else
		{
			s_c = (-d+b)/a;
		}
	}
	else
	{
		t_c = tn/td;
		s_c = sn/sd;
	}

	// compute difference vector and distance (not squared)
	MVector3 wc = w0 + s_c*u - t_c*v;
	return wc.Length();

	// old code, by pok
	// Test Code GetDistanceSegment(vec3(5057.6919, 4628.7607, 0), vec3(5057.6919, 4628.7607, 150.00000), vec3(5248.9121, 4689.0840, 62.034668), vec3(5098.0376, 4647.9277, 296.03467))
	// The right answer is 117 but old code return 151
	//MVector3 AB = b - a;
	//MVector3 CD = d - c;
	//MVector3 H = AB.CrossProduct(CD);
	//MVector3 N = AB.CrossProduct(H);

	//float CDDotN = CD.DotProduct(N);

	//if (fabs(CDDotN) < FLT_EPSILON)					
	//{
	//	// 두 선분이 평행이다.
	//	float f1 = GetDistancePointLineSegment(c, a, b);
	//	float f2 = GetDistancePointLineSegment(d, a, b);

	//	if(bParallel)
	//		*bParallel = true;

	//	if (f1 < f2)
	//		return f1;
	//	else
	//		return f2;
	//}
	//else
	//{
	//	// 교점이 생긴다. 
	//	// I0 -> 선분 ab위의, 선분 cd와 가장 가까운 점
	//	// I1 -> 선분 cd위의, 선분 ab와 가장 가까운 점

	//	float  t1 = (a - c).DotProduct(N) / CDDotN;
	//	t1 = (t1 < 0.0f)? 0.0f : (t1 > 1.0f)? 1.0f : t1;
	//	MVector3 I1 = c + t1 * CD;

	//	float  t0 = (I1 - a).DotProduct(AB) / (AB.DotProduct(AB));
	//	t0 = (t0 < 0.0f)? 0.0f : (t0 > 1.0f)? 1.0f : t0;
	//	MVector3 I0 = a + t0 * AB;

	//	if(bParallel)
	//		*bParallel = false;

	//	return I0.DistanceTo(I1);
	//}
}

float MMath::GetDistanceSegmentRay( const MVector3 &a, const MVector3 &b,const MVector3 &o,const MVector3 &dir, bool* bParallel /*= NULL*/ )
{
	MVector3 AB = b - a;
	MVector3 CD = dir;
	MVector3 H = AB.CrossProduct(CD);
	MVector3 N = AB.CrossProduct(H);

	float CDDotN = CD.DotProduct(N);

	if (fabs(CDDotN) < FLT_EPSILON)					
	{
		// 두 선분이 평행이다.
		float f1 = GetDistancePointRay(a, o, dir);
		float f2 = GetDistancePointRay(b, o, dir);

		if(bParallel)
			*bParallel = true;

		if (f1 < f2)
			return f1;
		else
			return f2;
	}
	else
	{
		// 교점이 생긴다. 
		// I0 -> 선분 ab위의,반직선 o, dir와 가장 가까운 점
		// I1 -> 반직선 o, dir위의, 선분 ab와 가장 가까운 점

		float  t1 = (a - o).DotProduct(N) / CDDotN;
		t1 = (t1 < 0.0f)? 0.0f : t1;
		MVector3 I1 = o + t1 * CD;

		float  t0 = (I1 - a).DotProduct(AB) / (AB.DotProduct(AB));
		t0 = (t0 < 0.0f)? 0.0f : (t0 > 1.0f)? 1.0f : t0;
		MVector3 I0 = a + t0 * AB;

		if(bParallel)
			*bParallel = false;

		return I0.DistanceTo(I1);
	}
}

MVector3 MMath::GetNearestPointOnLineRay( const MVector3 &a, const MVector3 &b,const MVector3 &o,const MVector3 &dir, bool* bParallel /*= NULL*/ )
{
	MVector3 AB = b - a;
	MVector3 CD = dir;
	MVector3 H = AB.CrossProduct(CD);
	MVector3 N = AB.CrossProduct(H);

	float CDDotN = CD.DotProduct(N);

	if (fabs(CDDotN) < FLT_EPSILON)					
	{
		// 두 선분이 평행이다.
		float f1 = GetDistancePointRay(a, o, dir);
		float f2 = GetDistancePointRay(b, o, dir);

		if(bParallel)
			*bParallel = true;

		if (f1 < f2)
			return a;
		else
			return b;
	}
	else
	{
		// 교점이 생긴다. 
		// I0 -> 선분 ab위의,반직선 o, dir와 가장 가까운 점
		// I1 -> 반직선 o, dir위의, 선분 ab와 가장 가까운 점

		float  t1 = (a - o).DotProduct(N) / CDDotN;
		t1 = (t1 < 0.0f)? 0.0f : t1;
		MVector3 I1 = o + t1 * CD;

		float  t0 = (I1 - a).DotProduct(AB) / (AB.DotProduct(AB));
		t0 = (t0 < 0.0f)? 0.0f : (t0 > 1.0f)? 1.0f : t0;
		MVector3 I0 = a + t0 * AB;

		if(bParallel)
			*bParallel = false;

		return I0;
	}
}

MVector3 MMath::GetNearestPointOnLine( const MVector3 &a, const MVector3 &b,const MVector3 &c,const MVector3 &d, bool* bParallel /*= NULL*/ )
{
	MVector3 AB = b - a;
	MVector3 CD = d - c;
	MVector3 H = AB.CrossProduct(CD);
	MVector3 N = AB.CrossProduct(H);

	float CDDotN = CD.DotProduct(N);

	if (fabs(CDDotN) < FLT_EPSILON)					
	{
		// 두 선분이 평행이다.
		float f1 = GetDistancePointLineSegment(a, c, d);
		float f2 = GetDistancePointLineSegment(b, c, d);

		if(bParallel)
			*bParallel = true;

		if (f1 < f2)
			return a;
		else
			return b;
	}
	else
	{
		// 교점이 생긴다. 
		// I0 -> 선분 ab위의, 선분 cd와 가장 가까운 점
		// I1 -> 선분 cd위의, 선분 ab와 가장 가까운 점

		float  t1 = (a - c).DotProduct(N) / CDDotN;
		t1 = (t1 < 0.0f)? 0.0f : (t1 > 1.0f)? 1.0f : t1;
		MVector3 I1 = c + t1 * CD;

		float  t0 = (I1 - a).DotProduct(AB) / (AB.DotProduct(AB));
		t0 = (t0 < 0.0f)? 0.0f : (t0 > 1.0f)? 1.0f : t0;
		MVector3 I0 = a + t0 * AB;

		if(bParallel)
			*bParallel = false;

		return I0;
	}
}

struct line 
{
	float a, b, c;
};

bool eq(double x, double y)
{
	return fabs(x - y) < FLT_EPSILON;
}

bool ptol(const MVector2 &p1, const MVector2 &p2, line &l) // point to ax + by + c = 0;
{
	if(p1 == p2) return false;
	if(eq(p1.x, p2.x)) { l.a = 1.0; l.b = 0.0; l.c = -p1.x; }
	else if(eq(p1.y, p2.y)) { l.a = 0.0; l.b = 1.0; l.c = -p1.y; }
	else { l.a = p1.y - p2.y; l.b = p2.x - p1.x;
	l.c = p1.y * (p1.x - p2.x) - p1.x * (p1.y - p2.y); }
	return true;
}

/* 두 직선의 교점. 반환값 0 (교점 없음) 1 (한점에서 만남) 2 (같은 직선) */
int lineIntersect(const line &l1, const line &l2, MVector2 &p)
{
	if(eq(l1.a * l2.b, l2.a * l1.b))
	{ if(eq(l1.a * l2.c, l2.a * l1.c) && eq(l1.b * l2.c, l2.b * l1.c)) return 2;
	else return 0;
	} else {
		p.y = (l1.a * l2.c - l2.a * l1.c)/(l2.a * l1.b - l1.a * l2.b);
		p.x = (l1.b * l2.c-l2.b*l1.c)/(l1.a*l2.b-l2.a*l1.b);
	}
	return 1;
}


// 두 선분의 교점. 반환값 0 (교점 없음) 1 (한점에서 만남) 2 (같은 직선)
int MMath::IntersectSegment2D(const MVector2& p1, const MVector2& p2, const MVector2& p3, const MVector2 &p4, MVector2& p)
{
	p.x = FLT_MIN;
	p.y = FLT_MAX;
	int ret; line l1, l2;
//	p1.align(); p2.align(); p3.align(); p4.align();
	if(!ptol(p1, p2, l1)) return 0; if(!ptol(p3, p4, l2)) return 0;
	ret = lineIntersect(l1, l2, p);
//	p.align();
	if(ret == 0) return 0;
	else if(ret == 1)
	{
		if(p.x < min(p1.x, p2.x) || p.x > max(p1.x, p2.x)) return 0;
		else if(p.x < min(p3.x, p4.x) || p.x > max(p3.x, p4.x)) return 0;
		else if(p.y < min(p1.y, p2.y) || p.y > max(p1.y, p2.y)) return 0;
		else if(p.y < min(p3.y, p4.y) || p.y > max(p3.y, p4.y)) return 0;
		return 1;
	}

	/* 교점의 좌표를 얻고자 할때 ------------------------------------ */
	double a, b, c, d;
	int pa = 1, pb = 2, pc = 3, pd = 4;
	a = 0.0;
	if(p1.x == p2.x) { b = p2.y - p1.y; c = p3.y - p1.y; d = p4.y - p1.y; }
	else { b = p2.x - p1.x; c = p3.x - p1.x; d = p4.x - p1.x; }
	if(a > b) { swap(a, b); swap(pa, pb); }
	if(c > d) { swap(c, d); swap(pc, pd); }
	if((a <= c && c <= b) || (c <= a && a <= d))
	{
		if(eq(a, d)) if(pa == 1) p = p1; else p = p2;
		else if(eq(b, c)) if(pb == 1) p = p1; else p = p2;
		else return 2;
		return 1;
	}
	return 0;
}


// Cohen-Sutherland algorithm code from http://en.WIKIPEDIA.org
int MMath::LineClippingGetCode(MVector2 &p, const MRectF *pRect)
{
	int result = 0;

	// bit flag
	if ( p.x < pRect->left )		
		result |= 1;
	else if ( p.x > pRect->right )	
		result |= 2;

	if ( p.y > pRect->bottom )	
		result |= 4;
	else if ( p.y < pRect->top)	
		result |= 8;

	return result;
}

// 2D 좌표 Clipping 하는 함수, false 이면 직선이 사각형 안에 없는 것이다.
bool MMath::LineClipping2D(MVector2 &vP, MVector2 &vQ, const MRectF *pRect)
{
	_ASSERT(pRect);

	MVector2 P = vQ - vP;

	int startCode = MMath::LineClippingGetCode(vP, pRect);
	int endCode = MMath::LineClippingGetCode(vQ, pRect);

	float dxdy = 0;
	float dydx = 0;

	if ( P.x != 0 ) 
		dydx = P.y / P.x;
	if ( P.y != 0 ) 
		dxdy = P.x / P.y;

	for ( int stage = 0; stage<4; ++stage )
	{
		if ((startCode | endCode) == 0 )	
			return true;
		else if ((startCode & endCode) != 0 )
			return false;

		if ( startCode == 0 )
		{
			std::swap(startCode, endCode);
			std::swap(vP, vQ);
		}

		if ((startCode & 1) == 1)
		{
			vP.y += dydx * (pRect->left - vP.x);
			vP.x = pRect->left;
		}
		else if ((startCode & 2) == 2)
		{
			vP.y += dydx * (pRect->right - vP.x);
			vP.x = pRect->right;
		}
		else if ((startCode & 4) == 4)
		{
			vP.x += dxdy * (pRect->bottom - vP.y);
			vP.y = pRect->bottom;
		}
		else if ((startCode & 8) == 8)
		{
			vP.x += dxdy * (pRect->top - vP.y);
			vP.y = pRect->top;
		}

		startCode = LineClippingGetCode(vP, pRect);
	}

	return true;
}

bool MMath::IsIntersectionPlane(MVector3 &outVec, const MVector3 &pos, const MVector3 &dir, const MPlane &plane)
{
	float d = plane.GetDistanceTo(pos);
	if ( d < 0.0f )
		return false;

	float t = plane.DotNormal(dir);
	if ( t >= 0.0f )
		return false;

	outVec = pos + dir*(-t*d);
	return true;
}

bool MMath::ClipVertices(MVector3 *pDest, int *pnDestVertices, const MVector3 *pSrc, int nSrcVertices, const MPlane &plane)
{
	enum SIDE {
		SIDE_FRONT,
		SIDE_ON,
		SIDE_BACK
	};

	#define	ON_EPSILON	0.1

	SIDE sides[_MAX_PORTAL_PLANES];
	float dists[_MAX_PORTAL_PLANES];

	int nFront=0,nBack=0;
	int i;
	for(i=0;i<nSrcVertices;i++)
	{
		float fDot=plane.GetDistanceTo(pSrc[i]);
		if(fDot>ON_EPSILON) {
			sides[i]=SIDE_FRONT;
			nFront++;
		} else 
			if(fDot<-ON_EPSILON) {
				sides[i]=SIDE_BACK;
				nBack++;
			}
			else
				sides[i]=SIDE_ON;
		dists[i]=fDot;
	}
	sides[i]=sides[0];
	dists[i]=dists[0];

	*pnDestVertices = 0;

	if(!nFront)	return false; // 완전히 잘려나감.

	if(!nBack)	{	// 전혀 잘려지지 않음.
		*pnDestVertices = nSrcVertices;
		memcpy(pDest,pSrc,sizeof(MVector3)*nSrcVertices);
		return true;
	}

	for (i=0;i<nSrcVertices;i++)
	{
		const MVector3 *p1 = &pSrc[i];

		if( sides[i] != SIDE_BACK )
		{
			pDest[*pnDestVertices]=*p1;
			(*pnDestVertices)++;
		}

		if( sides[i] != SIDE_ON &&
			sides[i+1] != SIDE_ON &&
			sides[i] != sides[i+1] )
		{
			const MVector3 *p2  = &pSrc[(i+1)%nSrcVertices];
			MVector3 *mid = &pDest[*pnDestVertices];
			(*pnDestVertices)++;

			float t = dists[i] / (dists[i]-dists[i+1]);
			*mid = *p1 + t * ( *p2 - *p1 );
		}
	}
	return true;
}

bool MMath::ClipVertices(vector<MVector3>& dest, const vector<MVector3>& src, const MPlane &plane)
{
	enum SIDE
	{
		SIDE_FRONT,
		SIDE_ON,
		SIDE_BACK
	};

	#define	ON_EPSILON 0.1

	static vector<SIDE> sides;
	static vector<float> dists;

	sides.clear();
	dists.clear();

	size_t srcSize = src.size();
	int nFront = 0;
	int nBack = 0;
	for( size_t i = 0; i < srcSize; i++ )
	{
		float fDot = plane.GetDistanceTo( src[i] );
		if( fDot > ON_EPSILON )
		{
			sides.push_back( SIDE_FRONT );
			nFront++;
		}
		else
		if( fDot < -ON_EPSILON )
		{
			sides.push_back( SIDE_BACK );
			nBack++;
		}
		else
		{
			sides.push_back( SIDE_ON );
		}

		dists.push_back( fDot );
	}
	if( srcSize )
	{
		sides.push_back( sides.front() );
		dists.push_back( dists.front() );
	}

	dest.clear();

	// 완전히 잘려나감
	if( !nFront )
		return false;

	// 전혀 잘려지지 않음
	if( !nBack )
	{
		dest.assign( src.begin(), src.end() );
		return true;
	}

	for( size_t i = 0; i < srcSize; i++ )
	{
		SIDE side = sides[i];
		SIDE sideNext = sides[i+1];
		const MVector3& p1 = src[i];

		if( side != SIDE_BACK )
		{
			dest.push_back( p1 );
		}

		if( side != SIDE_ON &&
			sideNext != SIDE_ON &&
			side != sideNext )
		{
			const MVector3& p2 = src[ (i+1)%srcSize ];
			float t = dists[i] / ( dists[i] - dists[i+1] );
			dest.push_back( p1 + t * ( p2 - p1 ) );
		}
	}
	return true;
}

MVector3 MMath::RightHandedNormal( const MVector3& P0,const MVector3& P1,const MVector3& P2 )
{
	MVector3 L01 = P1 - P0;
	MVector3 L02 = P2 - P0;

	return L01.CrossProduct(L02);
}