#include "stdafx.h"
#include "RTypes.h"
#include "MXml.h"
#include "ROccluderSceneNode.h"

using namespace rs3;





TEST( TestROccluderSceneNode, ReadOccluderInfo )
{
	
	char OccluderXML[] =
	{
		"<DUMMY>\n\
		<OCCLUDERPOINT>\n\
		<P>1 2 3</P>\n\
		<P>-1 -2 -3</P>\n\
		<P>1.3 2.3 3.3</P>\n\
		<P>-1.3 -2.3 -3.3</P>\n\
		</OCCLUDERPOINT>\n\
		<OCCLUDERPOINT>\n\
		<P>4 5 6</P>\n\
		<P>-4 -5 -6</P>\n\
		<P>4.7 5.7 6.7</P>\n\
		<P>-4.7 -5.7 -6.7</P>\n\
		</OCCLUDERPOINT>\n\
		</DUMMY>"
	};// <DUMMY> 와 </DUMMY> 는 실제 포맷이 아니고, XML읽기 위해 붙인 임시문자

	MXml aXml;
	aXml.LoadStream(OccluderXML);
	MXmlHandle		docHandle = aXml.DocHandle();
	MXmlElement*	pChild = docHandle.FirstChildElement().Element();

	ROccluderSceneNode OccluderSceneNode;
	EXPECT_TRUE( OccluderSceneNode.CreateFromXML(*pChild,false) );

	
	ROccluderPointsSet *pOccluderPointsSet = OccluderSceneNode.GetOCcluderPointsSet();
	EXPECT_EQ( 2, pOccluderPointsSet->GetTheNumberOfOccludePoints() );

	EXPECT_EQ( 4, pOccluderPointsSet->GetOCcluderPoints(0).GetNumberOfPoints() );
	EXPECT_EQ( RVector3(1,2,3),				pOccluderPointsSet->GetOCcluderPoints(0).GetPoint(0) );
	EXPECT_EQ( RVector3(-1,-2,-3),			pOccluderPointsSet->GetOCcluderPoints(0).GetPoint(1) );
	EXPECT_EQ( RVector3(1.3f,2.3f,3.3f),	pOccluderPointsSet->GetOCcluderPoints(0).GetPoint(2) );
	EXPECT_EQ( RVector3(-1.3f,-2.3f,-3.3f),	pOccluderPointsSet->GetOCcluderPoints(0).GetPoint(3) );

	EXPECT_EQ( 4, pOccluderPointsSet->GetOCcluderPoints(1).GetNumberOfPoints() );
	EXPECT_EQ( RVector3(4,5,6),				pOccluderPointsSet->GetOCcluderPoints(1).GetPoint(0) );
	EXPECT_EQ( RVector3(-4,-5,-6),			pOccluderPointsSet->GetOCcluderPoints(1).GetPoint(1) );
	EXPECT_EQ( RVector3(4.7f,5.7f,6.7f),	pOccluderPointsSet->GetOCcluderPoints(1).GetPoint(2) );
	EXPECT_EQ( RVector3(-4.7f,-5.7f,-6.7f),	pOccluderPointsSet->GetOCcluderPoints(1).GetPoint(3) );
}


TEST( TestOccluderPoints, AddPoint )
{
	ROccluderPoints OccluderPoints;

	RVector3 P0(0,0,0);
	RVector3 P1(1,1,1);

	OccluderPoints.AddPoint(P0);
	OccluderPoints.AddPoint(P1);

	EXPECT_EQ( 2, OccluderPoints.GetNumberOfPoints() );

	EXPECT_EQ( P0 , OccluderPoints.GetPoint(0) );
	EXPECT_EQ( P1 , OccluderPoints.GetPoint(1) );
	EXPECT_TRUE( P1 != OccluderPoints.GetPoint(0) );
}

TEST( TestOccluderPoints, CenterPoint )
{
	ROccluderPoints OccluderPoints;
	RVector3 P0(0,0,0);
	RVector3 P1(2,2,2);

	OccluderPoints.AddPoint(P0);
	EXPECT_EQ(RVector3(0,0,0) , OccluderPoints.GetCenterPoint() );

	OccluderPoints.AddPoint(P1);
	EXPECT_EQ(RVector3(1,1,1) , OccluderPoints.GetCenterPoint() );

}

