#ifndef _MMATRIX_H
#define _MMATRIX_H

#include "RLib.h"
#include "MVector3.h"
#include "MVector4.h"
#include "MPlane.h"
#include <string.h>
#include <assert.h>
#include <xmmintrin.h>
#include <intrin.h>

#pragma intrinsic ( _mm_hadd_ps )

class MQuat;

// visual assist 쓸때 MVector3, 등등에 있는 union 안에 있는
// 이름없는 struct를 제대로 인식 못하는 버그 때문에, va forum 에서 가져온 해결책임.
#define ANONYMOUS_STRUCT

//__declspec(align(16)) 
class CML2_API MMatrix
{
public:
	union {
		struct ANONYMOUS_STRUCT {
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		} ;
		float _m[4][4];
		float m[16];
	};

	MMatrix()
	{
	}
	MMatrix(const float *f);
	MMatrix(const float (*f)[4]);
	MMatrix( float f11, float f12, float f13, float f14,
			float f21, float f22, float f23, float f24,
			float f31, float f32, float f33, float f34,
			float f41, float f42, float f43, float f44 );

	MMatrix& operator=(const MMatrix& other);
	bool operator==(const MMatrix& other) const;
	bool operator!=(const MMatrix& other) const;
	MMatrix& operator*=(const MMatrix& other);
	MMatrix operator*(const MMatrix& other) const;
	MMatrix operator*(float f) const;
	MMatrix operator-(const MMatrix& other) const;	// 090716, OZ
	MMatrix operator+(const MMatrix& other) const;	// 090716, OZ

	// casting
    operator float* ()				{ return m; }
    operator const float* () const	{ return (const float *) m; }

	// transform, mvector * mmatrix 로 대체가능.
	friend MVector3 operator * ( const MVector3& v, const MMatrix& tm);
	friend MVector4 operator * ( const MVector4& v, const MMatrix& tm);

	void TransformVect( MVector3& vect) const;
	void TransformVectHomogeneous( MVector3& vect) const;
	void TransformVect( const MVector3& in, MVector3& out) const;
	void TransformVect( const MVector3& in, MVector4& out) const;
	void TransformNormal( MVector3& vect) const;
	void TransformNormal( const MVector3& in, MVector3& out) const;
	void TransformPlane( const MPlane& in, MPlane& out) const;

	void MultiplyTo( const MMatrix& other , MMatrix& out) const; // out = this * other;

	void MakeZero();
	void MakeIdentity();
	
	// 콤포넌트만 세팅
	void SetTranslation(const MVector3& trans);
	void SetInverseTranslation( const MVector3& trans);
	MVector3 GetTranslation() const;
	void SetRotationRadians( const MVector3& rotation);
	void SetRotationDegrees( const MVector3& rotation);
	void SetScale( const MVector3& scale);
	void SetScale(float s);
	void SetRotationX(float fRadian);
	void SetRotationY(float fRadian);
	void SetRotationZ(float fRadian);

	// matrix 전체를 세팅해주는 헬퍼펑션들
	void SetRotationMatrix(float x,float y,float z)		{ SetRotationMatrix(MVector3(x,y,z)); }
	void SetRotationMatrix(const MVector3& rotation);// 단위 : 라디안
	void SetTranslationMatrix(float x,float y,float z)	{ SetTranslationMatrix(MVector3(x,y,z)); }
	void SetTranslationMatrix(const MVector3& trans);
	void SetScaleMatrix(float x,float y,float z)		{ SetScaleMatrix(MVector3(x,y,z)); }
	void SetScaleMatrix(const MVector3& scale);
	void SetProjectionMatrixRH(float w, float h, float zNear, float zFar);
	void SetProjectionMatrixLH(float w, float h, float zNear, float zFar);
	void SetProjectionMatrixFovRH(float fFOVy, float fAspectRatio, float zNear, float zFar);
	void SetProjectionMatrixFovLH(float fFOVy, float fAspectRatio, float zNear, float zFar);
	void SetLookAtMatrixLH(const MVector3& position, const MVector3& target, const MVector3& upVector);
	void SetLookAtMatrixRH(const MVector3& position, const MVector3& target, const MVector3& upVector);
	void SetOrthoLH(float w, float h, float zn, float zf);
	void SetOrthoRH(float w, float h, float zn, float zf);
	void SetOrthoOffCenterRH(float l, float r, float b, float t, float zn, float zf);
	void SetOrthoOffCenterLH(float l, float r, float b, float t, float zn, float zf);

	void SetRotationMatrixAxis(const MVector3& axis, float radian);
	void SetRotationYawPitchRoll(float yaw, float pitch, float roll);	// 테스트 되지 않았음.
	void SetLocalMatrix(const MVector3& position, const MVector3& dir, const MVector3& up);
	void SetLocalMatrix(const MVector3& position, const MVector3& dir, const MVector3& up, const MVector3& right);

	void SetScreenSpaceMatrix(DWORD dwScreenWidth, DWORD dwScreenHeight);

	bool GetInverse( MMatrix *pOut, float *fDet ) const;
	MMatrix GetTranspose() const;

	/// 행렬을 scale,translation,rotation 으로 분해
	bool Decompose(MVector3& vecScale, MVector3& vecTranslation, MQuat& quatRotation) const;
	// 셰이더로 넘겨주는 값으로 사용 목적 시 Vector4가 편하다. Decompose Vector4짜리 - 090722, OZ
	bool Decompose(MVector3& vecScale, MVector4& vecTranslation, MQuat& quatRotation) const;	
	// 스케일을 얻지 않는 Decompose() - 090724, OZ
	bool Decompose(MVector3& vecTranslation, MQuat& quatRotation) const;	

	/// 행렬의 scale 값을 얻어낸다
	void ExtractScale(MVector3& vecScale) const;

	static const MMatrix IDENTITY;
};

//////////////////////////////////////////////////////////////////////////
// Aligned 16 Matrix (for SSE)
__declspec(align(16)) 
class CML2_API MMatrixA16 : public MMatrix
{
public:
	MMatrixA16& operator=(const MMatrixA16& other);
	MMatrixA16& operator=(const MMatrix& other);

	void MultiplyTo( const MMatrixA16& other , MMatrixA16& out) const; // out = this * other;
};

