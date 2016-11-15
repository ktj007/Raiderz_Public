#include "stdafx.h"
#include "XNetActionGesture.h"
#include "XModuleNetControl.h"
#include "XEventID.h"
#include "XModuleAction.h"

XNetActionGesture::XNetActionGesture( XModuleNetControl* pOwner) : XNetActionState( pOwner)
{

}

XNetActionGesture::~XNetActionGesture()
{

}

void XNetActionGesture::Enter( void* pParam /*=NULL*/)
{
	XNetActionState::Enter( pParam);

	wchar_t* szMotionName = (wchar_t*)pParam;
	XModuleMotion* pModuleMotion = m_pOwner->GetOwner()->GetModuleMotion();
	pModuleMotion->ChangeMotion( szMotionName, MT_DEFAULT, true);
	pModuleMotion->PushMotion( MOTION_NAME_IDLE);
}

void XNetActionGesture::Exit()
{
	XNetActionState::Exit();
}

XEventResult XNetActionGesture::Event( XEvent& msg)
{
	switch(msg.m_nID)
	{
	case XEVTL_MOTION_PLAYONCE:
		{
			m_pOwner->DoAction( NET_ACTION_STATE_NA);
		}
		break;
	}

	return XNetActionState::Event(msg);
}


void XNetActionGesture::Update( float fDelta)
{
	XNetActionState::Update(fDelta);

}


bool XNetActionGesture::CheckEnterable( void* pParam)
{
	return true;
}


bool XNetActionGesture::CheckExitable( int nNextStateID)
{
	return true;
}
