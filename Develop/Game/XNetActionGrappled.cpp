#include "stdafx.h"
#include "XNetActionGrappled.h"
#include "XModuleNetControl.h"
#include "XEventID.h"



XNetActionGrappled::XNetActionGrappled( XModuleNetControl* pOwner) : XNetActionState( pOwner)
{
	m_Grappled.SetOwnerPlayer( GetOwnerPlayer());
}


XNetActionGrappled::~XNetActionGrappled()
{
	m_Grappled.OnFinish();
}


void XNetActionGrappled::Enter( void* pParam /*=NULL*/)
{
	XNetActionState::Enter( pParam);

	m_Grappled.OnStart( static_cast<_DamageRepeatInfo*>( pParam));
}


void XNetActionGrappled::Exit()
{
	m_Grappled.OnFinish();

	XNetActionState::Exit();
}


XEventResult XNetActionGrappled::Event( XEvent& msg)
{
	switch ( msg.m_nID)
	{
	case XEVTL_GRAPPLED_NEW_START:
		m_Grappled.OnStart( static_cast<_DamageRepeatInfo*>( msg.m_pData));
		break;

	case XEVTL_THROW:
		m_Grappled.OnBeThrown();
		break;
	}

	return MR_FALSE;
}


void XNetActionGrappled::Update( float fDelta)
{
	m_Grappled.OnUpdate( fDelta);


	if ( m_Grappled.IsFinish() == true)
	{
		m_pOwner->DoAction( NET_ACTION_STATE_NA);
	}
}


bool XNetActionGrappled::CheckEnterable( void* pParam)
{
	return true;
}


bool XNetActionGrappled::CheckExitable( int nNextStateID)
{
	if ( m_Grappled.IsFinish() == false)
		return false;

	return true;
}

void XNetActionGrappled::PostUpdate( float fDelta )
{
	m_Grappled.OnPostUpdate(fDelta);
}