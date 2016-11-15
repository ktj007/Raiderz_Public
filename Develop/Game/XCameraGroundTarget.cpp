#include "StdAfx.h"
#include "XCameraGroundTarget.h"

#define GROUND_TARGET_START_MOVE_SPEED	0.25f
#define GROUND_TARGET_END_MOVE_SPEED	0.2f

//const float GROUND_MOVE_THETA			= 1.8831f;
const float GROUND_MOVE_THETA			= 2.0571f;

XCameraGroundTarget::XCameraGroundTarget( void )
{
	m_bGroundTarget			= false;
	m_bGroundTargetMove		= false;

	m_fStartDis				= 0.0f;
	m_fEndDis				= 0.0f;
	m_fStartTheta			= 0.0f;
	m_fEndTheta				= 0.0f;

	m_fBaseTime				= 0.0f;
}

XCameraGroundTarget::~XCameraGroundTarget( void )
{

}

void XCameraGroundTarget::Start( XPolarCoord* pCur )
{
	//m_bGroundTarget = true;
	//m_bGroundTargetMove = true;
	//m_fTime = GROUND_TARGET_START_MOVE_SPEED;
	//m_fBaseTime = GROUND_TARGET_START_MOVE_SPEED; 

	//m_BackViewPolarCoord = *pCur;

	//m_fStartDis = pCur->fRadius;
	//m_fEndDis	= XCONST::MAX_CAMERA_DIST;
	//m_fStartTheta	= pCur->fTheta;
	//m_fEndTheta	= GROUND_MOVE_THETA;
}

void XCameraGroundTarget::End( XPolarCoord* pCur )
{
	//if(m_bGroundTarget == false)
	//	return;

	//m_bGroundTarget = false;
	//m_bGroundTargetMove = true;
	//m_fTime = GROUND_TARGET_END_MOVE_SPEED;
	//m_fBaseTime = GROUND_TARGET_END_MOVE_SPEED; 

	//m_fEndDis	= m_fStartDis;
	//m_fStartDis = pCur->fRadius;
	//m_fEndTheta	= m_fStartTheta;
	//m_fStartTheta	= pCur->fTheta;
}

void XCameraGroundTarget::StartEndUpdate( float fDelta, XPolarCoord* pCur )
{
	//if(m_bGroundTargetMove)
	//{
	//	m_fTime -= fDelta;

	//	float t;

	//	if(m_fTime <= 0.0f)
	//	{
	//		m_fTime = 0.0f;
	//		t = 0.0f;
	//		m_bGroundTargetMove = false;
	//	}
	//	else
	//		t = m_fTime / m_fBaseTime;

	//	pCur->fRadius = MMath::LinearInterpolation(m_fEndDis, m_fStartDis, t);
	//	pCur->fTheta = MMath::LinearInterpolation(m_fEndTheta, m_fStartTheta, t);
	//}
}

bool XCameraGroundTarget::IsGroundTargetActive()
{
	if(m_bGroundTarget && m_bGroundTargetMove == false)
		return true;

	return false;
}

void XCameraGroundTarget::CalcPosDirForGroundTarget( XPolarCoord* pCur, vec3 vTar, vec3& vPosition, vec3& vDirection )
{
	//vec3 vPosEye = MMath::SphericalToCartesian(m_fEndDis, pCur->fPhi, GROUND_MOVE_THETA);
	vec3 vPosEye = MMath::SphericalToCartesian(pCur->fRadius, pCur->fPhi, pCur->fTheta);

	vPosition = vTar - vPosEye;
	vDirection = MMath::SphericalToCartesian(0.001f, pCur->fPhi, pCur->fTheta);
}