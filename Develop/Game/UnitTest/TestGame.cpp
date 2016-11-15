#include "stdafx.h"
#include "TestGame.h"
#include "XGame.h"
#include "MockController.h"
#include "XCameraManager.h"
#include "XObjectManager.h"
#include "XMyPlayer.h"
#include "XNetPlayer.h"
#include "XNonPlayer.h"
#include "MSheet.h"
#include "MMainFrame.h"
#include "XUnitTestHelper.h"
#include "XModuleEntity.h"
#include "XSystem.h"
#include "XWidgetNameDef.h"
#include "XUnitTestUtil.h"

TestGame::TestGame()
{
	USES_CONVERSION_EX;

	InitGlobalVar();

	m_pFocusWidget = global.mint->NewWidget(MINT_SHEET, W2A_EX(UI_GAME_SHEET_NAME, 0), global.mint->GetMainFrame(), NULL);
	m_pFocusWidget->SetRect(1, 1, 500, 400);
	m_pFocusWidget->Show(true);

	AddPlayerColCapsule();

	m_pGame = new XGame(false);
	m_pController = new MockController();

	m_pGame->Create();
	m_pController->Create();

	global.ui->ShowMouse(false);

	XUnitTestHelper helper;
	helper.SetAniDataSet_Player(&m_PlayerAniDataSet);

	XMyPlayer* pMyPlayer = GetMyPlayer();
	if (pMyPlayer)
	{
		MockAnimationController* ac = dynamic_cast<MockAnimationController*>(pMyPlayer->GetModuleEntity()->GetAnimationController());
		if (ac)
		{
			ac->SetAniDataSet(&m_PlayerAniDataSet);
			ac->SetAnimation(wstring(L"none_idle"));
		}

		pMyPlayer->GetModuleEntity()->GetActor()->m_pMesh = m_pDefaultMesh;
	}

	REngine::GetSceneManager().SetRealtimeRenderingMethod(RENDERING_NULL);
}

TestGame::~TestGame()
{
	m_pGame->Destroy();
	m_pController->Destroy();

	SAFE_DELETE(m_pGame);
	SAFE_DELETE(m_pController);

	SAFE_DELETE(m_pFocusWidget);

	SAFE_DELETE(m_pDefaultMesh);
}

void TestGame::Update( float fDelta )
{
//	REngine::GetSceneManager().SyncTimerToApp(fDelta);
	REngine::GetSceneManager().UpdateTimerCustom( fDelta*1000 );
	m_pController->OnUpdate(fDelta);
	m_pGame->Update(fDelta);
	REngine::GetSceneManager().Update();

	REngine::GetDevice().BeginScene();
	REngine::GetSceneManager().RenderPrimaryCamera();
	REngine::GetDevice().EndScene();
	REngine::GetDevice().Flip();
}

XMyPlayer* TestGame::GetMyPlayer()
{
	XObject* pMyObject = gg.omgr->Find(XGetMyUID());

	if (pMyObject == NULL)
		return NULL;

	if (MIsExactlyClass(XMyPlayer, pMyObject) == false)
		return NULL;

	pMyObject->GetActor()->SetAnimationUpdated(true);
	return pMyObject->AsMyPlayer();
}

void TestGame::InitGlobalVar()
{
	gvar.MyInfo.ChrStatus.nHP = 100;
	gvar.MyInfo.ChrStatus.nEN = 100;
	gvar.MyInfo.ChrStatus.nSTA = 100;

	if (global.system)
		gvar.MyInfo.MyUID = global.system->GenerateLocalUID();

	gvar.MyInfo.SetUIID(XUnitTestUtil::NewUIID());
}

void TestGame::AddPlayerColCapsule()
{
	// Player 캡슐 입력
	MCapsule PlayerCapsule(vec3(0.0f, 0.0f, 40.0f), vec3(0.0f, 0.0f, 150.0f), 40.0f);

	// 메쉬 충돌 정보 입력
	XUnitTestHelper::SetMeshColCapsule(L"hf", PlayerCapsule);
	XUnitTestHelper::SetMeshColCapsule(L"hm", PlayerCapsule);
	XUnitTestHelper::SetMeshColCapsule(L"", PlayerCapsule);

	m_pDefaultMesh = new RMesh("hf", NULL);
}