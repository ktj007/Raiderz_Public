#include "stdafx.h"

#include "REngine.h"
#include "RSceneManager.h"
#include "RTreeSceneNode.h"
#include "RCameraSceneNode.h"

SUITE(Tree)
{
	TEST(TestLoadTree)
	{
		using namespace rs3;

		RUnitTestRunner::GetInstance().ResetEngine();
		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		RSceneManager *pSceneManager = REngine::GetSceneManagerPtr();
		_ASSERT(pSceneManager);

		RTreeSceneNode *pTree = static_cast<RTreeSceneNode*>(pSceneManager->CreateSceneNode(RSID_TREE));
		bool bResult = pTree->Build("Swamp_CinnamonFern_RT.spt");

		CHECK(bResult);

		pSceneManager->AddSceneNode(pTree);

		RVector vPos(RVector::ZERO);
		RVector vDir(RVector::AXISY);

		pTree->SetPosition(vPos);

		RSceneNodeUpdateVisitor visitor;
		pTree->Traverse(&visitor);

		const float* pTreeLocalAABB = pTree->GetLocalAABB();
		RBoundingBox aabb( pTreeLocalAABB[0], pTreeLocalAABB[1], pTreeLocalAABB[2], pTreeLocalAABB[3], pTreeLocalAABB[4], pTreeLocalAABB[5] );
		RVector sizeXYZ = aabb.vmax - aabb.vmin;
		
		RCameraSceneNode* pCamera = pSceneManager->GetPrimaryCamera();
		CHECK(pCamera);
		pCamera->Set(vPos + vDir * (sizeXYZ.y/2.f + 10.f), -vDir);

		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("LoadTreeSimple");		

		pTree->RemoveFromParent();
		delete pTree;
		pTree = NULL;

		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();
	}
}