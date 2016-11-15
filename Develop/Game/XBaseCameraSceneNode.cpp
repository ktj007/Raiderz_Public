#include "StdAfx.h"
#include "XBaseCameraSceneNode.h"
#include "XCameraBackViewController.h"
#include "XCameraHelper.h"
#include "RSpeedGrass.h"
#include "XModuleEntity.h"

XBaseCameraSceneNode::XBaseCameraSceneNode(void)
: rs3::RCameraSceneNode()
, m_uidTarget(MUID::ZERO)
{
	m_PolarCoord.fRadius= XCONST::CAMERA_TARGET_DIST;
	m_PolarCoord.fPhi	= MMath::HALFPI;		// -y방향을 보는게 초기값
	m_PolarCoord.fTheta = MMath::HALFPI;

	// 모션 블러
	global.smgr->SetScreenMotionBlurEnable(true);
	global.smgr->SetScreenMotionBlurActive(false);

	// 누적 모션 블러
	global.smgr->SetAccumMotionBlurEnable(true);
	global.smgr->SetAccumMotionBlurActive(false);

	// 픽셀 모션 블러
	global.smgr->SetPixelMotionBlurEnable(true);
	global.smgr->SetPixelMotionBlurActive(false);

	// 디스토션
	global.smgr->SetDistortionEnable(true);

	// PEEDGE
	global.smgr->SetPEEdgeEnable(true);
	global.smgr->SetPEQuality(2, 2);
	global.smgr->SetPEFactorTickNess(XCONST::INTERACTION_TARGET_EDGE_TICKNESS * 2.f, XCONST::INTERACTION_TARGET_EDGE_BLUR_FACTOR * 2.f, 1.75f);
}

XBaseCameraSceneNode::~XBaseCameraSceneNode(void)
{

}

void XBaseCameraSceneNode::OnUpdate()
{
	PFC_THISFUNC;
	rs3::RCameraSceneNode::OnUpdate();
}

void XBaseCameraSceneNode::SetPolarCoord( XPolarCoord* pCoord )
{
	m_PolarCoord.fRadius = pCoord->fRadius;
	m_PolarCoord.fPhi = pCoord->fPhi;
	m_PolarCoord.fTheta = pCoord->fTheta;
}

void XBaseCameraSceneNode::SetTargetMUID(MUID uid)
{
	m_uidTarget = uid;
}

bool XBaseCameraSceneNode::GetTargetPosition( vec3& outPos )
{
	if (gg.omgr == NULL) return false;
	XObject* pObject = gg.omgr->Find(m_uidTarget);
	if (pObject == NULL) return false;

	XModuleEntity* pModuleEntity = pObject->GetModuleEntity();
	if (pModuleEntity == NULL) return false;

	outPos = pModuleEntity->GetPosition();
	return true;
}

const float XBaseCameraSceneNode::GetDist()
{
	return m_PolarCoord.fRadius;
}