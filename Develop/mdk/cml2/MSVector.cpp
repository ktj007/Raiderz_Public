#include "stdafx.h"
#include "MSVector.h"
#include "MVector3.h"
#include <math.h>

MSVector::MSVector(float x, float y, float z)
{
	m_x = _32To16(x);
	m_y = _32To16(y);
	m_z = _32To16(z);
}

MSVector::MSVector(float *f)
{
	m_x = _32To16(f[0]);
	m_y = _32To16(f[1]);
	m_z = _32To16(f[2]);
}

MSVector::MSVector(const float *f)
{
	m_x = _32To16(f[0]);
	m_y = _32To16(f[1]);
	m_z = _32To16(f[2]);
}

MSVector& MSVector::operator=(const MVector3& other)
{
	m_x = _32To16(other.x);
	m_y = _32To16(other.y);
	m_z = _32To16(other.z);

	return *this;
}

bool MSVector::IsEqual( const MSVector& other ) const
{
	return (m_x == other.m_x && m_y == other.m_y && m_z == other.m_z);
}

MSVector2::MSVector2(float x, float y)
{
	m_x = _32To16(x);
	m_y = _32To16(y);
}

MSVector2::MSVector2(float *f)
{
	m_x = _32To16(f[0]);
	m_y = _32To16(f[1]);
}

MSVector2::MSVector2(const float *f)
{
	m_x = _32To16(f[0]);
	m_y = _32To16(f[1]);
}

MSVector2& MSVector2::operator=(const MVector3& other)
{
	m_x = _32To16(other.x);
	m_y = _32To16(other.y);

	return *this;
}

bool MSVector2::IsEqual( const MSVector2& other ) const
{
	return (m_x == other.m_x && m_y == other.m_y);
}