#include "stdafx.h"

#include "REngine.h"
#include "RSceneManager.h"
#include "RActor.h"
#include "RCameraSceneNode.h"

SUITE(ETC)
{
	TEST(TestEdge)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();
		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		// 인스턴스 생성
		RActor* pActor = static_cast< RActor* >( REngine::GetSceneManager().CreateSceneNode( rs3::RSID_ACTOR ) );
		CHECK(pActor);

		bool bRet = pActor->Create("goblin_Mage.elu");
		CHECK(bRet);

		// 씬매니저에 붙이기
		REngine::GetSceneManager().AddSceneNode(pActor);

		RVector vPos(0, 0, 0);
		RVector vDir(0,1,0);

		// 액터 설정 - 좌표
		pActor->SetTransform( vPos, -vDir );

		RSceneNodeUpdateVisitor visitor;
		pActor->Traverse(&visitor);

		pActor->EnableEdge( RVector( 1.f, 0.f, 1.f ), 5.0f );

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK(pCamera);
		pCamera->Set(vPos + vDir * 150 + RVector(0,0,70), -vDir);

		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("TestEdge");

		// 인스턴스 없애기
		pActor->RemoveFromParent();
		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();
		delete pActor;
	}
}
