#include "StdAfx.h"
#include "XCameraPVPFreeViewController.h"
#include "XCameraControllerGroup.h"
#include "XCameraManager.h"

XCameraPVPFreeViewController::XCameraPVPFreeViewController(void)
: XCameraFreeLookController( RCCT_PREUPDATE, XCameraTypeStr[CAMERA_PVP_FREEVIEW], true )
{
	m_eState = FVS_NONE;
}

XCameraPVPFreeViewController::XCameraPVPFreeViewController( RCAMERACONTROLLERTYPE eType /*= RCCT_PREUPDATE*/, const char* _pSzCameraControllerID /*= NULL*/, bool _bManagedByCamera /*= true*/ )
: XCameraFreeLookController( eType, _pSzCameraControllerID, _bManagedByCamera )
{
	m_eState = FVS_NONE;
}

XCameraPVPFreeViewController::~XCameraPVPFreeViewController(void)
{
}

void XCameraPVPFreeViewController::Init()
{
	m_moveCollision.SetRadius(100.0f);
	m_moveCollision.SetMoveAllowedHeight(0.0f);

	// 시작위치...
	m_pCameraSceneNode->SetPosition(vec3(5990, 6023, 250));
}

void XCameraPVPFreeViewController::UpdateCamera( float fDelta, int nDeltaX, int nDeltaY )
{
	m_bAutoRun = false;
	m_bMaskAutoRunFlag = false;

	//if(m_eState == FVS_START || m_eState == FVS_END)
	//{
	//	UpdateStartEndPVPFreeViewCamera(fDelta);
	//	return;
	//}
	
	// 이전 데이터 값 보관
	vec3 vFrom = m_pCameraSceneNode->GetPosition();

	// 업데이트
	XCameraFreeLookController::UpdateCamera(fDelta, nDeltaX, nDeltaY);

	// 높이 제한
	vec3 vResultPos = m_pCameraSceneNode->GetPosition();
	if(vResultPos.z >= 2600.0f)
		vResultPos.z = 2600.0f;

	// 충돌 처리
	MCapsule colCapsule;
	colCapsule.top = colCapsule.bottom = vResultPos;
	colCapsule.top.z += 10.0f;
	colCapsule.bottom.z -= 10.0f;
	colCapsule.radius = 100.0f;
	colCapsule.CalcHeight();

	vec3 vOut = vResultPos;
	vec3 vOutNor = vec3::AXISZ;
	vec3 vTo = vOut;

	m_moveCollision.FreeMoveTest(vFrom, vTo, vFrom, colCapsule, vOut, vOutNor);
	m_pCameraSceneNode->SetPosition(vOut);
}

void XCameraPVPFreeViewController::Start( vec3 vOwnerPos )
{
	vec3 vStartPos = vOwnerPos + vec3(0, 0, XCONST::CAMERA_TARGET_HEIGHT);


	m_eState = FVS_START;
}

void XCameraPVPFreeViewController::UpdateStartEndPVPFreeViewCamera( float fDelta )
{

}

void XCameraPVPFreeViewController::End()
{
	if(global.camera)
		global.camera->SetPrimaryCamera(CAMERA_MAIN_BACKVIEW);
}
