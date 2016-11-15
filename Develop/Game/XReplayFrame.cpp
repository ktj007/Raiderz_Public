#include "stdafx.h"
#include "XReplayFrame.h"
#include "XReplay.h"
#include "XGame.h"
#include "XController.h"
#include "XGameFrameUILoader.h"
#include "XGameFrameHelper.h"
#include "XMyPlayer.h"
#include "XModuleMyControl.h"
#include "XModuleNetControl.h"
#include "XModuleNonControl.h"
#include "XNonPlayer.h"
#include "RConfiguration.h"
#include "MRegistry.h"
#include "XBaseApplication.h"
#include "XSystem.h"
#include "XCameraManager.h"
#include "XScreenShotSaver.h"
#include "XMovableDebugger.h"
#include "XReplayAutoTest.h"
#include "XStrings.h"
#include "XWidgetNameDef.h"
#include "XReplayAnalyzer.h"
#include "XPlayFrame.h"
#include "XGameInitializer.h"

XReplayFrame::XReplayFrame() : XPlayFrame(), m_pReplay(NULL), m_bFinalizeReplay(false)
, m_bAutoTest(false), m_bReplayAnalyze(false), m_bReplayScreenShot(false)
{
	m_pReplayAutoTest = new XReplayAutoTest();
}

bool XReplayFrame::Open( int nPrevFrameID, const wchar_t* szArgs)
{
#ifndef _PUBLISH
	if (!m_pMovableDebugger)
		m_pMovableDebugger = new XMovableDebugger();
#endif

	SetupConfigFromAppParams();
	m_strReplayFileName = szArgs;

	if (m_bReplayAnalyze)
	{
		XReplayAnalyzer replayAnalyzer;
		if (replayAnalyzer.Load(m_strReplayFileName.c_str()))
		{
			replayAnalyzer.Analyze();
		}
		if (global.app) global.app->Quit();
		return false;
	}

	global.app->GetInitializer()->Init_While_LoadingLoginCampaign();

	if (m_bAutoTest) 
	{
		m_pReplayAutoTest->Start(m_bReplayScreenShot, m_ReplayTestBaseline);
	}

	XScreenDebugger::GetInstance()->Show();

	WriteReplayLink();

	m_pReplayAutoTest->CollectData(RTCDT_LOADING_START);


	m_uidCameraTarget.SetZero();
	m_pReplay = new XReplay();
	if (m_pReplay->Load(m_strReplayFileName.c_str()) == false)
	{
		if (m_bAutoTest) 
		{
			m_pReplayAutoTest->Fail(L"리플레이 파일의 버전이 맞지 않습니다.");
		}

		return false;
	}

	gvar.World.SetFieldID(m_pReplay->GetGameInfo().nFieldID);
	gvar.World.nGameTime = m_pReplay->GetGameInfo().nGameTime;
	gvar.World.nWeather = m_pReplay->GetGameInfo().nWeather;

	gvar.MyInfo.MyUID	= m_pReplay->GetGameInfo().MyUID;
	gvar.MyInfo.SetUIID(m_pReplay->GetGameInfo().nUIID);

	m_bFinalizeReplay = false;
	m_pReplay->Play();

	InitializeUI();

	m_pGame = new XGame();

//	CreateLoadingMgr(m_pGame, &m_LoadingFrame);

	global.camera->CreateCamerasOnFrameOpen(GetID());
	global.camera->SetPrimaryCamera(CAMERA_REPLAY_BACKVIEW);

	if (m_pGame->Create() == false)
	{
		mlog("Failed to Create Game\n");
		return false;
	}
	m_pGame->GetCurrentGameState()->EnableObjectUpdateCache(false);
	gvar.Game.pMyPlayer->RemoveFromSceneManager();
	gg.omgr->Erase(gvar.Game.pMyPlayer);
	gvar.MyInfo.MyUID = MUID::ZERO;
	gvar.MyInfo.SetUIID(UIID_INVALID);

	XCameraReplayBackViewController* pCameraController = global.camera->GetCameraController<XCameraReplayBackViewController>();
	if (pCameraController) pCameraController->Init(m_pReplay->GetGameInfo().MyUID);

	if (m_bAutoTest)
	{
		gg.controller->SetFreeLookCamera(false);
		XCameraReplayBackViewController* pCameraController = global.camera->GetCameraController<XCameraReplayBackViewController>();
		if (pCameraController)
			pCameraController->SetSyncDirToTargetDir(true);
	}

	//PFC_RESET;

	SetCameraTarget(true);

	m_pReplayAutoTest->CollectData(RTCDT_LOADING_END);

	m_ScreenShotDumper.Start(m_strReplayScreenShotPath);

	//m_pLoadingFrame->Start();
	m_pGame->OnWorldLoadingComplete();

	return true;
}

