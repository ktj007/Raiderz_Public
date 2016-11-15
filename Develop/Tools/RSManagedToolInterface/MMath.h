#pragma once

namespace RSManaged
{
	const float ROUNDING_ERROR	= 0.0001f;

	const float PI				= 3.14159f;

	const double GRAD_PI		= 180.0 / 3.1415926535897932384626433832795;

	const double GRAD_PI2		= 3.1415926535897932384626433832795 / 180.0;

	inline float mfabs(float f)
	{
		return System::Math::Abs(f);
	}

	public __value class RSMPolarCoord
	{
	public:
		float fRadius;
		float fTheta;
		float fPhi;
	};
}