inline MMatrixA16& MMatrixA16::operator=(const MMatrixA16& other)
{
	__asm
	{
		mov esi, other;
		mov edi, this;
		movdqa xmm0, xmmword ptr [esi];
		movdqa xmm1, xmmword ptr [esi + 0x10];
		movdqa xmm2, xmmword ptr [esi + 0x20];
		movdqa xmm3, xmmword ptr [esi + 0x30];

		movdqa xmmword ptr [edi], xmm0;
		movdqa xmmword ptr [edi + 0x10], xmm1;
		movdqa xmmword ptr [edi + 0x20], xmm2;
		movdqa xmmword ptr [edi + 0x30], xmm3;
	}
	//memcpy(&m, &other.m, sizeof(float)*16);
	return *this;
}

inline MMatrixA16& MMatrixA16::operator=(const MMatrix& other)
{
	__asm
	{
		mov esi, other;
		mov edi, this;
		movdqu xmm0, xmmword ptr [esi];
		movdqu xmm1, xmmword ptr [esi + 0x10];
		movdqu xmm2, xmmword ptr [esi + 0x20];
		movdqu xmm3, xmmword ptr [esi + 0x30];

		movdqa xmmword ptr [edi], xmm0;
		movdqa xmmword ptr [edi + 0x10], xmm1;
		movdqa xmmword ptr [edi + 0x20], xmm2;
		movdqa xmmword ptr [edi + 0x30], xmm3;
	}
	//memcpy(&m, &other.m, sizeof(float)*16);
	return *this;
}

inline void MMatrixA16::MultiplyTo( const MMatrixA16& other, MMatrixA16& mOut) const
{
	__asm
	{
		mov		   eax,	   mOut	   	   // dst
		mov		   ecx,	   other	  	   // src1
		mov		   edx,	   this	  	   // src2

		movaps	 xmm0,	  xmmword ptr [ecx]	  	   // xmm0 = src1[00, 01, 02, 03]
		movaps	 xmm1,	  xmmword ptr [ecx + 0x10]	   // xmm1 = src1[04, 05, 06, 07]
		movaps	 xmm2,	  xmmword ptr [ecx + 0x20]	   // xmm2 = src1[08, 09, 10, 11]
		movaps	 xmm3,	  xmmword ptr [ecx + 0x30]	   // xmm3 = src1[12, 13, 14, 15]

		movss	  xmm7,	  dword ptr [edx]		   // xmm7 = src2[00, xx, xx, xx]
		movss	  xmm4,	  dword ptr [edx + 0x4]	  // xmm4 = src2[01, xx, xx, xx]
		movss	  xmm5,	  dword ptr [edx + 0x8]	  // xmm5 = src2[02, xx, xx, xx]
		movss	  xmm6,	  dword ptr [edx + 0xc]	  	   // xmm6 = src2[03, xx, xx, xx]

		shufps	 xmm7,	  xmm7, 0x0	  	   // xmm7 = src2[00, 00, 00, 00]
		shufps	 xmm4,	  xmm4, 0x0	  	   // xmm4 = src2[01, 01, 01, 01]
		shufps	 xmm5,	  xmm5, 0x0	  	   // xmm5 = src2[02, 02, 02, 02]
		shufps	 xmm6,	  xmm6, 0x0	  	   // xmm6 = src2[03, 03, 03, 03]

		mulps	  xmm7,	  xmm0	   	   	   // xmm7 *= xmm0
		mulps	  xmm4,	  xmm1	   	   	   // xmm4 *= xmm1
		mulps	  xmm5,	  xmm2	   	   	   // xmm5 *= xmm2
		mulps	  xmm6,	  xmm3	   	   	   // xmm6 *= xmm3

		addps	  xmm7,	  xmm4	   	   	   // xmm7 += xmm4
		addps	  xmm7,	  xmm5	   	   	   // xmm7 += xmm5
		addps	  xmm7,	  xmm6	   	   	   // xmm7 += xmm6

		movaps	 xmmword ptr [eax], xmm7		   // eax = xmm7

		movss	  xmm7,	  dword ptr [edx + 0x10]	 // xmm7 = src2[04, xx, xx, xx]
		movss	  xmm4,	  dword ptr [edx + 0x14]	 // xmm4 = src2[05, xx, xx, xx]
		movss	  xmm5,	  dword ptr [edx + 0x18]	 // xmm5 = src2[06, xx, xx, xx]
		movss	  xmm6,	  dword ptr [edx + 0x1c]	 // xmm6 = src2[07, xx, xx, xx]

		shufps	 xmm7,	  xmm7, 0x0	  	   // xmm7 = src2[04, 04, 04, 04]
		shufps	 xmm4,	  xmm4, 0x0	  	   // xmm4 = src2[05, 05, 05, 05]
		shufps	 xmm5,	  xmm5, 0x0	  	   // xmm5 = src2[06, 06, 06, 06]
		shufps	 xmm6,	  xmm6, 0x0	  	   // xmm6 = src2[07, 07, 07, 07]

		mulps	  xmm7,	  xmm0	   	   	   // xmm7 *= xmm0
		mulps	  xmm4,	  xmm1	   	   	   // xmm4 *= xmm1
		mulps	  xmm5,	  xmm2	   	   	   // xmm5 *= xmm2
		mulps	  xmm6,	  xmm3	   	   	   // xmm6 *= xmm3

		addps	  xmm7,	  xmm4	   	   	   // xmm7 += xmm4
		addps	  xmm7,	  xmm5	   	   	   // xmm7 += xmm5
		addps	  xmm7,	  xmm6	   	   	   // xmm7 += xmm6

		movaps	 xmmword ptr [eax + 0x10], xmm7	 // eax = xmm7

		movss	  xmm7,	  dword ptr [edx + 0x20]	 // xmm7 = src2[08, xx, xx, xx]
		movss	  xmm4,	  dword ptr [edx + 0x24]	 // xmm4 = src2[09, xx, xx, xx]
		movss	  xmm5,	  dword ptr [edx + 0x28]	 // xmm5 = src2[10, xx, xx, xx]
		movss	  xmm6,	  dword ptr [edx + 0x2c]	 // xmm6 = src2[11, xx, xx, xx]

		shufps	 xmm7,	  xmm7, 0x0	  	   // xmm7 = src2[08, 08, 08, 08]
		shufps	 xmm4,	  xmm4, 0x0	  	   // xmm4 = src2[09, 09, 09, 09]
		shufps	 xmm5,	  xmm5, 0x0	  	   // xmm5 = src2[10, 10, 10, 10]
		shufps	 xmm6,	  xmm6, 0x0	  	   // xmm6 = src2[11, 11, 11, 11]

		mulps	  xmm7,	  xmm0	   	   	   // xmm7 *= xmm0
		mulps	  xmm4,	  xmm1	   	   	   // xmm4 *= xmm1
		mulps	  xmm5,	  xmm2	   	   	   // xmm5 *= xmm2
		mulps	  xmm6,	  xmm3	   	   	   // xmm6 *= xmm3

		addps	  xmm7,	  xmm4	   	   	   // xmm7 += xmm4
		addps	  xmm7,	  xmm5	   	   	   // xmm7 += xmm5
		addps	  xmm7,	  xmm6	   	   	   // xmm7 += xmm6

		movaps	 xmmword ptr [eax + 0x20], xmm7	 // eax = xmm7

		movss	  xmm7,	  dword ptr [edx + 0x30]	 // xmm7 = src2[12, xx, xx, xx]
		movss	  xmm4,	  dword ptr [edx + 0x34]	 // xmm4 = src2[13, xx, xx, xx]
		movss	  xmm5,	  dword ptr [edx + 0x38]	 // xmm5 = src2[14, xx, xx, xx]
		movss	  xmm6,	  dword ptr [edx + 0x3c]	 // xmm6 = src2[15, xx, xx, xx]

		shufps	 xmm7,	  xmm7, 0x0	  	   // xmm7 = src2[12, 12, 12, 12]
		shufps	 xmm4,	  xmm4, 0x0	  	   // xmm4 = src2[13, 13, 13, 13]
		shufps	 xmm5,	  xmm5, 0x0	  	   // xmm5 = src2[14, 14, 14, 14]
		shufps	 xmm6,	  xmm6, 0x0	  	   // xmm6 = src2[15, 15, 15, 15]

		mulps	  xmm7,	  xmm0	   	   	   // xmm7 *= xmm0
		mulps	  xmm4,	  xmm1	   	   	   // xmm4 *= xmm1
		mulps	  xmm5,	  xmm2	   	   	   // xmm5 *= xmm2
		mulps	  xmm6,	  xmm3	   	   	   // xmm6 *= xmm3

		addps	  xmm7,	  xmm4	   	   	   // xmm7 += xmm4
		addps	  xmm7,	  xmm5	   	   	   // xmm7 += xmm5
		addps	  xmm7,	  xmm6	   	   	   // xmm7 += xmm6

		movaps	 xmmword ptr [eax + 0x30], xmm7	 // eax = xmm7
	}
}