void XReplayFrame::Close( int nNextFrameID)
{
#ifndef _PUBLISH
	if (m_pMovableDebugger)
	{
		SAFE_DELETE(m_pMovableDebugger);
	}
#endif

//	DestroyLoadingMgr();

	SAFE_DELETE(m_pReplay);

	if (m_pGame)
	{
		m_pGame->Destroy();
	}

	if (gvar.Game.pMyPlayer)
	{
		gvar.Game.pMyPlayer->Destroy();
		delete gvar.Game.pMyPlayer;
		gvar.Game.pMyPlayer = NULL;
	}

	global.camera->DestroyCamerasOnFrameClose();
	FinalizeUI();

	SAFE_DELETE(m_pGame);
}

void XReplayFrame::Update( float fDelta )
{
	PFC_THISFUNC;

	//if (fDelta > 1.0f) fDelta = 0.1f;

	if (m_pReplay)
	{
		if (!m_pReplay->IsDone())
		{
			m_pReplay->Update(fDelta);

			m_pReplayAutoTest->CollectData(RTCDT_GAME_TICK);
		}
		else
		{
			if (!m_bFinalizeReplay)
			{
				OnStopReplay();
				m_bFinalizeReplay = true;

				if (m_bAutoTest)
				{
					m_pReplayAutoTest->End();
				}
			}
		}
	}

	if (m_bReplayScreenShot)
	{
		m_ScreenShotDumper.Update();
	}

	XPlayFrame::Update(fDelta);
}

bool XReplayFrame::OnEvent( MEvent& e )
{
	if (m_pGame == NULL) return false;

	if (XBaseFrame::OnEvent(e)) return true;
	if (XGameFrameHelper::OnDebugEvent(e)) return true;

	if ((gg.controller) /*&& (global.ui->IsFocusMainInput())*/)
	{
		switch(e.nMessage)
		{
			case MWM_CHAR:
				{
					switch (e.nKey) 
					{
					case 'g':
						{
							StartReplay();
						}
						return true;
					case 'h':
						{
							TogglePauseReplay();
						}
						return true;
					case 'o':
						{
							SpeedUpReplay();
						}
						return true;
					case 'p':
						{
							SpeedDownReplay();
						}
						return true;
					case ',':
						{
#ifndef _PUBLISH
							m_Debugger.ToggleFreeLookCamera();
#endif
						}
						return true;
					}
				}
				break;
			case MWM_LBUTTONDOWN:
				{
					SetCameraTarget(false);
				}
				break;
			case MWM_RBUTTONDOWN:
				{
					SetCameraTarget(true);
				}
				break;
			case MWM_MOUSEWHEEL:
				{
					CameraControlOnMouseWheel(e);

				}break;
			case MWM_KEYDOWN:
				{
					if (e.bCtrl == true && e.bAlt == false)
					{
						for (int i = 0; i < 4; i++)
						{
							if (e.nKey == VK_F1 + i)
							{
								if (XScreenDebugger::GetInstance()->IsVisible() && (int)XScreenDebugger::GetInstance()->GetSelLayer() == XScreenDebugger::LAYER1 + i)
								{
									XScreenDebugger::GetInstance()->Show(false);
								}
								else
								{
									XScreenDebugger::GetInstance()->Show(true, XScreenDebugger::_Layer(i));
								}
								break;
							}
						}
					}
					if (e.nKey == 'P')
					{
						static bool bFillSolid = true;
						if (bFillSolid) global.device->SetFillMode(RFILL_WIRE);
						else global.device->SetFillMode(RFILL_SOLID);
						bFillSolid = !bFillSolid;
					}
					if(e.nKey == VK_ESCAPE)
					{
						// 종료하자
						if (global.app)
							PostMessage(global.app->GetWindowHandle(), WM_CLOSE, 0, 0);

						return true;
					}
				}
				break;
			case MWM_KEYUP:
				{
					switch (e.nKey)
					{
					case VK_SNAPSHOT:
						{
							XScreenShotSaver saver;
							saver.Save();
						}
						break;
					}
				}
				break;

		}
	}
	return false;
}

