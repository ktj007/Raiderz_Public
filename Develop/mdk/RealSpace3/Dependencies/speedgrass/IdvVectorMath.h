///////////////////////////////////////////////////////////////////////  
//  IdvVector.h
//
//  *** INTERACTIVE DATA VISUALIZATION (IDV) PROPRIETARY INFORMATION ***
//
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Interactive Data Visualization and may
//  not be copied or disclosed except in accordance with the terms of
//  that agreement.
//
//      Copyright (c) 2003-2006 IDV, Inc.
//      All Rights Reserved.
//
//      IDV, Inc.
//      http://www.idvinc.com

#pragma once

#include <cfloat>
#include <cassert>
#include <cmath>


///////////////////////////////////////////////////////////////////////  
//  Vector constants

namespace idv
{

/////////////////////////////////////////////////////////////////////////////
//	VectorSinD

inline float VectorSinD(float x) 
{ 
	return sinf(x / 57.29578f); 
}


/////////////////////////////////////////////////////////////////////////////
//	VectorSinD

inline float VectorCosD(float x)
{ 
	return cosf(x / 57.29578f);
}


/////////////////////////////////////////////////////////////////////////////
//	RadToDeg

inline float RadToDeg(float fRadians)
{
	return fRadians * 57.2957795f;
}


/////////////////////////////////////////////////////////////////////////////
//	DegToRad

inline float DegToRad(float fDegrees)
{
	return fDegrees / 57.2957795f;
}

const float Pi = 3.1415926535897932384626433832795f;
const float HalfPi = Pi * 0.5f;
const float QuarterPi = Pi * 0.25f;
const float TwoPi = 2.0f * Pi;


///////////////////////////////////////////////////////////////////////  
//  Forward references

class Transform;
class RotTransform;
class Region;
class Vec;


///////////////////////////////////////////////////////////////////////  
//  class Vec3 declaration
//
//
//  Description:
//
//      Construction:
//      -------------
//      Vec3( )										Builds a vector equal to (0.0f, 0.0f, 0.0f).
//      Vec3(bool bSkipInitialization)				Builds a vector within uninitialized member variables
//      Vec3(float x, y, z)							Builds a vector with values (x, y, z)
//      Vec3(float x, y)							Builds a vector with values (x, y, 0.0f);
//
//      Variables:
//      ----------
//      m_afData[3]                                 Vector data storage.  Public for OpenGL convenience and fast access.
//
//      Inspectors:
//      -----------
//      AbsoluteValue( )                            Returns a Vec3 where each m_afData[i] = fabs((*this)[i]).
//      AngleBetween(const Vec3& Vec3)				Returns the angle (in radians) between *this and Vec3 (assumes normalized vectors)
//      Distance(const Vec3& cPoint)				Returns the distance between *this and cPoint.
//      GetSize( )                                  Returns the size of the Vec3.
//      GetTolerance( )                             Returns the current tolerance setting (used in RoughlyEqual( )) for all Vec3s.
//      Magnitude( )                                Computes the magnitude of the Vec3.
//      MaxAxis( )                                  Returns the largest value in the Vec3.
//      Midpoint(const Vec3& cVec)					Computes the point half way between cVec and *this.
//      MinAxis( )                                  Returns the smallest value in the Vec3.
//      RoughlyEqual(const Vec3& Vec3)				Determines if *this Vec3 is equivalent to Vec3 within the specified tolerance.
//
//      Mutators:
//      ---------
//      Set(...)                                    All forms set the specified data points and leave the others unchanged.
//      SetTolerance(float fTolerance)              Sets the RoughlyEqual tolerance for ALL Vec3s.
//      SetSize(int nSize)                          Sets the size (number of data points considered valid) of *this.
//      Normalize( )                                Makes the Vec3 a unit vector.
//
//      Operators:
//      ----------
//      operator float*( )                          Converts a Vec3 to a float*.
//      operator[](int nIndex)                      Returns a reference to the specified data point (const version allows access for other const member functions).
//      operator<(const Vec3 &Vec3)					Returns true if the x coord of *this is less than that of Vec3.  If they are equal, the test moves to the y coord and then the z.
//      operator<=(const Vec3 &Vec3)				Returns true if the x coord of *this is less than or equal to that of Vec3.
//      operator>(const Vec3 &Vec3)					Returns true if the x coord of *this is greater than that of Vec3.  If they are equal, the test moves to the y coord and then the z.
//      operator>=(const Vec3 &Vec3)				Returns true if the x coord of *this is greater than or equal to that of Vec3.
//      operator+(const Vec3& Vec3)					Returns *this + Vec3.
//      operator+=(const Vec3& Vec3)				Adds Vec3 to *this.
//      operator-(const Vec3& Vec3)					Returns *this - Vec3.
//      operator-=(const Vec3& Vec3)				Subtracts Vec3 from *this.
//      operator-( )                                Returns -(*this).
//      operator*(float fValue)                     Returns a Vec3 equal to *this with each data point multiplied by fValue.
//      operator*=(float fValue)                    Each data point of *this is multiplied by fValue.
//      operator/(float fValue)                     Returns a Vec3 equal to *this with each data point divided by fValue.
//      operator/=(float fValue)                    Each data point of *this is divided by fValue.
//      operator==(const Vec3& Vec3)				Returns true when *this and Vec3 are identical.
//      operator!=(const Vec3& Vec3)				Returns true when *this and Vec3 are not identical.
//      operator*(const Transform& CTrans)			Returns a Vec3 that is equal to *this * Transform3 (the vector is on the left).
//      operator*(const RotTransform3& CTrans)		Returns a Vec3 that is equal to *this * Transform3 (the vector is on the left).

class Vec3
{
public:
		Vec3( );
		Vec3(float fX, float fY, float fZ);
		Vec3(float fX, float fY);
		Vec3(const float* pPos); // assumes three points
		~Vec3( ) { } // do nothing 

