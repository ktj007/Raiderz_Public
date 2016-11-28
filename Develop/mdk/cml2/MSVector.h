#ifndef _MSVECTOR_H
#define _MSVECTOR_H

#include "RLib.h"

class MVector3;

// 부동소수점수를 압축한 벡터. size = 6byte
// 오차율이 크므로 너무 큰 값은 사용하면 안된다. 주로 normal vector 등에만 사용하기 바람.
// SEEE EMMM MMMM MMMM
class CML2_API MSVector
{
private:
	unsigned short _32To16(float t)
	{
		int s = ((*((unsigned long*)&t) & 0x80000000) >> 31);
		int e = ((*((unsigned long*)&t) & 0x7F800000) >> 23) - 127;
		int m = (*((unsigned long*)&t) & 0x007FFFFF);

		if (e < -7) return 0;

		return (unsigned short)( (s << 15) | ((e + 7) << 11) | (m >> (23 - 11)) );
	}
	static unsigned int floatToHalfI(unsigned int i)
	{
		int s =  (i >> 16) & 0x00008000;
		int e = ((i >> 23) & 0x000000ff) - (127 - 15);
		int m =   i        & 0x007fffff;

		if (e <= 0)
		{
			if (e < -10)
			{
				return 0;
			}
			m = (m | 0x00800000) >> (1 - e);

			return s | (m >> 13);
		}
		else if (e == 0xff - (127 - 15))
		{
			if (m == 0) // Inf
			{
				return s | 0x7c00;
			} 
			else    // NAN
			{
				m >>= 13;
				return s | 0x7c00 | m | (m == 0);
			}
		}
		else
		{
			if (e > 30) // Overflow
			{
				return s | 0x7c00;
			}

			return s | (e << 10) | (m >> 13);
		}
	}

	static unsigned int halfToFloatI(unsigned short var)
	{
		int s = (var >> 15) & 0x00000001;
		int e = (var >> 10) & 0x0000001f;
		int m =  var        & 0x000003ff;

		if (e == 0)
		{
			if (m == 0) // Plus or minus zero
			{
				return s << 31;
			}
			else // Denormalized number -- renormalize it
			{
				while (!(m & 0x00000400))
				{
					m <<= 1;
					e -=  1;
				}

				e += 1;
				m &= ~0x00000400;
			}
		}
		else if (e == 31)
		{
			if (m == 0) // Inf
			{
				return (s << 31) | 0x7f800000;
			}
			else // NaN
			{
				return (s << 31) | 0x7f800000 | (m << 13);
			}
		}

		e = e + (127 - 15);
		m = m << 13;

		return (s << 31) | (e << 23) | m;
	}

	unsigned short floatToHalf(float i)
	{
		union { float f; unsigned int i; } v;
		v.f = i;
		return floatToHalfI(v.i);
	}

	float halfToFloat(unsigned short var)
    {
        union { float f; unsigned int i; } v;
        v.i = halfToFloatI(var);
        return v.f;
	}
	union {
		struct {
			unsigned short m_x, m_y, m_z;
		} ;
		unsigned short m_v[3];
	};
public:
	MSVector() { }
	MSVector(const MSVector& other) : m_x(other.m_x), m_y(other.m_y), m_z(other.m_z) {}
	MSVector(float x, float y, float z);
	MSVector(float *f);
	MSVector(const float *f);

	void SetX(float x) { m_x = _32To16(x); }
	void SetY(float y) { m_y = _32To16(y); }
	void SetZ(float z) { m_z = _32To16(z); }
	float X() { return halfToFloat(m_x); }
	float Y() { return halfToFloat(m_y); }
	float Z() { return halfToFloat(m_z); }

	bool IsEqual(const MSVector& other) const;

	MSVector& operator=(const MSVector& other)	{ m_x = other.m_x; m_y = other.m_y; m_z = other.m_z; return *this; }
	MSVector& operator=(const MVector3& other);
	bool operator==(const MSVector& other) const { return IsEqual(other); }
};

// 2차원 MSVector
class CML2_API MSVector2
{
private:
	unsigned short _32To16(float t)
	{
		int s = ((*((unsigned long*)&t) & 0x80000000) >> 31);
		int e = ((*((unsigned long*)&t) & 0x7F800000) >> 23) - 127;
		int m = (*((unsigned long*)&t) & 0x007FFFFF);

		if (e < -7) return 0;

		return (unsigned short)( (s << 15) | ((e + 7) << 11) | (m >> (23 - 11)) );
	}
	float _16To32(unsigned short t)
	{
		int s = ((t & 0x8000) >> 15);
		int e = ((t & 0x7800) >> 11) - 7;
		int m = (t & 0x007FF);

		if (t == 0) return 0.0f;

		unsigned long f = (s << 31) | ((e + 127) << 23) | (m << (23 - 11));
		return *((float*)&f);
	}
	union {
		struct {
			unsigned short m_x, m_y;
		} ;
		unsigned short m_v[2];
	};
public:
	MSVector2() { }
	MSVector2(const MSVector& other) : m_x(other.m_x), m_y(other.m_y) {}
	MSVector2(float x, float y);
	MSVector2(float *f);
	MSVector2(const float *f);

	void SetX(float x) { m_x = _32To16(x); }
	void SetY(float y) { m_y = _32To16(y); }
	float X() { return _16To32(m_x); }
	float Y() { return _16To32(m_y); }

	bool IsEqual(const MSVector2& other) const;

	MSVector2& operator=(const MSVector2& other)	{ m_x = other.m_x; m_y = other.m_y; return *this; }
	MSVector2& operator=(const MVector3& other);
	bool operator==(const MSVector2& other) const { return IsEqual(other); }
};

#endif