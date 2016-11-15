#include "stdafx.h"
#include "XGamePlayController.h"
#include "XGame.h"
#include "XGlobal.h"
#include "RCameraSceneNode.h"
#include "RJoystick.h"
#include "RSceneManager.h"
#include "XProfiler.h"
#include "XMyPlayer.h"
#include "XBaseApplication.h"
#include "XCameraManager.h"
#include "XModuleBuff.h"
#include "XEventID.h"
#include "XGameInterface.h"

#define MOUSEMOVE_CYCLETIME 0.3f

XGamePlayController::XGamePlayController()
: XController()
, m_bFreeLookCamera(false)
, m_bCameraRotationButtonDown(false)
, m_bMouseMoveButtonDown(false)
,m_fMouseMoveCycleTime(0.0f)
, m_bMouseClickActivate(true)
{
}


XGamePlayController::~XGamePlayController()
{
	Destroy();
}


bool XGamePlayController::Create()
{
	XGetGameInterface().OPTIONS.ApplyControlKeyMap();

	XController::Create();
	return true;
}

bool XGamePlayController::OnEvent( MEvent& e)
{
//	if ( IsEnabled() == false  ||  global.ui == NULL)		return false;

	if ( global.camera == NULL  ||  global.camera->IsPrimaryCameraType( CAMERA_FREE_LOOK))
		return false;


	if ( XGameInput::OnEvent( e))
	{
		SetFreeLookCamera( false);
		return true;
	}


	static int nCurrMouseX = 0;
	static int nCurrMouseY = 0;

	switch ( e.nMessage)
	{
	case MWM_LBUTTONDOWN:
		{
			if (global.ui  &&  global.ui->IsMouseVisible())
			{
				if ( IsEnabled() == true &&
					m_bMouseClickActivate == true)
					SendEventMouseMoveClick(e.Pos.x, e.Pos.y);

				m_bMouseMoveButtonDown = true;
				m_fMouseMoveCycleTime = MOUSEMOVE_CYCLETIME;
			}

//			::SetCapture(global.app->GetWindowHandle());			// Mint 쪽으로 이동
		}
		break;
	case MWM_LBUTTONUP:
		{
			nCurrMouseX = e.Pos.x;
			nCurrMouseY = e.Pos.y;

			m_bMouseMoveButtonDown = false;

			m_bMouseClickActivate = global.app->IsActive();

//			::ReleaseCapture();										// Mint 쪽으로 이동
		}
		break;

	case MWM_RBUTTONDOWN:
		{
			nCurrMouseX = e.Pos.x;
			nCurrMouseY = e.Pos.y;

			if ( global.ui  &&  global.ui->IsMouseVisible())
			{
				if ( IsEnabled() == true  &&  gg.omgr)
				{
					XEvent evtClick;

					MPoint ptMousePos(e.Pos.x, e.Pos.y);
					evtClick.m_nID = XEVTL_INTERACTION_CLICK;
					evtClick.m_pData = &ptMousePos;

					gg.omgr->PostEvent( XGetMyUID(), evtClick);
				}

				m_bCameraRotationButtonDown = true;
				global.ui->ShowMouse(false);
			}

//			::SetCapture(global.app->GetWindowHandle());			// Mint 쪽으로 이동
		}
		break;
	case MWM_RBUTTONUP:
		{
			if(m_bCameraRotationButtonDown)
			{
				if ( global.ui)		global.ui->ShowMouse(true);
			}

			m_bCameraRotationButtonDown = false;

//			::ReleaseCapture();										// Mint 쪽으로 이동
		}
		break;
	case MWM_KEYDOWN:
		{
			switch ( e.nKey)
			{
			// 엔터키에 대한 처리 - 채팅창
			case VK_RETURN:
				{
					const int nParam1_Activate = 1;

					if ( global.script->GetGlueGameEvent().OnGameEvent( "CHATTINGBOX", "ACTIVATEINPUT", nParam1_Activate, 0) == true)
						return true;
				}
				break;
			case VK_OEM_2:
				{
					const int nParam1_Activate_And_Add_Slash = 2;
					if ( global.script->GetGlueGameEvent().OnGameEvent( "CHATTINGBOX", "ACTIVATEINPUT", nParam1_Activate_And_Add_Slash, 0) == true)
						return true;
				}
				break;
			}

			break;
		}

	case MWM_MOUSEWHEEL:
		{
			if ( !XCONST::CAMERA_FIX)
			{
				int nDelta = e.nDelta;

				XBaseCameraSceneNode* pCameraNode = global.camera->GetCamera();

				XPolarCoord* pPolarCoord = pCameraNode->GetPolarCoord();

				float fSensibility = XCONST::MOUSE_WHEEL_SENSIBILITY;

				// 카메라가 가까워지면 좀 더 조금씩 줄어든다.
				if (pPolarCoord->fRadius < 70)
				{
					fSensibility = fSensibility * 0.1f;
				}
				else if (pPolarCoord->fRadius < 110)
				{
					fSensibility = fSensibility * 0.2f;
				}

				pPolarCoord->fRadius -= (float)nDelta * fSensibility;

				pPolarCoord->fRadius = max( XCONST::MIN_CAMERA_DIST, pPolarCoord->fRadius);
				pPolarCoord->fRadius = min( XCONST::MAX_CAMERA_DIST, pPolarCoord->fRadius);
				pCameraNode->SetPolarCoord( pPolarCoord);
			}
			break;
		}
	}
	

	return false;
}


