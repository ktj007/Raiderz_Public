
#include "stdafx.h"
#include "RStaticMeshSceneNode.h"
#include "RShaderCompositeManager.h"
#include "RStaticMeshGStageRenderStrategy.h"
#include "RMaterial.h"

namespace rs3 {


bool RStaticMeshGStageRenderStrategy::CheckAndSetTechnique(  RCameraSceneNode* pCurrentCamera, RenderInfo* pRenderInfo, int nLightMapID, bool bFogEnable )
{
	m_pShaderCompositeManager->SetMaterialRenderState(pRenderInfo->m_pMaterial, true, true, false);

	RDevice* pDevice = REngine::GetDevicePtr();
	// 일단은 DSF등 알파 오브젝트의 지오메트리 버퍼 처리가 되기 전까지는 스태틱 메쉬의 알파 처리를 끈다.
	//http://dev:8181/projects/engine/ticket/1063
// 	if( pMaterial->dwFlags & RM_FLAG_ADDITIVE )
// 	{
// 		// 알파 애디티브면 지오메트리 패스서는 그리지 않는다.
// 		return false;
// 	}
// 	else if( pMaterial->dwFlags & RM_FLAG_USEOPACITY || m_pStaticMesh->m_bPointColor )
// 	{
// 		// 알파 쓰는 것은 지오메트리 패스서는 그리지 않는다.
// 		return false;
// 	}
// 	else	// 알파 아닌 상태
	{
		pDevice->SetFogEnable(bFogEnable);
		pDevice->SetBlending(RBLEND_NONE,RBLEND_NONE);
	}

	m_pTechnique = &pRenderInfo->m_GStageTechnique;

	return true;
}


}