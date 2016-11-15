#include "stdafx.h"
#include "XMyActionChangeChannel.h"
#include "XPost_Channel.h"
#include "XModuleMyControl.h"
#include "XModuleAction.h"
#include "XEventID.h"
#include "XStrings.h"

bool XMyActionChangeChannel::CheckEnterable( void* pParam )
{
	XModuleAction* pModuleAction = m_pOwner->GetOwner()->GetModuleAction();
	if (pModuleAction)
	{
		pModuleAction->Stop();
	}

	return true;
}

void XMyActionChangeChannel::Enter( void* pParam/*=NULL*/ )
{
	m_Timer.SetTickTime(CHANNEL_CHANGE_PREPARE_TIME);
	m_Timer.Start();
}

void XMyActionChangeChannel::Update( float fDelta )
{
	if (m_Timer.IsReady(fDelta))
	{
		m_Timer.Stop();

// 		if (global.script)
// 			global.script->GetGlueGameEvent().OnChangeChannelRequireWindowHide();

		XPostChannel_ChangeChannel();

		m_pOwner->DoActionIdleWithSpecialState();
	}
	else
	{
//		int sec = ((int)m_Timer.GetElapsedTime()) % 60;
//
// 		if (global.script)
// 			global.script->GetGlueGameEvent().OnChangeChannelRequireWindowShow(CHANNEL_CHANGE_PREPARE_TIME - sec);
	}
}

void XMyActionChangeChannel::Exit()
{
	m_Timer.Stop();

	if ( global.script)		global.script->GetGlueGameEvent().OnGameEvent( "CHANNEL", "CHANGED", -1);
}

XEventResult XMyActionChangeChannel::Event( XEvent& msg )
{
	switch (msg.m_nID)
	{
	case XEVTL_VIRTUAL_KEY_PRESSED:
	case XEVTL_VIRTUAL_KEY_PRESSEDONCE:
	case XEVTL_VIRTUAL_KEY_RELEASED:
		{
			XVirtualKey nVirtualKey = *static_cast<XVirtualKey*>(msg.m_pData);
			if ( nVirtualKey > VKEY_NA  &&  nVirtualKey < VKEY_COUNT)
				return MR_TRUE;
		}
		break;
	case XEVTD_TALENT_HIT:
		{
			global.ui->OnSystemMsg( XGetStr( L"CHANNEL_CANCELLED"));

			XPostChannel_ChangeChannelCancel();
			m_pOwner->DoActionIdle();
		}
		break;
	}

	return XMyActionState::Event(msg);
}
