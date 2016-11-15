#pragma once

#include "RLib.h"
#include "MVector3.h"
#include "MPlane.h"
#include <string.h>

class CML2_API MMatrix3
{
public:
	union {
		struct {
			float _11, _12, _13;
			float _21, _22, _23;
			float _31, _32, _33;
		} ;
		float _m[3][3];
		float m[9];
	};

	MMatrix3() {  }
	MMatrix3(float *f);
	MMatrix3(float (*f)[3]);
	MMatrix3( float f11, float f12, float f13,
			float f21, float f22, float f23, 
			float f31, float f32, float f33 );

	MMatrix3& operator=(const MMatrix3& other);
	bool operator==(const MMatrix3& other) const;
	bool operator!=(const MMatrix3& other) const;
	MMatrix3& operator*=(const MMatrix3& other);
	MMatrix3 operator*(const MMatrix3& other) const;
	MMatrix3 operator-(const MMatrix3& other) const;

	MMatrix3 operator*(float f) const;
	float& operator()(int row, int col) { return m[col * 3 + row]; }
	const float& operator()(int row, int col) const {  return m[col * 3 + row]; }

	// casting
    operator float* ()				{ return m; }
    operator const float* () const	{ return (const float *) m; }

	// transform, mvector * MMatrix3 로 대체가능.
	friend MVector3 operator * ( const MVector3& v, const MMatrix3& tm);

	void TransformVect( MVector3& vect) const;
	void TransformVect( const MVector3& in, MVector3& out) const;
	void MultiplyTo( const MMatrix3& other , MMatrix3& out) const; // out = this * other;

	void MakeIdentity();
	bool GetInverse( MMatrix3 *pOut, float *fDet ) const;

	MMatrix3 GetTranspose() const;
	static const MMatrix3 IDENTITY;
};


inline MMatrix3::MMatrix3(float *f)
{
	memcpy(&_11, f, sizeof(float)*3*3);
}

inline MMatrix3::MMatrix3(float (*f)[3])
{
	for(int i=0; i<3; i++)
		memcpy(&_11 + 3*i, f[i], sizeof(float)*3);
}

inline MMatrix3::MMatrix3( float f11, float f12, float f13,
						float f21, float f22, float f23,
						float f31, float f32, float f33)
{
    _11 = f11; _12 = f12; _13 = f13;
    _21 = f21; _22 = f22; _23 = f23;
    _31 = f31; _32 = f32; _33 = f33;
}


inline MMatrix3& MMatrix3::operator=(const MMatrix3& other)
{
	for (int i=0; i<9; i++)
	{
		MMatrix3::m[i] = other.m[i];
	}
	return *this;
}

inline bool MMatrix3::operator==(const MMatrix3& other) const
{
	for (int i=0; i<9; i++)
	{
		if (MMatrix3::m[i] != other.m[i]) return false;
	}
	return true;
}

inline bool MMatrix3::operator!=(const MMatrix3& other) const
{
	return !(*this == other);
}

inline void MMatrix3::MultiplyTo( const MMatrix3& other , MMatrix3& out) const // out = this * other;
{
	const MMatrix3& m1 = *this, &m2 = other;

	out._11 = m1._11*m2._11 + m1._12*m2._21 + m1._13*m2._31;
	out._12 = m1._11*m2._12 + m1._12*m2._22 + m1._13*m2._32;
	out._13 = m1._11*m2._13 + m1._12*m2._23 + m1._13*m2._33;
	out._21 = m1._21*m2._11 + m1._22*m2._21 + m1._23*m2._31;
	out._22 = m1._21*m2._12 + m1._22*m2._22 + m1._23*m2._32;
	out._23 = m1._21*m2._13 + m1._22*m2._23 + m1._23*m2._33;
	out._31 = m1._31*m2._11 + m1._32*m2._21 + m1._33*m2._31;
	out._32 = m1._31*m2._12 + m1._32*m2._22 + m1._33*m2._32;
	out._33 = m1._31*m2._13 + m1._32*m2._23 + m1._33*m2._33;
}

inline MMatrix3& MMatrix3::operator*=(const MMatrix3& other)
{
	MMatrix3 newMatrix;
	this->MultiplyTo(other,newMatrix);
	for(int i=0;i<9;i++)
	{
		m[i] = newMatrix.m[i];
	}
	return *this;
}

inline MMatrix3 MMatrix3::operator*(const MMatrix3& other) const
{
	MMatrix3 tmtrx;
	this->MultiplyTo(other, tmtrx);
	return tmtrx;
}

inline MMatrix3 MMatrix3::operator-(const MMatrix3& other) const
{
	MMatrix3 mat;
	for(int i=0;i<9;i++)
	{
		mat.m[i] = this->m[i] - other.m[i];
	}
	return mat;
}

inline MMatrix3 MMatrix3::operator*(float f) const
{
	MMatrix3 mat;
	for(int i=0;i<9;i++)
		mat[i]=m[i]*f;
	return mat;
}

inline void MMatrix3::MakeIdentity()
{
	for (int i = 0; i < 9; i++)
		m[i] = 0.0f;

	_11 = _22 = _33 = 1.0f;
}

inline void MMatrix3::TransformVect( MVector3& vect) const
{
	float vector[3];

	vector[0] = vect.x*_11 + vect.y*_21 + vect.z*_31;
	vector[1] = vect.x*_12 + vect.y*_22 + vect.z*_32;
	vector[2] = vect.x*_13 + vect.y*_23 + vect.z*_33;

	vect.x = vector[0];
	vect.y = vector[1];
	vect.z = vector[2];
}

inline void MMatrix3::TransformVect( const MVector3& in, MVector3& out) const
{
	out.x = in.x*_11 + in.y*_21 + in.z*_31;
	out.y = in.x*_12 + in.y*_22 + in.z*_32;
	out.z = in.x*_13 + in.y*_23 + in.z*_33;
}


inline MMatrix3 MMatrix3::GetTranspose() const
{
	MMatrix3 mat;
	mat._11 = this->_11;
	mat._12 = this->_21;
	mat._13 = this->_31;
	mat._21 = this->_12;
	mat._22 = this->_22;
	mat._23 = this->_32;
	mat._31 = this->_13;
	mat._32 = this->_23;
	mat._33 = this->_33;
	return mat;
}

inline MVector3 operator * ( const MVector3& v, const MMatrix3& tm)
{ 
	MVector3 ret; 
	tm.TransformVect(v,ret); 
	return ret; 
}