//////////////////////////////////////////////////////////////////////////

inline MMatrix::MMatrix(const float *f)
{
	memcpy(&_11, f, sizeof(float)*4*4);
}

inline MMatrix::MMatrix(const float (*f)[4])
{
	for(int i=0; i<4; i++)
		memcpy(&_11 + 4*i, f[i], sizeof(float)*4);
}

inline MMatrix::MMatrix( float f11, float f12, float f13, float f14,
                        float f21, float f22, float f23, float f24,
                        float f31, float f32, float f33, float f34,
                        float f41, float f42, float f43, float f44 )
{
    _11 = f11; _12 = f12; _13 = f13; _14 = f14;
    _21 = f21; _22 = f22; _23 = f23; _24 = f24;
    _31 = f31; _32 = f32; _33 = f33; _34 = f34;
    _41 = f41; _42 = f42; _43 = f43; _44 = f44;
}


inline MMatrix& MMatrix::operator=(const MMatrix& other)
{
	__asm
	{
		mov esi, other;
		mov edi, this;
		movdqu xmm0, xmmword ptr [esi];
		movdqu xmm1, xmmword ptr [esi + 0x10];
		movdqu xmm2, xmmword ptr [esi + 0x20];
		movdqu xmm3, xmmword ptr [esi + 0x30];

		movdqu xmmword ptr [edi], xmm0;
		movdqu xmmword ptr [edi + 0x10], xmm1;
		movdqu xmmword ptr [edi + 0x20], xmm2;
		movdqu xmmword ptr [edi + 0x30], xmm3;
	}
	return *this;
}

inline bool MMatrix::operator==(const MMatrix& other) const
{
	for (int i=0; i<16; i++)
	{
		if (MMatrix::m[i] != other.m[i]) return false;
	}
	return true;
}

inline bool MMatrix::operator!=(const MMatrix& other) const
{
	return !(*this == other);
}