		//  inspectors
		Vec3		AbsoluteValue(void) const;
		float       AngleBetween(const Vec3& cVec) const;
		Vec3		Cross(const Vec3& cVec) const;
		float       Distance(const Vec3& cPoint) const;
		float       Dot(const Vec3& cVec) const;
		float       GetTolerance(void) const                          { return m_fTolerance; }
		float       Magnitude(void) const;
		float       MaxAxis(void) const;
		Vec3		Midpoint(const Vec3& cVec) const;
		float       MinAxis(void) const;
		bool        RoughlyEqual(const Vec3& cVec) const;

		//  mutators
		void        Normalize(void);
		void        Set(float fX, float fY);
		void        Set(float fX, float fY, float fZ);
		void        SetTolerance(float fTolerance)                    { m_fTolerance = fTolerance; }

		//  operators
		operator	float*(void)                                      { return &m_afData[0]; }
		operator	const float*(void) const                          { return &m_afData[0]; }
		float&      operator[](int nIndex)                            { return m_afData[nIndex]; }
		bool        operator<(const Vec3& cVec)    const;  
		bool        operator<=(const Vec3& cVec) const;
		bool        operator>(const Vec3& cVec)    const;
		bool        operator>=(const Vec3& cVec) const;
		Vec3		operator+(const Vec3& cVec) const;
		Vec3&		operator+=(const Vec3& cVec);
		Vec3		operator-(const Vec3& cVec) const;
		Vec3&		operator-=(const Vec3& cVec);
		Vec3		operator-(void) const;
		Vec3		operator*(float fValue) const;
		Vec3&		operator*=(float fValue);
		Vec3		operator/(float fValue) const;
		Vec3&		operator/=(float fValue);
		bool        operator==(const Vec3& cVec) const;
		bool        operator!=(const Vec3& cVec) const;
		Vec3		operator*(const Transform& cTransform) const;
		Vec3		operator*(const RotTransform& cTransform) const;

		float       m_afData[3];            // store the x, y, & z components of the Vec3

private:
static  float       m_fTolerance;
};



///////////////////////////////////////////////////////////////////////  
//  class RotTransform declaration
//
//
//  Description:  Optimized class used for matrix rotations & scales only (3 x 3 instead of 4 x 4)
//
//      Construction:
//      -------------
//      RotTransform( )								Builds a transformation matrix.  LoadIdentity( ) is called.
//      RotTransform(bool)							Builds a matrix without initialization.
//
//      Variables:
//      ----------
//      m_afData[3][3]                              Matrix data storage.  Public for OpenGL convenience and fast access.
//
//      Mutators:
//      ---------
//      Set(int nRow, int nCol, float fValue)       Sets the element at [nRow][nCol] to fValue.
//      Set(float afData[3][3])                     Sets the entire matrix.
//      LoadIdentity( )                             Makes *this the identity matrix.
//      Scale(float fFactor)                        Scales *this on the x, y, and z axes by fFactor.
//      Scale(float fX, float fY, float fZ)         Scales *this.
//      RotateX(float fAngle)                       Rotates *this fAngle degrees around the X axis (faster than general rotate)
//      RotateY(float fAngle)                       Rotates *this fAngle degrees around the Y axis (faster than general rotate)
//      RotateZ(float fAngle)                       Rotates *this fAngle degrees around the Z axis (faster than general rotate)
//      RotateYZ(float fAngle)                      Rotates *this fAngle degrees around the Y then Z axes (faster than separate calls)
//      RotateAxis(float fAngle, Vec3 cAxis)		Rotates *this fAngle degrees around the axis represented by cAxis.  *cAxis is assumed normalized.
//      RotateAxis(float fAngle, Vec cAxis)			Rotates *this fAngle degrees around the axis represented by cAxis.  *cAxis is assumed normalized.
//
//      Operators:
//      ----------
//      operator float*( )                          Converts a Transform3 to a float*.
//      operator*(const RotTransform3& )			Returns a Transform3 equal to *this * Transform3.
//      operator*(const Vec3& Vec3)					Returns a Vec3 equal to *this * Vec3.
//      operator*(const Vec& Vec)					Returns a Vec equal to *this * Vec.
//      operator+(const RotTransform3& )			Returns a Transform3 equal to *this + Transform3.
//      operator-(const RotTransform3& )			Returns a Transform3 equal to *this - Transform3.

class RotTransform
{
public:
	// constructors
	RotTransform( );

