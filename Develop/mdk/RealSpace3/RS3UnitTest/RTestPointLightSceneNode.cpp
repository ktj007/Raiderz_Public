#include "stdafx.h"
#include "RS3UnitTestUtility.h"
#include "RSceneManager.h"
#include "RTypes.h"
#include "RPointLightSceneNode.h"

using namespace rs3;

class TestPointLightSceneNode : public NullDeviceFixture
{
};

TEST_F( TestPointLightSceneNode, TestBoundingVolume_SetRadius )
{
	// 설치
	RVector WORLD_POS( 20.f, -40.f, 120.f );
	const float EXPECT_RADIUS = 1000.f;
	RSphere EXPECT_BOUNDING_SPHERE( WORLD_POS, EXPECT_RADIUS );

	RAABB EXPECT_AABB;
	EXPECT_AABB.vmin = WORLD_POS - EXPECT_RADIUS;
	EXPECT_AABB.vmax = WORLD_POS + EXPECT_RADIUS;

	RPointLightSceneNode pointLight;
	REngine::GetSceneManager().GetRootSceneNode().AddChild( &pointLight );

	EXPECT_EQ( 1.f, pointLight.GetScale() );
	EXPECT_FALSE( pointLight.IsAreaLight() );
	pointLight.SetPosition( WORLD_POS );
	pointLight.SetRadius( 0.f );
	pointLight.NeedUpdateTransform();
	pointLight.UpdateTransform();
	UpdateSceneManager();

	// 실행
	pointLight.SetRadius( EXPECT_RADIUS );
	UpdateSceneManager();

	// 검증
	EXPECT_TRUE( EXPECT_AABB.IsEqual( pointLight.GetAABB() ) );
	EXPECT_EQ( EXPECT_BOUNDING_SPHERE.center, pointLight.GetSphere().center );
	EXPECT_EQ( EXPECT_BOUNDING_SPHERE.radius, pointLight.GetSphere().radius );

	pointLight.RemoveFromParent();
}

TEST_F( TestPointLightSceneNode, TestBoundingVolume_SetAreaRange )
{
	// 설치
	const float RADIUS = 100.f;
	const RVector AREA_RANGE( 10.f, 12.f, 37.f );
	RVector WORLD_POS( 20.f, -40.f, 120.f );
	RMatrix WORLD_MAT;
	WORLD_MAT.SetTranslationMatrix( WORLD_POS );

	RAABB LOCAL_AABB;
	LOCAL_AABB.vmin = RVector3(-1,-1,-1)*(RVector3(RADIUS)+AREA_RANGE*0.5f);
	LOCAL_AABB.vmax = RVector3(1,1,1)*(RVector3(RADIUS)+AREA_RANGE*0.5f);

	RAABB EXPECT_AABB;
	TransformBox(&EXPECT_AABB, LOCAL_AABB, WORLD_MAT);

	RPointLightSceneNode pointLight;
	REngine::GetSceneManager().GetRootSceneNode().AddChild( &pointLight );

	EXPECT_EQ( 1.f, pointLight.GetScale() );
	EXPECT_FALSE( pointLight.IsAreaLight() );
	pointLight.SetPosition( WORLD_POS );
	pointLight.SetRadius( RADIUS );
	pointLight.NeedUpdateTransform();
	pointLight.UpdateTransform();
	UpdateSceneManager();

	// 실행
	pointLight.SetAreaRange( AREA_RANGE );
	UpdateSceneManager();

	// 검증
	EXPECT_TRUE( pointLight.IsAreaLight() );
	EXPECT_TRUE( EXPECT_AABB.IsEqual( pointLight.GetAABB() ) );
	EXPECT_EQ( WORLD_POS, pointLight.GetSphere().center );
	EXPECT_EQ( EXPECT_AABB.GetRadius(), pointLight.GetSphere().radius );

	pointLight.RemoveFromParent();
}