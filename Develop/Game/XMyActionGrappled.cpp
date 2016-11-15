#include "stdafx.h"
#include "XMyActionGrappled.h"
#include "XModuleMyControl.h"
#include "XCameraManager.h"
#include "XEventID.h"
#include "XGame.h"
#include "XWorld.h"



XMyActionGrappled::XMyActionGrappled( XModuleMyControl* pOwner) : XMyActionState( pOwner)
{
	m_Grappled.SetOwnerPlayer( GetOwnerPlayer());
}


XMyActionGrappled::~XMyActionGrappled()
{
	m_Grappled.OnFinish();
}


void XMyActionGrappled::Enter( void* pParam /*=NULL*/)
{
	XMyActionState::Enter( pParam);

	m_Grappled.OnStart( static_cast<_DamageRepeatInfo*>( pParam));

	_SetCamera();
}


void XMyActionGrappled::Exit()
{
	m_Grappled.OnFinish();

	_ReleaseCamera();

	XMyActionState::Exit();
}


XEventResult XMyActionGrappled::Event( XEvent& msg)
{
	switch ( msg.m_nID)
	{
	case XEVTL_GRAPPLED_NEW_START:
		m_Grappled.OnStart( static_cast<_DamageRepeatInfo*>( msg.m_pData));
		_SetCamera();
		break;

	case XEVTL_THROW:
		m_Grappled.OnBeThrown();
		break;
	}

	return MR_FALSE;
}


void XMyActionGrappled::Update( float fDelta)
{
	m_Grappled.OnUpdate( fDelta);


	if ( m_Grappled.IsFinish() == true)
	{
		if ( GetOwnerPlayer()->IsDead() == true)
			m_pOwner->DoAction( ACTION_STATE_DIE);

		else
		{
			float height = gg.currentgamestate->GetWorld()->GetGroundZ( GetOwnerPlayer()->GetPosition()) + 150.0f;

			if ( GetOwnerPlayer()->GetPosition().z > height)
			{
				m_pOwner->DoAction( ACTION_STATE_FALLING);
			}
			else
				m_pOwner->DoAction( ACTION_STATE_IDLE);
		}
	}
}


bool XMyActionGrappled::CheckEnterable( void* pParam)
{
	m_pOwner->DoForceCameraLock();
	return true;
}


bool XMyActionGrappled::CheckExitable( int nNextStateID)
{
	if ( m_Grappled.IsFinish() == false)
		return false;

	return true;
}


void XMyActionGrappled::_SetCamera()
{
/*	return;


	아직 사용하지 않습니다.
*/

	if ( !global.camera->GetPrimaryCamera())
		return;


	RCameraSceneNode* pNextCameraSceneNode = NULL;

	if ( global.camera->IsPrimaryCameraType( CAMERA_MAIN_BACKVIEW))
	{
		global.camera->SetPrimaryCamera( CAMERA_GRAPPLED);
		XCameraGrappledController* pGrappledCamera = global.camera->GetCameraController<XCameraGrappledController>();
		if (pGrappledCamera)
		{
			pGrappledCamera->Init( XGetMyUID());
		}
	}
}


void XMyActionGrappled::_ReleaseCamera()
{
/*	return;


	아직 사용하지 않습니다.
*/

	if ( !global.camera->GetPrimaryCamera())
		return;

	if ( global.camera->IsPrimaryCameraType( CAMERA_GRAPPLED))
	{
		XCameraGrappledController* pGrappledCamera = global.camera->GetCameraController<XCameraGrappledController>();
		if (pGrappledCamera)
		{
			pGrappledCamera->Release();
		}
	}
}

void XMyActionGrappled::PostUpdate( float fDelta )
{
	m_Grappled.OnPostUpdate(fDelta);
}