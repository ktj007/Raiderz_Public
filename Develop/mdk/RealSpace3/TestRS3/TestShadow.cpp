#include "stdafx.h"

#include "REngine.h"
#include "RSceneManager.h"
#include "REnvironmentSet.h"
#include "RActor.h"
#include "RMesh.h"
#include "RMeshMgr.h"
#include "RShadowStrategy.h"

SUITE(Shadow)
{
	const char *ZONE_FILE_NAME = "Login.zone.xml";
	const char *ZONE_ENV_FILE_NAME = "Login.zone.env.xml";

	TEST(TestShadow)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();

		RShadowDesc desc;
		REngine::GetSceneManager().SetRealtimeShadowMethod(desc);

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
		pCamera->SetPosition(RVector3(15370.0f, 6735.0f, 218.0f)); // 걍 임의의 포지션
		pCamera->SetDirection( RVector(-0.61f, 0.73f, -0.3f) );

		// 액터 인스턴스 생성
		RActor* pActor = static_cast< RActor* >( REngine::GetSceneManager().CreateSceneNode( rs3::RSID_ACTOR ) );
		CHECK(pActor);

		bRet = pActor->Create("goblin_Mage.elu");
		CHECK(bRet);

		// 씬매니저에 붙이기
		REngine::GetSceneManager().AddSceneNode(pActor);

		RVector vPos(15150.0f, 6920.0f, 0.0f);
		RVector vDir(0,1,0);

		// 액터 설정 - 애니메이션 / 좌표
		pActor->SetAnimation( "idle" );
		pActor->SetTransform( vPos, -vDir );
		RSceneNodeUpdateVisitor visitor;
		pActor->Traverse(&visitor);
		//pActor->Update();

		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("TestShadow");

		// 액터 인스턴스 없애기
		pActor->RemoveFromParent();
		delete pActor;

		// 환경 씬노드 소멸
		pEnvironmentSet->RemoveFromParent();
		delete pEnvironmentSet;

		desc.eMethod = SHADOW_NONE;
		REngine::GetSceneManager().SetRealtimeShadowMethod(desc);
	}

	TEST(TestCloudShadow)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();

		RShadowDesc desc;
		REngine::GetSceneManager().SetRealtimeShadowMethod(desc);

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
		bRet = pEnvironmentSet->SetCurrentEnvSet(REnvironment::DAYTIME, REnvironment::CLOUD, false, 0.f, false);
		CHECK(bRet);
		REngine::GetSceneManager().UseLighting(true);

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK( pCamera );
		pCamera->SetPosition(RVector3(15010.0f, 8510.0f, 410.0f)); // 걍 임의의 포지션

		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("TestCloudShadow");

		// 환경 씬노드 소멸
		pEnvironmentSet->RemoveFromParent();
		delete pEnvironmentSet;

		desc.eMethod = SHADOW_NONE;
		REngine::GetSceneManager().SetRealtimeShadowMethod(desc);
	}

	TEST(TestCloudShadowBlend)
	{
		RUnitTestRunner::GetInstance().ResetEngine();
		RSceneManager *pSceneManager = REngine::GetSceneManagerPtr();
		_ASSERT(pSceneManager);

		RShadowDesc desc;
		REngine::GetSceneManager().SetRealtimeShadowMethod(desc);

		// 존 로딩
		bool bRet = REngine::GetSceneManager().LoadWorldSceneNodeFromRootXMLFile( REngine::GetSceneManager().GetCurrentRootSceneNode(true),ZONE_FILE_NAME);
		CHECK(bRet);

		// 환경 씬노드 생성
		REnvironmentSet *pEnvironmentSet = new REnvironmentSet;
		REnvironment *pEnvironment = pEnvironmentSet->GetCurrentEnvironment();
		pEnvironmentSet->hTillingTexture = REngine::GetDevicePtr()->CreateTexture("cloudshadow.tga");
		CHECK(pEnvironmentSet->hTillingTexture!=R_NONE);
		
		REnvironment::SEnvSet *pEnv1 = pEnvironment->CreateEnv(REnvironment::DAWN, REnvironment::SUNNY);
		pEnv1->m_bCloudVisible = true;
		pEnv1->m_vCloudShadowScale = RVector(1000.f, 100.f, 1.0f);
		pEnv1->m_vCloudShadowTranslate = RVector(1, 1, 1);
		
		REnvironment::SEnvSet *pEnv2 = pEnvironment->CreateEnv(REnvironment::DAWN, REnvironment::CLOUD);
		pEnv2->m_bCloudVisible = true;
		pEnv2->m_vCloudShadowScale = RVector(500.f, 500.f, 1.0f);
		pEnv2->m_vCloudShadowTranslate = RVector(1, 1, 1);

		// 씬매니저에 더하기
		REngine::GetSceneManager().AddSceneNode(pEnvironmentSet);

		bRet = pEnvironmentSet->SetCurrentEnvSet(REnvironment::DAWN, REnvironment::SUNNY, false, 0.f, false);
		CHECK(bRet);
		pEnvironmentSet->CommitEnvSet();

		bRet = pEnvironmentSet->SetCurrentEnvSet(REnvironment::DAWN, REnvironment::CLOUD, true, 1.f, false);	// 블렌딩하는 시간을 1초로 설정 -
		CHECK(bRet);

		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK( pCamera );
		pCamera->SetPosition(RVector3(15010.0f, 8510.0f, 410.0f)); // 걍 임의의 포지션

		for ( int i = 0; i<45; ++i )	// 45 * 10 = 450 (ms)
		{
			RUnitTestRunner::GetInstance().UpdateScene(true);	// UpdateScene 할 때 마다 10ms 씩 타이머가 갱신된다.
			RUnitTestRunner::GetInstance().GrabScreenShot(NULL);
		}

		REngine::GetSceneManager().UseLighting(true);

		
		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("TestCloudShadowBlend");

		pEnvironmentSet->RemoveFromParent();
		delete pEnvironmentSet;
		pEnvironmentSet = NULL;

		desc.eMethod = SHADOW_NONE;
		REngine::GetSceneManager().SetRealtimeShadowMethod(desc);
	}
}
