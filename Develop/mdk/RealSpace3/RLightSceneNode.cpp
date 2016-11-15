#include "StdAfx.h"
#include "RLightSceneNode.h"
#include "RCameraSceneNode.h"
#include "RLODProcessorBase.h"

namespace rs3 {

MImplementRTTI(RLightSceneNode, RSceneNode);

RLightSceneNode::RLightSceneNode(RRENDERPASS eRenderPass) :
  RSceneNode(eRenderPass)
, m_fIntensity(1.f)
, m_fApplyIntensity(1.f)
, m_color(1,1,1)
, m_fAttenuationStart(0.f)
{
	/// LOD 사용
	CreateLODContext( LODP_FOG | LODP_PROJECTION);
}

RLightSceneNode::~RLightSceneNode(void)
{
}

bool RLightSceneNode::GetEnable()
{
	if( m_fApplyIntensity <= 0.0f )
		return false;

	return GetVisible();
}

void RLightSceneNode::SetColorRGB(DWORD dwColor)
{
	// GetRValue, GetGValue, GetBValue 함수는 BGR 순으로 처리하기 때문에
	// R과 B의 순서를 바꿔준다
	float fR = ((float)( GetBValue( dwColor )))/255.f;
	float fG = ((float)( GetGValue( dwColor )))/255.f;
	float fB = ((float)( GetRValue( dwColor )))/255.f;

	m_color = RVector(fR, fG, fB);
}

DWORD RLightSceneNode::GetColorRGB() const
{
	WORD wR = m_color.x*255;
	WORD wG = m_color.y*255;
	WORD wB = m_color.z*255;

	return D3DCOLOR_ARGB(0xFF, wR, wG, wB);
}

float RLightSceneNode::GetApplyIntensity()
{
	return m_fApplyIntensity;
}


bool RLightSceneNode::CheckCameraInLight(RCameraSceneNode* pCamera)
{
	float fDist = (GetAABB().GetCenter() - pCamera->GetWorldPosition()).Length() - pCamera->GetNearZ();
	return fDist < GetAABB().GetRadius();
}


}
