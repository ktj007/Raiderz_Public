#include "stdafx.h"
#include "XEngineDebugFrame.h"
#include "RSceneManager.h"
#include "XDef.h"
#include "XGame.h"
#include "XController.h"
#include "XPost_GM.h"
#include "XGameFrameUILoader.h"
#include "XGameFrameHelper.h"
#include "XBaseApplication.h"
#include "XCameraManager.h"
#include "XScreenShotSaver.h"
#include "RBackgroundWorker.h"
#include "XMovableDebugger.h"
#include "XEngineDebugger.h"
#include "XGameInitializer.h"

#include "XWorld.h"

XEngineDebugFrame::XEngineDebugFrame()
{
	m_bInitAfterLoading = false;
	m_pEngineDebugger = new XEngineDebugger;
}

XEngineDebugFrame::~XEngineDebugFrame()
{
	delete m_pEngineDebugger;
}

void XEngineDebugFrame::Update(float fDelta)
{
	PFC_THISFUNC;

	if (m_bInitAfterLoading == false)
	{
		global.ui->ToggleConsole();

		if (global.script)
		{
			// World를 만들때 이 구문이 한번더 들어간다
			XScript* pScript = global.script;
			global.script = NULL;
			pScript->GetGlueGameEvent().OnDebugOnGameStarted();
			global.script = pScript;
		}

		m_bInitAfterLoading = true;
	}

	// base frame update : net / ui / mint
	XBaseFrame::Update(fDelta);

	// 컨트롤러 코어 업데이트
	if (gg.controller)
		gg.controller->OnUpdate(fDelta);

	// 게임 코어 업데이트
	if (m_pGame)
		m_pGame->Update(fDelta);

	// 디버거 업데이트
	m_pEngineDebugger->Update(fDelta);
}

void XEngineDebugFrame::OnRender()
{
	PFC_THISFUNC;
	XPlayFrame::OnRender();
}

bool XEngineDebugFrame::OnEvent(MEvent& e)
{
	if (e.nMessage == MWM_KEYDOWN)
	{
		if (global.ui)
		{
			if (e.nKey == 192 && e.bCtrl == true)
			{
				// 콘솔
				global.ui->ToggleConsole();

				// -_- 이동 입력등 게임 컨트롤이 먹으려면 마우스 보임이 꺼져야한다.
				global.ui->ShowMouse(global.ui->IsConsoleVisible());
				return true;
			}
		}
	}

	if (XPlayFrame::OnEvent(e))
		return true;

	if (XGameFrameHelper::OnDebugEvent(e))
		return true;

	return false;
}

bool XEngineDebugFrame::Open( int nPrevFrameID, const wchar_t* szArgs)
{
	XCONST::AUTO_REPORT_CRASHDUMP = true;

	global.app->GetInitializer()->Init_While_LoadingLoginCampaign();
	global.app->SetClearColor(0xFF000000);
	XPlayFrame::Open( nPrevFrameID, szArgs);
	return true;
}

void XEngineDebugFrame::Close( int nNextFrameID)
{
	XPlayFrame::Close( nNextFrameID);
}

bool XEngineDebugFrame::OnOpenWorldLoadingInit()
{
	return true;
}

bool XEngineDebugFrame::OnOpenControllerInit()
{
	if (gg.controller->Create() == false)
	{
		assert(!"XPlayFrame::Open - Failed to Create Controller");
		return false;
	}

	return true;
}

bool XEngineDebugFrame::OnOpenCameraInit()
{
	global.camera->CreateCamerasOnFrameOpen(GetID());
	global.camera->SetPrimaryCamera(CAMERA_ENGINE_DEBUG);

	return true;
}

bool XEngineDebugFrame::OnOpenGameInit()
{
	return true;
}
