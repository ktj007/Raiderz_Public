#include "stdafx.h"

#include "REngine.h"
#include "RSceneManager.h"
#include "RCameraSceneNode.h"
#include "REffectInstance.h"
#include "REnvironmentSet.h"
#include "RShaderCompositeManager.h"

SUITE(Effect)
{
	/* 스크린샷이 빌드 때마다 결과가 달라져 임시로 뺌,2010.06.25 by ignore
	TEST(TestEffect)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();
		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		RVector vPos(0, 0, 0);
		RVector vDir(1,1,0);

		// 이팩트
		REffectInstanceSceneNode effectInstance;
		bool bRet = effectInstance.Create("ef_slash_dmg1");
		CHECK(bRet);

		if(!bRet) return;

		REffectSceneNode* pEffect = effectInstance.GetEffectSceneNode();
		pEffect->RemoveFromParent();

		// 신에 더해준다
		REngine::GetSceneManager().AttachEffect( pEffect, NULL, NULL );

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK(pCamera);
		pCamera->Set(vPos + vDir * 300 , -vDir);

		// 100 ms 정도 흘려보냄
		for(int i=0;i<10;i++)
		{
			RUnitTestRunner::GetInstance().UpdateScene(true);
 			RUnitTestRunner::GetInstance().GrabScreenShot(NULL);
		}

		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("Effect");

		// 인스턴스 없애기, 이팩트부터
		pEffect->RemoveFromParent();
		delete pEffect;
		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();


	}
	*/

	TEST(TestTrailEffect)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();
		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		RVector vPos(0, 0, 0);
		RVector vDir(0,1,0);

		// 이팩트
		REffectInstanceSceneNode effectInstance;
		bool bRet = effectInstance.Create("ef_dt");
		CHECK(bRet);

		REffectSceneNode* pEffect = effectInstance.GetEffectSceneNode();
		pEffect->RemoveFromParent();

		// 신에 더해준다
		REngine::GetSceneManager().AttachEffect( pEffect, NULL, NULL );

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK(pCamera);
		pCamera->Set(vPos + vDir * 400 , -vDir);

		// 250 ms 정도 흘려보냄
		for(int i=0;i<25;i++)
		{
			RUnitTestRunner::GetInstance().UpdateScene(true);
			RUnitTestRunner::GetInstance().GrabScreenShot(NULL);
		}
		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("EffectTrail");

		// 인스턴스 없애기, 이팩트부터
		pEffect->RemoveFromParent();
		delete pEffect;
		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();
	}

	TEST(TestEffectDistortion)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();
		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		RVector vPos(0, 0, 30);
		RVector vDir(0,1,0);

		// 이팩트
		REffectInstanceSceneNode effectInstance;
		bool bRet = effectInstance.Create("ef_breath_stream_02");
		CHECK(bRet);

		REffectSceneNode* pEffect = effectInstance.GetEffectSceneNode();
		pEffect->RemoveFromParent();

		// 신에 더해준다
		REngine::GetSceneManager().AttachEffect( pEffect, NULL, NULL );

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK(pCamera);
		pCamera->Set(vPos + vDir * 150 , -vDir);

		//Distortion 활성화
		REngine::GetSceneManager().SetDistortionEnable(true);

		// 800 ms 정도 흘려보냄
		for(int i=0;i<80;i++)
		{
			RUnitTestRunner::GetInstance().UpdateScene(true);
			RUnitTestRunner::GetInstance().GrabScreenShot(NULL);
		}
		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("EffectDistortion");

		// 인스턴스 없애기, 이팩트부터
		pEffect->RemoveFromParent();
		delete pEffect;
		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();
	}

	const char *INSTANCING_FILE_NAME = "Login2.zone.xml";
	const char *INSTANCING_ENV_FILE_NAME = "Login2.zone.env.xml";

// 소프트 파티클 테스트가 돌리 시 마다 결과가 다르다. 우선 테스트 뺌
// 	TEST(TestSoftParticle)
// 	{
// 		// 엔진 리셋
// 		RUnitTestRunner::GetInstance().ResetEngine();
// 
// 		// 존 로딩
// 		bool bRet = REngine::GetSceneManager().LoadRootSceneFromXML(INSTANCING_FILE_NAME);
// 		CHECK(bRet);
// 
// 		// 카메라 설정
// 		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
// 		CHECK( pCamera );
// 		pCamera->SetPosition(RVector3( 21742, 18343, 1953)); 
// 		pCamera->SetDirection( RVector( -0.021f, -0.485f, -0.873f) );
// 		pCamera->UpdateTransform();
// 
// 		// 시간 흘려보냄
// 		for(int i=0;i<200;i++)
// 		{
// 			RUnitTestRunner::GetInstance().UpdateScene(true);
// 			RUnitTestRunner::GetInstance().GrabScreenShot(NULL);
// 		}
// 
// 		// 스크린 샷
// 		RUnitTestRunner::GetInstance().UpdateScene(true);
// 		RUnitTestRunner::GetInstance().GrabScreenShot("TestSoftParticle");
// 	}
}
