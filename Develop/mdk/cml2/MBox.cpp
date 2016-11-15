#include "stdafx.h"
#include "MSphere.h"

const MBox MBox::INFINITY( -FLT_MAX, -FLT_MAX, -FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX );
const MBox MBox::INVALID( FLT_MAX, FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX );

/**
 이 .cpp 를 하나의 네임스페이스로 사용합니다.
 매크로 사용하기 싫어서, 이렇게 만들었습니다.
*/

// source code from http://www.cs.lth.se/home/Tomas_Akenine_Moller/code/tribox3.txt
float fex, fey, fez;
MVector3 v0;
MVector3 v1;
MVector3 v2;

MVector3 e0;
MVector3 e1;
MVector3 e2;

MVector3 center;
MVector3 halfsize;

//////////////////////////////////////////////////////////////////////////
// X - Test
inline bool AXISTEST_X01(float &a, float &b, float &fa, float &fb)
{
	float p0 = a*v0.y - b*v0.z;
	float p2 = a*v2.y - b*v2.z;
	float minf, maxf;
	if (p0<p2)
	{	minf = p0; maxf = p2; }
	else
	{	minf = p2; maxf = p0; }
	float rad = fa*halfsize.y * fb*halfsize.z;
	if ( minf>rad || maxf<-rad)
		return false;
	return true;
};

inline bool AXISTEST_X2(float &a, float &b, float &fa, float &fb)
{
	float p0 = a*v0.y - b*v0.z;
	float p1 = a*v1.y - b*v1.z;
	float minf, maxf;
	if (p0<p1)
	{	minf = p0; maxf = p1; }
	else
	{	minf = p1; maxf = p0; }

	float rad = fa*halfsize.y + fb*halfsize.z;
	if ( minf>rad || maxf<-rad)
		return false;
	return true;
}

///////////////////////////////////////////////////////////////////////////
// Y - Test
inline bool AXISTEST_Y02(float &a, float &b, float &fa, float &fb)
{
	float p0 =-a*v0.x + b*v0.z;
	float p2 =-a*v2.x + b*v2.z;
	float minf, maxf;
	if (p0<p2)
	{	minf = p0; maxf = p2; }
	else
	{	minf = p2; maxf = p0; }

	float rad = fa*halfsize.x + fb*halfsize.z;
	if ( minf>rad || maxf<-rad)
		return false;
	return true;
}

inline bool AXISTEST_Y1(float &a, float &b, float &fa, float &fb)
{
	float p0 =-a*v0.x + b*v0.z;
	float p1 =-a*v1.x + b*v1.z;
	float minf, maxf;
	if (p0<p1)
	{	minf = p0; maxf = p1; }
	else
	{	minf = p1; maxf = p0; }

	float rad = fa*halfsize.x + fb*halfsize.z;
	if ( minf>rad || maxf<-rad)
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Z - Test

inline bool AXISTEST_Z12(float &a, float &b, float &fa, float &fb)
{
	float p1 = a*v1.x - b*v1.y;
	float p2 = a*v2.x - b*v2.y;
	float minf, maxf;
	if (p1<p2)
	{	minf = p1; maxf = p2; }
	else
	{	minf = p2; maxf = p1; }
	float rad = fa*halfsize.x + fb*halfsize.y;
	if ( minf>rad || maxf<-rad)
		return false;
	return true;
}

inline bool AXISTEST_Z0(float &a, float &b, float &fa, float &fb)
{
	float p0 = a*v0.x - b*v0.y;
	float p1 = a*v1.x - b*v1.y;
	float minf, maxf;
	if (p0<p1)
	{	minf = p0; maxf = p1; }
	else
	{	minf = p1; maxf = p0; }

	float rad = fa*halfsize.x + fb*halfsize.y;
	if ( minf>rad || maxf<-rad)
		return false;
	return true;
}

inline void FindMinMax(float x0, float x1, float x2, float &minf, float &maxf)
{
	minf = maxf = x0;
	minf = min(minf, x1);
	minf = min(minf, x2);
	maxf = max(maxf, x1);
	maxf = max(maxf, x2);
}


bool PlaneBoxOVerlap(MVector3 &normal, MVector3 &vert, MVector3 &h)
{
	MVector3 vmin, vmax;

	for ( int q = 0; q<3; ++q )
	{
		float v = vert.v[q];
		if (normal.v[q]>0.0f)
		{
			vmin.v[q] =-h.v[q] - v;
			vmax.v[q] = h.v[q] - v; 
		}
		else
		{
			vmin.v[q] = h.v[q] - v;
			vmax.v[q] =-h.v[q] - v; 
		}
	}

	if ( normal.DotProduct(vmin) > 0.0f)	return false;
	if ( normal.DotProduct(vmax) >=0.0f)	return true;

	return false;
}

bool MBox::ContainsPolygon(const MVector3 &tri0, const MVector3 &tri1, const MVector3 &tri2)
{
	center = (vmax + vmin) * 0.5f;
	halfsize = center - vmin;

	v0 = tri0 - center;
	v1 = tri1 - center;
	v2 = tri2 - center;

	e0 = v1 - v0;
	e1 = v2 - v1;
	e2 = v0 - v2;

	// Bullet 3
	fex = abs(e0.x);
	fey = abs(e0.y);
	fez = abs(e0.z);

	if (!AXISTEST_X01(e0.z, e0.y, fez, fey))	return false;
	if (!AXISTEST_Y02(e0.z, e0.x, fez, fex))	return false;
	if (!AXISTEST_Z12(e0.y, e0.x, fey, fex))	return false;

	fex = abs(e1.x);
	fey = abs(e1.y);
	fez = abs(e1.z);

	if (!AXISTEST_X01(e1.z, e1.y, fez, fey))	return false;
	if (!AXISTEST_Y02(e1.z, e1.x, fez, fex))	return false;
	if (!AXISTEST_Z0 (e1.y, e1.x, fey, fex))	return false;

	fex = abs(e2.x);
	fey = abs(e2.y);
	fez = abs(e2.z);

	if (!AXISTEST_X2(e2.z, e2.y, fez, fey))		return false;
	if (!AXISTEST_Y1(e2.z, e2.x, fez, fex))		return false;
	if (!AXISTEST_Z12(e2.y, e2.x, fey, fex))	return false;
	
	// Bullet 1
	float minf, maxf;
	
	FindMinMax(v0.x, v1.x, v2.x, minf, maxf);
	if ( minf>halfsize.x || maxf<-halfsize.x)	return false;
	
	FindMinMax(v0.y, v1.y, v2.y, minf, maxf);
	if ( minf>halfsize.y || maxf<-halfsize.y)	return false;

	FindMinMax(v0.z, v1.z, v2.z, minf, maxf);
	if ( minf>halfsize.z || maxf<-halfsize.z)	return false;

	// Bullet 2
	MVector3 normal;
	normal = e0.CrossProduct(e1);

	if ( !PlaneBoxOVerlap(normal, v0, halfsize))
		return false;

	return true;
}