#include "stdafx.h"
#include "..\MPlane.h"


TEST( MPlane, MakePlaneFrom3Point )
{
	MVector3 P0(0,0,0);
	MVector3 P1(1,0,0);
	MVector3 P2(0,1,0);

	MPlane Plane;
	
	Plane.SetPlane(P0,P1,P2);
	EXPECT_EQ( MVector3(0,0,1) , Plane.Normal() );

	Plane.SetPlane(P0,P1,P2,MVector3(0,0,-1));
	EXPECT_EQ( MVector3(0,0,-1) , Plane.Normal() );
}