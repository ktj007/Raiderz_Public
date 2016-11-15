#include "stdafx.h"
#include "XCutSceneController.h"
#include "XCutSceneState.h"
#include "XCameraManager.h"

XCutSceneController::XCutSceneController(XCutSceneState* pState)
: XController(), m_pCutSceneState(pState)
{
}


XCutSceneController::~XCutSceneController()
{
	Destroy();
}

void XCutSceneController::OnUpdate( float fDelta )
{
	XController::OnUpdate(fDelta);

	if (global.camera && global.camera->GetPrimaryCamera())
	{
		RSceneNodeUpdateVisitor v;
		global.camera->GetPrimaryCamera()->Traverse( &v );
	}
}

bool XCutSceneController::OnEvent( MEvent& e)
{
	if(m_pCutSceneState->OnEvent(e))
		return true;

	return false;
}
