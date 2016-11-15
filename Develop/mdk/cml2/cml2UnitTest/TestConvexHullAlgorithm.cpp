#include "stdafx.h"
#include "..\MConvexHullAlgorithm.h"

TEST( TestMakeConvexHullByCounterClockWise3D, Triangle )
{
	std::vector<MVector3> Points;
	MVector3 A(0,5,0);
	MVector3 B(0,5,4);
	MVector3 C(4,5,0);
	Points.push_back(A);//0
	Points.push_back(MVector3(0,5,1));
	Points.push_back(B);//1
	Points.push_back(MVector3(1,5,0));	
	Points.push_back(C);//2


	std::vector<MVector3> ConvexPoints;
	MConvexHullAlgorithm::MakeConvexHullByCounterClockWise3D(Points,ConvexPoints);
	ASSERT_EQ( 3, ConvexPoints.size() );
	EXPECT_TRUE( MMath::Equals( A , ConvexPoints[0] ) );
	EXPECT_TRUE( MMath::Equals( B , ConvexPoints[1] ) );
	EXPECT_TRUE( MMath::Equals( C , ConvexPoints[2] ) );
}

TEST( TestMakeConvexHullByCounterClockWise3D, Quad )
{
	// D    C
	//   E
	// A    B
	std::vector<MVector3> Points;
	MVector3 A(-2,5,-2);
	MVector3 B( 2,5,-2);
	MVector3 C( 2,5, 2);
	MVector3 D(-2,5, 2);
	MVector3 E( 0,5, 0);

	Points.push_back(A); Points.push_back(B); Points.push_back(E);
	Points.push_back(B); Points.push_back(E); Points.push_back(C);
	Points.push_back(D); Points.push_back(E); Points.push_back(C);
	Points.push_back(E); Points.push_back(D); Points.push_back(A);

	std::vector<MVector3> ConvexPoints;
	MConvexHullAlgorithm::MakeConvexHullByCounterClockWise3D(Points,ConvexPoints);
	ASSERT_EQ( 4, ConvexPoints.size() );
	EXPECT_TRUE( MMath::Equals( A , ConvexPoints[0] ) );
	EXPECT_TRUE( MMath::Equals( B , ConvexPoints[1] ) );
	EXPECT_TRUE( MMath::Equals( C , ConvexPoints[2] ) );
	EXPECT_TRUE( MMath::Equals( D , ConvexPoints[3] ) );
}


TEST( TransformPlanePointsToXYPlane, DistancedPlane )
{

	std::vector<MVector3> DistancedXYPoints;
	DistancedXYPoints.push_back(MVector3(0,0,1));
	DistancedXYPoints.push_back(MVector3(1,0,1));
	DistancedXYPoints.push_back(MVector3(0,1,1));

	std::vector<MVector3> XYPoints;
	MConvexHullAlgorithm::TransformPlanePointsToXYPlane(DistancedXYPoints,XYPoints);
	ASSERT_EQ(XYPoints.size(), DistancedXYPoints.size() );

	MVector3 n = MMath::RightHandedNormal(XYPoints[0],XYPoints[1],XYPoints[2]);
	EXPECT_TRUE( MVector3(0,0,1) == n );

	for(unsigned int i=0;i<XYPoints.size();++i)
	{
		EXPECT_FLOAT_EQ( 0.0f , XYPoints[i].z );
	}
}

TEST( TransformPlanePointsToXYPlane, XZPlane )
{
	std::vector<MVector3> XZPoints;
	XZPoints.push_back(MVector3(0,0,0));
	XZPoints.push_back(MVector3(1,0,0));
	XZPoints.push_back(MVector3(0,0,1));

	std::vector<MVector3> XYPoints;
	MConvexHullAlgorithm::TransformPlanePointsToXYPlane(XZPoints,XYPoints);
	ASSERT_EQ(XYPoints.size(), XZPoints.size() );

	MVector3 n = MMath::RightHandedNormal(XYPoints[0],XYPoints[1],XYPoints[2]);
	EXPECT_TRUE( MVector3(0,0,1) == n );

	for(unsigned int i=0;i<XYPoints.size();++i)
	{
		EXPECT_FLOAT_EQ( 0.0f , XYPoints[i].z );
	}
}