void XGamePlayController::OnUpdate( float fDelta)
{
	XController::OnUpdate(fDelta);

	// 카메라 업데이트를 back-view모드 여부에 따라 나누어서 업데이트
	//UpdateCamera( fDelta);
	UpdateMouseMove(fDelta);

	// back-view모드가 아니면 여기서 카메라 업데이트, back-view모드이면 게임업데이트후 카메라 업데이트
	if (false == global.camera->IsPrimaryCameraType( CAMERA_MAIN_BACKVIEW))
		UpdateCamera(fDelta);

	// 마우스 클릭 활성화, 비활성화 업데이트
	UpdateMouseClickActivate();
}

void XGamePlayController::OnChat( CSMsgType nMsgType, const wstring& strMsg)
{	
/*
	wstring strHeader;

	switch ( nMsgType)
	{
	case MT_SYSTEM:				strHeader = "[시스템]";		break;
	case MT_SAY:				strHeader = "[지역]";		break;
	case MT_SHOUT:				strHeader = "[외치기]";		break;
	case MT_SHOUT_FAKE:			strHeader = "[외치기]";		break;
	case MT_GLOBAL:				strHeader = "[전체]";		break;
	case MT_WHISPER:			strHeader = "[귓말]";		break;
	case MT_GUILD:				strHeader = "[길드]";		break;
	case MT_PARTY:				strHeader = "[파티]";		break;	
	case MT_AUCTION:			strHeader = "[옥션]";		break;
	case MT_ANNOUNCE:			strHeader = "[운영자]";		break;
	case MT_NARRATION:			strHeader = "[나레이션]";	break;
	case MT_CHANNEL:			strHeader = "[채널]";		break;
	case MT_FIELD:				strHeader = "[필드]";		break;
	case MT_NOTICE:				strHeader = "[알림]";		break;	
	}
*/

	// 특수 문자 제거
	wstring strBuff = strMsg;
	size_t i;
	wstring _token;

	i = 0;
	_token = L"\n";
	while ( (i = strBuff.find( _token)) != wstring::npos)
		strBuff.replace( i, _token.size(), L"");

// 아래 주석 풀지마세요.
// 	i = 0;
// 	_token = L"{";
// 	while ( (i = strBuff.find( _token)) != wstring::npos)
// 		strBuff.replace( i, _token.size(), L"");
// 
// 	i = 0;
// 	_token = L"}";
// 	while ( (i = strBuff.find( _token)) != wstring::npos)
// 		strBuff.replace( i, _token.size(), L"");



	// 타입 재설정
	XMsgType _type;
	switch ( nMsgType)
	{
	case MT_SAY :			_type = MSGTYPE_GENERAL;	break;
	case MT_NARRATION :		_type = MSGTYPE_GENERAL;	break;
	case MT_SHOUT :			_type = MSGTYPE_SHOUTING;	break;
	case MT_SHOUT_FAKE :	_type = MSGTYPE_SHOUTING;	break;
	case MT_AUCTION :		_type = MSGTYPE_AUCTION;	break;
	case MT_WHISPER :		_type = MSGTYPE_WHISPER;	break;
	case MT_WHISPER_BACK :	_type = MSGTYPE_WHISPER;	break;
	case MT_PARTY :			_type = MSGTYPE_PARTY;		break;
	case MT_GUILD :			_type = MSGTYPE_GUILD;		break;
	case MT_CHANNEL :		_type = MSGTYPE_CHANNEL;	break;
	case MT_FIELD :			_type = MSGTYPE_FIELD;		break;
	case MT_GLOBAL :		_type = MSGTYPE_GENERAL;	break;
	case MT_SYSTEM :		_type = MSGTYPE_SYSTEM;		break;
	case MT_BATTLE :		_type = MSGTYPE_BATTLE;		break;
	case MT_ANNOUNCE :		_type = MSGTYPE_ANNOUNCE;	break;
	case MT_NOTICE :		_type = MSGTYPE_GM;			break;
	case MT_SOCIAL:			_type = MSGTYPE_SOCIAL;		break;
	default :				_type = MSGTYPE_GENERAL;	break;
	}

	if (global.script)		global.script->GetGlueGameEvent().OnGameEvent( "CHATTINGMSG", MLocale::ConvUTF16ToAnsi(strBuff.c_str()), (int)_type);
}

