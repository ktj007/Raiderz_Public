#include "stdafx.h"

SUITE(ETC)
{
	TEST(TestMaterialDecal)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();

		// 인스턴스 생성
		RActor* pActor = static_cast< RActor* >( REngine::GetSceneManager().CreateSceneNode( rs3::RSID_ACTOR ) );
		CHECK(pActor);

		// 자원으로부터 로딩
		bool bRet = pActor->Create("hf_face_01.elu");
		CHECK(bRet);

		// 씬매니저에 붙이기
		REngine::GetSceneManager().AddSceneNode(pActor);

		RVector vPos(0, 0, 0);
		RVector vDir(0,1,0);

		// 액터 설정 - 애니메이션 / 좌표
		pActor->SetTransform( vPos, -vDir );

		//Material Decal 설정
		RTextureDesc desc;
		desc.TextureName = "scream.dds";
		desc.UVTransform.SetScale(RVector2(4,4));
		desc.UVTransform.SetOffset(RVector2(-0.3f,0.3f));
		desc.textureAddressU =RTADDRESS_WRAP;
		desc.textureAddressV =RTADDRESS_WRAP;

		pActor->GetMaterialDecal()->AddDecalLayer("at_face", &desc, NULL);


		RSceneNodeUpdateVisitor visitor;
		pActor->Traverse(&visitor);

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK(pCamera);
		pCamera->Set(vPos + vDir * 30 + RVector(0,0,160), -vDir);

		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("MaterialDecal");

		// 인스턴스 없애기
		pActor->RemoveFromParent();

		delete pActor;
	}

	TEST(TestMaterialDecalMaskMap)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();

		// 인스턴스 생성
		RActor* pActor = static_cast< RActor* >( REngine::GetSceneManager().CreateSceneNode( rs3::RSID_ACTOR ) );
		CHECK(pActor);

		// 자원으로부터 로딩
		bool bRet = pActor->Create("hf_face_01.elu");
		CHECK(bRet);

		// 씬매니저에 붙이기
		REngine::GetSceneManager().AddSceneNode(pActor);

		RVector vPos(0, 0, 0);
		RVector vDir(0,1,0);

		// 액터 설정 - 애니메이션 / 좌표
		pActor->SetTransform( vPos, -vDir );

		//Material Decal 설정
		RTextureDesc DiffuseDesc;
		DiffuseDesc.TextureName = "red256.jpg";

		RTextureDesc MaskDesc;
		MaskDesc.TextureName = "scream.dds";
		MaskDesc.UVTransform.SetScale(RVector2(4,4));
		MaskDesc.UVTransform.SetOffset(RVector2(-0.3f,0.3f));
		MaskDesc.textureAddressU =RTADDRESS_WRAP;
		MaskDesc.textureAddressV =RTADDRESS_WRAP;

		pActor->GetMaterialDecal()->AddDecalLayer("at_face", &DiffuseDesc, &MaskDesc);


		RSceneNodeUpdateVisitor visitor;
		pActor->Traverse(&visitor);

		// 카메라 설정
		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK(pCamera);
		pCamera->Set(vPos + vDir * 30 + RVector(0,0,160), -vDir);

		// 스크린샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("MaterialDecalMasking");

		// 인스턴스 없애기
		pActor->RemoveFromParent();

		delete pActor;
	}
}