#include "stdafx.h"
#include "RS3UnitTestUtility.h"
#include "RTypes.h"
#include "RSceneManager.h"
#include "RDecalPolygonSceneNode.h"

using namespace rs3;

class TestDecal : public NullDeviceFixture
{
};

TEST_F( TestDecal, TestBoundingVolume_UpdateAABB )
{
	// 설치
	const RVector VERTEX_POS1( 200.f, 300.f, 500.f );
	const RVector VERTEX_POS2( 0.f, -10.f, 700.f );
	RBoundingBox EXPECT_AABB;
	EXPECT_AABB.Initialize();
	std::vector<RVt_pos_clr_tex1> decalVertexList;
	{
		RVt_pos_clr_tex1 vertex;
		vertex.vPos = VERTEX_POS1;
		decalVertexList.push_back( vertex );
		EXPECT_AABB.Add( vertex.vPos );

		vertex.vPos = VERTEX_POS2;
		decalVertexList.push_back( vertex );
		EXPECT_AABB.Add( vertex.vPos );
	}
	RDecalPolygonRenderSceneNode decal( 0, 0 );

	REngine::GetSceneManager().GetRootSceneNode().AddChild( &decal );

	// 실행
	decal.UpdateAABB( decalVertexList );
	UpdateSceneManager();

	// 검증
	EXPECT_TRUE( EXPECT_AABB.IsEqual( decal.GetAABB() ) );
	EXPECT_EQ( EXPECT_AABB.GetRadius(), decal.GetSphere().radius );
	EXPECT_EQ( EXPECT_AABB.GetCenter(), decal.GetSphere().center );
}