bool XGamePlayController::CheckEnableKey( XVirtualKey nVKey)
{
	if ( m_bEnable == false)
		return false;


	// 일부 키는 마우스가 보이는 상태에선 동작할 수 없다
	if ( nVKey == VKEY_ACTION  ||  nVKey == VKEY_ACTION2)
	{
		if ( global.ui->IsMouseVisible() == true)
			return false;
	}


	// 버프에 의한 액션/이동 금지 확인
	switch ( nVKey)
	{
	case VKEY_FORWARD :
	case VKEY_BACKWARD :
	case VKEY_LEFT :
	case VKEY_RIGHT :
	case VKEY_JUMP :
	case VKEY_CHANGE_STANCE :
	case VKEY_ACTION :
	case VKEY_ACTION2 :
	case VKEY_AUTO_RUN :
	case VKEY_CHANGE_WEAPON :
	case VKEY_NPCINTERACTION :
	case VKEY_PCINTERACTION :
	case VKEY_SIT :
	case VKEY_UI_PALETTE0 :
	case VKEY_UI_PALETTE1 :
	case VKEY_UI_PALETTE2 :
	case VKEY_UI_PALETTE3 :
	case VKEY_UI_PALETTE4 :
	case VKEY_UI_PALETTE5 :
	case VKEY_UI_PALETTE6 :
	case VKEY_UI_PALETTE7 :
	case VKEY_UI_PALETTE8 :
	case VKEY_UI_PALETTE9 :
		{
			if ( gvar.Game.pMyPlayer)
			{
				XBuffAttribute attrBuff = gvar.Game.pMyPlayer->GetModuleBuff()->GetBuffAttribute();
				if ( attrBuff.IsMesmerizable()  ||  attrBuff.IsMovable() == false)
					return false;
			}
		}
	}

	return true;
}

void XGamePlayController::UpdateCamera( float fDelta)
{
	int nDeltaX=0, nDeltaY=0;
	POINT pt;

	#define _USE_DINPUT
	#ifdef _USE_DINPUT
	global.ui->UpdateDInputMouse(&nDeltaX, &nDeltaY);
	#else
	global.ui->UpdateDInputMouse();
	#endif

	if ( global.app->IsActive()  &&  !global.ui->IsMouseVisible())
	{
		#ifdef _USE_DINPUT
		// by pok. DInput과 GetCursroPos(윈도우)의 기본 감도가 다른듯 하다.
		nDeltaX = int((float)nDeltaX * 1.5f);
		nDeltaY = int((float)nDeltaY * 1.5f);
		#else
		GetCursorPos( &pt);
		ScreenToClient( global.app->GetWindowHandle(), &pt);
		nDeltaX = pt.x - global.device->GetScreenWidth() / 2;
		nDeltaY = pt.y - global.device->GetScreenHeight() / 2;
		#endif
	}
	#ifdef _USE_DINPUT
	else
	{
		nDeltaX = 0;
		nDeltaY = 0;
	}
	#endif

	if ( (global.ui->IsMouseVisible() && m_bCameraRotationButtonDown == false)  ||  
		m_bActiveJustNow)
	{
		nDeltaX = 0;
		nDeltaY = 0;
	}

	m_bActiveJustNow = false;

	
	_UpdateCamera( fDelta, nDeltaX, nDeltaY);


	if ( global.app->IsActive()  &&  !global.ui->IsMouseVisible())
	{
		pt.x = global.device->GetScreenWidth() / 2;
		pt.y = global.device->GetScreenHeight() / 2;

		ClientToScreen( global.app->GetWindowHandle(), &pt);
		SetCursorPos( pt.x, pt.y);
	}


	XObject* pMyObject = gg.currentgamestate->GetObjectManager()->Find( XGetMyUID());
	if ( pMyObject)
	{
		if ( global.camera->IsPrimaryCameraType( CAMERA_MAIN_BACKVIEW))
		{			
			XBaseCameraSceneNode* pCameraSceneNode = global.camera->GetCamera();
			if( pCameraSceneNode  &&  pCameraSceneNode->GetPolarCoord()->fRadius < 50.0f)
				pMyObject->SetVisible( false);
			else
				pMyObject->SetVisible( true);
		}
	}
}


void XGamePlayController::_UpdateCamera( float fDelta, int nDeltaX, int nDeltaY )
{
	global.camera->UpdateCamera(fDelta, nDeltaX, nDeltaY, m_bFreeLookCamera);
}

