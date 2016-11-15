#include "stdafx.h"

#include "RStaticMeshRenderStrategy.h"
#include "RStaticMeshSceneNode.h"
#include "RShaderCompositeManager.h"
#include "RMaterial.h"

namespace rs3 {

RStaticMeshRenderStrategy::RStaticMeshRenderStrategy( RStaticMeshSceneNode* pStaticMesh ) :
  m_pStaticMesh(pStaticMesh)
, m_pTechnique(NULL)
, m_pCurrentTechnique(NULL)
, m_pShaderCompositeManager(NULL)
{
	m_pShaderCompositeManager = REngine::GetSceneManager().GetShaderCompositeManager();
}

RStaticMeshRenderStrategy::~RStaticMeshRenderStrategy()
{
}

bool RStaticMeshRenderStrategy::CheckAndSetTechnique( RCameraSceneNode* pCurrentCamera,  RenderInfo* pRenderInfo, int nLightMapID, bool bFogEnable )
{
	TIMEPROFILE_THIS_FUNCTION;

	RMaterial* pMaterial = pRenderInfo->m_pMaterial;
	m_pShaderCompositeManager->SetMaterialRenderState( pMaterial, true, true, false);

	// 구름 그림자
//	m_pShaderCompositeManager->SetCloudRenderState( pCurrentCamera);

	RDevice* pDevice = REngine::GetDevicePtr();

	if( pMaterial->CheckFlag(RM_FLAG_USEOPACITY) || m_pStaticMesh->m_bPointColor )
	{
		pDevice->SetBlending(RBLEND_SRCALPHA,RBLEND_INVSRCALPHA);
	}
	else //알파 아닌상태.
	{
		pDevice->SetBlending(RBLEND_NONE,RBLEND_NONE);
	}

	m_pTechnique = bFogEnable ? &pRenderInfo->m_fogTechnique : &pRenderInfo->m_technique;

	return true;
}

void RStaticMeshRenderStrategy::BeginTechnique()
{
	_ASSERT(m_pTechnique);

	if( !m_pCurrentTechnique )
	{
		m_pCurrentTechnique = m_pTechnique;
		m_pCurrentTechnique->Begin(0,TECH_PRESERVE_NOT);
		m_pCurrentTechnique->BeginPass();
	}
	else
	if( false == m_pTechnique->IsEqual( *m_pCurrentTechnique ) )
	{
		if( m_pCurrentTechnique->m_bUsing )
		{
			m_pCurrentTechnique->EndPass();
			m_pCurrentTechnique->End();
		}

		m_pCurrentTechnique = m_pTechnique;
		m_pCurrentTechnique->Begin(0,TECH_PRESERVE_NOT);
		m_pCurrentTechnique->BeginPass();
	}

	m_pCurrentTechnique->CommitChanges();
}

void RStaticMeshRenderStrategy::EndTechnique()
{
	if( m_pCurrentTechnique->m_bUsing )
	{
		m_pCurrentTechnique->EndPass();
		m_pCurrentTechnique->End();
		m_pCurrentTechnique = NULL;
	}
}

}