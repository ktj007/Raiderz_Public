#ifndef _MQUAT_H
#define _MQUAT_H

#include "MMatrix.h"
#include "MVector3.h"

class MQuat
{
public:
	float x, y, z, w;

	MQuat() { }
	MQuat(float xx, float yy, float zz, float ww) {
		x=xx;y=yy;z=zz;w=ww;
	}
	MQuat(const MMatrix& mat);
	MQuat(const MVector3& axis, float fAngle);
	MQuat(const float pitch, const float yaw, const float roll);

	bool operator==(const MQuat& other) const;
	bool operator!=(const MQuat& other) const;
	MQuat& operator=(const MQuat& other);
	MQuat& operator=(const MMatrix& other);
	MQuat operator+(const MQuat& other) const;
	MQuat operator*(const MQuat& other) const;
	MQuat operator*(float s) const;
	MQuat& operator*=(float s);
	MVector3 MQuat::operator* (const MVector3& v) const;
	MVector4 MQuat::operator* (const MVector4& v) const;	// MVector3와의 *연산에 w만 1로 - 090722, OZ
	MQuat& operator*=(const MQuat& other);

	MMatrix Conv2Matrix() const;
	void SetMatrixRotation(MMatrix& m) const;
	float DotProduct(const MQuat& other) const;
	MQuat Slerp(MQuat q1, MQuat q2, float time);
	MQuat Inverse() const;	// unit quaternion 의 inverse, 유닛이 아니면 normalize 요망
	MQuat Conjugate() { return Inverse(); }

	void Normalize();
	MVector3 ToEuler() const;
	void SetEuler(const float pitch, const float yaw, const float roll);

};


// 더블 쿼터니언 사용 하여 스키닝 연산을 수행한다. 
// - 스키닝 이점
// - 전통적인 방식에 쉽게 적용 가능
// - 스키닝 이점
// - GPU friendly
// - 전통적인 매트릭스 방식보다는 연산이 많음
// http://isg.cs.tcd.ie/projects/DualQuaternions
class MDQuat	// 더블 쿼터니언 090701, OZ
{
public:
	MQuat qNoneDualPart;
	MQuat qDualPart;

	MDQuat()	{}
	MDQuat( MQuat q1, MVector3 vTran)
	{
		// 논 튜얼 파트는 그냥 적용
		qNoneDualPart = q1;

		// 듀얼 파트 계산
		qDualPart.x = -0.5f * ( vTran.x*q1.y + vTran.y*q1.z + vTran.z*q1.w);
		qDualPart.y =  0.5f * ( vTran.x*q1.x + vTran.y*q1.w + vTran.z*q1.z);
		qDualPart.z =  0.5f * ( vTran.x*q1.w + vTran.y*q1.x + vTran.z*q1.y);
		qDualPart.w =  0.5f * ( vTran.x*q1.z + vTran.y*q1.y + vTran.z*q1.x);
	}

};


inline MQuat::MQuat(const MMatrix& mat)
{
	(*this) = mat;
}

inline MQuat::MQuat(const MVector3& axis, float fAngle)
{
	float s = sinf(fAngle/2.f);
	x = axis.x * s;
	y = axis.y * s;
	z = axis.z * s;
	w = cosf(fAngle/2.f);
}

inline MQuat::MQuat(const float pitch, const float yaw, const float roll)
{
	SetEuler(pitch, yaw, roll);
}

// m 의 3x3 부분에 회전값을 써넣는다
inline void MQuat::SetMatrixRotation(MMatrix& m) const
{
	float _xx, _yy, _zz, _xy, _xz, _yz, _wx, _wy, _wz;

	_xx = 2.0f * x * x;
	_yy = 2.0f * y * y;
	_zz = 2.0f * z * z;
	_xy = 2.0f * x * y;
	_xz = 2.0f * x * z;
	_yz = 2.0f * y * z;
	_wx = 2.0f * w * x;
	_wy = 2.0f * w * y;
	_wz = 2.0f * w * z;

	m._11 = 1 - (_yy + _zz); 
	m._21 = (_xy - _wz);
	m._31 = (_xz + _wy);

	m._12 = (_xy + _wz);
	m._22 = 1 - (_xx + _zz);
	m._32 = (_yz - _wx);

	m._13 = (_xz - _wy);
	m._23 = (_yz + _wx);
	m._33 = 1 - (_xx + _yy);
}

inline MMatrix MQuat::Conv2Matrix() const
{
	MMatrix m;
	SetMatrixRotation(m);
	m._14 = 0.0f;
	m._24 = 0.0f;
	m._34 = 0.0f;

	m._41 = 0.0f;
	m._42 = 0.0f;
	m._43 = 0.0f;
	m._44 = 1.0f;
	return m;
}

inline bool MQuat::operator==(const MQuat& other) const
{
	if(x != other.x)
		return false;
	if(y != other.y)
		return false;
	if(z != other.z)
		return false;
	if(w != other.w)
		return false;

	return true;
}

inline bool MQuat::operator!=(const MQuat& other) const
{
	if(x != other.x)
		return true;
	if(y != other.y)
		return true;
	if(z != other.z)
		return true;
	if(w != other.w)
		return true;

	return false;
}

inline MQuat& MQuat::operator=(const MQuat& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	w = other.w;
	return *this;
}

