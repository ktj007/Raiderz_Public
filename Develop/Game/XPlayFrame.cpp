#include "stdafx.h"
#include "XPlayFrame.h"
#include "XGame.h"
#include "XBaseApplication.h"
#include "XCameraManager.h"
#include "XGameFrameHelper.h"
#include "XGameFrameUILoader.h"
#include "XController.h"
#include "XWidgetNameDef.h"
#include "XScreenShotSaver.h"
#include "XLoadingChecker_BackgroundWork.h"
#include "XLoadingChecker_Server.h"
#include "XLoadingChecker_World.h"
#include "XLoadingChecker_MyPlayer.h"
#include "XMovableDebugger.h"
#include "XPost_Local.h"
#include "XHelp.h"				// by tripleJ 110412

#include "XCutSceneFieldIntroMgr.h"

enum XLoadingType
{
	XLT_WORLD		= 0,
	XLT_FIELD,
	XLT_PREPARE
};





// XPlayFrame
XPlayFrame::XPlayFrame() 
: XBaseFrame()
{
	m_pGame = NULL;
	m_pLoadingFrame = new XLoadingFrame( this);
	m_pLoadingFrame->AddLoadingChecker( new XLoadingChecker_BackgroundWork());
	m_pLoadingFrame->AddLoadingChecker( new XLoadingChecker_World());
	m_pLoadingFrame->AddLoadingChecker( new XLoadingChecker_MyPlayer());
	m_pLoadingFrame->PauseOnLoadingComplete( true);


#ifndef _PUBLISH
	m_pMovableDebugger = NULL;
#endif
}


// ~XPlayFrame
XPlayFrame::~XPlayFrame()
{
	if ( m_pLoadingFrame != NULL)
	{
		delete m_pLoadingFrame;
		m_pLoadingFrame = NULL;
	}
}


// Open
bool XPlayFrame::Open( int nPrevFrameID, const wchar_t* szArgs)
{
	dlog( "Enter frame : Game\n");

	// Load account info
	gvar.Char.m_AccountInfo.Load();
	gvar.Char.m_AccountInfo.ArrangeJournalIndicator();


	// Clear UI
	global.ui->Clear();

	// Load UI
	XGameFrameUILoader ui_loader;
	ui_loader.Load( L"CutScene");
	ui_loader.Load( UI_GAME_SHEET_NAME);
	global.ui->ShowSheet( UI_GAME_SHEET_NAME);


	// Call event
	if ( global.script)
	{
		XFieldInfo* pFieldInfo = info.field->GetSimpleFieldInfo( gvar.World.GetFieldID());
		if (pFieldInfo)
		{
			global.script->GetGlueGameEvent().OnGameEvent( "UI", "ENTER");
			global.script->GetGlueGameEvent().OnGameEvent( "UI", "LOADING", pFieldInfo->m_nZoneID);
		}
	}


	// Create game
	_ASSERT( m_pGame == NULL);
	m_pGame = new XGame();

	// Start loading
	m_nLoadingType = XLT_WORLD;
	m_pLoadingFrame->Start();


	// Create debug info
#ifndef _PUBLISH

	dlog( "Opened game play frame.\n");

	_ASSERT( m_pMovableDebugger == NULL);
	m_pMovableDebugger = new XMovableDebugger();

	m_Debugger.Create();

	ReloadDashBoard();

	global.smgr->GetRenderingStrategy()->SetFXAA_Level(4);

	if (global.app->GetMode() == EM_LOCAL_TEST)
	{
		m_pLoadingFrame->Stop();
		OnLoading();
		OnLoaded();
	}
#endif


	PFC_RESET;
	
	return true;
}


// Close
void XPlayFrame::Close( int nNextFrameID)
{
	dlog( "Leave frame : Game\n");

	// UI
	if ( global.script)		global.script->GetGlueGameEvent().OnGameEvent( "UI", "LEAVE");
	global.ui->DeleteSheet( L"CutScene");
	global.ui->DeleteSheet( UI_GAME_SHEET_NAME);


	// Save account info
	gvar.Char.m_AccountInfo.Save();


	// Delete debug info
#ifndef _PUBLISH

	m_Debugger.Destroy();

	if ( m_pMovableDebugger != NULL)
		SAFE_DELETE( m_pMovableDebugger);

#endif


	// Delete game
	if ( m_pGame != NULL)
	{
		m_pGame->Destroy();
		SAFE_DELETE( m_pGame);
	}

#ifndef _PUBLISH
	PFC_FINALANALYSIS( "profile_game_c.txt");
#endif

	global.camera->DestroyCamerasOnFrameClose();

	if ( global.sound)
	{
		global.sound->SetDefalutBGM( NULL);
		global.sound->StopAll();
	}
}


