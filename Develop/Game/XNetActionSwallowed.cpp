#include "stdafx.h"
#include "XNetActionSwallowed.h"
#include "XModuleNetControl.h"
#include "XCameraManager.h"
#include "XEventID.h"
#include "XGame.h"
#include "XWorld.h"



XNetActionSwallowed::XNetActionSwallowed( XModuleNetControl* pOwner) : XNetActionState( pOwner)
{
	m_Eaten.SetOwnerPlayer( GetOwnerPlayer());
}


XNetActionSwallowed::~XNetActionSwallowed()
{
	m_Eaten.OnFinish();
}


void XNetActionSwallowed::Enter( void* pParam /*=NULL*/)
{
	XNetActionState::Enter( pParam);

	m_Eaten.OnStart( static_cast<_DamageRepeatInfo*>( pParam));
}


void XNetActionSwallowed::Exit()
{
	m_Eaten.OnFinish();

	XNetActionState::Exit();
}


XEventResult XNetActionSwallowed::Event( XEvent& msg)
{
	switch ( msg.m_nID)
	{
	case XEVTL_SWALLOWED_NEW_START:
		m_Eaten.OnStart( static_cast<_DamageRepeatInfo*>( msg.m_pData));
		break;
	case XEVTL_SWALLOWED_DISAPPEAR:
		m_Eaten.OnDisAppear();
		break;

	case XEVTL_THROW:
		m_Eaten.OnAppear();
		m_Eaten.OnBeThrown();
		break;
	}

	return MR_FALSE;
}


void XNetActionSwallowed::Update( float fDelta)
{
	m_Eaten.OnUpdate( fDelta);


	if ( m_Eaten.IsFinish() == true)
	{
		m_pOwner->DoAction( NET_ACTION_STATE_NA);
	}
}


bool XNetActionSwallowed::CheckEnterable( void* pParam)
{
	return true;
}


bool XNetActionSwallowed::CheckExitable( int nNextStateID)
{
	if ( m_Eaten.IsFinish() == false)
		return false;

	return true;
}

void XNetActionSwallowed::PostUpdate( float fDelta )
{
	m_Eaten.OnPostUpdate(fDelta);
}