inline MQuat& MQuat::operator=(const MMatrix& other)
{
	float t, l, s;

	t = other._11 + other._22+ other._33;

	if (t > 0.0f)
	{
		l = sqrt(t + 1.0f);
		s = 0.5f / l;
		w = l * 0.5f;
		x = (other._23 - other._32) * s;
		y = (other._31 - other._13) * s;
		z = (other._12 - other._21) * s;
	}	else
	{
		switch((other._11 > other._22) ? (other._11 > other._33 ? 0 : 2) : (other._22 > other._33 ? 1 : 2))
		{
			case 0 :
				l = sqrt(1.0f + other._11 - other._22 - other._33);	// 이건 중복되는데, switch 위로 올리는게 나을듯.
				s = 0.5f / l;
				x = l * 0.5f;
				y = (other._12 + other._21) * s;
				z = (other._31 + other._13) * s;
				w = (other._23 - other._32) * s;
				break;

			case 1 :
				l = sqrt(1.0f - other._11 + other._22 - other._33);
				s = 0.5f / l;
				x = (other._21 + other._12) * s;
				y = l * 0.5f;
				z = (other._32 + other._23) * s;
				w = (other._31 - other._13) * s;
				break;

			case 2 :
				l = sqrt(1.0f - other._11 - other._22 + other._33);
				s = 0.5f / l;
				x = (other._13 + other._31) * s;
				y = (other._32 + other._23) * s;
				z = l * 0.5f;
				w = (other._12 - other._21) * s;
				break;
		}
	}
	return *this;
}

inline MQuat MQuat::operator+(const MQuat& other) const
{
	return MQuat(x+other.x, y+other.y, z+other.z, w+other.w);
}

inline MQuat MQuat::operator*(const MQuat& other) const
{
	MQuat tmp;

	tmp.w = (other.w * w) - (other.x * x) - (other.y * y) - (other.z * z);
	tmp.x = (other.w * x) + (other.x * w) + (other.y * z) - (other.z * y);
	tmp.y = (other.w * y) + (other.y * w) + (other.z * x) - (other.x * z);
	tmp.z = (other.w * z) + (other.z * w) + (other.x * y) - (other.y * x);

	return tmp;
}

inline MQuat MQuat::operator*(float s) const
{
	return MQuat(s*x, s*y, s*z, s*w);
}

inline MQuat& MQuat::operator*=(float s)
{
	x *= s; y*=s; z*=s; w*=s;
	return *this;
}

inline MVector3 MQuat::operator*(const MVector3& v) const
{
	MVector3 uv, uuv; 
	MVector3 qvec(x, y, z); 
	uv = qvec.CrossProduct(v); 
	uuv = qvec.CrossProduct(uv); 
	uv *= (2.0f * w); 
	uuv *= 2.0f; 

	return v + uv + uuv; 
}

inline MVector4 MQuat::operator*(const MVector4& v) const	// 090722, OZ
{
	MVector4 uv, uuv; 
	MVector4 qvec(x, y, z, 1); 
	uv = qvec.CrossProduct(v); 
	uuv = qvec.CrossProduct(uv); 
	uv *= (2.0f * w); 
	uuv *= 2.0f; 

	return v + uv + uuv; 
}

inline MQuat& MQuat::operator*=(const MQuat& other)
{
	*this = other * (*this);
	return *this;
}


inline float MQuat::DotProduct(const MQuat& other) const
{
	return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w);
}

inline MQuat MQuat::Slerp(MQuat q1, MQuat q2, float time)
{
    float angle = q1.DotProduct(q2);

    if (angle < 0.0f) 
    {
		q1 *= -1.0f;
		angle *= -1.0f;
    }

	float scale;
	float invscale;

    if ((angle + 1.0f) > 0.05f) 
    {
		if ((1.0f - angle) >= 0.05f)  // spherical interpolation
		{
			float theta = (float)acos(angle);
			float invsintheta = 1.0f / (float)sin(theta);
			scale = (float)sin(theta * (1.0f-time)) * invsintheta;
			invscale = (float)sin(theta * time) * invsintheta;
		}
		else // linear interploation
		{
			scale = 1.0f - time;
			invscale = time;
		}
    }
    else 
    {
        q2 = MQuat(-q1.y, q1.x, -q1.w, q1.z);
        scale = (float)sin(3.14159f * (0.5f - time));
        invscale = (float)sin(3.14159f * time);
    }

	*this = (q1*scale) + (q2*invscale);
	return *this;
}

inline MQuat MQuat::Inverse() const
{
	// unit 이라고 가정
	return MQuat(-x,-y,-z,w);
}

inline void MQuat::Normalize()
{
	float fInvLength = 1.f/sqrt(x * x + y * y + z * z + w * w);
	x *= fInvLength;
	y *= fInvLength;
	z *= fInvLength;
	w *= fInvLength;
}

inline MVector3 MQuat::ToEuler() const
{
	MVector3 result;
	double m00 = 1.0 - (2.0 * ((z*z) + y*y));
	double m01 = 2.0 * (x * z - w * y);

	result.x = (float)atan2( 2.0 * (y * z - w * x), 1.0 - (2.0 * ((x*x) + (z*z))));
	result.y = (float)atan2(m01, m00);
	result.z = (float)atan2(-2.0 * (x * y + w * z), sqrt((m00*m00) + (m01*m01)));

	return result;	
}

inline void MQuat::SetEuler(const float pitch, const float yaw, const float roll)
{
	const float halfPitch	= pitch * 0.5f;
	const float halfYaw		= yaw   * 0.5f;
	const float halfRoll	= roll  * 0.5f;

	const float cY = cos( halfRoll );
	const float sY = sin( halfRoll );
	const float cP = cos( halfPitch );
	const float sP = sin( halfPitch );
	const float cR = cos( halfYaw );
	const float sR = sin( halfYaw );

	x = cY * sP * cR - sY * cP * sR;
	y = cY * cP * sR - sY * sP * cR;
	z = cY * sP * sR + sY * cP * cR;
	w = -(cY * cP * cR + sY * sP * sR);
}


#endif