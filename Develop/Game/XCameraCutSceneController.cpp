#include "StdAfx.h"
#include "XCameraCutSceneController.h"
#include "RSpeedGrass.h"

#include "XCameraControllerGroup.h"

XCameraCutSceneController::XCameraCutSceneController(void)
: XCameraSceneNodeController( RCCT_PREUPDATE, XCameraTypeStr[CAMERA_CUT_SCENE], true )
{
}

XCameraCutSceneController::~XCameraCutSceneController(void)
{
}

void XCameraCutSceneController::UpdateCameraController( RMatrix& _rMatApplyToCamera )
{
	UpdateSceneNode();
}

void XCameraCutSceneController::Init( vec3& vPos, vec3& vDir )
{
	GetSceneNode()->SetTargetMUID(MUID::ZERO);

	m_pCameraSceneNode->SetPosition(vPos);
	m_pCameraSceneNode->SetDirection(vDir);

	if ( global.smgr != NULL  &&  global.smgr->GetCurrentTerrain())
	{
		int _range = XGETCFG_VIDEO_GRNDOBJVISIBLERANGE;
		global.smgr->GetCurrentTerrain()->SetGrassLodParams( _range, _range + 2000);
		global.smgr->GetCurrentTerrain()->SetDetailDistance( _range * 3);
	}
}