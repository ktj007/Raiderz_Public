#include "stdafx.h"

#include "REngine.h"
#include "RSceneManager.h"
#include "REnvironmentSet.h"
#include "RWaterPassRenderer.h"

SUITE(Environment)
{
	const char *ZONE_FILE_NAME = "Login.zone.xml";
	const char *ZONE_ENV_FILE_NAME = "Login.zone.env.xml";
	const char *ZONE_FILE_NAME2 = "Login_NoObject.zone.xml";

	TEST(Environment_Load)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();

		// 존 로딩
		bool bRet = REngine::GetSceneManager().LoadWorldSceneNodeFromRootXMLFile( REngine::GetSceneManager().GetCurrentRootSceneNode(true),ZONE_FILE_NAME);
		CHECK(bRet);

		// 환경 씬노드 생성
		REnvironmentSet* pEnvironmentSet = new REnvironmentSet;

		// 파일로부터 만들기
		bRet = pEnvironmentSet->CreateFromFile(ZONE_ENV_FILE_NAME, false);
		CHECK(bRet);

		// 씬매니저에 더하기
		REngine::GetSceneManager().AddSceneNode(pEnvironmentSet);

		// 환경 설정
		bRet = pEnvironmentSet->SetCurrentEnvSet(REnvironment::DAWN, REnvironment::SUNNY, false, 0.f, false);
		CHECK(bRet);
		REngine::GetSceneManager().UseLighting(true);

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK( pCamera );
		pCamera->SetPosition(RVector3(15010.0f, 8510.0f, 410.0f)); // 걍 임의의 포지션

		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("Environment_Load");

		// 환경 씬노드 소멸
		pEnvironmentSet->RemoveFromParent();
		delete pEnvironmentSet;
	}

	TEST(Environment_Change)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();

		// 존 로딩
		bool bRet = REngine::GetSceneManager().LoadWorldSceneNodeFromRootXMLFile( REngine::GetSceneManager().GetCurrentRootSceneNode(true),ZONE_FILE_NAME);
		CHECK(bRet);

		// 환경 씬노드 생성
		REnvironmentSet* pEnvironmentSet = new REnvironmentSet;

		// 파일로부터 만들기
		bRet = pEnvironmentSet->CreateFromFile(ZONE_ENV_FILE_NAME, false);
		CHECK(bRet);

		// 씬매니저에 더하기
		REngine::GetSceneManager().AddSceneNode(pEnvironmentSet);

		// 환경 설정
		bRet = pEnvironmentSet->SetCurrentEnvSet(REnvironment::DAWN, REnvironment::SUNNY, false, 0.f, false);
		CHECK(bRet);
		REngine::GetSceneManager().UseLighting(true);

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK( pCamera );
		pCamera->SetPosition(RVector3(15010.0f, 8510.0f, 410.0f)); // 걍 임의의 포지션

		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("Environment_Change_DAWN_SUNNY");

		// 환경 변경
		bRet = pEnvironmentSet->SetCurrentEnvSet(REnvironment::NIGHT, REnvironment::HEAVY_RANIY, false, 0.f, false);
		CHECK(bRet);		

		/* 스크린샷이 빌드 때마다 결과가 달라져 임시로 뺌,2010.06.25 by ignore
		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("Environment_Change_NIGHT_HEAVY_RANIY");
		*/

		// 환경 씬노드 소멸
		pEnvironmentSet->RemoveFromParent();
		delete pEnvironmentSet;
	}

	TEST(TestFogOff)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();

		// 존 로딩
		bool bRet = REngine::GetSceneManager().LoadWorldSceneNodeFromRootXMLFile( REngine::GetSceneManager().GetCurrentRootSceneNode(true),ZONE_FILE_NAME);
		CHECK(bRet);

		// 환경 씬노드 생성
		REnvironmentSet* pEnvironmentSet = new REnvironmentSet;

		// 파일로부터 만들기
		bRet = pEnvironmentSet->CreateFromFile(ZONE_ENV_FILE_NAME, false);
		CHECK(bRet);

		// 씬매니저에 더하기
		REngine::GetSceneManager().AddSceneNode(pEnvironmentSet);

		// 환경 설정
		bRet = pEnvironmentSet->SetCurrentEnvSet(REnvironment::DAWN, REnvironment::SUNNY, false, 0.f, false);
		CHECK(bRet);

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK( pCamera );
		pCamera->SetPosition(RVector3(15010.0f, 8510.0f, 410.0f)); // 걍 임의의 포지션

		pCamera->SetFogEnable( false );

		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("TestLoadEnvironmentFogOff");

		// 환경 씬노드 소멸
		pEnvironmentSet->RemoveFromParent();
		delete pEnvironmentSet;
	}
}