// OnEvent
bool XPlayFrame::OnEvent( MEvent& e)
{
	// Check loading
	if ( m_pLoadingFrame->IsLoading() == true)
	{
		// Process UI event
		if ( global.ui->OnEvent( e) == true)			return true;

		return false;
	}



	if ( m_pGame == NULL)								return false;


	// Process pre-forced game event
	if ( m_pGame->OnForceEvent( e) == true)				return true;

	// Process UI event
	if ( global.ui->OnEvent( e) == true)				return true;

	// Process game event
	if ( m_pGame->OnEvent( e) == true)					return true;

	// Process debug info
	if ( XGameFrameHelper::OnDebugEvent( e) == true)	return true;

	// Process debuger event
	if ( m_Debugger.OnDebugEvent( e) == true)			return true;


#ifndef _PUBLISH

	// Process dash board event
	if ( m_DebugDashBoard.OnDebugEvent( e) == true)		return true;

	if ( gg.controller  &&  e.nMessage == MWM_CHAR  &&  e.nKey == ',')
	{
		static XCameraType nLastCameraType;

		if ( global.camera->GetPrimaryCamera())
		{
			RCameraSceneNode* pNextCameraSceneNode = NULL;
			vec3 vOldCameraPos = global.camera->GetPrimaryCamera()->GetPosition();
			vec3 vOldCameraDir = global.camera->GetPrimaryCamera()->GetDirection();

			bool bEnableFog = global.camera->GetPrimaryCamera()->GetFogEnable();
			float fFogNear = global.camera->GetPrimaryCamera()->GetFogNear();
			float fFogFar = global.camera->GetPrimaryCamera()->GetFogFar();
			DWORD dwFogColor = global.camera->GetPrimaryCamera()->GetFogColor();

			bool bToggle = false;
			if (global.camera->IsPrimaryCameraType(CAMERA_MAIN_BACKVIEW))
			{
				nLastCameraType = CAMERA_MAIN_BACKVIEW;
				bToggle = true;
			}
			else if (global.camera->IsPrimaryCameraType(CAMERA_REPLAY_BACKVIEW))
			{
				nLastCameraType = CAMERA_REPLAY_BACKVIEW;
				bToggle = true;
			}

			if ( bToggle)
			{
				global.camera->SetPrimaryCamera(CAMERA_FREE_LOOK);
				pNextCameraSceneNode = global.camera->GetPrimaryCamera();
				if (pNextCameraSceneNode)
				{
					pNextCameraSceneNode->SetPosition(vOldCameraPos);
					pNextCameraSceneNode->SetDirection(vOldCameraDir);
					pNextCameraSceneNode->SetFog( fFogNear, fFogFar, dwFogColor);
					pNextCameraSceneNode->SetFogEnable( bEnableFog);
				}
			}
			else if (global.camera->IsPrimaryCameraType(CAMERA_FREE_LOOK))
				global.camera->SetPrimaryCamera( nLastCameraType);
		}
	}

#endif


	// Dispatch event
	return DispatchEvent( e);
}


