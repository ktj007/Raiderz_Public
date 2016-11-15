#include "stdafx.h"

#include "REngine.h"
#include "RSceneManager.h"
#include "REnvironmentSet.h"
#include "RFont.h"
#include "RTextSceneNode.h"

SUITE(ETC)
{
	const char *INSTANCING_FILE_NAME = "Instancing.zone.xml";
	const char *INSTANCING_ENV_FILE_NAME = "Instancing.zone.env.xml";

	TEST(TestInstancing)
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

		pCamera->SetNearFarZ( 2000, 6000);
		pCamera->SetPosition( RVector3( 29395, 28137, 2488) + RVector(0.012f, 0.854f, -0.518f) * 150); 
		//pCamera->SetDirection( RVector(0.012f, 0.854f, -0.518f) );
		pCamera->SetDirection( RVector(0.012f, 0.854f, -0.518f) );
		pCamera->UpdateViewFrustrum();
		pCamera->UpdateTransform();

		
		// 프로파일링 결과를 얻기 위해 한번 랜더 후 텍스트 셋팅 하고 한번 더 랜더
		// 스크린 샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot(NULL);



		// 정보 텍스트
		RFont* pFont		= new RFont;
		pFont->Create( REngine::GetDevicePtr() ,L"굴림",12);

		RTextSceneNode* pTextNode = new RTextSceneNode;
		pTextNode->SetFont( pFont);
		pTextNode->SetText( "test");
		pTextNode->SetPosition( 0, 0);
		pTextNode->SetColor( 0xffff00ff);
		REngine::GetSceneManager().AddSceneNode( pTextNode);

		char szText[256];
		// 텍스트 셋팅
		sprintf_s(szText," Status of Instancing \n Draw Primitive of Tree: %d \n - leaf:%d frond:%d branch:%d billboard:%d \n Draw Primitive of Actor: %d \n - static:%d physique:%d\n\n", 
			// tree
			REngine::GetResourceProfiler().GetDrawCallCount( PPT_TREELEAF) + 
			REngine::GetResourceProfiler().GetDrawCallCount( PPT_TREEFROND) + 
			REngine::GetResourceProfiler().GetDrawCallCount( PPT_TREEBRANCH) + 
			REngine::GetResourceProfiler().GetDrawCallCount( PPT_TREEBILLBOARD),
			// detail
			REngine::GetResourceProfiler().GetDrawCallCount( PPT_TREELEAF),
			REngine::GetResourceProfiler().GetDrawCallCount( PPT_TREEFROND),
			REngine::GetResourceProfiler().GetDrawCallCount( PPT_TREEBRANCH),
			REngine::GetResourceProfiler().GetDrawCallCount( PPT_TREEBILLBOARD),
			// actor
			REngine::GetResourceProfiler().GetDrawCallCount( PPT_ACTOR) +
			REngine::GetResourceProfiler().GetDrawCallCount( PPT_ACTORPHYSIQUE),
			// detail
			REngine::GetResourceProfiler().GetDrawCallCount( PPT_ACTOR),
			REngine::GetResourceProfiler().GetDrawCallCount( PPT_ACTORPHYSIQUE)			
			);

		pTextNode->SetText(szText);


		// 스크린 샷
		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("TestInstancing");
		

		// 환경 씬노드 소멸
		pEnvironmentSet->RemoveFromParent();
		delete pEnvironmentSet;

		SAFE_DELETE( pFont);
		
		if( pTextNode)
			pTextNode->RemoveFromParent();
		SAFE_DELETE( pTextNode);
	}
}
