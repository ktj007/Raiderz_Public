#include "stdafx.h"
#include "XMyActionGesture.h"
#include "XModuleMyControl.h"
#include "XModuleAction.h"

XMyActionGesture::XMyActionGesture( XModuleMyControl* pOwner )
: XMyActionState(pOwner)
, m_bCancelableGesture(false)
{
}

void XMyActionGesture::Enter( void* pParam/*=NULL*/ )
{
	// Á¤Áö
	XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
	if ( pModuleAction)
	{
		pModuleAction->StopMove();
		if(pModuleAction->IsStun())	pModuleAction->EndStun();
	}

	wstring strMotionName = (wchar_t*)pParam;
	static wstring strCancelableGesture(L"_cancelable_gesture_");
	if (strMotionName.find(strCancelableGesture) == 0)
	{
		m_bCancelableGesture = true;
		strMotionName = strMotionName.substr(strCancelableGesture.length());
	}
	else
	{
		m_bCancelableGesture = false;
	}

	XModuleMotion* pModuleMotion = m_pOwner->GetOwner()->GetModuleMotion();
	pModuleMotion->ChangeMotion( strMotionName.c_str(), MT_DEFAULT, true);
}

XEventResult XMyActionGesture::Event(XEvent& msg)
{
	switch(msg.m_nID)
	{
	case XEVTL_MOTION_PLAYONCE:
		{
			m_bCancelableGesture = true;
			m_pOwner->DoActionIdle();
			return MR_TRUE;
		}
		break;
	case XEVTL_VIRTUAL_KEY_PRESSED:
	case XEVTL_VIRTUAL_KEY_PRESSEDONCE:
	case XEVTL_VIRTUAL_KEY_RELEASED:
		{
			if (m_bCancelableGesture)
				m_pOwner->DoActionIdle();

			return MR_TRUE;
		}
		break;
	};

	return XMyActionState::Event(msg);
}

bool XMyActionGesture::CheckExitable( int nNextStateID )
{
	if (nNextStateID == ACTION_STATE_DIE) return true;
	if (nNextStateID == ACTION_STATE_HIT) return true;

	return m_bCancelableGesture;
}