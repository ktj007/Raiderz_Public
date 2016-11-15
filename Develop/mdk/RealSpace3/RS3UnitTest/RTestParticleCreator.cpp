#include "stdafx.h"
#include "RParticle_Create.h"

using namespace rs3;

TEST( TestParticleCreater, GetDirection )
{
	class TestParticleCreater : public RParticleCreator
	{
	public:
		virtual RVector3 GetDirection(float fAngle) override
		{
			return GetRandomDirection(fAngle);
		}

		virtual RVector3 GetPosition(const RVector& vRange) override
		{
			return RVector3::ZERO;
		}

		virtual float GetRandomFloat( float fMin, float fMax ) override
		{
			return fMin;
		}
	};

	RVector vDirection;

	TestParticleCreater creater;
	vDirection = creater.GetDirection( 0.f );
	EXPECT_EQ( RVector::AXISZ, vDirection );

	vDirection = creater.GetDirection( 180.f );
	EXPECT_EQ( -RVector::AXISX, vDirection );

	vDirection = creater.GetDirection( 360.f );
	EXPECT_EQ( -RVector::AXISZ, vDirection );
}

TEST( TestParticleCreater, BoxEmitterPosition )
{
	const RVector vRange( 100.f, 200.f, 300.f );

	{
		class TestMinRandomRBoxEmit : public RBoxEmit
		{
		protected:
			virtual float GetRandomFloat( float fMin, float fMax ) override
			{
				return fMin;
			}
		};

		const RVector vExpectPosition = vRange * -0.5f;

		TestMinRandomRBoxEmit creater;
		RVector vPosition = creater.GetPosition( vRange );
		EXPECT_EQ( vExpectPosition, vPosition );
	}

	{
		class TestMaxRandomRBoxEmit : public RBoxEmit
		{
		protected:
			virtual float GetRandomFloat( float fMin, float fMax ) override
			{
				return fMax;
			}
		};

		const RVector vExpectPosition = vRange * 0.5f;

		TestMaxRandomRBoxEmit creater;
		RVector vPosition = creater.GetPosition( vRange );
		EXPECT_EQ( vExpectPosition, vPosition );
	}
}

TEST( TestParticleCreater, SphereEmitterPosition )
{
	const RVector vRange( 100.f, 200.f, 300.f );

	{
		class TestMinRandomRSphereEmit : public RSphereEmit
		{
		protected:
			virtual float GetRandomFloat( float fMin, float fMax ) override
			{
				return fMin;
			}

			virtual RVector3 GetRandomDirection(float fAngle) override
			{
				return RVector::AXISZ;
			}
		};

		const RVector vExpectPosition = RVector::ZERO;

		TestMinRandomRSphereEmit creater;
		RVector vPosition = creater.GetPosition( vRange );
		EXPECT_EQ( vExpectPosition, vPosition );
	}

	{
		class TestMaxRandomRSphereEmit : public RSphereEmit
		{
		protected:
			virtual float GetRandomFloat( float fMin, float fMax ) override
			{
				return fMax;
			}

			virtual RVector3 GetRandomDirection(float fAngle) override
			{
				return RVector::AXISZ;
			}
		};

		const RVector vExpectPosition = vRange * 0.5f * RVector::AXISZ;

		TestMaxRandomRSphereEmit creater;
		RVector vPosition = creater.GetPosition( vRange );
		EXPECT_EQ( vExpectPosition, vPosition );
	}
}

TEST( TestParticleCreater, RingEmitterPosition )
{
	const RVector vRange( 100.f, 200.f, 300.f );

	{
		class TestMinRandomRRingEmit : public RRingEmit
		{
		protected:
			virtual float GetRandomFloat( float fMin, float fMax ) override
			{
				return fMin;
			}
		};

		const RVector vExpectPosition = RVector( 0.0f, vRange.y * 0.25f, vRange.z * -0.5f );

		TestMinRandomRRingEmit creater;
		RVector vPosition = creater.GetPosition( vRange );
		EXPECT_EQ( vExpectPosition, vPosition );
	}

	{
		class TestMaxRandomRRingEmit : public RRingEmit
		{
		protected:
			virtual float GetRandomFloat( float fMin, float fMax ) override
			{
				return fMax;
			}
		};

		const RVector vExpectPosition = RVector( 0.0f, vRange.y * 0.5f, vRange.z * 0.5f );

		TestMaxRandomRRingEmit creater;
		RVector vPosition = creater.GetPosition( vRange );
		EXPECT_EQ( vExpectPosition, vPosition );
	}
}