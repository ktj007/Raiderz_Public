#include "stdafx.h"

#include "REngine.h"
#include "RSceneManager.h"
#include "RCameraSceneNode.h"
#include "REnvironmentSet.h"
#include "RTerrain.h"

SUITE(Map)
{
	const char *ZONE_FILE_NAME1 = "Login.zone.xml";
	const char *ZONE_FILE_NAME2 = "BirdTown.zone.xml";

	TEST(TestLoadZone)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();

		// 존 로딩
		bool bRet = REngine::GetSceneManager().LoadWorldSceneNodeFromRootXMLFile( REngine::GetSceneManager().GetCurrentRootSceneNode(true),ZONE_FILE_NAME1);
		CHECK(bRet);

		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK( pCamera );
		pCamera->SetPosition(RVector3(15010.0f, 8510.0f, 410.0f)); // 걍 임의의 포지션

		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("TestLoadZone");
		// 로딩된 존은 RSceneManager Clear시에 알아서 지워진다. 따로 지워줄필요 없음

		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();
	}

	TEST(TestGridZone)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();

		// 존 로딩
		bool bRet = REngine::GetSceneManager().LoadWorldSceneNodeFromRootXMLFile( REngine::GetSceneManager().GetCurrentRootSceneNode(true),ZONE_FILE_NAME1);
		CHECK(bRet);

		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK( pCamera );
		pCamera->SetPosition(RVector3(15010.0f, 8510.0f, 410.0f)); // 걍 임의의 포지션

		if( REngine::GetSceneManager().GetCurrentTerrain())
		{
			REngine::GetSceneManager().GetCurrentTerrain()->SetGridFlag(true);
			REngine::GetSceneManager().GetCurrentTerrain()->SetGrassLodParams( 0, 0);
		}
		
		pCamera->SetPassIgnore((RRENDERPASS)0);
		pCamera->SetPassIgnore( RRP_TERRAIN, false);

		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("TestGridZone");
		// 로딩된 존은 RSceneManager Clear시에 알아서 지워진다. 따로 지워줄필요 없음

		if( REngine::GetSceneManager().GetCurrentTerrain())
			REngine::GetSceneManager().GetCurrentTerrain()->SetGridFlag(false);

		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();
	}

	TEST(TestChangeZone)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();

		// 존 로딩
		bool bRet = REngine::GetSceneManager().LoadWorldSceneNodeFromRootXMLFile( REngine::GetSceneManager().GetCurrentRootSceneNode(true),ZONE_FILE_NAME1);
		CHECK(bRet);

		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		RCameraSceneNode* pOldCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK( pOldCamera );
		pOldCamera->SetPosition(RVector3(15010.0f, 8510.0f, 410.0f)); // 걍 임의의 포지션

		// 스크린샷2
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("ResetZoneLogin");

		// Primary Camera를 NULL로 만들고 예전에 붙어있던 Primary Camera는 띄어준다.
		RCameraSceneNode* pCamera = REngine::GetSceneManager().SetPrimaryCamera(NULL);
		CHECK( pOldCamera == pCamera );
		pCamera->RemoveFromParent();

		// 씬매니저 Clear
		REngine::GetSceneManager().Clear();

		// 존 로딩
		bRet = REngine::GetSceneManager().LoadWorldSceneNodeFromRootXMLFile( REngine::GetSceneManager().GetCurrentRootSceneNode(true),ZONE_FILE_NAME2);
		CHECK(bRet);

		// SetPrimaryCamera를 하면 예전의 Camera를 리턴해준다.
		// 씬매니저 Clear전에 SetPrimary(NULL)을 했으므로 NULL을 리턴해야 한다.
		RCameraSceneNode* pPreviousCamera = REngine::GetSceneManager().SetPrimaryCamera(pCamera);
		CHECK( pPreviousCamera == NULL );
		CHECK(pCamera);
		REngine::GetSceneManager().AddSceneNode(pCamera);

		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("ResetZoneBirdTown");

		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();
	}
}
