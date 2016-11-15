#include "stdafx.h"

#include "RStaticMeshDepthRenderStrategy.h"
#include "RStaticMeshSceneNode.h"
#include "RShaderCompositeManager.h"
#include "RShadowStrategy.h"

namespace rs3 {

bool RStaticMeshDepthRenderStrategy::CheckAndSetTechnique(  RCameraSceneNode* pCurrentCamera, RenderInfo* pRenderInfo, int nLightMapID, bool bFogEnable )
{
	_ASSERT(REngine::GetSceneManager().GetRealtimeShadowMethod() != SHADOW_NONE);

	RShaderCompositeManager::SetCullMode(true);
	RShaderCompositeManager::SetBlending(0, false); //not use blending

	if(pRenderInfo->m_pMaterial->CheckFlag(RM_FLAG_USEALPHATEST))
	{
		RShaderCompositeManager::SetTexture(AS_DIFFUSE, pRenderInfo->m_pMaterial->GetTexLayerByType(pRenderInfo->m_pMaterial->GetTextureTypeForAlphaUsage(ALPHAUSAGE_ALPHA)));
		RShaderCompositeManager::m_cAlphaRef.SetFloat(pRenderInfo->m_pMaterial->GetAlphaRefValue()/255.0f);
	}	

	m_pTechnique = &pRenderInfo->m_depthTechnique;

	return true;
}

}