inline void MMatrix::MultiplyTo( const MMatrix& other , MMatrix& mout) const // out = this * other;
{
	/*const MMatrix& m1 = *this, &m2 = other;
#define CALCCOMPONENT(i,j) out._m[i][j] = m1._m[i][0]*m2._m[0][j] + m1._m[i][1]*m2._m[1][j] + \
										 m1._m[i][2]*m2._m[2][j] + m1._m[i][3]*m2._m[3][j];
	CALCCOMPONENT(0,0) CALCCOMPONENT(0,1) CALCCOMPONENT(0,2) CALCCOMPONENT(0,3)
	CALCCOMPONENT(1,0) CALCCOMPONENT(1,1) CALCCOMPONENT(1,2) CALCCOMPONENT(1,3)
	CALCCOMPONENT(2,0) CALCCOMPONENT(2,1) CALCCOMPONENT(2,2) CALCCOMPONENT(2,3)
	CALCCOMPONENT(3,0) CALCCOMPONENT(3,1) CALCCOMPONENT(3,2) CALCCOMPONENT(3,3)*/
	__asm
	{
		mov		   eax,	   mout	   	   // dst
		mov		   ecx,	   other	  	   // src1
		mov		   edx,	   this	  	   // src2

		movups	 xmm0,	  xmmword ptr [ecx]	  	   // xmm0 = src1[00, 01, 02, 03]
		movups	 xmm1,	  xmmword ptr [ecx + 0x10]	   // xmm1 = src1[04, 05, 06, 07]
		movups	 xmm2,	  xmmword ptr [ecx + 0x20]	   // xmm2 = src1[08, 09, 10, 11]
		movups	 xmm3,	  xmmword ptr [ecx + 0x30]	   // xmm3 = src1[12, 13, 14, 15]

		movss	  xmm7,	  dword ptr [edx]		   // xmm7 = src2[00, xx, xx, xx]
		movss	  xmm4,	  dword ptr [edx + 0x4]	  // xmm4 = src2[01, xx, xx, xx]
		movss	  xmm5,	  dword ptr [edx + 0x8]	  // xmm5 = src2[02, xx, xx, xx]
		movss	  xmm6,	  dword ptr [edx + 0xc]	  	   // xmm6 = src2[03, xx, xx, xx]

		shufps	 xmm7,	  xmm7, 0x0	  	   // xmm7 = src2[00, 00, 00, 00]
		shufps	 xmm4,	  xmm4, 0x0	  	   // xmm4 = src2[01, 01, 01, 01]
		shufps	 xmm5,	  xmm5, 0x0	  	   // xmm5 = src2[02, 02, 02, 02]
		shufps	 xmm6,	  xmm6, 0x0	  	   // xmm6 = src2[03, 03, 03, 03]

		mulps	  xmm7,	  xmm0	   	   	   // xmm7 *= xmm0
		mulps	  xmm4,	  xmm1	   	   	   // xmm4 *= xmm1
		mulps	  xmm5,	  xmm2	   	   	   // xmm5 *= xmm2
		mulps	  xmm6,	  xmm3	   	   	   // xmm6 *= xmm3

		addps	  xmm7,	  xmm4	   	   	   // xmm7 += xmm4
		addps	  xmm7,	  xmm5	   	   	   // xmm7 += xmm5
		addps	  xmm7,	  xmm6	   	   	   // xmm7 += xmm6

		movups	 xmmword ptr [eax], xmm7		   // eax = xmm7

		movss	  xmm7,	  dword ptr [edx + 0x10]	 // xmm7 = src2[04, xx, xx, xx]
		movss	  xmm4,	  dword ptr [edx + 0x14]	 // xmm4 = src2[05, xx, xx, xx]
		movss	  xmm5,	  dword ptr [edx + 0x18]	 // xmm5 = src2[06, xx, xx, xx]
		movss	  xmm6,	  dword ptr [edx + 0x1c]	 // xmm6 = src2[07, xx, xx, xx]

		shufps	 xmm7,	  xmm7, 0x0	  	   // xmm7 = src2[04, 04, 04, 04]
		shufps	 xmm4,	  xmm4, 0x0	  	   // xmm4 = src2[05, 05, 05, 05]
		shufps	 xmm5,	  xmm5, 0x0	  	   // xmm5 = src2[06, 06, 06, 06]
		shufps	 xmm6,	  xmm6, 0x0	  	   // xmm6 = src2[07, 07, 07, 07]

		mulps	  xmm7,	  xmm0	   	   	   // xmm7 *= xmm0
		mulps	  xmm4,	  xmm1	   	   	   // xmm4 *= xmm1
		mulps	  xmm5,	  xmm2	   	   	   // xmm5 *= xmm2
		mulps	  xmm6,	  xmm3	   	   	   // xmm6 *= xmm3

		addps	  xmm7,	  xmm4	   	   	   // xmm7 += xmm4
		addps	  xmm7,	  xmm5	   	   	   // xmm7 += xmm5
		addps	  xmm7,	  xmm6	   	   	   // xmm7 += xmm6

		movups	 xmmword ptr [eax + 0x10], xmm7	 // eax = xmm7

		movss	  xmm7,	  dword ptr [edx + 0x20]	 // xmm7 = src2[08, xx, xx, xx]
		movss	  xmm4,	  dword ptr [edx + 0x24]	 // xmm4 = src2[09, xx, xx, xx]
		movss	  xmm5,	  dword ptr [edx + 0x28]	 // xmm5 = src2[10, xx, xx, xx]
		movss	  xmm6,	  dword ptr [edx + 0x2c]	 // xmm6 = src2[11, xx, xx, xx]

		shufps	 xmm7,	  xmm7, 0x0	  	   // xmm7 = src2[08, 08, 08, 08]
		shufps	 xmm4,	  xmm4, 0x0	  	   // xmm4 = src2[09, 09, 09, 09]
		shufps	 xmm5,	  xmm5, 0x0	  	   // xmm5 = src2[10, 10, 10, 10]
		shufps	 xmm6,	  xmm6, 0x0	  	   // xmm6 = src2[11, 11, 11, 11]

		mulps	  xmm7,	  xmm0	   	   	   // xmm7 *= xmm0
		mulps	  xmm4,	  xmm1	   	   	   // xmm4 *= xmm1
		mulps	  xmm5,	  xmm2	   	   	   // xmm5 *= xmm2
		mulps	  xmm6,	  xmm3	   	   	   // xmm6 *= xmm3

		addps	  xmm7,	  xmm4	   	   	   // xmm7 += xmm4
		addps	  xmm7,	  xmm5	   	   	   // xmm7 += xmm5
		addps	  xmm7,	  xmm6	   	   	   // xmm7 += xmm6

		movups	 xmmword ptr [eax + 0x20], xmm7	 // eax = xmm7

		movss	  xmm7,	  dword ptr [edx + 0x30]	 // xmm7 = src2[12, xx, xx, xx]
		movss	  xmm4,	  dword ptr [edx + 0x34]	 // xmm4 = src2[13, xx, xx, xx]
		movss	  xmm5,	  dword ptr [edx + 0x38]	 // xmm5 = src2[14, xx, xx, xx]
		movss	  xmm6,	  dword ptr [edx + 0x3c]	 // xmm6 = src2[15, xx, xx, xx]

		shufps	 xmm7,	  xmm7, 0x0	  	   // xmm7 = src2[12, 12, 12, 12]
		shufps	 xmm4,	  xmm4, 0x0	  	   // xmm4 = src2[13, 13, 13, 13]
		shufps	 xmm5,	  xmm5, 0x0	  	   // xmm5 = src2[14, 14, 14, 14]
		shufps	 xmm6,	  xmm6, 0x0	  	   // xmm6 = src2[15, 15, 15, 15]

		mulps	  xmm7,	  xmm0	   	   	   // xmm7 *= xmm0
		mulps	  xmm4,	  xmm1	   	   	   // xmm4 *= xmm1
		mulps	  xmm5,	  xmm2	   	   	   // xmm5 *= xmm2
		mulps	  xmm6,	  xmm3	   	   	   // xmm6 *= xmm3

		addps	  xmm7,	  xmm4	   	   	   // xmm7 += xmm4
		addps	  xmm7,	  xmm5	   	   	   // xmm7 += xmm5
		addps	  xmm7,	  xmm6	   	   	   // xmm7 += xmm6

		movups	 xmmword ptr [eax + 0x30], xmm7	 // eax = xmm7
	}
}