	// data gathering/setting
	void                    Set(int nRow, int nCol, float fValue)       { m_afData[nRow][nCol] = fValue; }
	void                    Set(float afData[3][3]);
	void                    SetRow(int nRow, const float* pRow)         { assert(pRow); m_afData[nRow][0] = pRow[0]; m_afData[nRow][1] = pRow[1]; m_afData[nRow][2] = pRow[2]; }
	void                    LoadIdentity(void);

	// standard affine transform functions
	void                    RotateX(float fAngle);
	void                    RotateY(float fAngle);
	void                    RotateZ(float fAngle);
	void                    RotateYZ(float fYangle, float fZangle);
	void                    RotateAxis(float fAngle, const Vec3& cAxis);
	void                    RotateAxisFromIdentity(float fAngle, const Vec3& cAxis);

	// C++ operator overloads
							operator float*(void)                        { return &m_afData[0][0]; }
							operator const float*(void) const            { return &m_afData[0][0]; }
	RotTransform			operator*(const RotTransform& cTrans) const;
	Vec3					operator*(const Vec3& cVec) const;

	float                   m_afData[3][3];             // 3 x 3 rotation matrix
};



///////////////////////////////////////////////////////////////////////  
//  class Transform declaration
//
//
//  Description:  Summary of public interface
//
//      Construction:
//      -------------
//      Transform( )                               Builds a transformation matrix.  LoadIdentity( ) is called.
//
//      Variables:
//      ----------
//      m_afData                                    Matrix data storage.  Public for OpenGL convenience and fast access.
//
//      Inspectors:
//      -----------
//      IsIdentity( )                               Returns true if *this represents the identity matrix.
//
//      Mutators:
//      ---------
//      Set(int nRow, int nCol, float fValue)       Sets the element at [nRow][nCol] to fValue.
//      Set(float afData[4][4])                     Sets the entire matrix.
//      LoadIdentity( )                             Makes *this the identity matrix.
//      Scale(float fFactor)                        Scales *this on the x, y, and z axes by fFactor.
//      Scale(float fX, float fY, float fZ)         Scales *this.
//      Translate(float fX, float fY, float fZ)     Translates *this.
//      Rotate(float fAngle, char chAxis)           Rotates *this fAngle degrees around chAxis ('x', 'X', 'y', etc.)
//      RotateAxis(float fAngle, Vec cAxis)        Rotates *this fAngle degrees around the axis represented by cAxis.  cAxis is normalized.
//      LookAt(const Vec& cEye, const Vec& ...)   Mimics the gluLookAt routine.
//
//      Operators:
//      ----------
//      operator float*( )                          Converts a Transform to a float*.
//      operator*(const Transform& cTransform)     Returns a Transform equal to *this * cTransform.
//      operator*(const Vec& cVec)                 Returns a Vec equal to *this * cVec.
//      operator*(const Region& cRegion)           Returns a Region where cRegion.m_cMin/m_cMax are multiplied by *this.
//      operator+(const Transform& cTransform)     Returns a Transform equal to *this + cTransform.
//      operator-(const Transform& cTransform)     Returns a Transform equal to *this - cTransform.

class Transform
{
public:
		enum EInversionCodeType
		{
			OK, SINGULAR
		};

		Transform( );
		~Transform( );

		// direct access
		void                    Set(int nRow, int nCol, float fValue)			{ m_afData[nRow][nCol] = fValue; }
		void                    Set(const float afData[4][4]);
		void					Set(const float* pData);

		// inspectors
		bool                    IsIdentity(void) const;

		// mutators
		void                    LoadIdentity(void);
		void                    Scale(float fFactor);
		void                    Scale(float fX, float fY, float fZ);
		void                    Translate(float fX, float fY, float fZ);
		void                    Rotate(float fAngle, char chAxis);
		void                    RotateAxis(float fAngle, const Vec3& cAxis);
		void                    LookAt(const Vec3& cEye, const Vec3& cCenter, const Vec3& cUp);

		// operators
								operator float*(void)							{ return &m_afData[0][0]; }
								operator const float*(void) const				{ return &m_afData[0][0]; }
		Transform				operator*(const Transform& cTransform) const;
		Vec3					operator*(const Vec3& cVec3) const;
		Region					operator*(const Region& cRegion) const;
		Transform				operator+(const Transform& cTransform) const;
		Transform				operator-(const Transform& cTransform) const;

