#include "stdafx.h"

#include "REngine.h"
#include "RSceneManager.h"
#include "REnvironmentSet.h"
#include "RPointLightSceneNode.h"
#include "RSpotLightSceneNode.h"

SUITE(ETC)
{
	const char *ZONE_FILE_NAME = "Login.zone.xml";
	const char *ZONE_ENV_FILE_NAME = "Login.zone.env.xml";

	TEST(TestLight)
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
		pCamera->SetPosition(RVector3(17000.0f, 6300.0f, 800.0f)); // 걍 임의의 포지션
		pCamera->SetDirection( RVector(-0.46f, 0.78f, -0.4f) );

		//Point Light 생성
		RPointLightSceneNode PointLight;
		PointLight.SetPosition(16000.0f, 7000.0f, 100.0f);
		PointLight.SetRadius(1000.0f);
		PointLight.SetColorRGB(0xFFFF0000);
		PointLight.m_fApplyIntensity =2;
		PointLight.SetAreaRange(RVector3(500, 10, 10));
		PointLight.UpdateTransform();

		//Spot Light 생성
		RSpotLightSceneNode SpotLight;
		SpotLight.SetTransform(RVector3(17000.0f, 7500.0f, 600.0f), RVector3(0,0,-1), RVector3(0,1,0));
		SpotLight.SetSpotLightTexture("batman-logo.jpg");
		SpotLight.m_fApplyIntensity =2;
		SpotLight.UpdateTransform();
		RSceneNodeUpdateVisitor visitor;
		SpotLight.Traverse(&visitor);
		

		// 씬매니저에 붙이기
		REngine::GetSceneManager().AddSceneNode(&PointLight);
		REngine::GetSceneManager().AddSceneNode(&SpotLight);


		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("TestLight");

		// Light 없애기
		PointLight.RemoveFromParent();
		SpotLight.RemoveFromParent();
		
		// 환경 씬노드 소멸
		pEnvironmentSet->RemoveFromParent();
		delete pEnvironmentSet;
	}
}