TEST( TestOccluderPoints, GetAABB )
{
	ROccluderPoints OccluderPoints;
	RVector3 P0(0,0,0);
	RVector3 P1(2,2,2);

	OccluderPoints.AddPoint(P0);
	OccluderPoints.AddPoint(P1);

	RAABB aabb; aabb.Initialize();
	aabb.Add(P0);
	aabb.Add(P1);

	EXPECT_TRUE( aabb.IsEqual(OccluderPoints.GetAABB()) );
}

TEST( ROccluderPointsSet, GetAABB )
{
	RVector3 P0(0,0,0);
	RVector3 P1(2,2,2);
	RVector3 P2(-1,-2,-3);
	RVector3 P3(6.7f,2.1f,2.3f);

	ROccluderPoints OccluderPoints0;
	ROccluderPoints OccluderPoints1;

	ROccluderPointsSet OccluderPointsSet;

	RAABB aabb; aabb.Initialize();

	OccluderPoints0.AddPoint(P0);
	OccluderPoints0.AddPoint(P1);
	OccluderPoints1.AddPoint(P2);
	OccluderPoints1.AddPoint(P3);

	OccluderPointsSet.AddOccluderPoints(OccluderPoints0);
	OccluderPointsSet.AddOccluderPoints(OccluderPoints1);

	aabb.Add(P0);
	aabb.Add(P1);
	aabb.Add(P2);
	aabb.Add(P3);

	EXPECT_TRUE( aabb.IsEqual(OccluderPointsSet.GetAABB()) );

}


TEST( TestOccluderUtil,ConvertOccluderToVariableFrusum)
{
	RVector3 ViewCenter(0,0,0);

	ROccluderPoints OccluderPoints;
	RVector3 P0(1,0,0);
	RVector3 P1(0,1,0);
	RVector3 P2(0,0,1);
	OccluderPoints.AddPoint(P0);
	OccluderPoints.AddPoint(P1);
	OccluderPoints.AddPoint(P2);

	RVariableFrustum VariableFrustum;

	ROccluderUtil::ConvertOccluderPointsToVariableFrusum(ViewCenter,OccluderPoints,VariableFrustum);

	EXPECT_EQ( OccluderPoints.GetNumberOfPoints() + 1 , VariableFrustum.GetNumberOfPlanes() );

	EXPECT_FLOAT_EQ( 0 , VariableFrustum.GetPlane(0).a );
	EXPECT_FLOAT_EQ( 0 , VariableFrustum.GetPlane(0).b );
	EXPECT_FLOAT_EQ( 1 , VariableFrustum.GetPlane(0).c );
	EXPECT_FLOAT_EQ( 0 , VariableFrustum.GetPlane(0).d );

	EXPECT_FLOAT_EQ( 1 , VariableFrustum.GetPlane(1).a );
	EXPECT_FLOAT_EQ( 0 , VariableFrustum.GetPlane(1).b );
	EXPECT_FLOAT_EQ( 0 , VariableFrustum.GetPlane(1).c );
	EXPECT_FLOAT_EQ( 0 , VariableFrustum.GetPlane(1).d );

	EXPECT_FLOAT_EQ( 0 , VariableFrustum.GetPlane(2).a );
	EXPECT_FLOAT_EQ( 1 , VariableFrustum.GetPlane(2).b );
	EXPECT_FLOAT_EQ( 0 , VariableFrustum.GetPlane(2).c );
	EXPECT_FLOAT_EQ( 0 , VariableFrustum.GetPlane(2).d );

	float k = 1/ sqrt(3.0f);
	EXPECT_FLOAT_EQ( k , VariableFrustum.GetPlane(3).a );
	EXPECT_FLOAT_EQ( k , VariableFrustum.GetPlane(3).b );
	EXPECT_FLOAT_EQ( k , VariableFrustum.GetPlane(3).c );
	EXPECT_FLOAT_EQ( -k , VariableFrustum.GetPlane(3).d );
}