		float                   m_afData[4][4];             // standard 4 x 4 transform matrix
};



///////////////////////////////////////////////////////////////////////  
//  class Region declaration
//
//
//  Description:  Summary of public interface
//
//
//      Construction:
//      -------------
//      Region( )                              Builds a CBox and calls Initialize( ).
//
//      Variables:
//      ----------
//      m_cMin                                  Minimum extents vector.  Public for OpenGL convenience and fast access.
//      m_cMax                                  Maximum extents vector.  Public for OpenGL convenience and fast access.
//
//      Inspectors:
//      -----------
//      Dimension(int nAxis)                    Returns the length of the specified axis (0 = x, 1 = y, 2 = z).
//      MidValue(int nAxis)                     Returns the middle of the specified axis.
//      Midpoint( )                             Returns the midpoint of the extents.
//      LongestExtent( )                        Returns the length of the longest axis.
//      ShortestExtent( )                       Returns the length of the shortest axis.
//      AverageExtent( )                        Returns the average length of all axes.
//      IsSet( )                                Returns true when any member of *this does not match the initialized state.
//
//
//      Mutators:
//      ---------
//      Initialize( )                           Sets m_cMin to (FLT_MAX, FLT_MAX, FLT_MAX) and m_cMax to -m_cMin.  Called automatically by the constructor.
//      Scale(float fScale)                     Scales *this by fScale relative to the origin (NOT the midpoint).
//
//      Operators:
//      ----------
//      operator+(const Region& cRegion)       Returns a Region exactly large enough to include *this and cRegion.
//      operator+(const Vec& cVec)             Returns a Region where cVec is added to m_cMin/m_cMax.
//      operator^(const Vec3& cVec)            Returns a Region where min and max include those of cVec
//      operator*(float fScale)                 Returns a Region scaled by fScale relative to the midpoint of *this (NOT the origin).
//      operator*(const Transform& cTransform) Returns a Region where m_cMin/m_cMax of *this are multiplied by cTransform.
//      operator<(const Region& cRegion)       Returns true when *this is contained within cRegion (using <= and >=).

class Region
{
public:
		Region( );

		// inspectors
		float                   Dimension(int nAxis) const				{ return m_cMax.m_afData[nAxis] - m_cMin.m_afData[nAxis]; }
		float                   MidValue(int nAxis) const				{ return (m_cMax.m_afData[nAxis] + m_cMin.m_afData[nAxis]) * 0.5f; }
		Vec3					Midpoint(void) const					{ return m_cMin.Midpoint(m_cMax); }
		float                   LongestExtent(void) const				{ return (((m_cMax - m_cMin).AbsoluteValue( )).MaxAxis( )); }
		float                   ShortestExtent(void) const				{ return (((m_cMax - m_cMin).AbsoluteValue( )).MinAxis( )); }
		float                   AverageExtent(void) const;
		bool                    IsSet(void) const;

		// mutators
		void                    Initialize(void);
		void                    Scale(float fScale);

		// operators
		Region					operator+(const Region& cRegion) const;
		Region					operator+(const Vec3& cVec) const;
		Region					operator^(const Vec3& cVec) const;
		Region					operator*(float fScale) const;
		Region					operator*(const Transform& cTransform) const;
		bool                    operator<(const Region& cRegion) const;

