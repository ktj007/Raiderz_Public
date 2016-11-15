#include "stdafx.h"
#include "RS3UnitTestUtility.h"
#include "RTrail.h"

using namespace rs3;

class TestTrail : public NullDeviceFixture
{
};

TEST_F( TestTrail, IsAlive )
{
	RTrail trail;

	trail.SetLooping( false );
	bool bAlive = trail.IsAlive();
	EXPECT_FALSE( bAlive );

	trail.SetLooping( true );
	bAlive = trail.IsAlive();
	EXPECT_TRUE( bAlive );
}