#include "stdafx.h"
#include "RStaticMeshSceneNode.h"
#include "RShaderCompositeManager.h"
#include "RStaticMeshMStageRenderStrategy.h"
#include "RMaterial.h"

namespace rs3 {


bool RStaticMeshMStageRenderStrategy::CheckAndSetTechnique(  RCameraSceneNode* pCurrentCamera, RenderInfo* pRenderInfo, int nLightMapID, bool bFogEnable )
{
	m_pShaderCompositeManager->SetMaterialRenderState(pRenderInfo->m_pMaterial, false, false, false);

	RMaterial* pMaterial = pRenderInfo->m_pMaterial;
	RDevice* pDevice = REngine::GetDevicePtr();

	m_pStaticMesh->CreateShaderForMStage( pRenderInfo, bFogEnable );
	m_pTechnique = bFogEnable ? &pRenderInfo->m_MStageFogTechnique : &pRenderInfo->m_MStageTechnique;

	return true;
}


}