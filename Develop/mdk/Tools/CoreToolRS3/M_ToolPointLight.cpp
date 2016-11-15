#include "stdafx.h"
#include "M_ToolPointLight.h"

#include "RRenderHelper.h"

//#include "MainFrm.h"
#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperRender.h"

namespace rs3 {

RToolPointLightSceneNode::RToolPointLightSceneNode(CControlMediator* pControlMediator)
: m_pControlMediator( pControlMediator )
{
}

void RToolPointLightSceneNode::OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMethod)
{
	RPointLightSceneNode::OnRender( pCurrentCamera, nSubMaterialID, renderMethod);

	m_pControlMediator->GetHelperRender()->RegisterToRenderPointLight(this);
}

bool RToolPointLightSceneNode::OnPick(RPICKINFO& pickInfo)
{
	RVector vPos = GetWorldPosition();
	RBox aabb;
	aabb.vmax = vPos + RVector(30, 30, 30);
	aabb.vmin = vPos - RVector(30, 30, 30);

	if ( aabb.ContainsLineSegment(pickInfo.inPickOrigin,pickInfo.inPickTo))
	{
		pickInfo.outPosition = vPos;
		return true;
	}
	return false;
}

RSceneNode* RToolPointLightSceneNode::Clone()
{
	RToolSceneManager* pToolSceneManager = m_pControlMediator->GetToolSceneManager();
	RToolPointLightSceneNode* pPointLight = static_cast<RToolPointLightSceneNode*> ( pToolSceneManager->CreateSceneNode(RSID_LIGHTSCENENODE) );

	// common
	pPointLight->m_strNodeName = m_strNodeName;
	pPointLight->SetTransform(GetLocalTransform());

	// property
	pPointLight->m_color = m_color;
	pPointLight->m_fIntensity = m_fIntensity;
	pPointLight->m_fApplyIntensity = m_fApplyIntensity;
	pPointLight->SetRadius( GetRadius() );
	pPointLight->m_fAttenuationStart = m_fAttenuationStart;

	if (QueryAttribute(RSNA_DO_NOT_SAVE))
		pPointLight->AddAttribute(RSNA_DO_NOT_SAVE);
	else
		pPointLight->RemoveAttribute(RSNA_DO_NOT_SAVE);

	pPointLight->SetAreaRange(GetAreaRange());

// 	// Tool Property
// 	pPointLight->SetProperty( GetProperty() );

	//Light가 Actor의 자식으로 있을 경우 Light도 Actor와 같이 애니메이션되어야 하므로
	//AutoDate 여부도 복사한다.
	pPointLight->SetAutoUpdate(IsAutoUpdate());

	return pPointLight;
}

}