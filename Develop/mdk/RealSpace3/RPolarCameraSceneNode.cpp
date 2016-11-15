#include "stdafx.h"
#include "RPolarCameraSceneNode.h"

namespace rs3 {

RPolarCameraSceneNode::RPolarCameraSceneNode(void) : RCameraSceneNode(), m_vLookAt(0.0f, 0.0f, 0.0f)
{
	m_PolarCoord.fRadius = 180.0f;
	m_PolarCoord.fTheta = MMath::HALFPI;
	m_PolarCoord.fPhi = 0.0f;
}

RPolarCameraSceneNode::~RPolarCameraSceneNode(void)
{

}

void RPolarCameraSceneNode::SetCoord(RPolarCoord* pCoord)
{
	m_PolarCoord.fRadius = pCoord->fRadius;
	m_PolarCoord.fTheta = pCoord->fTheta;
	m_PolarCoord.fPhi = pCoord->fPhi;

	UpdateCamera();
}

void RPolarCameraSceneNode::UpdateCamera()
{
	if (REngine::GetSceneManager().GetPrimaryCamera() != this) return;

	RVector3 tar = m_vLookAt;

	RVector3 eye = RVector3(0,1,0);
	eye.x = m_PolarCoord.fRadius * cos(m_PolarCoord.fTheta) * sin(m_PolarCoord.fPhi + MMath::HALFPI);
	eye.y = m_PolarCoord.fRadius * sin(m_PolarCoord.fTheta) * sin(m_PolarCoord.fPhi + MMath::HALFPI);
	eye.z = m_PolarCoord.fRadius * cos(m_PolarCoord.fPhi + MMath::HALFPI);

	//m_position = tar - eye;
	//m_direction = tar - m_position;
	//m_direction.Normalize();

	SetTransform(tar-eye, tar-GetPosition());
}

void RPolarCameraSceneNode::LookAt(RVector3& at)
{
	m_vLookAt = at;
	UpdateCamera();
}

}