inline MMatrix& MMatrix::operator*=(const MMatrix& other)
{
	MMatrix newMatrix;
	MultiplyTo(other,newMatrix);
	memcpy(&m, &newMatrix.m, sizeof(float)*16);
	return *this;
}

inline MMatrix MMatrix::operator*(const MMatrix& other) const
{
	MMatrix tmtrx;
	MultiplyTo(other,tmtrx);
	return tmtrx;

}

inline MMatrix MMatrix::operator*(float f) const
{
	MMatrix mat;
	for(int i=0;i<16;i++)
		mat[i]=m[i]*f;
	return mat;
}

inline MMatrix MMatrix::operator-(const MMatrix& mat) const	// 090716, OZ
{
	return MMatrix(	_11 - mat._11, _12 - mat._12, _13 - mat._13, _14 - mat._14,
					_21 - mat._21, _22 - mat._22, _23 - mat._23, _24 - mat._24,
					_31 - mat._31, _32 - mat._32, _33 - mat._33, _34 - mat._34,
					_41 - mat._41, _42 - mat._42, _43 - mat._43, _44 - mat._44);
}

inline MMatrix MMatrix::operator+(const MMatrix& mat) const	// 090716, OZ
{
	return MMatrix(	_11 + mat._11, _12 + mat._12, _13 + mat._13, _14 + mat._14,
					_21 + mat._21, _22 + mat._22, _23 + mat._23, _24 + mat._24,
					_31 + mat._31, _32 + mat._32, _33 + mat._33, _34 + mat._34,
					_41 + mat._41, _42 + mat._42, _43 + mat._43, _44 + mat._44);
}

inline void MMatrix::MakeZero()
{
	memset(&m, 0, sizeof(float) * 16);
}

inline void MMatrix::MakeIdentity()
{
	MakeZero();
	m[0] = m[5] = m[10] = m[15] = 1.0f;
}

inline void MMatrix::SetTranslation(const MVector3& trans)
{
	_41 = trans.x;
	_42 = trans.y;
	_43 = trans.z;
}

inline void MMatrix::SetInverseTranslation( const MVector3& trans)
{
	_41 = -trans.x;
	_42 = -trans.y;
	_43 = -trans.z;

}

inline MVector3 MMatrix::GetTranslation() const
{
	return MVector3(_41, _42, _43);
}

inline void MMatrix::SetRotationDegrees( const MVector3& rotation)
{
	SetRotationRadians( rotation * (float)3.1415926535897932384626433832795 / 180.0 );
}

inline void MMatrix::SetRotationRadians( const MVector3& rotation)
{
	double cr = cos( rotation.x );
	double sr = sin( rotation.x );
	double cp = cos( rotation.y );
	double sp = sin( rotation.y );
	double cy = cos( rotation.z );
	double sy = sin( rotation.z );

	m[0] = float(cp*cy);
	m[1] = float(cp*sy);
	m[2] = float( -sp );

	double srsp = sr*sp;
	double crsp = cr*sp;

	m[4] = (float)( srsp*cy-cr*sy );
	m[5] = (float)( srsp*sy+cr*cy );
	m[6] = (float)( sr*cp );

	m[8] = (float)( crsp*cy+sr*sy );
	m[9] = (float)( crsp*sy-sr*cy );
	m[10] = (float)( cr*cp );
}

inline void MMatrix::SetScale(const MVector3& scale)
{
	_11 = scale.x;
	_22 = scale.y;
	_33 = scale.z;
}

inline void MMatrix::SetScale(float s)
{
	_11 = _22 = _33 = s;
}

inline void MMatrix::SetRotationMatrix(const MVector3& rotation)
{
	MakeIdentity();
	SetRotationRadians(rotation);
}

inline void MMatrix::SetTranslationMatrix(const MVector3& trans)
{
	MakeIdentity();
	SetTranslation(trans);
}

inline void MMatrix::SetScaleMatrix(const MVector3& scale)
{
	MakeIdentity();
	SetScale(scale);
}

inline void MMatrix::TransformVect( MVector3& vect) const
{
	float vector[3];

	vector[0] = vect.x*_11 + vect.y*_21 + vect.z*_31 + _41;
	vector[1] = vect.x*_12 + vect.y*_22 + vect.z*_32 + _42;
	vector[2] = vect.x*_13 + vect.y*_23 + vect.z*_33 + _43;

	vect.x = vector[0];
	vect.y = vector[1];
	vect.z = vector[2];

}

inline void MMatrix::TransformVectHomogeneous( MVector3& vect) const
{
	float vector[3];

	vector[0] = vect.x*_11 + vect.y*_21 + vect.z*_31 + _41;
	vector[1] = vect.x*_12 + vect.y*_22 + vect.z*_32 + _42;
	vector[2] = vect.x*_13 + vect.y*_23 + vect.z*_33 + _43;
	float invw = 1.f / (vect.x*_14 + vect.y*_24 + vect.z* _34 + _44);

	vect.x = vector[0] * invw;
	vect.y = vector[1] * invw;
	vect.z = vector[2] * invw;

}

inline void MMatrix::TransformVect( const MVector3& in, MVector3& out) const
{
	assert(&in!=&out); // 이경우 에러납니다.
	out.x = in.x*_11 + in.y*_21 + in.z*_31 + _41;
	out.y = in.x*_12 + in.y*_22 + in.z*_32 + _42;
	out.z = in.x*_13 + in.y*_23 + in.z*_33 + _43;
}

