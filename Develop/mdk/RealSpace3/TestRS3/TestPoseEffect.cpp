#include "stdafx.h"

#include "REngine.h"
#include "RSceneManager.h"
#include "REnvironmentSet.h"
#include "RFont.h"
#include "RTextSceneNode.h"

SUITE(PostEffect)
{
	const char *INSTANCING_FILE_NAME = "BirdTown2.zone.xml";
	const char *INSTANCING_ENV_FILE_NAME = "BirdTown2.zone.env.xml";

	RActor* CreateActor( RVector3& _vPos, RVector3& _vDir, RVector3& _vColor )
	{
		RActor* pActor = static_cast< RActor* >( REngine::GetSceneManager().CreateSceneNode( rs3::RSID_ACTOR ) );
		if( NULL == pActor )
			return NULL;

		bool bRet = pActor->Create("goblin_mage.elu");

		// 씬매니저에 붙이기
		REngine::GetSceneManager().AddSceneNode(pActor);

		// 액터 설정 - 좌표
		pActor->SetTransform( _vPos, _vDir );

		RSceneNodeUpdateVisitor visitor;
		pActor->Traverse(&visitor);

		// 외곽선 켜기
		pActor->EnablePEEdge( _vColor );

		return pActor;
	}

	TEST(TestDOFnBulr)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();

		// 존 로딩
		bool bRet = REngine::GetSceneManager().LoadWorldSceneNodeFromRootXMLFile( REngine::GetSceneManager().GetCurrentRootSceneNode(true), INSTANCING_FILE_NAME);
		CHECK(bRet);

		// 환경 씬노드 생성
		REnvironmentSet* pEnvironmentSet = new REnvironmentSet;

		// 파일로부터 만들기
		bRet = pEnvironmentSet->CreateFromFile(INSTANCING_ENV_FILE_NAME, false);
		CHECK(bRet);

		// 씬매니저에 더하기
		REngine::GetSceneManager().AddSceneNode(pEnvironmentSet);

		// 환경 설정
		bRet = pEnvironmentSet->SetCurrentEnvSet(REnvironment::NIGHT, REnvironment::SUNNY, false, 0.f, false);
		CHECK(bRet);

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK( pCamera );

		pCamera->SetPosition( RVector3( 29395, 28137, 2488));			
		//pCamera->SetDirection( RVector(0.012f, 0.854f, -0.518f) );
		pCamera->SetDirection( RVector(0.012f, 0.854f, -0.518f) );
		pCamera->UpdateViewFrustrum();
		pCamera->UpdateTransform();


		// HDR은 SI 테스트에서 행해지므로 스킵 ---------------------------------------------

		// DOF -----------------------------------------------------------------------------
		// 카메라에 후처리 달기
		REngine::GetSceneManager().SetDOFEnable(true);
		REngine::GetSceneManager().SetDOFFocalDist(0);
		REngine::GetSceneManager().SetDOFNearBlurDist( 0);
		REngine::GetSceneManager().SetDOFFarBlurDist( 5500);
		REngine::GetSceneManager().SetDOFCOCSize( 7);
		// 스크린 샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("TestDOF");
		// 후처리 끄기
		REngine::GetSceneManager().SetDOFEnable(false);

		// SetScreenMotionBlur --------------------------------------------------------------
		// 카메라에 후처리 달기
		REngine::GetSceneManager().SetScreenMotionBlurEnable(true);
		REngine::GetSceneManager().SetScreenMotionBlurActive(true);
		REngine::GetSceneManager().SetScreenMotionBlurSamplerCount(10);
		REngine::GetSceneManager().SetScreenMotionBlurSpeed(9);
		REngine::GetSceneManager().SetScreenMotionBlurBasisByEpicenter();
		// 스크린 샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("TestScreenMotionBlur");
		// 후처리 끄기
		REngine::GetSceneManager().SetScreenMotionBlurEnable(false);


		// 환경 씬노드 소멸
		pEnvironmentSet->RemoveFromParent();
		delete pEnvironmentSet;
	}

	TEST(TestPostEffectEdge)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();
		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		// 외곽선 포스트 이펙트 활성화
		REngine::GetSceneManager().SetPEEdgeEnable( true );
		REngine::GetSceneManager().SetPEQuality( 2, 2 );
		REngine::GetSceneManager().SetPEFactorTickNess( 5.6f, 3.1f, 2.75f );

		RVector vPos(0, 0, 0);
		RVector vDir(0,1,0);

		RActor* pActor	= CreateActor(  RVector(-60.f, 0, 0), -vDir, RVector3(1, 0, 1) );
		RActor* pActor2 = CreateActor(  RVector(60.f, 0, 0), -vDir, RVector3(1, 1, 0) );

		CHECK(pActor);
		CHECK(pActor2);

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK(pCamera);
		pCamera->Set(vPos + vDir * 150 + RVector(0,0,70), -vDir);

		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("TestPostEffectEdge");

		pActor->DisablePEEdge();
		pActor2->DisablePEEdge();

		// 인스턴스 없애기
		pActor->RemoveFromParent();
		pActor2->RemoveFromParent();

		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();
		delete pActor;
		delete pActor2;
	}
}
