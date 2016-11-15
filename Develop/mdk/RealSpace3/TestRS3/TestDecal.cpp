#include "stdafx.h"

#include "REngine.h"
#include "RSceneManager.h"
#include "RSceneNodeID.h"
#include "RDecalProjectionSceneNode.h"
#include "RDecalPolygonSceneNode.h"

SUITE(DECAL)
{
	TEST(TestDecal)
	{
		// 엔진 리셋
		RUnitTestRunner::GetInstance().ResetEngine();

		// 존 로딩
		bool bRet = REngine::GetSceneManager().LoadWorldSceneNodeFromRootXMLFile( REngine::GetSceneManager().GetCurrentRootSceneNode(true),"BirdTown.zone.xml");
		CHECK(bRet);

		// 라이트 붙임
		RUnitTestRunner::GetInstance().AddDirectionalLight();

		RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
		CHECK( pCamera );
		pCamera->Set(RVector3(10757,36992,1444), RVector(0.232f,0.78f,-0.57f));
		pCamera->UpdateTransform();

		RVector3 vPos = RVector3(10333,39310,0);
		// Projection
		RDecalProjectionSceneNode* pDecalProjectionSceneNode = static_cast<RDecalProjectionSceneNode*>(REngine::GetSceneManager().CreateSceneNode(RSID_DECAL_PROJECTION));
		pDecalProjectionSceneNode->Create( NULL, "magic_circle_1.DDS", vPos, 500, 500, 500, 0 );
		REngine::GetSceneManager().AddSceneNode(pDecalProjectionSceneNode);
		
		// PolygonDecal
		RDecalPolygonSceneNode* pDecalPolygonSceneNode = static_cast<RDecalPolygonSceneNode*>(REngine::GetSceneManager().CreateSceneNode(RSID_DECAL_POLYGON));
		REngine::GetSceneManager().AddSceneNode(pDecalPolygonSceneNode);

		vPos = RVector3(11333,39310,0);
		pDecalPolygonSceneNode->Create( NULL );	// 처음 한번만 설정해 주자
		pDecalPolygonSceneNode->Add( "magic_circle_1.DDS", vPos, 500, 500, 500, 0);

		RUnitTestRunner::GetInstance().UpdateScene(true);
		RUnitTestRunner::GetInstance().GrabScreenShot("TestDecal");
		// 로딩된 존은 RSceneManager Clear시에 알아서 지워진다. 따로 지워줄필요 없음

		// 라이트 없애기
		RUnitTestRunner::GetInstance().DelDirectionalLight();

		pDecalProjectionSceneNode->RemoveFromParent();
		pDecalPolygonSceneNode->RemoveFromParent();
		SAFE_DELETE(pDecalProjectionSceneNode);
		SAFE_DELETE(pDecalPolygonSceneNode);
	}
}