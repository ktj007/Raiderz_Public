#include "stdafx.h"
#include "RParticle_Create.h"

namespace rs3 {

float RParticleCreator::GetRandomFloat( float fMin, float fMax )
{
	return MMath::RandomNumber( fMin, fMax );
}

RVector3 RParticleCreator::GetRandomDirection(float fAngle)
{
	_ASSERT( fAngle >= 0.f );
	_ASSERT( fAngle <= 360.f );

	float fScaledAngle = 1.0f - fAngle/360.0f;	// 0.0f~1.0f
	float fDirectionRange = ( fScaledAngle - 0.5f ) * 2.0f;
	_ASSERT( fDirectionRange >= -1.0f );
	_ASSERT( fDirectionRange <=  1.0f );


	RVector3 vResult;

	vResult.z = GetRandomFloat( fDirectionRange, 1.0f );
	float fR = sqrtf( 1.0f - vResult.z * vResult.z );

	float theta = GetRandomFloat( -MMath::PI, MMath::PI );
	vResult.x = cosf( theta ) * fR;
	vResult.y = sinf( theta ) * fR;

	return vResult;
}

//////////////////////////////////////////////////////////////////////////
RVector3 RBoxEmit::GetDirection(float fAngle)
{
	return GetRandomDirection(fAngle);
}

RVector3 RBoxEmit::GetPosition(const RVector& vRange)
{
	RVector vHalfRange = vRange * 0.5f;

	RVector3 vResult = RVector::ZERO;
	vResult += RVector::AXISX*GetRandomFloat( -vHalfRange.x, vHalfRange.x );
	vResult += RVector::AXISY*GetRandomFloat( -vHalfRange.y, vHalfRange.y );
	vResult += RVector::AXISZ*GetRandomFloat( -vHalfRange.z, vHalfRange.z );

	return vResult;
}

//////////////////////////////////////////////////////////////////////////
RVector3 RSphereEmit::GetDirection(float fAngle)
{
	return GetRandomDirection(fAngle);
}

RVector3 RSphereEmit::GetPosition(const RVector& vRange)
{
	RVector vHalfRange = vRange * 0.5f;

	RVector3 vResult = GetRandomDirection(360.f);
	vResult.x *= GetRandomFloat( 0.0f, vHalfRange.x );
	vResult.y *= GetRandomFloat( 0.0f, vHalfRange.y );
	vResult.z *= GetRandomFloat( 0.0f, vHalfRange.z );

	return vResult;
}

//////////////////////////////////////////////////////////////////////////
RVector3 RRingEmit::GetDirection(float fAngle)
{
	return RVector::ZERO;
}

RVector3 RRingEmit::GetPosition(const RVector& vRange)
{
	RVector vHalfRange = vRange * 0.5f;

	float theta = GetRandomFloat( 0.f, 2 * MMath::PI );

	RVector3 vResult;
	vResult.x = sinf(theta) * GetRandomFloat( vHalfRange.x - vHalfRange.x * 0.5f, vHalfRange.x );
	vResult.y = cosf(theta) * GetRandomFloat( vHalfRange.y - vHalfRange.y * 0.5f, vHalfRange.y );
	vResult.z = GetRandomFloat( -vHalfRange.z, vHalfRange.z );

	return vResult;
}

}