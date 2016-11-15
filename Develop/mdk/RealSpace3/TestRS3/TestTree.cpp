#include "stdafx.h"

#include "REngine.h"
#include "RSceneManager.h"
#include "RTreeSceneNode.h"
#include "RCameraSceneNode.h"

SUITE(Tree)
{
	TEST(TestTreeBillboard)
	{
		using namespace rs3;

		const int TREE_NUM = 4;

		RUnitTestRunner::GetInstance().ResetEngine();
		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		RSceneManager *pSceneManager = REngine::GetSceneManagerPtr();
		_ASSERT( pSceneManager );

		RTreeSceneNode *pTree[TREE_NUM];
		for( int i = 0; i < TREE_NUM; ++i )
		{
			pTree[i] = static_cast<RTreeSceneNode*>(pSceneManager->CreateSceneNode(RSID_TREE));
			bool bResult = pTree[i]->Build("k_teress_CommonOlive_RT.spt");
			CHECK( bResult );
			pSceneManager->AddSceneNode( pTree[i] );
		}

		const float* pTreeLocalAABB = pTree[0]->GetLocalAABB();
		RBoundingBox aabbTree( pTreeLocalAABB[0], pTreeLocalAABB[1], pTreeLocalAABB[2], pTreeLocalAABB[3], pTreeLocalAABB[4], pTreeLocalAABB[5] );
		RVector aabbSize = aabbTree.vmax - aabbTree.vmin;
		for( int i = 0; i < TREE_NUM; ++i )
		{
			RVector pos( i * aabbSize.x, 0.f, 0.f );

			RMatrix mat;
			mat.SetRotationZ( i * MMath::HALFPI );
			mat.SetTranslation( pos );
			pTree[i]->SetTransform( mat );
			pTree[i]->UpdateTransform();
		}

		RCameraSceneNode* pCamera = pSceneManager->GetPrimaryCamera();
		CHECK( pCamera );
		pCamera->Set( RVector( aabbSize.x * 0.5f * TREE_NUM - aabbSize.x * 0.5f, aabbSize.x * 0.6f * TREE_NUM , aabbSize.z * 0.5f ), -RVector::AXISY );

		pCamera->SetTreeUserLOD( true, 0.0f );
		RUnitTestRunner::GetInstance().UpdateScene( true );
		RUnitTestRunner::GetInstance().GrabScreenShot( "TestTreeBillboard_0" );

		pCamera->SetTreeUserLOD( true, 1.0f );
		RUnitTestRunner::GetInstance().UpdateScene( true );
		RUnitTestRunner::GetInstance().GrabScreenShot( "TestTreeBillboard_1" );

		for( int i = 0; i < TREE_NUM; ++i )
		{
			pTree[i]->RemoveFromParent();
			delete pTree[i];
		}

		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();
	}
}