TEST( TransformPlanePointsToXYPlane, YZPlane )
{
	std::vector<MVector3> YZPoints;
	YZPoints.push_back(MVector3(0,0,0));
	YZPoints.push_back(MVector3(0,1,0));
	YZPoints.push_back(MVector3(0,0,1));

	std::vector<MVector3> XYPoints;
	MConvexHullAlgorithm::TransformPlanePointsToXYPlane(YZPoints,XYPoints);
	ASSERT_EQ(XYPoints.size(), YZPoints.size() );

	MVector3 n = MMath::RightHandedNormal(XYPoints[0],XYPoints[1],XYPoints[2]);
	EXPECT_TRUE( MVector3(0,0,1) == n );

	for(unsigned int i=0;i<XYPoints.size();++i)
	{
		EXPECT_FLOAT_EQ( 0.0f , XYPoints[i].z );
	}
}


TEST( TestMakeConvexHullByCounterClockWise2D, Triangle )
{
	std::vector<MVector2> Points;
	Points.push_back(MVector2(0,0));
	Points.push_back(MVector2(3,1));
	Points.push_back(MVector2(4,0));
	Points.push_back(MVector2(4,4));

	std::vector<MVector2> ConvexPoints;
	MConvexHullAlgorithm::MakeConvexHullByCounterClockWise2D(Points,ConvexPoints);
	ASSERT_EQ( 3, ConvexPoints.size() );
	EXPECT_TRUE( MVector2(4,0) == ConvexPoints[0] );
	EXPECT_TRUE( MVector2(4,4) == ConvexPoints[1] );
	EXPECT_TRUE( MVector2(0,0) == ConvexPoints[2] );
}

TEST( TestMakeConvexHullByCounterClockWise2D, Quad )
{
	//   C   B
	//     E
	//   D   A 


	std::vector<MVector2> Points;
	MVector2 A(4,0);
	MVector2 B(4,4);
	MVector2 C(0,4);
	MVector2 D(0,0);
	MVector2 E(2,2);	

	Points.push_back(B);
	Points.push_back(E);
	Points.push_back(C);
	Points.push_back(E);
	Points.push_back(A);
	Points.push_back(D);
	Points.push_back(E);

	std::vector<MVector2> ConvexPoints;
	MConvexHullAlgorithm::MakeConvexHullByCounterClockWise2D(Points,ConvexPoints);
	ASSERT_EQ( 4, ConvexPoints.size() );
	EXPECT_TRUE( A == ConvexPoints[0] );
	EXPECT_TRUE( B == ConvexPoints[1] );
	EXPECT_TRUE( C == ConvexPoints[2] );
	EXPECT_TRUE( D == ConvexPoints[3] );
}

TEST( TestMakeConvexHullByCounterClockWise2D, Pentagon )
{
	std::vector<MVector2> Points;
	MVector2 A( 1, 0);
	MVector2 B( 3, 1);
	MVector2 C( 0, 2);
	MVector2 D(-3, 1);
	MVector2 E(-1, 0);

	Points.push_back(E); 
	Points.push_back(MVector2( 0,1));//concave point
	Points.push_back(A);
	Points.push_back(B); 
	Points.push_back(MVector2( 2,1));//concave point
	Points.push_back(C); 
	Points.push_back(D); 

	std::vector<MVector2> ConvexPoints;
	MConvexHullAlgorithm::MakeConvexHullByCounterClockWise2D(Points,ConvexPoints);
	ASSERT_EQ( 5, ConvexPoints.size() );
	EXPECT_TRUE( A == ConvexPoints[0] );
	EXPECT_TRUE( B == ConvexPoints[1] );
	EXPECT_TRUE( C == ConvexPoints[2] );
	EXPECT_TRUE( D == ConvexPoints[3] );
	EXPECT_TRUE( E == ConvexPoints[4] );
}

TEST( TestSortByAngle, SortByAngle )
{
	std::vector<MVector2> Points;
	Points.push_back(MVector2(0,0));
	Points.push_back(MVector2(0,1));
	Points.push_back(MVector2(1,0));
	Points.push_back(MVector2(1,1));

	MConvexHullAlgorithm::SortByAngle(Points);
	
	EXPECT_TRUE( MVector2(1,0) == Points[0] );
	EXPECT_TRUE( MVector2(1,1) == Points[1] );
	EXPECT_TRUE( MVector2(0,1) == Points[2] );
	EXPECT_TRUE( MVector2(0,0) == Points[3] );
}