void XReplayFrame::SetCameraTarget( bool bMyPlayer )
{
	if (!gg.omgr) return;

	list<XObject*> listObject;

	for (XObjectMap::iterator itor = gg.omgr->BeginItor(); itor != gg.omgr->EndItor(); ++itor)
	{
		XObject* pObject = (*itor).second;
		if (pObject == gvar.Game.pMyPlayer) continue;
		if (MIsDerivedFromClass(XActor, pObject))
		{
			XActor* pActor = static_cast<XActor*>(pObject);
			if (pActor->GetUID() != m_pReplay->GetGameInfo().MyUID && pActor->IsDead()) continue;
		}

		listObject.push_back(pObject);
	}

	if (bMyPlayer)
	{
		m_uidCameraTarget = m_pReplay->GetGameInfo().MyUID;
	}
	else
	{
		for (list<XObject*>::iterator itor = listObject.begin(); itor != listObject.end(); ++itor)
		{
			XObject* pObject = (*itor);

			if (m_uidCameraTarget == MUID(0,0)) 
			{
				m_uidCameraTarget = pObject->GetUID();
				break;
			}
			if (pObject->GetUID() == m_uidCameraTarget) m_uidCameraTarget = MUID(0,0);
		}

		if (m_uidCameraTarget == MUID(0,0) && !listObject.empty())
		{
			XObject* pObject = (*listObject.begin());
			m_uidCameraTarget = pObject->GetUID();
		}
	}

	XCameraReplayBackViewController* pCameraController = global.camera->GetCameraController<XCameraReplayBackViewController>();
	if (pCameraController && m_uidCameraTarget != MUID(0,0))
	{
		pCameraController->Init(m_uidCameraTarget);
	}
}

void XReplayFrame::OnStopReplay()
{
	for (XObjectMap::iterator itor = gg.omgr->BeginItor(); itor != gg.omgr->EndItor(); ++itor)
	{
		XObject* pObject = (*itor).second;

		XModuleActorControl* pActorControl = NULL;
		if (MIsExactlyClass(XNetPlayer, pObject))
		{
			XNetPlayer* pNetPlayer = AsNetPlayer(pObject);
			pActorControl = pNetPlayer->GetModuleNetControl();

			XModuleNetControl* pModuleControl = pNetPlayer->GetModuleNetControl();
			if (pModuleControl) 
			{
				pModuleControl->OnMoveStop(&pNetPlayer->GetPosition());
				pModuleControl->OnStop(&pNetPlayer->GetPosition(), &pNetPlayer->GetDirection());
			}
		}
		else if (MIsExactlyClass(XMyPlayer, pObject))
		{
			XMyPlayer* pMyPlayer = pObject->AsMyPlayer();
			pActorControl = pMyPlayer->GetModuleMyControl();
		}
		else if (MIsExactlyClass(XNonPlayer, pObject))
		{
			XNonPlayer* pNPC = AsNPC(pObject);
			pActorControl = pNPC->GetModuleNonControl();
		}
		if (pActorControl) pActorControl->Stop();
	}

}

