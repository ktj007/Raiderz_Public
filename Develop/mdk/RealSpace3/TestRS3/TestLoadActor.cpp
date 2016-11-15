#include "stdafx.h"

#include "REngine.h"
#include "RSceneManager.h"
#include "RActor.h"
#include "RMesh.h"
#include "RMeshMgr.h"
#include "RCameraSceneNode.h"
#include "REnvironmentSet.h"

#include "RActorController.h"
#include "RAnimationController.h"

SUITE(Actor)
{
	TEST(TestLoadActorSimple)
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
		pActor->SetTransform( vPos, -vDir );
		pActor->SetAnimation( "idle" );

		RSceneNodeUpdateVisitor visitor;
		pActor->Traverse(&visitor);

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK(pCamera);
		pCamera->Set(vPos + vDir * 150 + RVector(0,0,70), -vDir);

		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("LoadActorSimple");

		// 인스턴스 없애기
		pActor->RemoveFromParent();
		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();
		delete pActor;
	}

	TEST(TestLoadActorCustom)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();
		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		// 인스턴스 생성
		RActor* pActor = new RActor;
		CHECK(pActor);

		// 자원생성
		RMesh* pMesh = REngine::GetSceneManager().CreateResource<RMesh>("goblin_Mage.elu");
		bool bRet = pMesh->BuildAll();
		CHECK(bRet);

		// 자원으로부터 인스턴스 채우기
		bRet = pActor->Create(pMesh);
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

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK(pCamera);
		pCamera->Set(vPos + vDir * 150 + RVector(0,0,70), -vDir);

		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("LoadActorCustom");

		// 인스턴스 없애기, 인스턴스가 없어질때 자원은 같이 없어진다.
		pActor->RemoveFromParent();
		delete pActor;

		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();
	}

	TEST(TestActorAlpha2PassRedering)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();
		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		// 인스턴스 개수
		const int instanceNum = 3;
		const float offset = 25.0f;

		// 인스턴스 생성
		RActor* pActor[instanceNum];

		RVector vPos(0, 0, 0);
		RVector vDir(0, 1, 0);
		RSceneNodeUpdateVisitor visitor;

		for( int i = 0; i < instanceNum; ++i )
		{
			pActor[i] = static_cast< RActor* >( REngine::GetSceneManager().CreateSceneNode( rs3::RSID_ACTOR ) );
			CHECK(pActor[i]);

			// 자원으로부터 로딩
			bool bRet = pActor[i]->Create("goblin_Mage.elu");
			CHECK(bRet);

			// 알파 2패스 렌더링 설정.
			if( i & 1 )
			{
			}
			else
			{
				pActor[i]->SetAlpha2PassRendering(true);
				pActor[i]->SetVisibility( 0.5f );
			}

			// 씬매니저에 붙이기
			REngine::GetSceneManager().AddSceneNode(pActor[i]);

			// 액터 설정 - 애니메이션 / 좌표
			pActor[i]->SetAnimation( "idle" );
			pActor[i]->SetTransform( vPos + RVector( i * offset, i * -offset, 0.f ), -vDir );
			pActor[i]->Traverse(&visitor);
			//pActor[i]->Update();
		}

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK(pCamera);
		pCamera->Set(vPos + vDir * 150 + RVector(offset,0,70), -vDir);

		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("Alpha2PassRendering");

		// 인스턴스 없애기
		for( int i = 0; i < instanceNum; ++i )
		{
			pActor[i]->RemoveFromParent();
			delete pActor[i];
		}
		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();
	}

	TEST(TestLoadActorGhostFX)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();
		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		// 인스턴스 생성
		RActor* pActor = new RActor;
		CHECK(pActor);

		// 자원생성
		RMesh* pMesh = REngine::GetSceneManager().CreateResource<RMesh>("ghost_knight.elu");
		bool bRet = pMesh->BuildAll();
		CHECK(bRet);

		// 자원으로부터 인스턴스 채우기
		bRet = pActor->Create(pMesh);
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

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK(pCamera);
		pCamera->Set(vPos + vDir * 200 + RVector(0,0,100), -vDir);

		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("LoadActorGhostFX");

		// 인스턴스 없애기, 인스턴스가 없어질때 자원은 같이 없어진다.
		pActor->RemoveFromParent();
		delete pActor;
		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();
	}

	TEST(TestAlign)
	{
		// 규칙중에 Align 및 AlignZ 가 잘되는지 테스트.
		// 스크린샷의 등불에 달린 불빛 빌보드, 왼쪽은 Align 오른쪽은 AlignZ 이다.

		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();
		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		// 인스턴스 생성
		RActor* pActor = new RActor;
		CHECK(pActor);

		// 자원생성
		RMesh* pMesh = REngine::GetSceneManager().CreateResource<RMesh>("test_align.elu");
		bool bRet = pMesh->BuildAll();
		CHECK(bRet);

		// 자원으로부터 인스턴스 채우기
		bRet = pActor->Create(pMesh);
		CHECK(bRet);

		// 씬매니저에 붙이기
		REngine::GetSceneManager().AddSceneNode(pActor);

		RVector vPos(0, 0, 0);
		RVector vDir(0,1,1);

		// 애니메이션이 될때 테스트도 필요하면 추가하자
// 		// 액터 설정 - 애니메이션 / 좌표
// 		pActor->SetAnimation( "idle" );
// 		pActor->SetTransform( vPos, -vDir );

		RSceneNodeUpdateVisitor v;
		pActor->Traverse( &v );

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK(pCamera);
		pCamera->Set(vPos + vDir * 100 + RVector(0,0,100), -vDir);

		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("ActorAlign");

		// 인스턴스 없애기, 인스턴스가 없어질때 자원은 같이 없어진다.
		pActor->RemoveFromParent();
		delete pActor;
		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();

	}

	TEST(TestActorAdditive)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();
		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		// 인스턴스 생성
		RActor* pActor = static_cast< RActor* >( REngine::GetSceneManager().CreateSceneNode( rs3::RSID_ACTOR ) );
		CHECK(pActor);

		// 자원으로부터 로딩
		bool bRet = pActor->Create("ritz_sign_adventurer_01.elu");
		CHECK(bRet);

		// 씬매니저에 붙이기
		REngine::GetSceneManager().AddSceneNode(pActor);

		RVector vPos(0, 0, 0);
		RVector vDir(0,1,0);
		RVector vDir2(1,0,0);

		// 액터 설정 - 애니메이션 / 좌표
		pActor->SetTransform( vPos, -vDir2 );
		pActor->SetAnimation( "idle" );

		RSceneNodeUpdateVisitor visitor;
		pActor->Traverse(&visitor);

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK(pCamera);
		pCamera->Set(vPos + vDir * 700 + RVector(0,0,300), -vDir);

		REngine::GetSceneManager().SetHDREnable(true);
		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("ActorAdditive");

		// 인스턴스 없애기
		pActor->RemoveFromParent();
		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();
		delete pActor;
	}

	TEST(TestActorSelfIllumination)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();
		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		// 인스턴스 생성
		RActor* pActor = static_cast< RActor* >( REngine::GetSceneManager().CreateSceneNode( rs3::RSID_ACTOR ) );
		CHECK(pActor);

		// 자원으로부터 로딩
		bool bRet = pActor->Create("cerberus.elu");
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
		pCamera->Set(vPos + vDir * 700 + RVector(0,0,300), -vDir);

		REngine::GetSceneManager().SetHDREnable(true);
		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("ActorSelfIllumination");

		// 인스턴스 없애기
		pActor->RemoveFromParent();
		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();
		delete pActor;
	}

	TEST(TestMultiVertexAnimation)	// 같은 메쉬를 사용하는 다른 프레임의 버텍스 애니메이션 테스트
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();
		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		RVector vPos(0, 0, 0);
		RVector vDir(0,1,0);

		RActor* pActor[2];
		for( int i = 0; i < 2; ++ i )
		{
			// 인스턴스 생성
			pActor[i] = static_cast< RActor* >( REngine::GetSceneManager().CreateSceneNode( rs3::RSID_ACTOR ) );
			CHECK(pActor[i]);
		
			// 자원으로부터 로딩
			bool bRet = pActor[i]->Create("goblin_bomb.elu");
			CHECK(bRet);

			// 씬매니저에 붙이기
			REngine::GetSceneManager().AddSceneNode(pActor[i]);

			pActor[i]->SetAnimation( "bomb" );
		}

		pActor[0]->SetTransform(vPos + RVector( -100.0f, 0.0f, 0.0f), vDir);
		pActor[1]->SetTransform(vPos + RVector(  100.0f, 0.0f, 0.0f), vDir);

		pActor[0]->GetAnimationController().SetFrame(0);
		pActor[1]->GetAnimationController().SetFrame(1000);

		RSceneNodeUpdateVisitor visitor;
		pActor[0]->Traverse(&visitor);
		pActor[1]->Traverse(&visitor);

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK(pCamera);
		pCamera->Set(vPos + vDir * 500 + RVector(0,0,0), -vDir);

		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("TestMultiVertexAnimation");

		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();

		for( int i = 0; i < 2; ++i )
		{
			pActor[i]->RemoveFromParent();
			delete pActor[i];
		}
	}

	TEST(TestActorDiffusemapBlend)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();

		RActor* pActor[3];
		// 인스턴스 생성

		for(int i=0;i<3;i++)
		{
			pActor[i] = static_cast< RActor* >( REngine::GetSceneManager().CreateSceneNode( rs3::RSID_ACTOR ) );
			CHECK( pActor[i]);

			// 자원으로부터 로딩
			bool bRet = pActor[i]->Create("sphere_tiling.elu");
			CHECK(bRet);

			// 씬매니저에 붙이기
			REngine::GetSceneManager().AddSceneNode(pActor[i]);

			RVector vPos(0, 0, 0);
			RVector vinstancePos[3] = { RVector( -180, 0, 0), RVector( 0, 0, 0), RVector( 180, 0, 0)};
			RVector vDir(0,1,0);

			// 액터 설정 - 애니메이션 / 좌표
			pActor[i]->SetTransform( vinstancePos[i], -vDir );
			pActor[i]->SetAnimation( "idle" );

			RSceneNodeUpdateVisitor visitor;
			pActor[i]->Traverse(&visitor);

			// 카메라 설정
			RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
			CHECK(pCamera);
			pCamera->Set(vPos + vDir * 400 + RVector(0,0,0), -vDir);

			switch( i)
			{
			case 0:
				break;
			case 1:
				pActor[i]->ChangeMatetial( 0, 0, 1, 1.0f);
				break;
			case 2:
				pActor[i]->ChangeMatetial( 0, 0, 1, 0);
				break;
			}
		}

		for(int j=0;j<50;j++)
		{
			RUnitTestRunner::GetInstance().UpdateScene(true);
			RUnitTestRunner::GetInstance().GrabScreenShot(NULL);
		}

		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("ActorDiffusemapBlend");

		for(int i=0;i<3;i++)
		{
			// 인스턴스 없애기
			pActor[i]->RemoveFromParent();
			delete pActor[i];
		}
	}
}