TEST( TestSortByAngle, DuplicatedData )
{
	std::vector<MVector2> Points;
	Points.push_back(MVector2(0,0));
	Points.push_back(MVector2(0,1));
	Points.push_back(MVector2(0,0));
	Points.push_back(MVector2(1,0));
	Points.push_back(MVector2(1,1));
	Points.push_back(MVector2(1,0));
	Points.push_back(MVector2(1,1));
	Points.push_back(MVector2(0,1));
	unsigned int OriginalSize = Points.size();

	MConvexHullAlgorithm::SortByAngle(Points);

	ASSERT_EQ( OriginalSize,Points.size() );
	EXPECT_TRUE( MVector2(1,0) == Points[0] );
	EXPECT_TRUE( MVector2(1,0) == Points[1] );
	EXPECT_TRUE( MVector2(1,1) == Points[2] );
	EXPECT_TRUE( MVector2(1,1) == Points[3] );
	EXPECT_TRUE( MVector2(0,1) == Points[4] );
	EXPECT_TRUE( MVector2(0,1) == Points[5] );
	EXPECT_TRUE( MVector2(0,0) == Points[6] );
	EXPECT_TRUE( MVector2(0,0) == Points[7] );
}

TEST( TestBottomRightMostPointIndex, index)
{
	std::vector<MVector2> Points;
	Points.push_back(MVector2(0,0));
	Points.push_back(MVector2(0,1));
	Points.push_back(MVector2(1,0));
	Points.push_back(MVector2(1,1));

	EXPECT_EQ( 2, MConvexHullAlgorithm::BottomRightMostPointIndex(Points) );
}

TEST( TestGetAngle,angle)
{
	EXPECT_FLOAT_EQ(  0.0f, MConvexHullAlgorithm::GetAngle(MVector2(0,0),MVector2(0,0)) );

	EXPECT_FLOAT_EQ(  0.0f, MConvexHullAlgorithm::GetAngle(MVector2(0,0),MVector2(1,0)) );
	EXPECT_FLOAT_EQ( 45.0f, MConvexHullAlgorithm::GetAngle(MVector2(0,0),MVector2(1,1)) );
	EXPECT_FLOAT_EQ( 90.0f, MConvexHullAlgorithm::GetAngle(MVector2(0,0),MVector2(0,1)) );
	EXPECT_FLOAT_EQ(135.0f, MConvexHullAlgorithm::GetAngle(MVector2(0,0),MVector2(-1,1)) );
	EXPECT_FLOAT_EQ(180.0f, MConvexHullAlgorithm::GetAngle(MVector2(0,0),MVector2(-1,0)) );
	EXPECT_FLOAT_EQ(225.0f, MConvexHullAlgorithm::GetAngle(MVector2(0,0),MVector2(-1,-1)) );
	EXPECT_FLOAT_EQ(270.0f, MConvexHullAlgorithm::GetAngle(MVector2(0,0),MVector2(0,-1)) );
	EXPECT_FLOAT_EQ(315.0f, MConvexHullAlgorithm::GetAngle(MVector2(0,0),MVector2(1,-1)) );
	
}


TEST( TestQuadrangleArea, Area)
{
	MVector2 A(0.0f,0.0f);
	MVector2 B(1.0f,0.0f);
	MVector2 C(0.0f,1.0f);
	
	EXPECT_FLOAT_EQ(1.0f,MConvexHullAlgorithm::QuadrangleArea(A,B,C));	//positive area
	EXPECT_FLOAT_EQ(-1.0f,MConvexHullAlgorithm::QuadrangleArea(C,B,A));	//negative area
}

TEST( TestCounterClockWise, CounterClockWise)
{
	MVector2 A(0.0f,0.0f);
	MVector2 B(1.0f,0.0f);
	MVector2 C(0.0f,1.0f);

	EXPECT_EQ(true,MConvexHullAlgorithm::CounterClockWise(A,B,C));
	EXPECT_EQ(true,MConvexHullAlgorithm::CounterClockWise(B,C,A));
	EXPECT_EQ(true,MConvexHullAlgorithm::CounterClockWise(C,A,B));

	EXPECT_EQ(false,MConvexHullAlgorithm::CounterClockWise(C,B,A));
	EXPECT_EQ(false,MConvexHullAlgorithm::CounterClockWise(B,A,C));
	EXPECT_EQ(false,MConvexHullAlgorithm::CounterClockWise(A,C,B));
	
}