#include "stdafx.h"
#include "RAnimationKey.h"

using namespace rs3;

TEST( TestAnimation, AnimationTrack )
{
	RAnimationTrackTest<RVec3Key> testTrack;

	testTrack.Create(2);
	RVec3Key key0;
	key0.frame = 0;
	key0.x =100;
	key0.y =100;
	key0.z =100;
	
	testTrack.at(0) = key0;

	RVec3Key key1;
	key1.frame = 1000;
	key1.x =500;
	key1.y =100;
	key1.z =400;
	testTrack.at(1) = key1;

	RVec3Key keyInt;
	unsigned int nBookmark = 0;
	testTrack.GetInterpolatedKey( &keyInt, 500, nBookmark );

	EXPECT_NEAR( keyInt.x, 300, 0.01f );
	EXPECT_NEAR( keyInt.z, 250, 0.01f );
}