// DispatchEvent
bool XPlayFrame::DispatchEvent( MEvent& e)
{
	if ( gg.controller == NULL)			return false;

	switch ( e.nMessage)
	{
	case MWM_SYSCHAR :
		{
			switch ( e.nKey)
			{
			case 'z':
				{
					if (global.ui)
					{
						global.ui->ToggleShowGameUI();
						return true;
					}
				}
				break;
			};
		}
		break;
	case MWM_KEYDOWN :
		{
			// Check Camera Lock View
			if(CheckCameraStateClear(e))
				return true;

			// Check virtual key
			XVirtualKey nVKey = gg.controller->FindVirtualKey( e.nKey, MEvent::GetShiftState(), e.bCtrl, MEvent::GetAltState());
			string strVKey = gg.controller->GetVirtualKeyName( nVKey);

			if ( (nVKey > VKEY_UI_NA  &&  nVKey < VKEY_COUNT)  ||  nVKey == VKEY_TOGGLE_MOUSE)
			{
				bool bRetVal = false;

				switch ( nVKey)
				{
				case VKEY_TOGGLE_MOUSE :		m_CursorSmartShower.OnKeyDown();	break;

				case VKEY_UI_MAINMENU :			bRetVal = global.script->GetGlueGameEvent().OnGameEvent( "VKEY_DOWN", strVKey.c_str());		break;

				case VKEY_UI_CONTROLSHELP1 :	bRetVal = global.script->GetGlueGameEvent().OnGameEvent( "VKEY_DOWN", "CONTROLSHELP", 1);	break;
				case VKEY_UI_CONTROLSHELP2 :	bRetVal = global.script->GetGlueGameEvent().OnGameEvent( "VKEY_DOWN", "CONTROLSHELP", 2);	break;

				case VKEY_UI_PALETTE0 :			bRetVal = global.script->GetGlueGameEvent().OnGameEvent( "VKEY_DOWN", "PALETTE", 0);		break;
				case VKEY_UI_PALETTE1 :			bRetVal = global.script->GetGlueGameEvent().OnGameEvent( "VKEY_DOWN", "PALETTE", 1);		break;
				case VKEY_UI_PALETTE2 :			bRetVal = global.script->GetGlueGameEvent().OnGameEvent( "VKEY_DOWN", "PALETTE", 2);		break;
				case VKEY_UI_PALETTE3 :			bRetVal = global.script->GetGlueGameEvent().OnGameEvent( "VKEY_DOWN", "PALETTE", 3);		break;
				case VKEY_UI_PALETTE4 :			bRetVal = global.script->GetGlueGameEvent().OnGameEvent( "VKEY_DOWN", "PALETTE", 4);		break;
				case VKEY_UI_PALETTE5 :			bRetVal = global.script->GetGlueGameEvent().OnGameEvent( "VKEY_DOWN", "PALETTE", 5);		break;
				case VKEY_UI_PALETTE6 :			bRetVal = global.script->GetGlueGameEvent().OnGameEvent( "VKEY_DOWN", "PALETTE", 6);		break;
				case VKEY_UI_PALETTE7 :			bRetVal = global.script->GetGlueGameEvent().OnGameEvent( "VKEY_DOWN", "PALETTE", 7);		break;
				case VKEY_UI_PALETTE8 :			bRetVal = global.script->GetGlueGameEvent().OnGameEvent( "VKEY_DOWN", "PALETTE", 8);		break;
				case VKEY_UI_PALETTE9 :			bRetVal = global.script->GetGlueGameEvent().OnGameEvent( "VKEY_DOWN", "PALETTE", 9);		break;

				case VKEY_UI_PALETTE_SET0 :		bRetVal = global.script->GetGlueGameEvent().OnGameEvent( "VKEY_DOWN", "PALETTESET", 0);		break;
				case VKEY_UI_PALETTE_SET1 :		bRetVal = global.script->GetGlueGameEvent().OnGameEvent( "VKEY_DOWN", "PALETTESET", 1);		break;
				case VKEY_UI_PALETTE_SET2 :		bRetVal = global.script->GetGlueGameEvent().OnGameEvent( "VKEY_DOWN", "PALETTESET", 2);		break;
				case VKEY_UI_PALETTE_SET3 :		bRetVal = global.script->GetGlueGameEvent().OnGameEvent( "VKEY_DOWN", "PALETTESET", 3);		break;
				case VKEY_UI_PALETTE_SET4 :		bRetVal = global.script->GetGlueGameEvent().OnGameEvent( "VKEY_DOWN", "PALETTESET", 4);		break;

				default :						bRetVal = global.script->GetGlueGameEvent().OnGameEvent( "VKEY_DOWN", strVKey.c_str());		break;
				}

				return bRetVal;
			}
			break;
		}


	case MWM_KEYUP :
		{
			// Check virtual key
			XVirtualKey nVKey = gg.controller->FindVirtualKey( e.nKey, MEvent::GetShiftState(), e.bCtrl, MEvent::GetAltState());
			string strVKey = gg.controller->GetVirtualKeyName( nVKey);

			switch ( nVKey)
			{
			case VKEY_TOGGLE_MOUSE :
				m_CursorSmartShower.OnKeyUp();
				break;

			default :
				break;
			}


			// Check key
			switch ( e.nKey)
			{
			case VK_SNAPSHOT :
				// Recording
				if ( e.bCtrl == true)
				{
					if (global.bandi_capturer)
						global.bandi_capturer->ToggleStart();
				}
				// Screen capture
				else
				{
					XScreenShotSaver saver;
					saver.Save();
				}
				break;
			case VK_SCROLL :
				{
					if (global.bandi_capturer)
						global.bandi_capturer->ToggleStart();
				}
				break;
			default :
				break;
			}
			break;
		}


	default :
		break;
	}

	return false;
}


