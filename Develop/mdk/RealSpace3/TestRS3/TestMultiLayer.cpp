#include "stdafx.h"

#include "RSceneManager.h"
#include "RActor.h"
// #include "RMesh.h"
// #include "RMeshMgr.h"
#include "RCameraSceneNode.h"
//#include "REnvironmentSet.h"

//#include "RActorController.h"
//#include "RAnimationController.h"
#include "RShadowStrategy.h"

SUITE(ETC)
{
	TEST(TestMultiLayer)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();
// 		// 라이트 붙임
// 		RUnitTestRunner::GetInstance().AddDirectionalLight();

		// 인스턴스 생성
		RActor* pActor = static_cast< RActor* >( REngine::GetSceneManager().CreateSceneNode( rs3::RSID_ACTOR ) );
		CHECK(pActor);

		bool bRet = pActor->Create("N_bos_003.elu");
		CHECK(bRet);

		// 씬매니저에 붙이기
		REngine::GetSceneManager().AddSceneNode(pActor);

		RVector vPos(0, 0, 0);
		RVector vDir(0,1,0);

		// 액터 설정 - 애니메이션 / 좌표
		pActor->SetTransform( vPos, -vDir );
		pActor->SetAnimation( "idle" );

		RSceneNodeUpdateVisitor visitor;
		pActor->Traverse(&visitor);

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK(pCamera);
		pCamera->Set(vPos + RVector(-1000,1300,1000), RVector3(1, -1, -1));

		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("MultiLayer");

		// 인스턴스 없애기
		pActor->RemoveFromParent();
// 		// 라이트 없애기
// 		RUnitTestRunner::GetInstance().DelDirectionalLight();
		delete pActor;
	}
}