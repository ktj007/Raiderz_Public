#ifndef _MVECTOR4_H
#define _MVECTOR4_H

#include <math.h>
#include "MVector3.h"
#include "RLib.h"

// visual assist 쓸때 MVector3, 등등에 있는 union 안에 있는
// 이름없는 struct를 제대로 인식 못하는 버그 때문에, va forum 에서 가져온 해결책임.
#define ANONYMOUS_STRUCT

class CML2_API MVector4
{
public:
	union {
		struct ANONYMOUS_STRUCT {
			float r, g, b, a;
		} ;
		struct ANONYMOUS_STRUCT {
			float x, y, z, w;
		} ;
		float v[4];
	};

	MVector4() { }
	MVector4(const MVector3 &v3, float ww) : x(v3.x), y(v3.y), z(v3.z), w(ww) {}
	MVector4(float xx, float yy, float zz, float ww): x(xx), y(yy), z(zz), w(ww) { }
	MVector4(const float *f) { x=f[0];y=f[1];z=f[2];w=f[3]; }
	explicit MVector4( unsigned long dw )
	{
		const float f = 1.0f / 255.0f;
		x = f * (float) (unsigned char) (dw >> 16);
		y = f * (float) (unsigned char) (dw >>  8);
		z = f * (float) (unsigned char) (dw >>  0);
		w = f * (float) (unsigned char) (dw >> 24);
	}

	void Set(float xx, float yy, float zz, float ww) {x = xx; y = yy; z = zz; w = ww;}

	MVector4 operator*(const float v) const { return MVector4(x * v, y * v, z * v, w * v);	}
	MVector4& operator*=(const float v) { x*=v; y*=v; z*=v; w*=v; return *this; }

	MVector4 operator/(const float v) const { float i=(float)1.0/v; return MVector4(x * i, y * i, z * i, w * i);	}
	MVector4& operator/=(const float v) { float i=(float)1.0/v; x*=i; y*=i; z*=i; w*= i; return *this; }

	MVector4& operator += ( const MVector4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }

	// binary operators
	MVector4 operator + ( const MVector4& v) const
	{
		return MVector4(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	MVector4& operator -= ( const MVector4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }

	// binary operators
	MVector4 operator - ( const MVector4& v) const
	{
		return MVector4(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	bool operator!=(const MVector4& other) const { return other.x!=x || other.y!=y || other.z!=z || other.w!=w; }

	MVector4 GetInterpolated(const MVector4& other, float d) const
	{
		float inv = 1.0f - d;
		return MVector4(other.x*inv + x*d,
						other.y*inv + y*d,
						other.z*inv + z*d,
						other.w*inv + w*d);
	}

	MVector4 Multiply(const MVector4& other) const
	{
		return MVector4(other.x*x,other.y*y,other.z*z,other.w*w);
	}

	inline float Length(const MVector4 &v)
	{
		return v.Length();
	}

	inline float MVector4::Length() const
	{
		return (float)sqrt(x*x+y*y+z*z+w*w);
	}

	// Dot product of two vector4's
	inline float MVector4::DotProduct(const MVector4& p) 
	{
		return x * p.x + y * p.y + z * p.z + w * p.w;
	}

	inline MVector4 MVector4::CrossProduct(const MVector4& p) const
	{
		// 4차원 벡터는 원래 외적이 없어서 .w는 걍 서로 곱셈해버린다.
		return MVector4(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x, p.w * w);	
	}

    // casting
    operator float* ()				{ return v; }
    operator const float* () const	{ return (const float *) v; }
	operator unsigned long () const
	{
		unsigned long dwR = x >= 1.0f ? 0xff : x <= 0.0f ? 0x00 : (unsigned long) (x * 255.0f + 0.5f);
		unsigned long dwG = y >= 1.0f ? 0xff : y <= 0.0f ? 0x00 : (unsigned long) (y * 255.0f + 0.5f);
		unsigned long dwB = z >= 1.0f ? 0xff : z <= 0.0f ? 0x00 : (unsigned long) (z * 255.0f + 0.5f);
		unsigned long dwA = w >= 1.0f ? 0xff : w <= 0.0f ? 0x00 : (unsigned long) (w * 255.0f + 0.5f);

		return (dwA << 24) | (dwR << 16) | (dwG << 8) | dwB;
	}

	static const MVector4 ZERO;
};

#endif