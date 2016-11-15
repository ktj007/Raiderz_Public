#include "stdafx.h"
#include "RShaderComponentID.h"
#include "RMaterial.h"
#include "RShaderCompositeManager.h"

using namespace rs3;


TEST(TestShaderComponentID, TestHasComponent_HaveNoSameID_ShouldFalse)
{
	//설치
	RSCID testID = RSC_POSITION | RSC_TEXCOORD | RSC_DIFFUSEMAP;
	RSCID comparedID = RSC_TRANSFORM;


	//실행
	bool bResult = testID.HasComponent(comparedID);


	//검증
	EXPECT_FALSE(bResult);
}

TEST(TestShaderComponentID, TestHasComponent_HaveOneSameID_ShouldTrue)
{
	//설치
	RSCID testID = RSC_POSITION | RSC_TEXCOORD | RSC_DIFFUSEMAP;
	RSCID comparedID = RSC_POSITION;


	//실행
	bool bResult = testID.HasComponent(comparedID);


	//검증
	EXPECT_TRUE(bResult);
}

TEST(TestShaderComponentID, TestHasComponent_HaveSeveralSameID_ShouldTrue)
{
	//설치
	RSCID testID = RSC_POSITION | RSC_TEXCOORD | RSC_DIFFUSEMAP;
	RSCID comparedID = RSC_POSITION | RSC_TEXCOORD;


	//실행
	bool bResult = testID.HasComponent(comparedID);


	//검증
	EXPECT_TRUE(bResult);
}

TEST(TestShaderComponentID, TestHasComponent_HaveOneSameIDAndOneOtherID_ShouldTrue)
{
	//설치
	RSCID testID = RSC_POSITION | RSC_TEXCOORD | RSC_DIFFUSEMAP;
	RSCID comparedID = RSC_POSITION | RSC_TRANSFORM;


	//실행
	bool bResult = testID.HasComponent(comparedID);


	//검증
	EXPECT_TRUE(bResult);
}










TEST(TestShaderComponentID, TestHasAllComponentt_HaveNoSameID_ShouldFalse)
{
	//설치
	RSCID testID = RSC_POSITION | RSC_TEXCOORD | RSC_DIFFUSEMAP;
	RSCID comparedID = RSC_TRANSFORM;


	//실행
	bool bResult = testID.HasAllComponent(comparedID);


	//검증
	EXPECT_FALSE(bResult);
}

TEST(TestShaderComponentID, TestHasAllComponent_HaveOneSameID_ShouldTrue)
{
	//설치
	RSCID testID = RSC_POSITION | RSC_TEXCOORD | RSC_DIFFUSEMAP;
	RSCID comparedID = RSC_POSITION;


	//실행
	bool bResult = testID.HasAllComponent(comparedID);


	//검증
	EXPECT_TRUE(bResult);
}

TEST(TestShaderComponentID, TestHasAllComponent_HaveSeveralSameID_ShouldTrue)
{
	//설치
	RSCID testID = RSC_POSITION | RSC_TEXCOORD | RSC_DIFFUSEMAP;
	RSCID comparedID = RSC_POSITION | RSC_TEXCOORD;


	//실행
	bool bResult = testID.HasAllComponent(comparedID);


	//검증
	EXPECT_TRUE(bResult);
}

TEST(TestShaderComponentID, TestHasAllComponent_HaveOneSameIDAndOneOtherID_ShouldFalse)
{
	//설치
	RSCID testID = RSC_POSITION | RSC_TEXCOORD | RSC_DIFFUSEMAP;
	RSCID comparedID = RSC_POSITION | RSC_TRANSFORM;


	//실행
	bool bResult = testID.HasAllComponent(comparedID);


	//검증
	EXPECT_FALSE(bResult);
}