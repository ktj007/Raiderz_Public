#include "StdAfx.h"
#include "XCameraSceneNodeController.h"

XCameraSceneNodeController::XCameraSceneNodeController( RCAMERACONTROLLERTYPE eType /*= RCCT_PREUPDATE*/, const char* _pSzCameraControllerID /*= NULL*/, bool _bManagedByCamera /*= true*/ )
: RCameraSceneNodeController( RCCT_PREUPDATE, _pSzCameraControllerID, _bManagedByCamera )
, m_uidObject(MUID::ZERO)
{

}

XCameraSceneNodeController::~XCameraSceneNodeController(void)
{
}

void XCameraSceneNodeController::UpdateSceneNode()
{
	if (m_pCameraSceneNode)
	{
		RSceneNodeUpdateVisitor v;
		m_pCameraSceneNode->Traverse( &v );
	}
}