void XReplayFrame::OnRender()
{
	PFC_THISFUNC;

#ifndef _PUBLISH
	m_Debugger.RenderDebug();
#endif

	if (!m_bAutoTest)
	{
		RenderReplayMessage();
	}
	else
	{
		RenderBenchmarkMessage();
	}

	if (m_pGame) m_pGame->OnRender();

	XBaseFrame::OnRender();
}

void XReplayFrame::RenderReplayMessage()
{
	if (!m_pReplay) return;

	wchar_t text[256];
	int height = REngine::GetConfig().m_nHeight;

	swprintf_s(text, L"Replay : %s", m_strReplayFileName.c_str());
	XScreenDebugger::GetInstance()->Print(10, height - 70, text);

	swprintf_s(text, L"Key : g(Play), h(Pause), o(Speed Up), p(Speed Down), ,(Camera), MouseL(Next Target), MouseR(Me)");
	XScreenDebugger::GetInstance()->Print(10, height - 50, text);

	float fPlayTime = (float)m_pReplay->GetPlayTime();
	float fMaxTime = (float)m_pReplay->GetMaxTime();

	swprintf_s(text, L"Time = %.1f Sec(%.1f) , Speed = %.1f", fPlayTime, fMaxTime, m_pReplay->GetSpeed());
	XScreenDebugger::GetInstance()->Print(10, height - 30, text);
}

void XReplayFrame::RenderBenchmarkMessage()
{
	if (!m_pReplay) return;

	wchar_t text[256];
	int height = REngine::GetConfig().m_nHeight;


	float fPlayTime = (float)m_pReplay->GetPlayTime();
	float fMaxTime = (float)m_pReplay->GetMaxTime();

	//sprintf_s(text, "Score = %d", m_pReplayAutoTest->GetScore());
	//XScreenDebugger::GetInstance()->Print(10, 30, text);

	//sprintf_s(text, "Time = %.1f sec(%.1f)", fPlayTime, fMaxTime);
	//XScreenDebugger::GetInstance()->Print(10, 50, text);

	swprintf_s(text, L"Time = %s , Weather = %s", XStrings::GameTime(gvar.World.nGameTime), XStrings::Weather(gvar.World.nWeather));
	XScreenDebugger::GetInstance()->Print(10, 50, text);

}

void XReplayFrame::TogglePauseReplay()
{
	if (m_pReplay->IsPlaying())
	{
		m_pReplay->SetPause(true);
		global.system->GetTimer().SetPause(true);
	}
	else
	{
		m_pReplay->SetPause(false);
		global.system->GetTimer().SetPause(false);
	}
}
void XReplayFrame::StartReplay()
{
	bool bExistMyPlayer = false;

	//if (gvar.Game.pMyPlayer)
	//{
	//	XObjectMap::iterator itor = gg.omgr->Erase(gvar.Game.pMyPlayer);
	//	bExistMyPlayer = (itor != gg.omgr->EndItor());
	//	gvar.Game.pMyPlayer->RemoveFromSceneManager();
	//}

	gg.omgr->DestroyAllObject();
	if (bExistMyPlayer && gvar.Game.pMyPlayer) gg.omgr->Add(gvar.Game.pMyPlayer);

	m_bFinalizeReplay = false;
	m_pReplay->Play();
	global.system->GetTimer().ResetSpeed();
	global.system->GetTimer().SetPause(false);
}

void XReplayFrame::SpeedDownReplay()
{
	if (m_pReplay->GetSpeed() <= 0.1) return;
	m_pReplay->SetSpeed(m_pReplay->GetSpeed() - 0.1);
	global.system->GetTimer().SetSpeed(m_pReplay->GetSpeed());
}

void XReplayFrame::SpeedUpReplay()
{
	if (m_pReplay->GetSpeed() >= 7.99) return;
	m_pReplay->SetSpeed(m_pReplay->GetSpeed() + 0.1);
	global.system->GetTimer().SetSpeed(m_pReplay->GetSpeed());
}


