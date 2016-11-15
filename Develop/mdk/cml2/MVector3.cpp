#include "stdafx.h"
#include "MVector3.h"
#include "MSVector.h"
#include "MMath.h"

const MVector3 MVector3::ZERO(0.0f, 0.0f, 0.0f);
const MVector3 MVector3::AXISX(1.0f, 0.0f, 0.0f);
const MVector3 MVector3::AXISY(0.0f, 1.0f, 0.0f);
const MVector3 MVector3::AXISZ(0.0f, 0.0f, 1.0f);

MVector3& MVector3::operator=(const MSVector& other)
{ 
	MSVector other2 = other;
	x = other2.X(); 
	y = other2.Y(); 
	z = other2.Z(); 
	return *this; 
}

MVector3& MVector3::operator=(const MSVector2& other)
{ 
	MSVector2 other2 = other;
	x = other2.X(); 
	y = other2.Y(); 
	z = 0.0f;
	return *this; 
}

bool MVector3::IsEqual(const MVector3& other) const
{
	return (
		MMath::Equals(x, other.x) &&
		MMath::Equals(y, other.y) &&
		MMath::Equals(z, other.z)
		);
}