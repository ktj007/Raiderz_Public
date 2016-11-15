#include "stdafx.h"
#include "XNetActionSit.h"
#include "XModuleNetControl.h"
#include "XEventID.h"
#include "XModuleAction.h"

XNetActionSit::XNetActionSit( XModuleNetControl* pOwner) : XNetActionState( pOwner)
{

}

XNetActionSit::~XNetActionSit()
{

}

void XNetActionSit::Enter( void* pParam /*=NULL*/)
{
	XNetActionState::Enter( pParam);

	m_pOwner->GetOwner()->GetModuleAction()->SitDown();
}

void XNetActionSit::Exit()
{
	XNetActionState::Exit();
}

XEventResult XNetActionSit::Event( XEvent& msg)
{
	switch(msg.m_nID)
	{
	case XEVTL_SIT_RISE:
		{
			m_pOwner->GetOwner()->GetModuleAction()->SitRise();
			m_pOwner->DoAction( NET_ACTION_STATE_NA);
		}
		break;
	case XEVTD_TALENT_HIT:
		{
			m_pOwner->GetOwner()->GetModuleAction()->Idle();
			m_pOwner->DoAction( NET_ACTION_STATE_NA);
		}
		break;
	}

	return XNetActionState::Event(msg);
}


void XNetActionSit::Update( float fDelta)
{
	XNetActionState::Update(fDelta);

}


bool XNetActionSit::CheckEnterable( void* pParam)
{
	return true;
}


bool XNetActionSit::CheckExitable( int nNextStateID)
{
	return true;
}
