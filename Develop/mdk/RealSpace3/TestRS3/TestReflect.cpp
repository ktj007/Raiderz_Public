#include "stdafx.h"
#include "RSceneManager.h"
#include "RActor.h"
#include "RCameraSceneNode.h"
#include "RShadowStrategy.h"

SUITE(ETC)
{
	RActor* CreateActor( RVector3& _vPos, RVector3& _vDir )
	{
		// 인스턴스 생김
		RActor* pActor = static_cast< RActor* >( REngine::GetSceneManager().CreateSceneNode( rs3::RSID_ACTOR ) );
		if( NULL == pActor )
			return NULL;

		// 자원으로부터 로딩
		bool bRet = pActor->Create("hf_plate_armor.elu");

		// 씬매니저에 붙이기
		REngine::GetSceneManager().AddSceneNode(pActor);

		// 액터 설정 - 좌표
		pActor->SetTransform( _vPos, _vDir );
		RSceneNodeUpdateVisitor visitor;
		pActor->Traverse(&visitor);
		return pActor;
	}

	TEST(TestReflect)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();
		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		RVector vPos(7, 0, 69);
		RVector vDir(0,-1,0);

		RActor* pActor	= CreateActor( RVector(0.f, 0.f, 0.f), vDir );

		CHECK(pActor);

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK(pCamera);
		pCamera->Set(vPos + vDir * 25 + RVector(0,0,70), -vDir);

		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("Reflect");
	
		// 인스턴스 없애기
		pActor->RemoveFromParent();
		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();
		delete pActor;
	}
}