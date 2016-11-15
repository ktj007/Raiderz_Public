#ifndef _MVECTOR2_H
#define _MVECTOR2_H

#include <math.h>

// visual assist 쓸때 MVector3, 등등에 있는 union 안에 있는
// 이름없는 struct를 제대로 인식 못하는 버그 때문에, va forum 에서 가져온 해결책임.
#define ANONYMOUS_STRUCT

class MVector2
{
public:
	union {
		struct ANONYMOUS_STRUCT {
			float x, y;
		} ;
		float v[2];
	};

    MVector2() {};
	MVector2( const float *pf )		{	x = pf[0];y = pf[1]; }
	MVector2( float fx, float fy )	{   x = fx;y = fy; }

    // casting
    operator float* ()				{ return (float *) &x; }
    operator const float* () const	{ return (const float *) &x; }

    // assignment operators
	MVector2& operator += ( const MVector2& v ) {	x += v.x; y += v.y; return *this; }
	MVector2& operator -= ( const MVector2& v ) {	x -= v.x; y -= v.y; return *this; }

    MVector2& operator *= ( float f )	{ x *= f; y *= f; return *this; }
    MVector2& operator /= ( float f )	{ float fInv = 1.0f / f; x *= fInv; y *= fInv; return *this; }

    // unary operators
    MVector2 operator + () const { return *this; }
    MVector2 operator - () const { return MVector2(-x, -y); }

    // binary operators
    MVector2 operator + ( const MVector2& v ) const { return MVector2(x + v.x, y + v.y); }
    MVector2 operator - ( const MVector2& v ) const { return MVector2(x - v.x, y - v.y); }
    MVector2 operator * ( float f ) const { return MVector2(x * f, y * f); }
    MVector2 operator / ( float f ) const { float fInv = 1.0f / f; return MVector2(x * fInv, y * fInv); }

    friend MVector2 operator * ( float f, const MVector2& v ) { return MVector2(f * v.x, f * v.y); }

    bool operator == ( const MVector2& v ) const { return x == v.x && y == v.y; }
    bool operator != ( const MVector2& v ) const { return x != v.x || y != v.y; }

	float Length() const;
	float LengthSq() const;
	float DotProduct(const MVector2& other) const;
	float CrossProduct(const MVector2& other ) const
	{
		return x * other.y - y * other.x;
	}
	MVector2& Normalize();
	

};

inline float Length(const MVector2 &v)
{
	return v.Length();
}

inline float LengthSq(const MVector2 &v)
{
	return v.LengthSq();
}

inline float MVector2::Length() const
{
	return (float)sqrt(x*x+y*y);
}

inline float MVector2::LengthSq() const
{
	return (x*x+y*y);
}

inline float MVector2::DotProduct(const MVector2& other) const
{
	return x*other.x + y*other.y;
}

inline MVector2& MVector2::Normalize()
{
	float scale = (float)Length();

	if (scale == 0)
		return *this;

	scale = (float)1.0f / scale;
	x *= scale;
	y *= scale;
	return *this;
}

#endif