void XReplayFrame::WriteReplayLink()
{
#ifndef PUBLISH
	// 리플레이 파일 연동
	unsigned int time = XGetNowTime();
	HKEY hKey = HKEY_CLASSES_ROOT;

	MRegistry::Write(hKey, ".smv", "", "Raiderz.smv");

	MRegistry::Write(hKey, "Raiderz.smv", "", "");

	char szValue[512];

	string strCurrentPath = MGetCurrentDirectoryA() + "Raiderz.exe";

	sprintf_s(szValue, "%s,1", strCurrentPath.c_str());

	MRegistry::Write(hKey, "Raiderz.smv\\DefaultIcon", "", szValue);
	MRegistry::Write(hKey, "Raiderz.smv\\shell", "", "");
	MRegistry::Write(hKey, "Raiderz.smv\\shell\\open", "", "");

	sprintf_s(szValue, "\"%s\" \"%%1\"", strCurrentPath.c_str());
	MRegistry::Write(hKey, "Raiderz.smv\\shell\\open\\command", "", szValue);
#endif
}

void XReplayFrame::InitializeUI()
{
	XGameFrameUILoader ui_loader;
	ui_loader.Load(L"game");
	global.ui->ShowGameUI(false);

	global.ui->ShowMouse(false);

	global.ui->ShowSheet(UI_GAME_SHEET_NAME);
}

void XReplayFrame::FinalizeUI()
{
	if (global.ui) global.ui->HideSheet(UI_GAME_SHEET_NAME);
}

XReplayFrame::~XReplayFrame()
{
	SAFE_DELETE(m_pReplayAutoTest);
}

void XReplayFrame::CameraControlOnMouseWheel( MEvent &e )
{
	if (!XCONST::CAMERA_FIX && e.bCtrl == true && e.bAlt == false)
	{
		int nDelta = e.nDelta;

		XBaseCameraSceneNode* pCameraNode = global.camera->GetCamera();
		if (pCameraNode)
		{
			XPolarCoord* pPolarCoord = pCameraNode->GetPolarCoord();
			pPolarCoord->fRadius += (-(float)nDelta * XCONST::MOUSE_WHEEL_SENSIBILITY);

			pPolarCoord->fRadius = max(XCONST::MIN_CAMERA_DIST, pPolarCoord->fRadius);
			pPolarCoord->fRadius = min(XCONST::MAX_CAMERA_DIST * 2, pPolarCoord->fRadius);
			pCameraNode->SetPolarCoord(pPolarCoord);
		}
	}
}

void XReplayFrame::SetupConfigFromAppParams()
{
	if (global.app == NULL) return;

	const cml2::MCommandLine& cl = global.app->GetCommandLine();

	m_bAutoTest = cl.HasSwitch("autotest");
	m_bReplayAnalyze = cl.HasSwitch("replay_analyze");
	m_bReplayScreenShot = cl.HasSwitch("screenshot");
	m_strReplayScreenShotPath = MLocale::ConvAnsiToUTF16(cl.GetSwitchValue("screenshotpath").c_str());

	m_ReplayTestBaseline.nFPS = (uint32)cl.GetSwitchValueInt("baseline_fps");
}

void XScreenShotDumper::Update()
{
	unsigned int nNowTime = XGetNowTime();
	m_nElapsedTime += nNowTime - m_nLastTime;
	m_nLastTime = nNowTime;

	if (m_nElapsedTime > m_nTick)
	{
		SaveScreenShot();

		m_nElapsedTime = 0;
	}
}

void XScreenShotDumper::SaveScreenShot()
{
	wstring strPath = m_strReplayScreenShotPath + wstring(L"/") + m_strFolder;


	XScreenShotSaver saver;
	saver.Save(strPath.c_str());
}

void XScreenShotDumper::Start(wstring strReplayScreenShotPath)
{
	m_strReplayScreenShotPath = strReplayScreenShotPath;

	m_nLastTime = XGetNowTime();
	m_nElapsedTime = 0;
	m_nTick = 10000;		// 10초마다 스크린샷을 남김

	SYSTEMTIME time;
	::GetLocalTime(&time);
	wchar_t folderName[512];
	swprintf_s( folderName, L"%d_%02d%02d_%02d%02d%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

	m_strFolder = folderName + wstring(L"/");
}