inline void MMatrix::TransformVect( const MVector3& in, MVector4& out) const
{
	out.x = in.x*_11 + in.y*_21 + in.z*_31 + _41;
	out.y = in.x*_12 + in.y*_22 + in.z*_32 + _42;
	out.z = in.x*_13 + in.y*_23 + in.z*_33 + _43;
	out.w = in.x*_14 + in.y*_24 + in.z*_34 + _44;
}

inline void MMatrix::TransformNormal( MVector3& vect) const
{
	float vector[3];

	vector[0] = vect.x*_11 + vect.y*_21 + vect.z*_31 ;
	vector[1] = vect.x*_12 + vect.y*_22 + vect.z*_32 ;
	vector[2] = vect.x*_13 + vect.y*_23 + vect.z*_33 ;

	vect.x = vector[0];
	vect.y = vector[1];
	vect.z = vector[2];

}

inline void MMatrix::TransformNormal( const MVector3& in, MVector3& out) const
{
	out.x = in.x*_11 + in.y*_21 + in.z*_31;
	out.y = in.x*_12 + in.y*_22 + in.z*_32;
	out.z = in.x*_13 + in.y*_23 + in.z*_33;
}


inline MVector3 operator * ( const MVector3& v, const MMatrix& tm)
{ 
	MVector3 ret; 
	tm.TransformVect(v,ret); 
	return ret; 
}

inline MVector4 operator * ( const MVector4& v, const MMatrix& tm)
{
	MVector4 vec4;

	vec4.x = v.x*tm._11 + v.y*tm._21 + v.z*tm._31 + v.w*tm._41;
	vec4.y = v.x*tm._12 + v.y*tm._22 + v.z*tm._32 + v.w*tm._42;
	vec4.z = v.x*tm._13 + v.y*tm._23 + v.z*tm._33 + v.w*tm._43;
	vec4.w = v.x*tm._14 + v.y*tm._24 + v.z*tm._34 + v.w*tm._44;
	return vec4;
}

inline bool MMatrix::GetInverse(MMatrix *pOut, float *fDet = NULL) const
{
	/// The inverse is calculated using Cramers rule.
	/// If no inverse exists then 'false' is returned.

	const MMatrix &m = *this;

	float d = (m._11 * m._22 - m._12 * m._21) * (m._33 * m._44 - m._34 * m._43)	- (m._11 * m._23 - m._13 * m._21) * (m._32 * m._44 - m._34 * m._42)
			+ (m._11 * m._24 - m._14 * m._21) * (m._32 * m._43 - m._33 * m._42)	+ (m._12 * m._23 - m._13 * m._22) * (m._31 * m._44 - m._34 * m._41)
			- (m._12 * m._24 - m._14 * m._22) * (m._31 * m._43 - m._33 * m._41)	+ (m._13 * m._24 - m._14 * m._23) * (m._31 * m._42 - m._32 * m._41);
	
	if(fDet) *fDet = d;

	if (d == 0.f)
		return false;

	d = 1.f / d;

	MMatrix &out = *pOut;

	out._11 = d * (m._22 * (m._33 * m._44 - m._34 * m._43) + m._23 * (m._34 * m._42 - m._32 * m._44) + m._24 * (m._32 * m._43 - m._33 * m._42));
	out._12 = d * (m._32 * (m._13 * m._44 - m._14 * m._43) + m._33 * (m._14 * m._42 - m._12 * m._44) + m._34 * (m._12 * m._43 - m._13 * m._42));
	out._13 = d * (m._42 * (m._13 * m._24 - m._14 * m._23) + m._43 * (m._14 * m._22 - m._12 * m._24) + m._44 * (m._12 * m._23 - m._13 * m._22));
	out._14 = d * (m._12 * (m._24 * m._33 - m._23 * m._34) + m._13 * (m._22 * m._34 - m._24 * m._32) + m._14 * (m._23 * m._32 - m._22 * m._33));
	out._21 = d * (m._23 * (m._31 * m._44 - m._34 * m._41) + m._24 * (m._33 * m._41 - m._31 * m._43) + m._21 * (m._34 * m._43 - m._33 * m._44));
	out._22 = d * (m._33 * (m._11 * m._44 - m._14 * m._41) + m._34 * (m._13 * m._41 - m._11 * m._43) + m._31 * (m._14 * m._43 - m._13 * m._44));
	out._23 = d * (m._43 * (m._11 * m._24 - m._14 * m._21) + m._44 * (m._13 * m._21 - m._11 * m._23) + m._41 * (m._14 * m._23 - m._13 * m._24));
	out._24 = d * (m._13 * (m._24 * m._31 - m._21 * m._34) + m._14 * (m._21 * m._33 - m._23 * m._31) + m._11 * (m._23 * m._34 - m._24 * m._33));
	out._31 = d * (m._24 * (m._31 * m._42 - m._32 * m._41) + m._21 * (m._32 * m._44 - m._34 * m._42) + m._22 * (m._34 * m._41 - m._31 * m._44));
	out._32 = d * (m._34 * (m._11 * m._42 - m._12 * m._41) + m._31 * (m._12 * m._44 - m._14 * m._42) + m._32 * (m._14 * m._41 - m._11 * m._44));
	out._33 = d * (m._44 * (m._11 * m._22 - m._12 * m._21) + m._41 * (m._12 * m._24 - m._14 * m._22) + m._42 * (m._14 * m._21 - m._11 * m._24));
	out._34 = d * (m._14 * (m._22 * m._31 - m._21 * m._32) + m._11 * (m._24 * m._32 - m._22 * m._34) + m._12 * (m._21 * m._34 - m._24 * m._31));
	out._41 = d * (m._21 * (m._33 * m._42 - m._32 * m._43) + m._22 * (m._31 * m._43 - m._33 * m._41) + m._23 * (m._32 * m._41 - m._31 * m._42));
	out._42 = d * (m._31 * (m._13 * m._42 - m._12 * m._43) + m._32 * (m._11 * m._43 - m._13 * m._41) + m._33 * (m._12 * m._41 - m._11 * m._42));
	out._43 = d * (m._41 * (m._13 * m._22 - m._12 * m._23) + m._42 * (m._11 * m._23 - m._13 * m._21) + m._43 * (m._12 * m._21 - m._11 * m._22));
	out._44 = d * (m._11 * (m._22 * m._33 - m._23 * m._32) + m._12 * (m._23 * m._31 - m._21 * m._33) + m._13 * (m._21 * m._32 - m._22 * m._31));

	return true;

}

