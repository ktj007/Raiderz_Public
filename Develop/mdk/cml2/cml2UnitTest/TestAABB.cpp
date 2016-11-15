#include "stdafx.h"
#include "../MVector3.h"
#include "../MBox.h"

TEST( TestAABB, BoundIntersection )
{
	MBox aabb;
	aabb.vmin = MVector3(0, 0, -6212.8262f);
	aabb.vmax = MVector3(80000, 80000, 17706.719f);

	MVector3 vP(62556, 62827, 2542.9443f);
	MVector3 vQ(62556, 62827, -3000000.0f);

	EXPECT_TRUE( aabb.ContainsLineSegment(vP, vQ) );
}

TEST( TestAABB, StaticValue )
{
	EXPECT_TRUE( MBox::INFINITY.IsValid() );
	EXPECT_EQ( MBox::INFINITY.vmin, -FLT_MAX );
	EXPECT_EQ( MBox::INFINITY.vmax, FLT_MAX );

	EXPECT_FALSE( MBox::INVALID.IsValid() );
	EXPECT_EQ( MBox::INVALID.vmin, FLT_MAX );
	EXPECT_EQ( MBox::INVALID.vmax, -FLT_MAX );
}