#include "stdafx.h"

#include "REngine.h"
#include "RSceneManager.h"
#include "RActor.h"
#include "RActorPassRenderer.h"

SUITE(ETC)
{
	const char *ZONE_FILE_NAME2 = "testAlphaSoring.scene.xml";

	TEST(TestAlphaSorting)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();

		// 존 로딩
		bool bRet = REngine::GetSceneManager().LoadWorldSceneNodeFromRootXMLFile( REngine::GetSceneManager().GetCurrentRootSceneNode(true),ZONE_FILE_NAME2);
		CHECK(bRet);

		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK( pCamera );
		pCamera->Set(RVector3(3447,3133,1310), RVector(0.59f,0.69f,-0.40f));

		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("TestAlphaSorting");
		// 로딩된 존은 RSceneManager Clear시에 알아서 지워진다. 따로 지워줄필요 없음

		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();
	}
}
