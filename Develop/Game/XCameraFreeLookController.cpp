#include "StdAfx.h"
#include "XCameraFreeLookController.h"
#include "RSpeedGrass.h"
#include "XCameraControllerGroup.h"
#include "XFreeCamTracer.h"


XCameraFreeLookController::XCameraFreeLookController(void)
: XCameraSceneNodeController( RCCT_PREUPDATE, XCameraTypeStr[CAMERA_FREE_LOOK], true )
, m_fMoveSpeed(200.0f)
, m_fRotateSpeed(0.005f)
{
	m_fCameraX = 1.6f;
	m_fCameraZ = .5f*MMath::PI;

	m_bAutoRun = false;
	m_bMaskAutoRunFlag = false;
}

XCameraFreeLookController::XCameraFreeLookController( RCAMERACONTROLLERTYPE eType /*= RCCT_PREUPDATE*/, const char* _pSzCameraControllerID /*= NULL*/, bool _bManagedByCamera /*= true*/ )
: XCameraSceneNodeController( eType, _pSzCameraControllerID, _bManagedByCamera )
, m_fMoveSpeed(200.0f)
, m_fRotateSpeed(0.005f)
{
	m_fCameraX = 1.6f;
	m_fCameraZ = .5f*MMath::PI;

	m_bAutoRun = false;
	m_bMaskAutoRunFlag = false;
}

XCameraFreeLookController::~XCameraFreeLookController(void)
{
}

void XCameraFreeLookController::Init()
{
	GetSceneNode()->SetTargetMUID(MUID::ZERO);

	m_pCameraSceneNode->SetShadowFar( 100000.0f);
	m_pCameraSceneNode->SetGrassNear( 0.0f);
	m_pCameraSceneNode->SetGrassFar( 100000.0f);

	if (global.smgr->GetCurrentTerrain())
	{
		global.smgr->GetCurrentTerrain()->SetGrassLodParams( m_pCameraSceneNode->GetGrassNear(), m_pCameraSceneNode->GetGrassFar());
	}
}

void XCameraFreeLookController::UpdateCamera( float fDelta, int nDeltaX, int nDeltaY )
{
	int iMouseDeltaX = 0, iMouseDeltaY = 0;

	m_fCameraZ += (-nDeltaX * m_fRotateSpeed);
	m_fCameraX += (nDeltaY * m_fRotateSpeed);
	m_fCameraX = max(0.01f, min(MMath::PI-0.01f, m_fCameraX));

	_UpdateCamera(fDelta);
}

void XCameraFreeLookController::_UpdateCamera( float fDelta )
{
	UpdateSceneNode();

	bool bEnable = true;
	if ( global.ui->IsMouseVisible() == true)
		bEnable = false;

	float fRotateStep= fDelta;

	float fMoveStep= fDelta * m_fMoveSpeed;

	if ( bEnable == true)
	{
		if ( MEvent::GetShiftState())			fMoveStep *= 8.0f;
		if ( MEvent::IsKeyDown( VK_LEFT))		m_fCameraZ += fRotateStep;
		if ( MEvent::IsKeyDown( VK_RIGHT))		m_fCameraZ -= fRotateStep;
		if ( MEvent::IsKeyDown( VK_UP))			m_fCameraX -= fRotateStep;
		if ( MEvent::IsKeyDown( VK_DOWN))		m_fCameraX += fRotateStep;
	}

	vec3 newDirection;
	if ( XGetFreeCamTracer()->IsRun() == true)
		newDirection = m_pCameraSceneNode->GetDirection();
	else
		newDirection= vec3( cosf( m_fCameraZ) * sinf( m_fCameraX), sinf( m_fCameraZ) * sinf( m_fCameraX), cosf( m_fCameraX));

	//mlog("üũ x = %f z = %f\n", m_fCameraX, m_fCameraZ);

	vec3 dir = newDirection;
	vec3 right = vec3( 0, 0, 1).CrossProduct( dir);
	right.Normalize();

	vec3 posDiff( 0, 0, 0);
	if ( bEnable == true)
	{
		if ( MEvent::IsKeyDown( 'W')  ||  m_bAutoRun)	posDiff += fMoveStep * dir;
		if ( MEvent::IsKeyDown( 'S'))					posDiff -= fMoveStep * dir;
		if ( MEvent::IsKeyDown( 'A'))					posDiff += fMoveStep * right;
		if ( MEvent::IsKeyDown( 'D'))					posDiff -= fMoveStep * right;
		if ( MEvent::IsKeyDown( VK_SPACE))				posDiff += fMoveStep * vec3( 0, 0, 1);
	}
	else
	{
		if ( m_bAutoRun == true)						posDiff += fMoveStep*dir;
	}

	vec3 newPosition = m_pCameraSceneNode->GetPosition() + posDiff;

	if ( bEnable == true)
	{
		if ( MEvent::IsKeyDown( 'R')  &&  m_bMaskAutoRunFlag == false)
		{
			m_bAutoRun = !m_bAutoRun;
			m_bMaskAutoRunFlag = true;
		}
		else if ( MEvent::IsKeyDown('R') == false  &&  m_bMaskAutoRunFlag)
			m_bMaskAutoRunFlag = false;
	}

	m_pCameraSceneNode->Set( newPosition, newDirection);
}