#include "stdafx.h"
#include "XMyActionSwallowed.h"
#include "XModuleMyControl.h"
#include "XCameraManager.h"
#include "XEventID.h"
#include "XGame.h"
#include "XWorld.h"



XMyActionSwallowed::XMyActionSwallowed( XModuleMyControl* pOwner) : XMyActionState( pOwner)
{
	m_Eaten.SetOwnerPlayer( GetOwnerPlayer());
}


XMyActionSwallowed::~XMyActionSwallowed()
{
	m_Eaten.OnFinish();
}


void XMyActionSwallowed::Enter( void* pParam /*=NULL*/)
{
	XMyActionState::Enter( pParam);

	m_Eaten.OnStart( static_cast<_DamageRepeatInfo*>( pParam));

//	_SetCamera();
}


void XMyActionSwallowed::Exit()
{
	m_Eaten.OnFinish();

//	_ReleaseCamera();

	XMyActionState::Exit();
}


XEventResult XMyActionSwallowed::Event( XEvent& msg)
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


void XMyActionSwallowed::Update( float fDelta)
{
	m_Eaten.OnUpdate( fDelta);


	if ( m_Eaten.IsFinish() == true)
	{
		if ( GetOwnerPlayer()->IsDead() == true)
			m_pOwner->DoAction( ACTION_STATE_DIE);

		else
		{
			float height = gg.currentgamestate->GetWorld()->GetGroundZ( GetOwnerPlayer()->GetPosition()) + 150.0f;

			if ( GetOwnerPlayer()->GetPosition() > height)
				m_pOwner->DoAction( ACTION_STATE_FALLING);
			else
				m_pOwner->DoAction( ACTION_STATE_IDLE);
		}
	}
}


bool XMyActionSwallowed::CheckEnterable( void* pParam)
{
	return true;
}


bool XMyActionSwallowed::CheckExitable( int nNextStateID)
{
	if ( m_Eaten.IsFinish() == false)
		return false;

	return true;
}


void XMyActionSwallowed::_SetCamera()
{
/*	return;


	아직 사용하지 않습니다.
*/

	//if ( !global.camera->GetPrimaryCamera())
	//	return;


	//RCameraSceneNode* pNextCameraSceneNode = NULL;

	//if ( global.camera->IsPrimaryCameraType( CAMERA_MAIN_BACKVIEW))
	//{
	//	XBackViewCameraSceneNode* pBackViewCameraNode = global.camera->GetCamera<XBackViewCameraSceneNode>();
	//	XPolarCoord coord = pBackViewCameraNode->GetPolarCoord();

	//	pNextCameraSceneNode = global.camera->SetPrimaryCamera( CAMERA_GRAPPLED);
	//	if ( pNextCameraSceneNode)
	//	{
	//		XGrappledCameraSceneNode* pGrappledCamera = global.camera->GetCamera<XGrappledCameraSceneNode>();
	//		pGrappledCamera->Init( XGetMyUID(), coord);
	//	}
	//}
}


void XMyActionSwallowed::_ReleaseCamera()
{
/*	return;


	아직 사용하지 않습니다.
*/

	//if ( !global.camera->GetPrimaryCamera())
	//	return;


	//RCameraSceneNode* pNextCameraSceneNode = NULL;

	//if ( global.camera->IsPrimaryCameraType( CAMERA_GRAPPLED))
	//{
	//	XGrappledCameraSceneNode* pGrappledCamera = global.camera->GetCamera<XGrappledCameraSceneNode>();
	//	pGrappledCamera->Release();
	//}
}

void XMyActionSwallowed::PostUpdate( float fDelta )
{
	m_Eaten.OnPostUpdate(fDelta);
}