// Message
void XPlayFrame::Message( const wchar_t* szMsg)
{
	// Change field
	if ( wcscmp( szMsg, L"ChangeField") == 0)
	{
		// Start loading
		global.ui->TargetUIEnd();
		global.script->GetGlueGameEvent().OnActTalentEnd();
		m_nLoadingType = XLT_FIELD;
		m_pLoadingFrame->Start();
	}

	// Start game
	else if ( wcscmp( szMsg, L"Field_StartGame") == 0)
	{
		// Continue loading frame
		m_pLoadingFrame->Continue();
	}

	else if ( wcscmp( szMsg, L"Prepare") == 0)
	{
		m_nLoadingType = XLT_PREPARE;
		OnPreLoading();

		XFieldInfo* pFieldInfo = info.field->GetSimpleFieldInfo(gvar.Temp.nNextFieldID);
		if (pFieldInfo)
		{
			if ( global.script)	global.script->GetGlueGameEvent().OnGameEvent( "UI", "PREPARE", pFieldInfo->m_nZoneID);
		}
	}

	else if (wcscmp( szMsg, L"CancelChangeField") == 0)
	{
		OnLoaded();
	}

	// Correct message
	else
	{
		_ASSERT( 0);
	}
}


// Update
void XPlayFrame::Update( float fDelta)
{
#ifdef _UNITTEST
	if ( global.app)				global.app->Quit();
	return;
#endif

	// Update loading frame
	m_pLoadingFrame->Update( fDelta);


	// Update base
	XBaseFrame::Update( fDelta);


	// Update game
	if ( m_pGame != NULL)			m_pGame->Update( fDelta);
}


// InitWorld
bool XPlayFrame::InitWorld()
{
	return true;
}


// InitCamera
bool XPlayFrame::InitCamera()
{
	global.camera->CreateCamerasOnFrameOpen( GetID());
	global.camera->SetPrimaryCamera( CAMERA_MAIN_BACKVIEW);

	return true;
}


// InitGame
bool XPlayFrame::InitGame()
{
	if ( m_pGame->Create() == false)
	{
		mlog( "Failed to create game\n");
		return false;
	}

	return true;
}


// OnPreRender
void XPlayFrame::OnPreRender()
{
	//PreLoad Intro CutScene Resource
	XCutSceneFieldIntroMgr::GetCutSceneFieldIntroMgr().PreLoadIntroCutScene();

	// Check loading
	if ( m_pLoadingFrame->IsLoading() == true)			return;

	// beginning field intro cutscene
	if (XCutSceneFieldIntroMgr::GetCutSceneFieldIntroMgr().beginIntroCutScene())
		return;

	// Pre-render base
	XBaseFrame::OnPreRender();


	// Pre-render game
	if ( m_pGame != NULL)			m_pGame->OnPreRender();
}


// OnRender
void XPlayFrame::OnRender()
{
	if ( m_pLoadingFrame->IsLoading() == true)
	{
		global.mint->Draw();
	}
	else
	{
#ifndef _PUBLISH

		m_Debugger.RenderDebug();
		m_Debugger.RenderRecordMessage();

#endif

		// Render game
		if ( m_pGame != NULL)		m_pGame->OnRender();


		// Render base
		XBaseFrame::OnRender();


		// Post-render object
		gg.omgr->PostFrameRender();
	}


	// Draw fade screen
	m_pLoadingFrame->Render( global.ui->GetDC());

	// Draw CutScene Mask
	XCutSceneFieldIntroMgr::GetCutSceneFieldIntroMgr().RenderMaskForCutScene();
}


// OnActive
void XPlayFrame::OnActive( bool bActive)
{
	if ( m_pGame != NULL)			m_pGame->OnActive( bActive);
}


// OnPreLoading
void XPlayFrame::OnPreLoading()
{
	// Set enabled controller
	if ( gg.controller)				gg.controller->SetEnabled( false);


	// Do not rendering world
	if ( global.camera  &&  global.camera->GetPrimaryCamera())
		global.camera->GetPrimaryCamera()->SetPassIgnore( RRP_WORLDSCENE, RRP_POSTEFFECT);


	// Clear loading percentage
	if ( gg.currentgamestate  &&  gg.currentgamestate->GetWorld())
	{
		RWorldSceneNode* pWorldSceneNode = gg.currentgamestate->GetWorld()->GetWorldSceneNode();
		if ( pWorldSceneNode)		pWorldSceneNode->Clean();
	}


	// UI
	if ( m_nLoadingType == XLT_FIELD)
	{
		XFieldInfo* pFieldInfo = info.field->GetSimpleFieldInfo( gvar.World.GetFieldID());
		if (pFieldInfo)
		{
			if ( global.script)	global.script->GetGlueGameEvent().OnGameEvent( "UI", "LOADING", pFieldInfo->m_nZoneID);
		}
	}

	// Helper 사용처리
	if( global.ui )
		global.ui->GetHelpMgr()->InitHelpMgr();

	REngine::GetBackgroundWorker().SetUpdateAcceptableTime( REngine::GetBackgroundWorker().GetUpdateAcceptableTime(), false );
}


