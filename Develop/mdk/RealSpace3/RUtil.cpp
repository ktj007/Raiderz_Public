#include "StdAfx.h"
#include "RUtil.h"
#include "RLib.h"

namespace rs3 {

// 선분과 선분 사이의 거리.. 선분 (a,aa) 과 선분 (c,cc)의 거리.
float GetDistanceBetweenLineSegment(const MVector3 &a,const MVector3 &aa,const MVector3 &c,const MVector3 &cc,MVector3 *ap,MVector3 *cp)
{
	MVector3 b=aa-a;
	MVector3 d=cc-c;

	// 두 선분에 공통 수선의 방향은 b x d
	MVector3 cm = CrossProduct(b,d);

	float fLength=Length(cm);

	if(fLength<1)	// 두 선분이 평행하다면...
	{

		MVector3 x;	// 두 선분을 포함하는 두 직선의 차.. 이걸 구하는 방법은 점과 직선의 거리구하는법을 참조.

		MVector3 edge=a-c;
		float temp=DotProduct(edge,d)/Length(d);
		MVector3 dir=d;
		dir.Normalize();
		x=c+temp*dir-a;

		float st0,st1;		// t=0 일때의 s 의 값, t=1 일때의 s 의 값

		st0=DotProduct(a+x-c,d)/DotProduct(d,d);
		st1=DotProduct(aa+x-c,d)/DotProduct(d,d);

        if(st0<0 && st1<0)
		{
			*cp=c;
			if(fabs(st0)>fabs(st1))
				*ap=aa;
			else
				*ap=a;
		}
		else
		if(st0>1 && st1>1)
		{
			*cp=cc;
			if(fabs(st0)>fabs(st1))
				*ap=aa;
			else
				*ap=a;
		}else
		{
			if(st0>=0 && st0<=1)
			{
				*ap=a;
				*cp=c+st0*d;
			}else
			if(st1>=0 && st1<=1)
			{
				*ap=aa;
				*cp=c+st1*d;
			}else
			{
				*cp=c;
				*ap=*cp-x;
			}
		}
		return Length(*ap - *cp);
	}

//	Normalize(cm);
	cm/=fLength;

	// c, c+d, c+cm 세 점을 지나는 평면의 방정식은 rp-rc=0 ( r = d x cm , 방향은 선분의 방향과 공통수수선 방향의 crossproduct 이며 점c를 지난다.)
	// 이 평면과 a+tb 직선의 교점에서 r(a+tb)-rc=0   ->  t=(rc-ra)/(rb)
	
	MVector3 r = CrossProduct(d,cm);
	r.Normalize();

	float t=(DotProduct(r,c)-DotProduct(r,a))/DotProduct(r,b);

	// a, a+b, a+cm 세 점을 지나는 평면의 방정식은 rp-ra=0 ( r = b x cm , 방향은 선분의 방향과 공통수수선 방향의 crossproduct 이며 점a를 지난다.)
	// 이 평면과 c+sd 직선의 교점에서 r(c+sd)-ra=0    ->  s=(ra-rc)/(rd)

	r = CrossProduct(b,cm);
	r.Normalize();

	float s=(DotProduct(r,a)-DotProduct(r,c))/DotProduct(r,d);

	if(t<0) t=0;
	if(t>1) t=1;

	if(s<0) s=0;
	if(s>1) s=1;

	*ap=a+t*b;
	*cp=c+s*d;
	return Length(*ap - *cp);
}

bool RGetScreenLine(int sx,int sy, RECT &rt, const MMatrix &View, const MMatrix &Projection, MVector3 *dir)
{
	if ( sx < rt.left )
		return false;
	if ( sy < rt.top )
		return false;
	if ( sx > rt.right )
		return false;
	if ( sy > rt.bottom )
		return false;

	float x = (float)sx;
	float y = (float)sy;

	MVector3 v;
	// Near Z 평면의 한점 
	v.x =  ( ( ( 2.0f * x ) / rt.right ) - 1 ) / Projection._11;
	v.y = -( ( ( 2.0f * y ) / rt.bottom ) - 1 ) / Projection._22;
	v.z = -1.0f;

	MMatrix m;
	View.GetInverse(&m, NULL);

	if ( dir )
	{
		dir->x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
		dir->y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
		dir->z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
	}
	//if ( pos )
	//{
	//	pos->x = m._41;
	//	pos->y = m._42;
	//	pos->z = m._43;
	//}

	return true;
}

void SplitPath(const char *pFullname, char *path, char *file)
{
	const char *p1 = strrchr(pFullname,'/');
	const char *p2 = strrchr(pFullname,'\\');
	const char *p = max(p1, p2);
	
	int namelen = strlen(pFullname);
	int pos = 0;
	if ( p )
		pos = p - pFullname + 1;

	if ( path )
	{
		strncpy(path,pFullname,pos);
		path[pos] = 0;
	}
	if ( file )
	{
		strncpy(file,&pFullname[pos],namelen-pos);
		file[namelen-pos] = 0;
	}

	return;
}

// Triangle-box Overlap test
// from T.A. Moller code ( http://www.cs.lth.se/home/Tomas_Akenine_Moller/code/tribox3.txt )

inline bool AXISTESTX01(const float &a, const float &b, const float &fa, const float &fb, const MVector3 vTri[], const MVector3 &vHalfSize)
{
	float p0 = a*vTri[0].y - b*vTri[0].z;
	float p2 = a*vTri[2].y - b*vTri[2].z;

	float min, max;
	if ( p0 < p2 )	{		min = p0;		max = p2;	}
	else	{		min = p2;		max = p0;	}

	float rad = fa * vHalfSize.y + fb * vHalfSize.z;
	if ( min > rad || max < -rad )
		return false;

	return true;
}

inline bool AXISTESTX2(const float &a, const float &b, const float &fa, const float &fb, const MVector3 vTri[], const MVector3 &vHalfSize)
{
	float p0 = a*vTri[0].y - b*vTri[0].z;
	float p1 = a*vTri[1].y - b*vTri[1].z;

	float min, max;
	if ( p0 < p1 )	{		min = p0;		max = p1;	}
	else	{		min = p1;		max = p0;	}

	float rad = fa * vHalfSize.y + fb * vHalfSize.z;
	if ( min > rad || max < -rad )
		return false;

	return true;
}


inline bool AXISTESTY02(const float &a, const float &b, const float &fa, const float &fb, const MVector3 vTri[], const MVector3 &vHalfSize)
{
	float p0 = -a*vTri[0].x + b*vTri[0].z;
	float p2 = -a*vTri[2].x + b*vTri[2].z;

	float min, max;
	if ( p0 < p2 )	{ min = p0; max = p2; }
	else	{ min = p2; max = p0; }

	float rad = fa * vHalfSize.x + fb * vHalfSize.z;
	if ( min > rad || max < -rad )	return false;

	return true;
}

inline bool AXISTESTY1(const float &a, const float &b, const float &fa, const float &fb, const MVector3 vTri[], const MVector3 &vHalfSize)
{
	float p0 = -a*vTri[0].x + b*vTri[0].z;
	float p1 = -a*vTri[1].x + b*vTri[1].z;

	float min, max;
	if ( p0 < p1 )	{ min = p0; max = p1; }
	else	{ min = p1; max = p0; }

	float rad = fa * vHalfSize.x + fb * vHalfSize.z;
	if ( min > rad || max < -rad )	return false;

	return true;
}

inline bool AXISTESTZ12(const float &a, const float &b, const float &fa, const float &fb, const MVector3 vTri[], const MVector3 &vHalfSize)
{
	float p1 = a*vTri[1].x - b*vTri[1].y;
	float p2 = a*vTri[2].x - b*vTri[2].y;

	float min, max;
	if ( p2 < p1 )	{	min = p2; max = p1; }
	else	{	min = p1; max = p2; }

	float rad = fa * vHalfSize.x + fb * vHalfSize.y;
	if ( min > rad || max <-rad )	return false;

	return true;
}

inline bool AXISTESTZ0(const float &a, const float &b, const float &fa, const float &fb, const MVector3 vTri[], const MVector3 &vHalfSize)
{
	float p0 = a*vTri[0].x - b*vTri[0].y;
	float p1 = a*vTri[1].x - b*vTri[1].y;

	float min, max;
	if ( p0 < p1 )	{	min = p0; max = p1; }
	else	{	min = p1; max = p0; }

	float rad = fa * vHalfSize.x + fb * vHalfSize.y;
	if ( min > rad || max <-rad )	return false;

	return true;
}

inline void FIND_MINMAX(const float &x0,const float &x1, const float &x2, float &min, float &max)
{
	min = max = x0;
	if ( x1 < min )		min = x1;
	if ( x1 > max )		max = x1;
	if ( x2 < min )		min = x2;
	if ( x2 > max )		max = x2;
}

bool IsIntersect( const MBox &aabb, const MVector3 &v0, const MVector3 &v1, const MVector3 &v2)
{
	MVector3 vHalfSize = (aabb.vmax - aabb.vmin) * .5f;
	MVector3 vCenter = aabb.vmin + vHalfSize;

	MVector3 v[3], e[3];
	v[0] = v0 - vCenter;
	v[1] = v1 - vCenter;
	v[2] = v2 - vCenter;

	e[0] = v[1] - v[0];
	e[1] = v[2] - v[1];
	e[2] = v[0] - v[2];

	float fex, fey, fez;

	// 3
	fex = abs(e[0].x);
	fey = abs(e[0].y);
	fez = abs(e[0].z);

	if ( !AXISTESTX01(e[0].z, e[0].y, fez, fey, v, vHalfSize))	return false;
	if ( !AXISTESTY02(e[0].z, e[0].x, fez, fex, v, vHalfSize))	return false;
	if ( !AXISTESTZ12(e[0].y, e[0].x, fey, fex, v, vHalfSize))	return false;

	fex = abs(e[1].x);
	fey = abs(e[1].y);
	fez = abs(e[1].z);

	if ( !AXISTESTX01(e[1].z, e[1].y, fez, fey, v, vHalfSize))	return false;
	if ( !AXISTESTY02(e[1].z, e[1].x, fez, fex, v, vHalfSize))	return false;
	if ( !AXISTESTZ0 (e[1].y, e[1].x, fey, fex, v, vHalfSize))	return false;

	fex = abs(e[2].x);
	fey = abs(e[2].y);
	fez = abs(e[2].z);

	if ( !AXISTESTX2 (e[2].z, e[2].y, fez, fey, v, vHalfSize))	return false;
	if ( !AXISTESTY1 (e[2].z, e[2].x, fez, fex, v, vHalfSize))	return false;
	if ( !AXISTESTZ12(e[2].y, e[2].x, fey, fex, v, vHalfSize))	return false;


	// 1
	float min, max;

	FIND_MINMAX(v[0].x, v[1].x, v[2].x, min, max);
	if ( min > vHalfSize.x || max < -vHalfSize.x )	return false;

	FIND_MINMAX(v[0].y, v[1].y, v[2].y, min, max);
	if ( min > vHalfSize.y || max < -vHalfSize.y )	return false;

	FIND_MINMAX(v[0].z, v[1].z, v[2].z, min, max);
	if ( min > vHalfSize.z || max < -vHalfSize.z )	return false;

	// 2
	MVector3 vNormal = e[0].CrossProduct(e[1]);
	MVector3 vmin, vmax;
	
	for ( int i = 0; i<3; ++i )
	{
		float fv = vHalfSize.v[i];
		float vert = v[0].v[i];

		if ( vNormal.v[i] > 0.0f )
		{
			vmin.v[i] =-fv - vert;
			vmax.v[i] = fv - vert;
		}
		else
		{
			vmin.v[i] = fv - vert;
			vmax.v[i] =-fv - vert;
		}
	}

	if ( vNormal.DotProduct(vmin) > 0.0f )	return false;
	if ( vNormal.DotProduct(vmax) >= 0.0f)	return true;

	return false;
}

// (P1, P2) - (P3, P4)
bool IntersectLine(MVector2 *pOut, const MVector2 &P1, const MVector2 &P2, const MVector2 &P3, const MVector2 &P4)
{
	float denom = ((P4.y - P3.y)*(P2.x - P1.x)) - ((P4.x - P3.x)*(P2.y - P1.y));
	if ( denom == 0.0f )
		return false;

	float nume_a = ((P4.x - P3.x)*(P1.y - P3.y)) - ((P4.y - P3.y)*(P1.x - P3.x));
	float nume_b = ((P2.x - P1.x)*(P1.y - P3.y)) - ((P2.y - P1.y)*(P1.x - P3.x));

	float ua = nume_a / denom;
	float ub = nume_b / denom;

	// Get the intersection point.
	if ( pOut )
	{
		pOut->x = P1.x + ua*(P2.x - P1.x);
		pOut->y = P1.y + ua*(P2.y - P1.y);
	}

	if(ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f)
		return true;

	return false;
}

}