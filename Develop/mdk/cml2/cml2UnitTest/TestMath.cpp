#include "stdafx.h"
#include "..\MMath.h"


TEST( MMath, PlaneNormalByRightHand )
{
	MVector3 P0(0,0,0);
	MVector3 P1(1,0,0);
	MVector3 P2(0,1,0);

	MVector3 n = MMath::RightHandedNormal(P0,P1,P2);

	EXPECT_EQ( MVector3(0,0,1) , n );
}