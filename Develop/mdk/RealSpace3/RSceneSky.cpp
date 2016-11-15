#include "stdafx.h"
#include "RDevice.h"
#include "RDeviceD3D.h"

#include "RSceneSky.h"
#include "RSkyManager.h"
#include "RVType.h"
#include "RMesh.h"
#include "RMeshNode.h"
#include "RMeshMgr.h"
#include "RConfiguration.h"
#include "RCameraSceneNode.h"
#include "RShaderCompositeManager.h"
#include "RUtil.h"
#include "RActorNodeRenderStrategy.h"

#include <assert.h>

namespace rs3 {

RSceneSky::RSceneSky( RSkyInfo* pSkyInfo ) 
: RActor(RRP_NORMAL), m_pSkyInfo(pSkyInfo)
{
	m_matLocal->MakeIdentity();
	m_bUsingActorPassRenderer = false;
	m_bSceneActor = true;
	m_nBGWKPriorityOffset = RBGWK_HIGHEST;

	m_bAutoAnimationEnable = true;

	SetUsable(false);
}

RSceneSky::~RSceneSky()
{
}

void RSceneSky::OnAutoAnimationLoaded()
{
	RActor::OnAutoAnimationLoaded();

	if(m_pSkyInfo)
		SetSpeed( m_pSkyInfo->fUVAniSpeed );
}

bool RSceneSky::Create(const char *filename, bool bBackgroundCreation)
{
	bool bResult = RActor::Create(filename, bBackgroundCreation);

	m_bUseShade = false;

	return bResult;
}

void RSceneSky::OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod)
{
	// 신매니저로부터 업데이트 하지 않음
}

int	RSceneSky::CalcPriority()
{
	return RBGWK_HIGHEST;
}

void RSceneSky::Update()
{
	RActor::OnUpdate();
}

void RSceneSky::Render( RCameraSceneNode* pCurrentCamera, const RVector4 &vMultiplyColor, float fogFactor, RRENDERINGMETHOD renderMothod)
{
	// 예전에 OnPreRender에서 했던일을 OnRegisterToRender쪽으로 옮깁니다.
	UpdateForRender();


	// (정책변경)부모에서 vis 를 세팅해주었기 때문에 각각 세팅할 필요없음. 
	// 단 액터노드는 manual update 정책이기때문에 UpdateVisibility를 해주어야 한다
	SetVisibility(vMultiplyColor.a);
	UpdateVisibility();

	m_vMultiplyColor = vMultiplyColor;
	
	RVector4 vFogFactor;
	DWORD dwFogColor;
	RShaderCompositeManager::GetFogInfo(vFogFactor, dwFogColor);

	if(pCurrentCamera->GetFogEnable())
	{
		// 세이더에서 포그 공식을 변경하여서 0:포그없음 1:포그만땅 으로 기존과 반대로 변경 되었으므로 1-f 값으로 넘겨준다.( develop:rev6487기준)
		RShaderCompositeManager::FogOn(0, 0, 1-fogFactor, pCurrentCamera->GetFogColor());
	}
	else
	{
		RShaderCompositeManager::FogOff();
	}



	// 메쉬 렌더
	if ( !m_sortedRenderableActorNode.empty() )
	{
		for( int i=0; i<(int)m_sortedRenderableActorNode.size(); i++)
		{
			RActorNode *pActorNode = m_sortedRenderableActorNode[i];
			pActorNode->UpdateVisibility();

			RActorNodeRenderStrategySky RenderStrategy;
			pActorNode->RenderSingle( pCurrentCamera, &RenderStrategy );
		}
	}

	RShaderCompositeManager::FogOn(vFogFactor.x, vFogFactor.y, vFogFactor.z, dwFogColor);
}

}