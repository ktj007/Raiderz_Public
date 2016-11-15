#include "stdafx.h"

#include "REngine.h"
#include "RSceneManager.h"
#include "RCameraSceneNode.h"
#include "RWaterSceneNode.h"
#include "RWaterPassRenderer.h"

SUITE(Water)
{
	const char *ZONE_FILE_NAME = "Login.zone.xml";

	bool GetAllWaterPlane(RSceneNode *pSceneNode)
	{
		if ( _stricmp(pSceneNode->GetNodeIDString(), "WATERPLANE") == 0 )
			return true;
		else
			return false;
	}

	TEST(TestWaterReflection)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();

		// 존 로딩
		bool bRet = REngine::GetSceneManager().LoadWorldSceneNodeFromRootXMLFile( REngine::GetSceneManager().GetCurrentRootSceneNode(true),ZONE_FILE_NAME);
		CHECK(bRet);

		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		// bump adjust factor = 0
		// reflect adjust = 1
		vector<RSceneNode*> vWaterArray;
		REngine::GetSceneManager().GetCurrentRootSceneNode(false)->SerializeToVector(GetAllWaterPlane, vWaterArray);

		for( size_t i = 0; i < vWaterArray.size(); ++i )
		{
			RWaterPlane *pPlane = (RWaterPlane*)vWaterArray[i];
			pPlane->GetWaterPlaneProperty()->m_fBumpAdjustFactor = 0.0f;
			pPlane->GetWaterPlaneProperty()->m_fReflectAdjust = 1.0f;
		}

		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK( pCamera );
		pCamera->SetPosition(RVector3(11652.0f, 11185.0f, -44.22f)); // 걍 임의의 포지션
		pCamera->SetDirection(RVector3(0.9829f, -0.1610f, -0.088f));

		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("TestWaterReflection");
		// 로딩된 존은 RSceneManager Clear시에 알아서 지워진다. 따로 지워줄필요 없음

		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();
	}

}