// OnLoading
void XPlayFrame::OnLoading()
{
	if ( global.sound)
	{
		global.sound->SetDefalutBGM( NULL);
		global.sound->StopAll();
		global.sound->SetSilence( true);
	}


	switch ( m_nLoadingType)
	{
	case XLT_WORLD :
		if(InitWorld() == false ||
			InitCamera() == false ||
			InitGame() == false)
		{
			// 실패하면 종료하겠습니다.
			if (global.app)
				PostMessage(global.app->GetWindowHandle(), WM_CLOSE, 0, 0);
		}
		break;

	case XLT_FIELD :
		m_pGame->ChangeField( gvar.World.GetFieldID(), gvar.World.GetNextMapStartPos(), gvar.World.GetNextMapStartDir());
		break;
	}

	//로딩 상태에서는 엔진쪽의 업데이트와 랜더를 하지 않게 하여 로딩 속도를 빠르게 합니다.
	global.smgr->TurnOffToUpdateAndRender(true);
	mlog("global.smgr->TurnOffToUpdateAndRender(true) \n");
}


// OnPaused
void XPlayFrame::OnPaused()
{
	// Set world loading complete
	if ( m_pGame != NULL)			m_pGame->OnWorldLoadingComplete();

	global.smgr->TurnOffToUpdateAndRender(false);
	mlog("global.smgr->TurnOffToUpdateAndRender(false) \n");
}


// OnLoaded
void XPlayFrame::OnLoaded()
{
	// Set camera
	if ( global.camera)
	{
		// Start rendering world
		if ( global.camera->GetPrimaryCamera())
			global.camera->GetPrimaryCamera()->RemoveAllIgnorePass();

		// Set camera
		XCameraBackViewController* pCameraController = global.camera->GetCameraController<XCameraBackViewController>();
		if ( pCameraController)
			pCameraController->Init( XGetMyUID());
	}


	// Set enabled controller
	if ( gg.controller)				gg.controller->SetEnabled( true);


	// UI
	if ( global.script)
	{
		global.script->GetGlueGameEvent().OnGameEvent( "FIELD", "CHANGED", gvar.World.GetFieldID());

		// 필드 이동때 다이내믹 필드에서 일반 필드로 나가면 해당 위젯을 숨김
		CSFieldFileInfo* pFieldInfo = info.field->GetFieldList().Find( gvar.World.GetFieldID());
		if ( pFieldInfo != NULL  &&  pFieldInfo->bDynamic == false)
			global.script->GetGlueGameEvent().OnGameEvent( "MISSIONPROG", "END");

		global.script->GetGlueGameEvent().OnGameEvent( "UI", "LOADED");
	}


	// Stop loading frame
	m_pLoadingFrame->Stop();

	if ( global.sound)
		global.sound->SetSilence( false);

	REngine::GetBackgroundWorker().SetUpdateAcceptableTime( REngine::GetBackgroundWorker().GetUpdateAcceptableTime(), true );
	global.smgr->TurnOffToUpdateAndRender(false);
	mlog("global.smgr->TurnOffToUpdateAndRender(false) \n");
}

bool XPlayFrame::IsLoadingFrame()
{
	if(m_pLoadingFrame == NULL)
		return false;

	return m_pLoadingFrame->IsLoading();
}

bool XPlayFrame::CheckCameraStateClear( MEvent& e )
{
	if(e.nKey == VK_ESCAPE)
	{
		if(gg.controller->IsCameraLockView())
		{
			// 카메라락 해제
			if ( global.camera->IsPrimaryCameraType( CAMERA_MAIN_BACKVIEW))
			{
				XCameraBackViewController* pBackViewCamera = global.camera->GetCameraController<XCameraBackViewController>();
				if (pBackViewCamera)
				{
					pBackViewCamera->EndCameraLock();
					return true;
				}
			}
		}
		else if(gg.controller->IsCameraPvPFreeView())
		{
			// pvp free view 해제
			XEvent msg;
			msg.m_nID = XEVTL_CAMERA_PVP_VIEW_CLEAR;
			XObject* pOwner = gg.omgr->FindObject(XGetMyUID());
			pOwner->Event(msg);
			return true;
		}
	}

	return false;
}

#ifndef _PUBLISH

// ReloadDashBoard
void XPlayFrame::ReloadDashBoard()
{
	m_DebugDashBoard.Create( global.ui->GetMint());
}

#endif