void XGamePlayController::UpdateCameraAfterGameUpdate(float fDelta)
{
	if (global.camera->IsPrimaryCameraType( CAMERA_MAIN_BACKVIEW))
	{
		// CAMERA_MAIN_BACKVIEW 경우에만 UpdateCamera를 pCameraController->UpdateCamera()와 가깝게 하려고
		// 이곳에서 해주고 나머지 경우에는 기존과 동일하게 하기위해 위쪽에서 업데이트 해준다. by pok
		UpdateCamera(fDelta);

		// UpdateCamera에 이미 pCameraController->UpdateCamera() 가 포함되어 있다. by pok
		//XCameraBackViewController* pCameraController = global.camera->GetCameraController<XCameraBackViewController>();
		//if (pCameraController)
		//	pCameraController->UpdateCamera();
		return;
	}
	else
	{
		global.ui->UpdateDInputMouse();
	}
}

vec3 XGamePlayController::GetBackViewCameraDir()
{
	vec3 vCameraDir( 0.0f, 0.0f, 0.0f);

	XCameraBackViewController* pCameraController = global.camera->GetCameraController<XCameraBackViewController>();
	if (pCameraController)
		vCameraDir = pCameraController->GetObjectDir();

	return vCameraDir;
}


bool XGamePlayController::IsMainBackViewCamera()
{
	return (global.camera  &&  global.camera->IsPrimaryCameraType( CAMERA_MAIN_BACKVIEW));
}


void XGamePlayController::LimitDegreeOnFreeLook( float fTolerRad)
{
	if ( IsMainBackViewCamera() == false)
		return;

	XCameraBackViewController* pCameraNode = global.camera->GetCameraController<XCameraBackViewController>();
	if ( pCameraNode)
		pCameraNode->LimitDegreeOnFreeLook( fTolerRad);
}


void XGamePlayController::LimitDegreeOnFreeLook2( float fTolerRad)
{
	if ( IsMainBackViewCamera() == false)
		return;

	XCameraBackViewController* pCameraNode = global.camera->GetCameraController<XCameraBackViewController>();
	if ( pCameraNode)
		pCameraNode->LimitDegreeOnFreeLook2( fTolerRad);
}


void XGamePlayController::ReleaseLimitDegreeOnFreeLook()
{
	if ( IsMainBackViewCamera() == false)
		return;

	XCameraBackViewController* pCameraNode = global.camera->GetCameraController<XCameraBackViewController>();
	if ( pCameraNode)
		pCameraNode->ReleaseLimitDegreeOnFreeLook();
}

void XGamePlayController::SetBackViewCameraDir( const vec3& dir )
{
	XCameraBackViewController* pCameraController = global.camera->GetCameraController<XCameraBackViewController>();
	if (pCameraController)
		pCameraController->SetDirection(dir);
}

void XGamePlayController::UpdateMouseMove( float fDelta )
{
	if(m_bMouseMoveButtonDown == false)
		return;

	if(global.ui->IsMouseVisible() == false)
	{
		m_bMouseMoveButtonDown = false;
		return;
	}

	m_fMouseMoveCycleTime -= fDelta;

	if(m_fMouseMoveCycleTime <= 0.0f)
	{
		POINT pt;
		GetCursorPos( &pt);
		ScreenToClient( global.app->GetWindowHandle(), &pt);

		SendEventMouseMoveClick(pt.x, pt.y);

		m_fMouseMoveCycleTime = MOUSEMOVE_CYCLETIME;
	}
}

void XGamePlayController::SendEventMouseMoveClick( int x, int y )
{
	XEvent evtClick;

	MPoint point(x, y);
	evtClick.m_nID = XEVTL_MOUSE_MOVE_CLICK;
	evtClick.m_pData = &point;

	gg.omgr->PostEvent( XGetMyUID(), evtClick);
}

bool XGamePlayController::OnFoceEvent( MEvent& e )
{
	switch ( e.nMessage)
	{
	case MWM_LBUTTONUP:
		{
			m_bMouseMoveButtonDown = false;

			::ReleaseCapture();
		}
		break;
	}

	return false;
}

void XGamePlayController::UpdateMouseClickActivate()
{
	if(global.app->IsActive() == false)
		m_bMouseClickActivate = false;
}

bool XGamePlayController::IsCameraLockView()
{
	if ( IsMainBackViewCamera() == false)
		return false;

	XCameraBackViewController* pCameraNode = global.camera->GetCameraController<XCameraBackViewController>();
	if ( pCameraNode)
		return pCameraNode->IsCameraLock();

	return false;
}

bool XGamePlayController::IsCameraPvPFreeView()
{
	return (global.camera  &&  global.camera->IsPrimaryCameraType( CAMERA_PVP_FREEVIEW));
}
