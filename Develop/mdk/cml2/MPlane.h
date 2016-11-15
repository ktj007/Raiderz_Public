#ifndef _MPLANE_H
#define _MPLANE_H

#include "MVector3.h"

class MPlane
{
public:
	float a, b, c, d;

	MPlane() { }
	MPlane(float _a, float _b, float _c, float _d ) : a(_a), b(_b), c(_c), d(_d) { }
	MPlane(const float *f) { a=f[0]; b=f[1]; c=f[2]; d=f[3]; }
	MPlane(const MVector3& normal, float dis)	// from normal and distance
	{	
		SetPlane(normal,dis);
	}
	MPlane(const MVector3& normal, const MVector3& pos)	// from normal & point
	{
		SetPlane(normal,pos);
	}
	MPlane(const MVector3& point1, const MVector3& point2, const MVector3& point3 ) //	from points
	{
		SetPlane(point1,point2,point3);
	}

	MPlane operator-() const { return MPlane(-a, -b, -c, -d);   }

    // casting
    operator float* ()				{ return &a; }
    operator const float* () const	{ return (const float *) &a; }

	const MVector3& Normal() const {
		return *((MVector3*)&a);
	}

	float GetDistanceTo(const MVector3& point) const
	{
		return point.x*a + point.y*b + point.z*c + d;
	}

	float DotNormal(const MVector3& normal) const
	{
		return Normal().DotProduct(normal);
	}

	bool Intersect(MVector3 &out, const MVector3 &p, const MVector3 &q) const;

	void SetPlane(float _a, float _b, float _c, float _d );
	void SetPlane(const MVector3& normal, float dis);		// from normal and distance
	void SetPlane(const MVector3& normal, const MVector3& pos);	// from normal & point
	void SetPlane(const MVector3& point1, const MVector3& point2, const MVector3& point3); // from points
	void SetPlane(const MVector3& point1, const MVector3& point2, const MVector3& point3,const MVector3& normalDir); // from points and normal이 있어야 하는 방향

	void Normalize();

};

// inline functions
inline void MPlane::SetPlane(float _a, float _b, float _c, float _d )
{
	a = _a;
	b = _b;
	c = _c;
	d = _d;
}

inline void MPlane::SetPlane(const MVector3& normal, float dis)		// from normal and distance
{
	a = normal.x;
	b = normal.y;
	c = normal.z;
	d = dis;
}

inline void MPlane::SetPlane(const MVector3& normal, const MVector3& pos)	// from normal & point
{
	a = normal.x;
	b = normal.y;
	c = normal.z;
	d = - normal.DotProduct(pos);
}

inline void MPlane::SetPlane(const MVector3& point1, const MVector3& point2, const MVector3& point3) // from points
{
	MVector3 normal;
	normal = (point2 - point1).CrossProduct(point3 - point1);
	normal.Normalize();
	a = normal.x;
	b = normal.y;
	c = normal.z;
	d = - normal.DotProduct(point1);
}

inline void MPlane::SetPlane( const MVector3& point1, const MVector3& point2, const MVector3& point3,const MVector3& normalDir )
{
	SetPlane(point1,point2,point3);
	MVector3 n(a,b,c);

	if( n.DotProduct(normalDir) < 0 )//원하는 노말 방향과 평면의 노말 방향이 반대 방향이면
	{//평면의 노말 방향을 뒤짚어 준다.
		a=-a;
		b=-b;
		c=-c;
		d=-d;
	}
}

inline void MPlane::Normalize()
{
	float fInvLength = 1.f/Normal().Length();
	a *= fInvLength;
	b *= fInvLength;
	c *= fInvLength;
	d *= fInvLength;
}

inline bool MPlane::Intersect(MVector3 &out, const MVector3 &p, const MVector3 &q) const
{
	float d1 = GetDistanceTo(p);
	float d2 = GetDistanceTo(q);

	if ( d1*d2 > 0.0f )
		return false;
    
	d1 = fabs(d1);
	d2 = fabs(d2);

	MVector3 vL = q - p;
	out = p + vL*(d1/(d1+d2));

	return true;
}

#endif