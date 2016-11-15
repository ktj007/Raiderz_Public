#include "stdafx.h"
#include "XMyActionSit.h"
#include "XModuleAction.h"
#include "XModuleMyControl.h"
#include "XPost_Action.h"
#include "XEvent.h"
#include "XEventID.h"
#include "XMyPlayer.h"
#include "XMotionHelper.h"
#include "XMyActionIdle.h"
#include "XCameraManager.h"
#include "XStrings.h"
#include "XInteractionInfo.h"

void XMyActionSit::Enter( void* pParam/*=NULL*/ )
{
	XGetInteractionInfo().SetInteractionSearchUID(MUID::ZERO);

	// 정지
	XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
	if ( pModuleAction)
		pModuleAction->StopMove();

	m_bStandUpKeyDown = false;
	m_bSitDown	= false;
	GetOwnerPlayer()->SetSitting(true);
	ActionSitDown();
	m_pOwner->InitAutoMove();
}

void XMyActionSit::Update( float fDelta )
{
	XModuleMotion* pModuleMotion = m_pOwner->GetOwner()->GetModuleMotion();
	if (pModuleMotion)
	{
		if (XMotionHelper::IsIdleMotion(pModuleMotion->GetCurrMotion()))
		{
			m_pOwner->DoActionIdle();
		}
	}
}

void XMyActionSit::Exit()
{
	if (m_bStandUpKeyDown == false)	ActionSitRise();

	//ClearPvPFreeViewCamera();
	GetOwnerPlayer()->SetSitting(false);
}

void XMyActionSit::ActionSitDown()
{
	SetPvPFreeViewCamera();

	XPostSit();
	m_pOwner->GetOwner()->GetModuleAction()->SitDown();

}

void XMyActionSit::ActionSitRise()
{
	ClearPvPFreeViewCamera();

	XPostSitRise();
	m_pOwner->GetOwner()->GetModuleAction()->SitRise();
}

XEventResult XMyActionSit::Event(XEvent& msg)
{
	switch(msg.m_nID)
	{
	case XEVTL_MOTION_PLAYONCE:
		{
			m_bSitDown = true;
		}
		break;
	case XEVTD_TALENT_HIT:
		{
			m_pOwner->DoActionIdle();
		}
		break;
	case XEVTL_VIRTUAL_KEY_PRESSED:
	case XEVTL_VIRTUAL_KEY_PRESSEDONCE:
		{
			XVirtualKey nVirtualKey = *static_cast<XVirtualKey*>(msg.m_pData);
			switch (nVirtualKey)
			{
			case VKEY_FORWARD:
			case VKEY_BACKWARD:
			case VKEY_LEFT:
			case VKEY_RIGHT:
			case VKEY_SIT:
			case VKEY_ACTION:
				{
					if ( CheckVirtualKeyForPvPFreeView(nVirtualKey) && m_bSitDown && !m_bStandUpKeyDown)
					{
						ActionSitRise();
						m_bStandUpKeyDown = true;
					}
					return MR_TRUE;
				}
				break;
			case VKEY_AUTO_RUN:
				{
					return MR_TRUE;
				}
				break;
			}
		}
		break;
	case XEVTL_SIT_RISE:
	case XEVTL_MOUSE_MOVE_CLICK:
		{
			ActionSitRise();
			m_bStandUpKeyDown = true;
		}
		break;
	case XEVTL_UPPER_MOTION_PLAYONCE:
		{
			XMyActionIdleParam idleParam;
			idleParam.bSetIdleAnimation = false;
			m_pOwner->DoAction(ACTION_STATE_IDLE, &idleParam);
			return MR_TRUE;
		}
		break;

	case XEVTL_CAMERA_PVP_VIEW_CLEAR:
		{
			ClearPvPFreeViewCamera();
			return MR_TRUE;
		}
		break;
	};

	return XMyActionState::Event(msg);
}

bool XMyActionSit::CheckAnimationRise()
{
	XModuleMotion* pModuleMotion = m_pOwner->GetOwner()->GetModuleMotion();
	if (pModuleMotion)
	{
		if(pModuleMotion->GetCurrMotion() == MOTION_NAME_SIT_RISE)
			return true;
	}

	return false;	
}

bool XMyActionSit::CheckExitable( int nNextStateID )
{
	if(CheckAnimationRise() &&
		(nNextStateID == ACTION_STATE_IDLE ||
		nNextStateID == ACTION_STATE_HOLD))
	{
		// 애니메이션이 끝나면 알아서 idle로 될거다.
		return false;
	}

	return true;
}

bool XMyActionSit::CheckEnterable( void* pParam )
{
	bool bForce = false;
	if(pParam != NULL)
	{
		bForce = *static_cast<bool*>(pParam);
		if(bForce)
			return true;
	}

	if ( XGETCFG_GAME_COMBATSTANCESITTING == false)
	{
		if (GetOwnerPlayer()->GetStance() != CS_NORMAL)
		{
			return false;
		}
	}

	// 브레이커블파츠 무기 사용중이어서 사용 가능한 액션인가?
	if(m_pOwner->UsableActionState_OverlappedEquipmentItem(GetID()) == false)
		return false;

	return true;
}

bool XMyActionSit::CheckVirtualKeyForPvPFreeView( XVirtualKey nVirtualKey )
{
	if(m_bPvPFreeView == false)
		return true;

	switch (nVirtualKey)
	{
	case VKEY_SIT:
		return true;
	}

	return false;
}

void XMyActionSit::ClearPvPFreeViewCamera()
{
	if(global.camera && global.camera->IsPrimaryCameraType(CAMERA_PVP_FREEVIEW))
	{
		m_bPvPFreeView = false;
		XCameraPVPFreeViewController* pPvPViewCamera = global.camera->GetCameraController<XCameraPVPFreeViewController>();
		if(pPvPViewCamera)
			pPvPViewCamera->End();
	}
}

void XMyActionSit::SetPvPFreeViewCamera()
{
	// 이벤트 PVP 관람용 프리카메라 셋팅(http://dev/redmine/issues/16031)
	if(global.camera && gvar.World.GetFieldID() == 8005)
	{
		global.ui->OnPresentationUpper( XGetStr( L"MSG_CANMOVECAMERABYKEYS"));

		m_bPvPFreeView = true;
		global.camera->SetPrimaryCamera(CAMERA_PVP_FREEVIEW);
	}
}