		Vec3					m_cMin;             // minimum corner of the region (x, y, z)
		Vec3					m_cMax;             // maximum corner of the region (x, y, z)
};


///////////////////////////////////////////////////////////////////////  
//  _idv_sqrt1 definition

inline float _idv_sqrt1(float fVal)
{
#ifdef PS3
	return sqrt(fVal);
#else
	int i = ((*reinterpret_cast<int*>(&fVal)) >> 1 ) + 0x1fc00000; 

	return *reinterpret_cast<float*>(&i);
#endif
}


///////////////////////////////////////////////////////////////////////  
//  class Vec3 inlined function definitions
//

// Vec3( )
inline Vec3::Vec3(void)
{
	m_afData[0] = m_afData[1] = m_afData[2] = 0.0f;
}

// Vec3(float fX, float fY, float fZ)
inline Vec3::Vec3(float fX, float fY, float fZ)
{
	m_afData[0] = fX;
	m_afData[1] = fY;
	m_afData[2] = fZ;
}

// Vec3(float fX, float fY)
inline Vec3::Vec3(float fX, float fY)
{
	m_afData[0] = fX;
	m_afData[1] = fY;
	m_afData[2] = 0.0f;
}

// Vec3(const float* pPos)
inline Vec3::Vec3(const float* pPos)
{
	assert(pPos);
	m_afData[0] = pPos[0];
	m_afData[1] = pPos[1];
	m_afData[2] = pPos[2];
}

// AbsoluteValue
inline Vec3 Vec3::AbsoluteValue(void) const
{
	return Vec3(static_cast<float>(fabs(m_afData[0])),
		        static_cast<float>(fabs(m_afData[1])),
		        static_cast<float>(fabs(m_afData[2])));
}

// AngleBetween
inline float Vec3::AngleBetween(const Vec3& cVec) const
{
	float fDot = this->Dot(cVec);
	if (fDot < -1.0f)
		fDot = -1.0f;
	if (fDot > 1.0f)
		fDot = 1.0f;
	return acosf(fDot);
}

// Dot
inline float Vec3::Dot(const Vec3& cVec) const
{
	return m_afData[0] * cVec.m_afData[0] +
		   m_afData[1] * cVec.m_afData[1] +
		   m_afData[2] * cVec.m_afData[2];
}

// Cross
inline Vec3 Vec3::Cross(const Vec3& cVec) const
{
	return Vec3(m_afData[1] * cVec.m_afData[2] - m_afData[2] * cVec.m_afData[1],
		        m_afData[2] * cVec.m_afData[0] - m_afData[0] * cVec.m_afData[2],
		        m_afData[0] * cVec.m_afData[1] - m_afData[1] * cVec.m_afData[0]);
}

// Distance
inline float Vec3::Distance(const Vec3& cVec) const
{
	return _idv_sqrt1(
					  ((cVec.m_afData[0] - m_afData[0]) * (cVec.m_afData[0] - m_afData[0])) +
					  ((cVec.m_afData[1] - m_afData[1]) * (cVec.m_afData[1] - m_afData[1])) +
					  ((cVec.m_afData[2] - m_afData[2]) * (cVec.m_afData[2] - m_afData[2]))
					 );
}

// Magnitude
inline float Vec3::Magnitude(void) const
{
	return _idv_sqrt1(m_afData[0] * m_afData[0] + 
		              m_afData[1] * m_afData[1] + 
		              m_afData[2] * m_afData[2]);
}

// MaxAxis
inline float Vec3::MaxAxis(void) const
{
	float fMax(m_afData[0]);

	if (m_afData[1] > fMax)
		fMax = m_afData[1];
	if (m_afData[2] > fMax)
		fMax = m_afData[2];

	return fMax;
}

// Midpoint
inline Vec3 Vec3::Midpoint(const Vec3& cVec) const
{
	return Vec3(0.5f * (cVec.m_afData[0] + m_afData[0]),
		        0.5f * (cVec.m_afData[1] + m_afData[1]),
		        0.5f * (cVec.m_afData[2] + m_afData[2]));
}

// MinAxis
inline float Vec3::MinAxis(void) const
{
	float fMin(m_afData[0]);

	if (m_afData[1] < fMin)
		fMin = m_afData[1];
	if (m_afData[2] < fMin)
		fMin = m_afData[2];

	return fMin;
}

// RoughlyEqual
inline bool Vec3::RoughlyEqual(const Vec3& cVec) const
{
	return ((fabs(m_afData[0] - cVec.m_afData[0]) < m_fTolerance) &&
		    (fabs(m_afData[1] - cVec.m_afData[1]) < m_fTolerance) &&
		    (fabs(m_afData[2] - cVec.m_afData[2]) < m_fTolerance));
}

// Normalize
inline void Vec3::Normalize(void)
{
	float fSumOfSquares = (m_afData[0] * m_afData[0]) +
						  (m_afData[1] * m_afData[1]) +
						  (m_afData[2] * m_afData[2]);
	float fInvSquareRoot = 1.0f / ((fSumOfSquares > 100.0f * FLT_EPSILON) ? sqrtf(fSumOfSquares) : 0.01f);
	//  float fInvSquareRoot = 1.0f / sqrtf(fSumOfSquares);

	m_afData[0] *= fInvSquareRoot;
	m_afData[1] *= fInvSquareRoot;
	m_afData[2] *= fInvSquareRoot;
}

// Set(float fX, float fY)
inline void Vec3::Set(float fX, float fY)
{
	m_afData[0] = fX;
	m_afData[1] = fY;
}

// Set(float fX, float fY. float fZ)
inline void Vec3::Set(float fX, float fY, float fZ)
{
	m_afData[0] = fX;
	m_afData[1] = fY;
	m_afData[2] = fZ;
}

// operator <
inline bool Vec3::operator<(const Vec3& cVec) const
{
	if (m_afData[0] < cVec.m_afData[0])
	{
		return true;
	}
	else if (m_afData[0] > cVec.m_afData[0])
	{
		return false;
	}
	else
	{
		if (m_afData[1] < cVec.m_afData[1])
			return true;
		else if (m_afData[1] > cVec.m_afData[1])
			return false;
		else
			return m_afData[2] < cVec.m_afData[2];
	}
}

// operator <=
inline bool Vec3::operator<=(const Vec3& cVec) const
{
	return (m_afData[0] <= cVec.m_afData[0]);
}

// operator >
inline bool Vec3::operator>(const Vec3& cVec) const
{
	if (m_afData[0] > cVec.m_afData[0])
	{
		return true;
	}
	else if (m_afData[0] < cVec.m_afData[0])
	{
		return false;
	}
	else
	{
		if (m_afData[1] > cVec.m_afData[1])
			return true;
		else if (m_afData[1] < cVec.m_afData[1])
			return false;
		else
			return m_afData[2] > cVec.m_afData[2];
	}
}

// operator >=
inline bool Vec3::operator>=(const Vec3& cVec) const
{
	return (m_afData[0] >= cVec.m_afData[0]);
}

// operator +
inline Vec3 Vec3::operator+(const Vec3& cVec) const
{
	return Vec3(m_afData[0] + cVec.m_afData[0],
		        m_afData[1] + cVec.m_afData[1],
		        m_afData[2] + cVec.m_afData[2]);
}

// operator +=
inline Vec3& Vec3::operator+=(const Vec3& cVec) 
{
	m_afData[0] += cVec.m_afData[0];
	m_afData[1] += cVec.m_afData[1];
	m_afData[2] += cVec.m_afData[2];

	return *this;
}

// operator -
inline Vec3 Vec3::operator-(const Vec3& cVec) const
{
	return Vec3(m_afData[0] - cVec.m_afData[0],
		        m_afData[1] - cVec.m_afData[1],
		        m_afData[2] - cVec.m_afData[2]);
}

// operator +=
inline Vec3& Vec3::operator-=(const Vec3& cVec) 
{
	m_afData[0] -= cVec.m_afData[0];
	m_afData[1] -= cVec.m_afData[1];
	m_afData[2] -= cVec.m_afData[2];

	return *this;
}

// operator-
inline Vec3 Vec3::operator-(void) const
{
	return Vec3(-m_afData[0], -m_afData[1], -m_afData[2]);
}

// operator*
inline Vec3 Vec3::operator*(float fScale) const
{
	return Vec3(m_afData[0] * fScale,
		m_afData[1] * fScale,
		m_afData[2] * fScale);
}

// operator*=
inline Vec3& Vec3::operator*=(float fScale) 
{
	m_afData[0] *= fScale;
	m_afData[1] *= fScale;
	m_afData[2] *= fScale;

	return *this;
}

// operator/
inline Vec3 Vec3::operator/(float fScale) const
{
	return Vec3(m_afData[0] / fScale, m_afData[1] / fScale, m_afData[2] / fScale);
}

// operator/=
inline Vec3& Vec3::operator/=(float fScale) 
{
	m_afData[0] /= fScale;
	m_afData[1] /= fScale;
	m_afData[2] /= fScale;

	return *this;
}

// operator==
inline bool Vec3::operator==(const Vec3& cVec) const
{
	return m_afData[0] == cVec.m_afData[0] &&
		   m_afData[1] == cVec.m_afData[1] &&
		   m_afData[2] == cVec.m_afData[2];
}

// operator!=
inline bool Vec3::operator!=(const Vec3& cVec) const
{
	return m_afData[0] != cVec.m_afData[0] ||
		   m_afData[1] != cVec.m_afData[1] ||
		   m_afData[2] != cVec.m_afData[2];
}

// operator*
inline Vec3 Vec3::operator*(const Transform& cTransform) const
{
	return Vec3(m_afData[0] * cTransform.m_afData[0][0] +
				m_afData[1] * cTransform.m_afData[1][0] +
				m_afData[2] * cTransform.m_afData[2][0] +
				cTransform.m_afData[3][0],
				m_afData[0] * cTransform.m_afData[0][1] +
				m_afData[1] * cTransform.m_afData[1][1] +
				m_afData[2] * cTransform.m_afData[2][1] +
				cTransform.m_afData[3][1],
				m_afData[0] * cTransform.m_afData[0][2] +
				m_afData[1] * cTransform.m_afData[1][2] +
				m_afData[2] * cTransform.m_afData[2][2] +
				cTransform.m_afData[3][2]);
}

// operator*
inline Vec3 Vec3::operator*(const RotTransform& cTransform) const
{
	return Vec3(m_afData[0] * cTransform.m_afData[0][0] +
				m_afData[1] * cTransform.m_afData[1][0] +
				m_afData[2] * cTransform.m_afData[2][0],
				m_afData[0] * cTransform.m_afData[0][1] +
				m_afData[1] * cTransform.m_afData[1][1] +
				m_afData[2] * cTransform.m_afData[2][1],
				m_afData[0] * cTransform.m_afData[0][2] +
				m_afData[1] * cTransform.m_afData[1][2] +
				m_afData[2] * cTransform.m_afData[2][2]);
}


///////////////////////////////////////////////////////////////////////  
//  class RotTransform inlined function definitions

// RotTransform
inline RotTransform::RotTransform(void)
{
	m_afData[0][0] = 1.0f;
	m_afData[0][1] = 0.0f;
	m_afData[0][2] = 0.0f;
	m_afData[1][0] = 0.0f;
	m_afData[1][1] = 1.0f;
	m_afData[1][2] = 0.0f;
	m_afData[2][0] = 0.0f;
	m_afData[2][1] = 0.0f;
	m_afData[2][2] = 1.0f;
}


// Set
inline void RotTransform::Set(float afData[3][3])
{
	m_afData[0][0] = afData[0][0]; 
	m_afData[0][1] = afData[0][1];
	m_afData[0][2] = afData[0][2];
	m_afData[1][0] = afData[1][0];
	m_afData[1][1] = afData[1][1];
	m_afData[1][2] = afData[1][2];
	m_afData[2][0] = afData[2][0];
	m_afData[2][1] = afData[2][1];
	m_afData[2][2] = afData[2][2];
}

// LoadIdentity
inline void RotTransform::LoadIdentity(void)
{
	m_afData[0][0] = 1.0f;
	m_afData[0][1] = 0.0f;
	m_afData[0][2] = 0.0f;
	m_afData[1][0] = 0.0f;
	m_afData[1][1] = 1.0f;
	m_afData[1][2] = 0.0f;
	m_afData[2][0] = 0.0f;
	m_afData[2][1] = 0.0f;
	m_afData[2][2] = 1.0f;
}


// operator*
inline RotTransform RotTransform::operator*(const RotTransform& cTrans) const
{
	RotTransform cTemp;

	cTemp.m_afData[0][0] = m_afData[0][0] * cTrans.m_afData[0][0] +
						   m_afData[0][1] * cTrans.m_afData[1][0] +
						   m_afData[0][2] * cTrans.m_afData[2][0];
	cTemp.m_afData[0][1] = m_afData[0][0] * cTrans.m_afData[0][1] +
						   m_afData[0][1] * cTrans.m_afData[1][1] +
						   m_afData[0][2] * cTrans.m_afData[2][1];
	cTemp.m_afData[0][2] = m_afData[0][0] * cTrans.m_afData[0][2] +
						   m_afData[0][1] * cTrans.m_afData[1][2] +
						   m_afData[0][2] * cTrans.m_afData[2][2];
	cTemp.m_afData[1][0] = m_afData[1][0] * cTrans.m_afData[0][0] +
						   m_afData[1][1] * cTrans.m_afData[1][0] +
						   m_afData[1][2] * cTrans.m_afData[2][0];
	cTemp.m_afData[1][1] = m_afData[1][0] * cTrans.m_afData[0][1] +
						   m_afData[1][1] * cTrans.m_afData[1][1] +
						   m_afData[1][2] * cTrans.m_afData[2][1];
	cTemp.m_afData[1][2] = m_afData[1][0] * cTrans.m_afData[0][2] +
						   m_afData[1][1] * cTrans.m_afData[1][2] +
						   m_afData[1][2] * cTrans.m_afData[2][2];
	cTemp.m_afData[2][0] = m_afData[2][0] * cTrans.m_afData[0][0] +
						   m_afData[2][1] * cTrans.m_afData[1][0] +
						   m_afData[2][2] * cTrans.m_afData[2][0];
	cTemp.m_afData[2][1] = m_afData[2][0] * cTrans.m_afData[0][1] +
						   m_afData[2][1] * cTrans.m_afData[1][1] +
						   m_afData[2][2] * cTrans.m_afData[2][1];
	cTemp.m_afData[2][2] = m_afData[2][0] * cTrans.m_afData[0][2] +
						   m_afData[2][1] * cTrans.m_afData[1][2] +
						   m_afData[2][2] * cTrans.m_afData[2][2];

	return cTemp;
}


// operator*
inline Vec3 RotTransform::operator*(const Vec3& cVec3) const
{
	return Vec3(m_afData[0][0] * cVec3.m_afData[0] +
				m_afData[0][1] * cVec3.m_afData[1] +
				m_afData[0][2] * cVec3.m_afData[2],
				m_afData[1][0] * cVec3.m_afData[0] +
				m_afData[1][1] * cVec3.m_afData[1] +
				m_afData[1][2] * cVec3.m_afData[2],
				m_afData[2][0] * cVec3.m_afData[0] +
				m_afData[2][1] * cVec3.m_afData[1] +
				m_afData[2][2] * cVec3.m_afData[2]);
}


// RotateX
inline void RotTransform::RotateX(float fAngle)
{
	RotTransform cRotMatrix;

	float fCosine = VectorCosD(fAngle);
	float fSine = VectorSinD(fAngle);

	cRotMatrix.m_afData[0][0] = 1.0f;
	cRotMatrix.m_afData[0][1] = 0.0f;
	cRotMatrix.m_afData[0][2] = 0.0f;
	cRotMatrix.m_afData[1][0] = 0.0f;
	cRotMatrix.m_afData[1][1] = fCosine;
	cRotMatrix.m_afData[1][2] = fSine;
	cRotMatrix.m_afData[2][0] = 0.0f;
	cRotMatrix.m_afData[2][1] = -fSine;
	cRotMatrix.m_afData[2][2] = fCosine;

	// this function can be further optimized by hardcoding
	// the multiplication here and removing terms with 0.0 multipliers.

	*this = cRotMatrix * *this;
}


// RotateY
inline void RotTransform::RotateY(float fAngle)
{
	RotTransform cRotMatrix;

	float fCosine = VectorCosD(fAngle);
	float fSine = VectorSinD(fAngle);

	cRotMatrix.m_afData[0][0] = fCosine;
	cRotMatrix.m_afData[0][1] = 0.0f;
	cRotMatrix.m_afData[0][2] = -fSine;
	cRotMatrix.m_afData[1][0] = 0.0f;
	cRotMatrix.m_afData[1][1] = 1.0f;
	cRotMatrix.m_afData[1][2] = 0.0f;
	cRotMatrix.m_afData[2][0] = fSine;
	cRotMatrix.m_afData[2][1] = 0.0f;
	cRotMatrix.m_afData[2][2] = fCosine;

	// this function can be further optimized by hardcoding
	// the multiplication here and removing terms with 0.0 multipliers.

	*this = cRotMatrix * *this;
}



// RotateZ
inline void RotTransform::RotateZ(float fAngle)
{
	RotTransform cRotMatrix;

	float fCosine = VectorCosD(fAngle);
	float fSine = VectorSinD(fAngle);

	cRotMatrix.m_afData[0][0] = fCosine;
	cRotMatrix.m_afData[0][1] = fSine;
	cRotMatrix.m_afData[0][2] = 0.0f;
	cRotMatrix.m_afData[1][0] = -fSine;
	cRotMatrix.m_afData[1][1] = fCosine;
	cRotMatrix.m_afData[1][2] = 0.0f;
	cRotMatrix.m_afData[2][0] = 0.0f;
	cRotMatrix.m_afData[2][1] = 0.0f;
	cRotMatrix.m_afData[2][2] = 1.0f;

	// this function can be further optimized by hardcoding
	// the multiplication here and removing terms with 0.0 multipliers.

	*this = cRotMatrix * *this;
}



// RotateYZ
inline void RotTransform::RotateYZ(float fYangle, float fZangle)
{
	RotTransform cRotMatrix;

	float fCosineY = VectorCosD(fYangle);
	float fSineY = VectorSinD(fYangle);
	float fCosineZ = VectorCosD(fZangle);
	float fSineZ = VectorSinD(fZangle);

	cRotMatrix.m_afData[0][0] = fCosineY * fCosineZ;
	cRotMatrix.m_afData[0][1] = fCosineY * fSineZ;
	cRotMatrix.m_afData[0][2] = -fSineY;
	cRotMatrix.m_afData[1][0] = -fSineZ;
	cRotMatrix.m_afData[1][1] = fCosineZ;
	cRotMatrix.m_afData[1][2] = 0.0f;
	cRotMatrix.m_afData[2][0] = fSineY * fCosineZ;
	cRotMatrix.m_afData[2][1] = fSineY * fSineZ;
	cRotMatrix.m_afData[2][2] = fCosineY;

	*this = cRotMatrix * *this;
}


// RotateAxis
inline void RotTransform::RotateAxis(float fAngle, const Vec3& cAxis)
{
	RotTransform cRotMatrix;

	float s = VectorSinD(fAngle);
	float c = VectorCosD(fAngle);
	float t = 1.0f - c;

	float x = cAxis.m_afData[0];
	float y = cAxis.m_afData[1];
	float z = cAxis.m_afData[2];

	cRotMatrix.m_afData[0][0] = t * x * x + c;
	cRotMatrix.m_afData[0][1] = t * x * y + s * z;
	cRotMatrix.m_afData[0][2] = t * x * z - s * y;
	cRotMatrix.m_afData[1][0] = t * x * y - s * z;
	cRotMatrix.m_afData[1][1] = t * y * y + c;
	cRotMatrix.m_afData[1][2] = t * y * z + s * x;
	cRotMatrix.m_afData[2][0] = t * x * z + s * y;
	cRotMatrix.m_afData[2][1] = t * y * z - s * x;
	cRotMatrix.m_afData[2][2] = t * z * z + c;

	*this = cRotMatrix * *this;
}   

// RotateAxisFromIdentity
inline void RotTransform::RotateAxisFromIdentity(float fAngle, const Vec3& cAxis)
{
	float s = VectorSinD(fAngle);
	float c = VectorCosD(fAngle);
	float t = 1.0f - c;

	float x = cAxis.m_afData[0];
	float y = cAxis.m_afData[1];
	float z = cAxis.m_afData[2];

	m_afData[0][0] = t * x * x + c;
	m_afData[0][1] = t * x * y + s * z;
	m_afData[0][2] = t * x * z - s * y;
	m_afData[1][0] = t * x * y - s * z;
	m_afData[1][1] = t * y * y + c;
	m_afData[1][2] = t * y * z + s * x;
	m_afData[2][0] = t * x * z + s * y;
	m_afData[2][1] = t * y * z - s * x;
	m_afData[2][2] = t * z * z + c;
}

}; // end namespace speedtree

