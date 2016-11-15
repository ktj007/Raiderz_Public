#include "stdafx.h"
#include "RS3UnitTestUtility.h"
#include "RTypes.h"
#include "RSceneNode.h"
#include "RSceneManager.h"

using namespace rs3;

class TestSceneNode : public NullDeviceFixture
{
};

/// 트랜스폼 업데이트 하지 않고 AABB만 변경해도 Bounding Volume Sphere가 제대로 갱신되는지 테스트
TEST_F( TestSceneNode, TestBoundingVolume_SetAABB )
{
	// 설치
	RBoundingBox expectAABB( RVector( -90.f, -180.f, 40.f ), RVector( 320.f, 530.f, 120.f ) );
	EXPECT_FALSE( RBoundingBox::INVALID.IsEqual( expectAABB ) );
	EXPECT_NE( 0, expectAABB.GetRadius() );
	EXPECT_NE( RVector::ZERO, expectAABB.GetCenter() );

	RSceneNode sceneNode;
	REngine::GetSceneManager().GetRootSceneNode().AddChild( &sceneNode );
	EXPECT_TRUE( RBoundingBox::INVALID.IsEqual( sceneNode.GetAABB() ) ) << "SceneNode BoundingBox 초기값은 invalid여야 한다.";

	// 실행
	sceneNode.SetAABB( expectAABB );
	// 검증
	EXPECT_TRUE( expectAABB.IsEqual( sceneNode.GetAABB() ) );
	EXPECT_NE( expectAABB.GetRadius(), sceneNode.GetSphere().radius ) << "AABB가 변경되어도 SceneManager Update 전에는 BoundingSphere값이 변하지 않는다.";
	EXPECT_NE( expectAABB.GetCenter(), sceneNode.GetSphere().center ) << "AABB가 변경되어도 SceneManager Update 전에는 BoundingSphere값이 변하지 않는다.";

	// 실행
	UpdateSceneManager();

	// 검증
	EXPECT_TRUE( expectAABB.IsEqual( sceneNode.GetAABB() ) );
	EXPECT_EQ( expectAABB.GetRadius(), sceneNode.GetSphere().radius ) << "AABB가 변경되면 SceneManager Update 후 BoundingSphere값이 올바르게 갱신되어야 한다.";
	EXPECT_EQ( expectAABB.GetCenter(), sceneNode.GetSphere().center ) << "AABB가 변경되면 SceneManager Update 후 BoundingSphere값이 올바르게 갱신되어야 한다.";

	sceneNode.RemoveFromParent();
}