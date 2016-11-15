#include "stdafx.h"

#include "REngine.h"
#include "RSceneManager.h"
#include "RSceneNodeID.h"
#include "REnvironmentSet.h"

SUITE(ETC)
{
	TEST(TestFog)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();

		// 존 로딩
		bool bRet = REngine::GetSceneManager().LoadWorldSceneNodeFromRootXMLFile( REngine::GetSceneManager().GetCurrentRootSceneNode(true),"Fog.scene.xml");
		CHECK(bRet);

		// 환경 씬노드 생성
		REnvironmentSet* pEnvironmentSet = new REnvironmentSet;
		pEnvironmentSet->CreateFromFile("Fog.zone.env.xml", false);

		// 씬매니저에 더하기
		REngine::GetSceneManager().AddSceneNode(pEnvironmentSet);

		// 환경 설정
		bRet = pEnvironmentSet->SetCurrentEnvSet(REnvironment::NIGHT, REnvironment::SUNNY, false, 0.f, false);
		CHECK(bRet);

		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		//카메라 세팅
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK( pCamera );
		//pCamera->Set(RVector3(8896,35204,1689), RVector(-0.03f,0.953f,-0.3f));
		pCamera->Set(RVector3(3587,2629,1591), RVector(-0.015f,0.953f,-0.3f));
		pCamera->UpdateTransform();

		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("TestFog");

		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();

		// 환경 씬노드 소멸
		pEnvironmentSet->RemoveFromParent();
		delete pEnvironmentSet;
	}
}