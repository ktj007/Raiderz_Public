#include "StdAfx.h"
#include "XCameraGrappledController.h"
#include "XModuleEntity.h"
#include "XCameraManager.h"

XCameraGrappledController::XCameraGrappledController(void)
: XCameraSceneNodeController(RCCT_PREUPDATE, XCameraTypeStr[CAMERA_GRAPPLED], true )
{
	m_nLastTime = 0;
	m_fInitialRadius = 0.0f;
}

XCameraGrappledController::~XCameraGrappledController(void)
{
}

void XCameraGrappledController::UpdateCameraController( RMatrix& _rMatApplyToCamera )
{

}

void XCameraGrappledController::_UpdateCamera( float fDelta )
{
	UpdateSceneNode();

	if (gg.omgr == NULL) return;
	XObject* pObject = gg.omgr->Find(m_uidTarget);
	if (pObject == NULL) return;

	XModuleEntity* pModuleEntity = pObject->GetModuleEntity();
	if (pModuleEntity == NULL) return;

	XPolarCoord* pPolarCoord = GetPolarCoord();

	vec3 tar = pModuleEntity->GetPosition();
	//	tar.z += XCONST::CAMERA_TARGET_HEIGHT;
	tar.z += XCONST::CAMERA_TARGET_HEIGHT * 0.25f;

	float fRadius = pPolarCoord->fRadius;
	float fTheta = pPolarCoord->fTheta;

	if (pPolarCoord->fRadius <= 0.0f) pPolarCoord->fRadius = 0.001f;

	vec3 eye = MMath::SphericalToCartesian(pPolarCoord->fRadius, pPolarCoord->fPhi, pPolarCoord->fTheta);	// 방향값을 구하기 위한 카메라(무보정)

	// 카메라 모드 체크
	vec3 position = tar - eye;

	vec3 direction = eye;
	direction.Normalize();

	// 내 캐릭터를 왼쪽으로 비껴보이게 하기 위함
	vec3 right = CrossProduct(direction, UP_VECTOR);
	right.z = 0.0f;
	right.Normalize();

	vec3 vColPos = position+ (right * XCONST::CAMERA_TARGET_LEFT);

	// 지형 충돌 체크
	bool bColTerrain = CheckTerrainCollision(vColPos, tar, vColPos);

	// 카메라를 부드럽게 이동
	UpdateCameraSmoothMove(vColPos, tar, direction, vColPos, direction, fDelta, bColTerrain);

	// 최종값
	m_pCameraSceneNode->SetTransform(vColPos, direction);
}

void XCameraGrappledController::UpdateCamera( float fDelta, int nDeltaX, int nDeltaY )
{
	XPolarCoord* pPolarCoord = GetPolarCoord();

	int nInvert = 1;
	if( XGETCFG_CONTROL_INVERTEDMOUSE == true)
	{
		nInvert = -1;
	}

	if(nDeltaX != 0 || nDeltaY != 0)
	{
		int a = 10;
	}

	pPolarCoord->fPhi -= (nDeltaX / CalCameraSpeed( 100 - XGETCFG_CONTROL_MOUSESENSITIVITY));
	pPolarCoord->fTheta += (nInvert * nDeltaY / CalCameraSpeed( 100 - XGETCFG_CONTROL_MOUSESENSITIVITY));
	pPolarCoord->fTheta = max(min(pPolarCoord->fTheta, MAX_CAMERA_THETA), MIN_CAMERA_THETA);

	GetSceneNode()->SetPolarCoord(pPolarCoord);

	Interpolate(fDelta);

	_UpdateCamera(fDelta);

	if (m_nState == RELEASE)
	{
		global.camera->SetPrimaryCamera(CAMERA_MAIN_BACKVIEW);
	}
}

void XCameraGrappledController::Init( const MUID& uidTarget )
{
	m_uidTarget = uidTarget;
	m_fInitialRadius = GetPolarCoord()->fRadius;

	static const float CAMERA_INTERPOLATION_TIME = 0.1f;

	m_ZoomOutInterpolator.Setup(0, CAMERA_DIST, CAMERA_INTERPOLATION_TIME);
	m_ZoomInInterpolator.Setup(0, CAMERA_DIST, CAMERA_INTERPOLATION_TIME);

	m_nState = ZOOM_OUT;

	GetSceneNode()->SetTargetMUID(m_uidTarget);

	SetCollisionRadius(m_pCameraSceneNode->GetNearZ(),m_pCameraSceneNode->GetFovX(), m_pCameraSceneNode->GetFovY());
}

void XCameraGrappledController::Release()
{
	m_nState = ZOOM_IN;
}

void XCameraGrappledController::Interpolate( float fDelta )
{
	XPolarCoord* pPolarCoord = GetPolarCoord();

	if (m_nState == ZOOM_OUT)
	{
		if (pPolarCoord->fRadius <= m_fInitialRadius + CAMERA_DIST)
		{
			m_ZoomOutInterpolator.Interpolate(fDelta);
			pPolarCoord->fRadius = m_fInitialRadius + m_ZoomOutInterpolator.GetValue();
		}
		else
		{
			m_nState = NORMAL;
		}
	}
	else if (m_nState == ZOOM_IN)
	{
		if (pPolarCoord->fRadius > m_fInitialRadius)
		{
			m_ZoomInInterpolator.Interpolate(fDelta);
			pPolarCoord->fRadius = m_fInitialRadius + CAMERA_DIST - m_ZoomInInterpolator.GetValue();
		}
		else
		{
			m_nState = RELEASE;
		}
	}
}