inline void MMatrix::SetProjectionMatrixRH(float w, float h, float zNear, float zFar)
{
	_11 = 2*zNear/w;
	_12 = 0;
	_13 = 0;
	_14 = 0;

	_21 = 0;
	_22 = 2*zNear/h;
	_23 = 0;
	_24 = 0;

	_31 = 0;
	_32 = 0;
	_33 = zFar/(zNear-zFar);
	_34 = -1;

	_41 = 0;
	_42 = 0;
	_43 = zNear*zFar/(zNear-zFar);
	_44 = 0;
}

inline void MMatrix::SetProjectionMatrixLH(float w, float h, float zNear, float zFar)
{
	_11 = 2*zNear/w;
	_12 = 0;
	_13 = 0;
	_14 = 0;

	_21 = 0;
	_22 = 2*zNear/h;
	_23 = 0;
	_24 = 0;

	_31 = 0;
	_32 = 0;
	_33 = zFar/(zFar-zNear);
	_34 = 1;

	_41 = 0;
	_42 = 0;
	_43 = zNear*zFar/(zNear-zFar);
	_44 = 0;
}

inline void MMatrix::SetProjectionMatrixFovRH(float fFovY, float fAspectRatio, float zNear, float zFar)
{
	float yScale = 1/tan(fFovY/2);
	float xScale = yScale / fAspectRatio ;	// dx9 문서에 잘못되어있다

	_11 = xScale;
	_12 = 0;
	_13 = 0;
	_14 = 0;

	_21 = 0;
	_22 = yScale;
	_23 = 0;
	_24 = 0;

	_31 = 0;
	_32 = 0;
	_33 = zFar/(zNear-zFar);
	_34 = -1;

	_41 = 0;
	_42 = 0;
	_43 = zNear*zFar/(zNear-zFar);
	_44 = 0;
}

inline void MMatrix::SetProjectionMatrixFovLH(float fFovY, float fAspectRatio, float zNear, float zFar)
{
	float yScale = 1/tan(fFovY/2);
	float xScale = yScale / fAspectRatio ;	// dx9 문서에 잘못되어있다

	_11 = xScale;
	_12 = 0;
	_13 = 0;
	_14 = 0;

	_21 = 0;
	_22 = yScale;
	_23 = 0;
	_24 = 0;

	_31 = 0;
	_32 = 0;
	_33 = zFar/(zFar-zNear);
	_34 = 1;

	_41 = 0;
	_42 = 0;
	_43 = zNear*zFar/(zNear-zFar);
	_44 = 0;
}

inline void MMatrix::SetLookAtMatrixRH(const MVector3& eye, const MVector3& at, const MVector3& up)
{
	MVector3 zaxis = eye - at;
	zaxis.Normalize();

	MVector3 xaxis = up.CrossProduct(zaxis);
	xaxis.Normalize();

	MVector3 yaxis = zaxis.CrossProduct(xaxis);
	yaxis.Normalize();

	_11 = xaxis.x;
	_12 = yaxis.x;
	_13 = zaxis.x;
	_14 = 0;

	_21 = xaxis.y;
	_22 = yaxis.y;
	_23 = zaxis.y;
	_24 = 0;

	_31 = xaxis.z;
	_32 = yaxis.z;
	_33 = zaxis.z;
	_34 = 0;

	_41 = -xaxis.DotProduct(eye);
	_42 = -yaxis.DotProduct(eye);
	_43 = -zaxis.DotProduct(eye);
	_44 = 1.0f;
}

inline void MMatrix::SetLookAtMatrixLH(const MVector3& eye, const MVector3& at, const MVector3& up)
{
	MVector3 zaxis = at - eye;
	zaxis.Normalize();

	MVector3 xaxis = up.CrossProduct(zaxis);
	xaxis.Normalize();

	MVector3 yaxis = zaxis.CrossProduct(xaxis);
	yaxis.Normalize();

	_11 = xaxis.x;
	_12 = yaxis.x;
	_13 = zaxis.x;
	_14 = 0;

	_21 = xaxis.y;
	_22 = yaxis.y;
	_23 = zaxis.y;
	_24 = 0;

	_31 = xaxis.z;
	_32 = yaxis.z;
	_33 = zaxis.z;
	_34 = 0;

	_41 = -xaxis.DotProduct(eye);
	_42 = -yaxis.DotProduct(eye);
	_43 = -zaxis.DotProduct(eye);
	_44 = 1.0f;
}

inline void MMatrix::SetOrthoLH(float w, float h, float zn, float zf)
{
	_11 = 2.0f/(w);	_12 = 0.0f;		_13 = 0.0f;			_14 = 0.0f;
	_21 = 0.0f;		_22 = 2.0f/(h);	_23 = 0.0f;			_24 = 0.0f;
	_31 = 0.0f;		_32 = 0.0f;		_33 = 1.0f/(zf-zn);	_34 = 0.0f;
	_41 = 0.0f;		_42 = 0.0f;		_43 = zn/(zn-zf);	_44 = 1.0f;

	return;
}

inline void MMatrix::SetOrthoRH(float w, float h, float zn, float zf)
{
	_11 = 2.0f/(w);	_12 = 0.0f;		_13 = 0.0f;			_14 = 0.0f;
	_21 = 0.0f;		_22 = 2.0f/(h);	_23 = 0.0f;			_24 = 0.0f;
	_31 = 0.0f;		_32 = 0.0f;		_33 = 1.0f/(zn-zf);	_34 = 0.0f;
	_41 = 0.0f;		_42 = 0.0f;		_43 = zn/(zn-zf);	_44 = 1.0f;

	return;
}

