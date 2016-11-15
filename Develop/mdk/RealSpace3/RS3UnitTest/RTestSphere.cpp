#include "stdafx.h"
#include "RTypes.h"

using namespace rs3;

TEST( TestRSphere, FitsIn )
{
	RBoundingSphere s;

	s.center = MVector3(0,0,0);
	s.radius = 10;
	EXPECT_TRUE( s.FitsIn(0,0,0,15) );

	s.center = MVector3(5,0,0);
	s.radius = 10;
	EXPECT_TRUE( s.FitsIn(0,0,0,15) );

	s.center = MVector3(6,0,0);
	s.radius = 10;
	EXPECT_TRUE( !s.FitsIn(0,0,0,15) );

	s.center = MVector3(1,2,1);
	s.radius = 10;
	EXPECT_TRUE( s.FitsIn(0,0,0,15) );
}