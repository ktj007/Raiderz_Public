#include "stdafx.h"
#include "XMyActionHold.h"
#include "XEventID.h"
#include "XController.h"
#include "XModuleMyControl.h"
#include "XPost_Interaction.h"

XEventResult XMyActionHold::Event( XEvent& msg )
{
	switch (msg.m_nID)
	{
	//case XEVTL_HOLD_KEYABLE:
	//	{
	//		// TODO : 인터랙션커맨드에 대한 응답이 오면 키프레스가 먹도록.
	//		m_bKeyPress = true;
	//	}
	//	break;
	case XEVTL_VIRTUAL_KEY_PRESSEDONCE:
		{
			//if(!m_bKeyPress)
			//{
			//	m_bKeyPress = true;
			//	return MR_FALSE;
			//}

			XVirtualKey nVirtualKey = *static_cast<XVirtualKey*>(msg.m_pData);
			switch (nVirtualKey)
			{
			//case VKEY_ACTION:
			case VKEY_FORWARD:
			case VKEY_BACKWARD:
			case VKEY_LEFT:
			case VKEY_RIGHT:
				m_pOwner->DoActionIdle();
				return MR_TRUE;
			}
		}
		break;
	case XEVTL_MYACTION_RELEASE_ACTION_HOLD:
		{
			m_pOwner->DoActionIdle();
			return MR_TRUE;
		}
		break;
	}

	return MR_FALSE;
}

void XMyActionHold::Enter( void* pParam/*=NULL*/ )
{
	m_pOwner->StopRun();

	if ( global.script)		global.script->GetGlueGameEvent().OnGameEvent( "ACTION", "INTERACTION", 1);
}

void XMyActionHold::Exit()
{
	if ( global.script)		global.script->GetGlueGameEvent().OnGameEvent( "ACTION", "INTERACTION", 0);
}

void XMyActionHold::Update( float fDelta )
{
}