inline void MMatrix::SetOrthoOffCenterRH(float l, float r, float b, float t, float zn, float zf)
{
	_11 = 2.0f/(r-l);	_12 = 0.0f;	_13 = 0.0f;	_14 = 0.0f;
	_21 = 0.0f;	_22 = 2.0f/(t-b);	_23 = 0.0f;	_24 = 0.0f;
	_31 = 0.0f;	_32 = 0.0f;	_33 = 1.0f/(zn-zf);	_34 = 0.0f;
	_41 = (l+r)/(l-r);_42 = (b+t)/(b-t);	_43 = zn/(zn-zf);	_44 = 1.0f;

	return;
}

inline void MMatrix::SetOrthoOffCenterLH(float l, float r, float b, float t, float zn, float zf)
{
	_11 = 2.0f/(r-l);	_12 = 0.0f;	_13 = 0.0f;	_14 = 0.0f;
	_21 = 0.0f;	_22 = 2.0f/(t-b);	_23 = 0.0f;	_24 = 0.0f;
	_31 = 0.0f;	_32 = 0.0f;	_33 = 1.0f/(zf-zn);	_34 = 0.0f;
	_41 = (l+r)/(l-r);_42 = (b+t)/(b-t);	_43 = -zn/(zf-zn);	_44 = 1.0f;

	return;
}

inline MMatrix MMatrix::GetTranspose() const
{
	MMatrix ret;

	for(int i=0;i<4; i++) {
		for(int j=0;j<4; j++) {
			ret._m[j][i] = _m[i][j];
		}
	}
	return ret;
}

inline void MMatrix::TransformPlane( const MPlane& in, MPlane& out) const
{
	MMatrix matrix;
	GetInverse(&matrix);
	matrix = matrix.GetTranspose();

	out = MPlane((float*)(MVector4((const float*)in)*matrix));


	/*
	MVector3 aPoint = - in.Normal() * in.d;
	TransformVect(aPoint);

	MVector3 normal = in.Normal();
	MVector3 origin(0,0,0);
	TransformVect(normal);
	TransformVect(origin);
	normal -= origin;
	out.SetPlane(normal,aPoint);
	*/
}

inline void MMatrix::SetRotationMatrixAxis(const MVector3& axis, float radian)
{
	float c = cos(radian);
	float s = sin(radian);

	float xx = axis.x*axis.x;
	float yy = axis.y*axis.y;
	float zz = axis.z*axis.z;

	float xy = axis.x*axis.y;
	float xz = axis.x*axis.z;
	float yz = axis.y*axis.z;

	_11 = c+(1.f-c)*xx;
	_12 = (1-c)*xy + s*axis.z;
	_13 = (1-c)*xz - s*axis.y;
	_14 = 0.0f;

	_21 = (1-c)*xy - s*axis.z;
	_22 = c+(1-c)*yy;
	_23 = (1-c)*yz + s*axis.x;
	_24 = 0.0f;

	_31 = (1-c)*xz + s*axis.y;
	_32 = (1-c)*yz - s*axis.x; 
	_33 = c+(1-c)*zz;
	_34 = 0.0f;

	_41 = 0.0f;
	_42 = 0.0f;
	_43 = 0.0f;
	_44 = 1.0f;

	return;
}

inline void MMatrix::SetRotationYawPitchRoll(float yaw, float pitch, float roll)
{
	// Z, X, Y 축 순서로 곱한다.
	// 찾아본 결과로 Yaw가 Up벡터 축을 뜻한다.

	float cy = cos(yaw);
	float sy = sin(yaw);

	float cp = cos(pitch);
	float sp = sin(pitch);
	float cr = cos(roll);
	float sr = sin(roll);

	_11 = cy*cp;
	_12 = sy*cp;
	_13 = -sy;
	_14 = 0.0f;

	_21 = cy*sp*sr - sy*sr;
	_22 = sy*sp*sr + cy*cr;
	_23 = cp*sy;
	_24 = 0.0f;

	_31 = cy*sp*cr + sy*sr;
	_32 = sy*sp*cr - cy*sp;
	_33 = cy*cr;
	_34 = 0.0f;

	_41 = _42 = _43 = 0.0f;
	_44 = 1.0f;

	/*
	// 폐기된 코드
	MMatrix z, y, x;

	z.SetRotationRadians(MVector3(0,0,yaw));
	x.SetRotationRadians(MVector3(pitch, 0, 0));
	y.SetRotationRadians(MVector3(0,-roll, 0));
	*this = z * x * y;
	*/
}

inline void MMatrix::SetLocalMatrix(const MVector3& position, const MVector3& dir, const MVector3& up, const MVector3& right)
{
	_11 = right.x;
	_12 = right.y;
	_13 = right.z;
	_14 = 0;

	_21 = dir.x;
	_22 = dir.y;
	_23 = dir.z;
	_24 = 0;

	_31 = up.x;
	_32 = up.y;
	_33 = up.z;
	_34 = 0;

	_41 = position.x;
	_42 = position.y;
	_43 = position.z;
	_44 = 1.0f;
}

inline void MMatrix::SetLocalMatrix(const MVector3& position, const MVector3& dir, const MVector3& up)
{
	MVector3 NRight = dir.CrossProduct(up);

	if(NRight == MVector3::ZERO)
	{
		NRight = dir.CrossProduct(MVector3(0, 1, 0));
	}

	MVector3 NDir = dir;
	MVector3 NUp = CrossProduct(NRight,dir);

	// 회전 값만 있는 행렬을 만들기 위해서 각 축성분을 Normalize해줘야 한다.
	NRight.Normalize();
	NDir.Normalize();
	NUp.Normalize();

	SetLocalMatrix(position, NDir, NUp, NRight);
}

inline void MMatrix::SetRotationX(float fRadian)
{
	float c = cos(fRadian);
	float s = sin(fRadian);

	MakeIdentity();
	_22 = c;	_23 = s;
	_32 = -s;	_33 = c;
}

inline void MMatrix::SetRotationY(float fRadian)
{
	float c = cos(fRadian);
	float s = sin(fRadian);

	MakeIdentity();
	_11 = c;	_13 = -s;
	_31 = s;	_33 = c;
}

inline void MMatrix::SetRotationZ(float fRadian)
{
	float c = cos(fRadian);
	float s = sin(fRadian);

	MakeIdentity();
	_11 = c;	_12 = s;
	_21 = -s;	_22 = c;
}

#endif