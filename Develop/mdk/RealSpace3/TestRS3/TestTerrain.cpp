#include "stdafx.h"

#include "REngine.h"
#include "RSceneManager.h"
#include "RCameraSceneNode.h"
#include "RTerrain.h"

SUITE(Terrain)
{
	const char *ZONE_FILE_NAME = "Login.zone.xml";
	const char *NORMALMAP_FILE_NAME = "BirdTownNormalmap.zone.xml";

	TEST(TestCreateBakedMap)
	{
		RUnitTestRunner::GetInstance().ResetEngine();

		bool bRet = REngine::GetSceneManager().LoadWorldSceneNodeFromRootXMLFile( REngine::GetSceneManager().GetCurrentRootSceneNode(true), ZONE_FILE_NAME );
		CHECK( bRet );

		RTerrain* pTerrain = REngine::GetSceneManager().GetCurrentTerrain();
		CHECK( pTerrain );

		bRet = pTerrain->CreateBakedMap();
		CHECK( bRet );

		RUnitTestRunner::GetInstance().SaveImageFile( "TestTerrainCreateBakedMap", pTerrain->GetBakedMap() );
	}

	TEST(TestNormalMap)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();

		// 존 로딩
		bool bRet = REngine::GetSceneManager().LoadWorldSceneNodeFromRootXMLFile( REngine::GetSceneManager().GetCurrentRootSceneNode(true), NORMALMAP_FILE_NAME);
		CHECK(bRet);

		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();
		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK( pCamera );

		pCamera->SetPosition( RVector3( 29395, 28137, 40));			
		pCamera->SetDirection( RVector(0.012f, 0.854f, -0.718f) );
		pCamera->UpdateViewFrustrum();
		pCamera->UpdateTransform();

		// 스크린 샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("TestTerrainNormalmap");

		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();
	}
}
