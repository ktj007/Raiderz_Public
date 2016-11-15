#include "stdafx.h"

#include "REngine.h"
#include "RSceneManager.h"
#include "RCameraSceneNode.h"

#include "RActor.h"
#include "REffectInstance.h"

SUITE(AttachEffect)
{
	TEST(TestAttachEffectToSceneNode)
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

		// 액터 설정 - 애니메이션 / 좌표
		pActor->SetAnimation( "idle" );
		pActor->SetTransform( vPos, -vDir );
		RSceneNodeUpdateVisitor visitor;
		pActor->Traverse(&visitor);
		//pActor->Update();

		// 이팩트
		REffectInstanceSceneNode effectInstance;
		bRet = effectInstance.Create("ef_head_icon_q02");
		CHECK(bRet);

		REffectSceneNode* pEffect = effectInstance.GetEffectSceneNode();
		pEffect->RemoveFromParent();

		// 이팩트 오프셋
		RMatrix matTransOffset;
		matTransOffset.MakeIdentity();
		matTransOffset.SetTranslation( RVector(0,0,120) );

		// 씬매니저를 통해 액터에 이팩트 붙이기
		REngine::GetSceneManager().AttachEffect(pEffect, pActor, NULL, matTransOffset);

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK(pCamera);
		pCamera->Set(vPos + vDir * 150 + RVector(0,0,70), -vDir);

		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("AttachEffectToSceneNode");

		// 인스턴스 없애기, 이팩트부터
		pEffect->RemoveFromParent();
		delete pEffect;
		pActor->RemoveFromParent();
		delete pActor;
		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();
	}

	TEST(TestAttachLoopEffectToSceneNode)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();
		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		// 인스턴스 생성
		RActor* pActor = static_cast< RActor* >( REngine::GetSceneManager().CreateSceneNode( rs3::RSID_ACTOR ) );
		CHECK(pActor);

		// 자원으로부터 로딩
		bool bRet = pActor->Create("goblin_Mage.elu");
		CHECK(bRet);

		// 씬매니저에 붙이기
		REngine::GetSceneManager().AddSceneNode(pActor);

		RVector vPos(0, 0, 0);
		RVector vDir(0,1,0);

		// 액터 설정 - 애니메이션 / 좌표
		pActor->SetAnimation( "idle" );
		pActor->SetTransform( vPos, -vDir );
		RSceneNodeUpdateVisitor visitor;
		pActor->Traverse(&visitor);
		//pActor->Update();

		// 이팩트
		REffectInstanceSceneNode effectInstance;
		bRet = effectInstance.Create("ef_head_icon_l00");
		CHECK(bRet);

		REffectSceneNode* pEffect = effectInstance.GetEffectSceneNode();
		pEffect->RemoveFromParent();

		// 이팩트 오프셋
		RMatrix matTransOffset;
		matTransOffset.MakeIdentity();
		matTransOffset.SetTranslation( RVector(0,0,120) );

		// 씬매니저를 통해 액터에 이팩트 붙이기
		REngine::GetSceneManager().AttachEffect(pEffect, pActor, NULL, matTransOffset);

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK(pCamera);
		pCamera->Set(vPos + vDir * 150 + RVector(0,0,70), -vDir);

		// loop 가 잘 되는지 보기위해 시간을 3초 정도 흘려보냄, 그래도 나오고 있으면 loop 동작중임
		for(int i=0;i<300;i++)
		{
			if(pEffect)
				REffectSceneNode::ControlEffectSceneNodeLife(&pEffect);	// app 와 맞춰줌
			RUnitTestRunner::GetInstance().UpdateScene(true);
			RUnitTestRunner::GetInstance().GrabScreenShot(NULL);
		}

		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("AttachLoopEffectToSceneNode");

		if(pEffect)
		{
			// 인스턴스 없애기, 이팩트부터
			pEffect->RemoveFromParent();
			delete pEffect;
		}

		pActor->RemoveFromParent();
		delete pActor;
		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();
	}

	TEST(TestAttachEffectToActorNode)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();
		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();
		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();
	}
}
