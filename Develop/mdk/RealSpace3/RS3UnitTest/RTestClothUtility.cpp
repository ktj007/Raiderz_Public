#include "stdafx.h"
#include "RTypes.h"
#include "RClothUtility.h"

using namespace rs3;


class TestClothUtility : public RClothUtility
{ 
public:
	using RClothUtility::calculateIndexAndTriangleTypeByHeightAndRedColor;
};

class TestClothUtilityFixture : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
		index0 = First;
		index1 = Second;
		index2 = Third;
	}

	float				r0,r1,r2;
	RC_VERTEX_INDEX		index0,index1,index2;
	RC_TRIANGLE_TYPE	triangleType;

	static const float LESS_ATTACHING_RED_VALUE;
	static const float GREATER_ATTACHING_RED_VALUE;
	static const int	First = 0;
	static const int	Second = 1;
	static const int	Third = 2;
};
const float TestClothUtilityFixture::LESS_ATTACHING_RED_VALUE = ATTACHING_RED_VALUE - 1.0f;
const float TestClothUtilityFixture::GREATER_ATTACHING_RED_VALUE = ATTACHING_RED_VALUE + 1.0f;


TEST_F( TestClothUtilityFixture, calculateIndexAndTriangleTypeByHeightAndRedColor_TYPE_3_RED)
{
	r0 = GREATER_ATTACHING_RED_VALUE;
	r1 = GREATER_ATTACHING_RED_VALUE;
	r2 = GREATER_ATTACHING_RED_VALUE;

	TestClothUtility::calculateIndexAndTriangleTypeByHeightAndRedColor(index0,index1,index2,triangleType,r0,r1,r2);

	//세정점이 모두 255이면, 세정점 모두 attaching point이고, 인덱스는 바뀔 필요가 없다
	EXPECT_EQ(RC_TRIANGLE_TYPE_3_RED,triangleType);
	EXPECT_EQ(First,index0);
	EXPECT_EQ(Second,index1);
	EXPECT_EQ(Third,index2);
}

TEST_F( TestClothUtilityFixture, calculateIndexAndTriangleTypeByHeightAndRedColor_TYPE_2_RED_0)
{
	r0 = GREATER_ATTACHING_RED_VALUE;
	r1 = GREATER_ATTACHING_RED_VALUE;
	r2 = LESS_ATTACHING_RED_VALUE;

	TestClothUtility::calculateIndexAndTriangleTypeByHeightAndRedColor(index0,index1,index2,triangleType,r0,r1,r2);

	EXPECT_EQ(RC_TRIANGLE_TYPE_2_RED,triangleType);
	EXPECT_EQ(First,index0);
	EXPECT_EQ(Second,index1);
	EXPECT_EQ(Third,index2);
}

TEST_F( TestClothUtilityFixture, calculateIndexAndTriangleTypeByHeightAndRedColor_TYPE_2_RED_1)
{
	r0 = GREATER_ATTACHING_RED_VALUE;
	r1 = LESS_ATTACHING_RED_VALUE;
	r2 = GREATER_ATTACHING_RED_VALUE;

	TestClothUtility::calculateIndexAndTriangleTypeByHeightAndRedColor(index0,index1,index2,triangleType,r0,r1,r2);

	EXPECT_EQ(RC_TRIANGLE_TYPE_2_RED,triangleType);
	EXPECT_EQ(First,index0);
	EXPECT_EQ(Third,index1);
	EXPECT_EQ(Second,index2);
}

TEST_F( TestClothUtilityFixture, calculateIndexAndTriangleTypeByHeightAndRedColor_TYPE_2_RED_2)
{
	r0 = LESS_ATTACHING_RED_VALUE;
	r1 = GREATER_ATTACHING_RED_VALUE;
	r2 = GREATER_ATTACHING_RED_VALUE;

	TestClothUtility::calculateIndexAndTriangleTypeByHeightAndRedColor(index0,index1,index2,triangleType,r0,r1,r2);

	EXPECT_EQ(RC_TRIANGLE_TYPE_2_RED,triangleType);
	EXPECT_EQ(Third,index0);
	EXPECT_EQ(Second,index1);
	EXPECT_EQ(First,index2);
}

TEST_F( TestClothUtilityFixture, calculateIndexAndTriangleTypeByHeightAndRedColor_TYPE_1_RED_0)
{
	r0 = GREATER_ATTACHING_RED_VALUE;
	r1 = LESS_ATTACHING_RED_VALUE;
	r2 = LESS_ATTACHING_RED_VALUE;

	TestClothUtility::calculateIndexAndTriangleTypeByHeightAndRedColor(index0,index1,index2,triangleType,r0,r1,r2);

	EXPECT_EQ(RC_TRIANGLE_TYPE_1_RED,triangleType);
	EXPECT_EQ(First,index0);
	EXPECT_EQ(Second,index1);
	EXPECT_EQ(Third,index2);
}

TEST_F( TestClothUtilityFixture, calculateIndexAndTriangleTypeByHeightAndRedColor_TYPE_1_RED_1)
{
	r0 = LESS_ATTACHING_RED_VALUE;
	r1 = GREATER_ATTACHING_RED_VALUE;
	r2 = LESS_ATTACHING_RED_VALUE;

	TestClothUtility::calculateIndexAndTriangleTypeByHeightAndRedColor(index0,index1,index2,triangleType,r0,r1,r2);

	EXPECT_EQ(RC_TRIANGLE_TYPE_1_RED,triangleType);
	EXPECT_EQ(Second,index0);
	EXPECT_EQ(First,index1);
	EXPECT_EQ(Third,index2);
}

TEST_F( TestClothUtilityFixture, calculateIndexAndTriangleTypeByHeightAndRedColor_TYPE_1_RED_2)
{
	r0 = LESS_ATTACHING_RED_VALUE;
	r1 = LESS_ATTACHING_RED_VALUE;
	r2 = GREATER_ATTACHING_RED_VALUE;

	TestClothUtility::calculateIndexAndTriangleTypeByHeightAndRedColor(index0,index1,index2,triangleType,r0,r1,r2);

	EXPECT_EQ(RC_TRIANGLE_TYPE_1_RED,triangleType);
	EXPECT_EQ(Third,index0);
	EXPECT_EQ(Second,index1);
	EXPECT_EQ(First,index2);
}

TEST_F( TestClothUtilityFixture, calculateIndexAndTriangleTypeByHeightAndRedColor_TYPE_0_RED)
{
	r0 = LESS_ATTACHING_RED_VALUE;
	r1 = LESS_ATTACHING_RED_VALUE;
	r2 = LESS_ATTACHING_RED_VALUE;

	TestClothUtility::calculateIndexAndTriangleTypeByHeightAndRedColor(index0,index1,index2,triangleType,r0,r1,r2);

	EXPECT_EQ(RC_TRIANGLE_TYPE_0_RED,triangleType);
	EXPECT_EQ(First,index0);
	EXPECT_EQ(Second,index1);
	EXPECT_EQ(Third,index2);
}