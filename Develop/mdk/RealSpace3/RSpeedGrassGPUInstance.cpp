#include "stdafx.h"
#include "RSpeedGrassGPUInstance.h"
#include "RCameraSceneNode.h"
#include "RShaderFX.h"
#include "RGrassSystem.h"
#include "REnvironmentSet.h"
#include "RShaderCompositeManager.h"
#include "RShaderFX.h"
#include "RShadowStrategy.h"
#include "RTerrain.h"
#include "RSpeedGrassShader.h"

namespace rs3
{

RSpeedGrassGPUInstance::RSpeedGrassGPUInstance(RDevice* dev)
: RSpeedGrass(dev)
{
	m_nGrassCountLimitPerRegion = GRASS_LIMIT_PER_REGION;
	m_pSpeedGrassShader = NULL;
}

RSpeedGrassGPUInstance::~RSpeedGrassGPUInstance()
{
	
}


bool RSpeedGrassGPUInstance::Create()
{
	m_pSpeedGrassShader = new RSpeedGrassShader;
	if( m_pSpeedGrassShader)
		m_pSpeedGrassShader->Init();

	RVertexAttribute grassdattribs[] = 
	{ 
		TYPE_VERTEX, FORMAT_FLOAT, 3,
		TYPE_NORMAL, FORMAT_FLOAT, 3,
		TYPE_COLOR, FORMAT_FLOAT, 4, 
		TYPE_TEXCOORD, FORMAT_FLOAT, 2,
		TYPE_TEXCOORD, FORMAT_FLOAT, 4,
	};
	m_hGrassVertex = m_pDevice->CreateVertexFormat(grassdattribs, _countof(grassdattribs));

	return true;
}

void RSpeedGrassGPUInstance::Destroy()
{
	RSpeedGrass::Destroy();

	if( m_pSpeedGrassShader)
		m_pSpeedGrassShader->Destroy();
	SAFE_DELETE( m_pSpeedGrassShader);
}

void RSpeedGrassGPUInstance::UpdateDXBuffers( RTerrain* pTerrain)
{
	// 전체 Regions의 Blades에 대해 모두 업데이트 한다.
	for (int i = 0; i < (int)m_vRegions.size(); ++i)
	{
		SRegion& nRegion = m_vRegions[i];
		SRegionData& nRegionData = m_vRegionDatas[i];

		static vector<SGrassVertex> vert;
		static vector<WORD> index;

		vert.clear();
		index.clear();

		vert.reserve(nRegion.m_vBlades.size() *4);
		index.reserve(nRegion.m_vBlades.size()*6);

		// TODO : CPU 버전을 만들 때, Grass의 Bounce 계산을 하는 부분을 분리하자.

		// Regions의 Blades Array의 것을 배열로 나열한다.
		vector<SBlade>::iterator it;
		for ( it = nRegion.m_vBlades.begin(); it != nRegion.m_vBlades.end(); ++it )
		{
			SBlade &nBlade = (*it);
			GetGrassVertex(nBlade, vert, index, vert.size(), pTerrain);

			for ( int i = 0; i<3; ++i )
			{
				if ( nRegion.m_afMax[i] < nBlade.m_afPos[i] )	nRegion.m_afMax[i] = nBlade.m_afPos[i];
				if ( nRegion.m_afMin[i] > nBlade.m_afPos[i] )	nRegion.m_afMin[i] = nBlade.m_afPos[i];
			}
		}
		
		// vertex buffer 생성
		if ( nRegion.m_vBlades.empty() 
			&& nRegionData.hVertexBuffer!=R_NONE
			&& nRegionData.hIndexBuffer !=R_NONE )
		{
			m_pDevice->DeleteVertexBuffer(nRegionData.hVertexBuffer);
			m_pDevice->DeleteIndexBuffer (nRegionData.hIndexBuffer);
			
			nRegionData.nPrimitiveNum = 0;
			nRegionData.nIndexBufferSize = 0;
			nRegionData.nVertexNum = 0;
			nRegionData.nVertexBufferSize = 0;

			nRegionData.hVertexBuffer = R_NONE;
			nRegionData.hIndexBuffer = R_NONE;
		}
		else if ( nRegion.m_vBlades.size())
		{
			// 현재 할당된 크기보다 더 클 때만 새로 생성한다.
			if ( nRegionData.nVertexBufferSize < vert.size()
				|| nRegionData.nIndexBufferSize < index.size())
			{
				m_pDevice->DeleteVertexBuffer(nRegionData.hVertexBuffer);
				m_pDevice->DeleteIndexBuffer (nRegionData.hIndexBuffer);

				nRegionData.nVertexBufferSize = vert.size();
				nRegionData.nIndexBufferSize  = index.size();
				
				nRegionData.hVertexBuffer 
					= m_pDevice->CreateVertexBuffer( nRegionData.nVertexBufferSize*sizeof(SGrassVertex), sizeof(SGrassVertex));
				nRegionData.hIndexBuffer
					= m_pDevice->CreateIndexBuffer( nRegionData.nIndexBufferSize, RFMT_INDEX16);
			}

			// Overwrite VertexBuffer
			void *ptrVetex = m_pDevice->LockVertexBuffer(nRegionData.hVertexBuffer, 0);
			memcpy(ptrVetex,&vert[0],vert.size()*sizeof(SGrassVertex));
			m_pDevice->UnlockVertexBuffer(nRegionData.hVertexBuffer);
			nRegionData.nVertexNum = vert.size();

			// Overwrite Indexbuffer
			void *ptrIndex = m_pDevice->LockIndexBuffer(nRegionData.hIndexBuffer, 0);
			memcpy(ptrIndex,&index[0],index.size()*sizeof(WORD));
			m_pDevice->UnlockIndexBuffer(nRegionData.hIndexBuffer);
			nRegionData.nPrimitiveNum = index.size()/3;
		}

		nRegion.m_afCenter[0] = 0.5f * (nRegion.m_afMin[0] + nRegion.m_afMax[0]);
		nRegion.m_afCenter[1] = 0.5f * (nRegion.m_afMin[1] + nRegion.m_afMax[1]);
		nRegion.m_afCenter[2] = 0.5f * (nRegion.m_afMin[2] + nRegion.m_afMax[2]);

		// compute culling radius
		nRegion.m_fCullingRadius = 1.1f * sqrt(
			((nRegion.m_afMax[0] - nRegion.m_afCenter[0]) * (nRegion.m_afMax[0] - nRegion.m_afCenter[0])) +
			((nRegion.m_afMax[1] - nRegion.m_afCenter[1]) * (nRegion.m_afMax[1] - nRegion.m_afCenter[1])) +
			((nRegion.m_afMax[2] - nRegion.m_afCenter[2]) * (nRegion.m_afMax[2] - nRegion.m_afCenter[2])) 
			);
	}	
}


void RSpeedGrassGPUInstance::SetBasicRenderState( RGrassSystem *pGrassSystem, RCameraSceneNode* pCamera)
{
	if(!pCamera)
		return;

	if( m_pSpeedGrassShader)
	{
		RRenderingStrategy* pRenderingStrategy = REngine::GetSceneManager().GetRenderingStrategy();
		_ASSERT(pRenderingStrategy);

		m_pSpeedGrassShader->SetTransformConstant( RMatrix::IDENTITY, pCamera->GetViewMatrix(), pCamera->GetViewProjectionMatrix(), pRenderingStrategy->GetWidth(), pRenderingStrategy->GetHeight(), pCamera->GetFarZ());

		//시간을 [0,2Phi]사이 값으로 정규화
		float fNormalizedTime = 0;
		if( m_fGrassJigglingPeriod != 0)	// devide by zero를 반드시 체크 하여야 한다. 
		{
			int   iQuotient = pGrassSystem->m_fTime / (6.28 / m_fGrassJigglingPeriod);
			fNormalizedTime = pGrassSystem->m_fTime - iQuotient * (6.28 / m_fGrassJigglingPeriod);
		}
		m_pSpeedGrassShader->SetWindConstant( fNormalizedTime, m_vWindDirection, m_fWindStrength, m_fGrassJigglingPeriod, m_fGrassJigglingStrength, m_fGrassIncliningStrength);
		m_pSpeedGrassShader->SetLodConstant( m_fLodFarDistance, m_fLodTransitionLength);
		const float* pUnitBillboard = GetUnitBillboard();
		m_pSpeedGrassShader->SetBillboardConstant( pUnitBillboard);
	}

	//Set Render State
	{
		m_pDevice->SetTexture( AS_DIFFUSE, pGrassSystem->m_hGrassCompositeTexture );
		m_pDevice->SetTextureFilter( AS_DIFFUSE, RTF_BILINEAR );
		//m_pDevice->SetTextureMaxAnisotropy( AS_DIFFUSE, 1);
		m_pDevice->SetCullMode(RCULL_NONE);
		m_pDevice->SetTextureMipmapLodBias(AS_DIFFUSE, REngine::GetConfig().GetSpeedGrassTextureMipmapLodBias() );
	}
}



void RSpeedGrassGPUInstance::ResetRenderState()
{
	m_pDevice->SetCullMode(RCULL_CW);
	m_pDevice->SetTextureMipmapLodBias(AS_DIFFUSE, REngine::GetConfig().m_fGlobalTextureMipmapLodBias);
}

int RSpeedGrassGPUInstance::RenderTillingEffect()
{
	return 0;
}

int RSpeedGrassGPUInstance::Render( RShaderFX* pShader, RTechnique* pTechnique)
{
	RPFC_THISFUNC;
	if ( !pShader || !pTechnique)
		return 0;
	
	int nGrassCount = 0;

	m_pDevice->SetVertexFormat(m_hGrassVertex);
	{
		// nRegion은 타일 기반의 분류고 그 안에 풀 데이터인 m_vBlades를 가진다.
		unsigned int uiNumRegions = 0;
		const SRegion* pRegions = GetRegions(uiNumRegions);

		// run through the regions and render those that aren't culled
		for (int i = 0; i < (int)uiNumRegions; ++i)
		{
			if (!pRegions[i].m_bCulled )	// region - 공간정보만, regiondata - 실제 렌더링시 필요한 정보
			{
				if ( m_vRegionDatas[i].nPrimitiveNum && m_vRegionDatas[i].nVertexNum )
				{
					// 정점 버퍼와 인덱스 버퍼 세트
					m_pDevice->SetVertexBuffer( m_vRegionDatas[i].hVertexBuffer, 0, 0);
					m_pDevice->SetIndexBuffer ( m_vRegionDatas[i].hIndexBuffer );
					// 테크닉 pass 0 을 세트, 그리기
					pTechnique->Begin(NULL, TECH_PRESERVE_NOT);
					pTechnique->BeginPass(0);
					nGrassCount += m_vRegionDatas[i].nPrimitiveNum;
					// 텍스쳐는 RSpeedGrassGPUInstance::SetRenderState()에서 설정.
					m_pDevice->DrawIndexedPrimitive( RPT_TRIANGLELIST, 0, 0, m_vRegionDatas[i].nVertexNum, 0, m_vRegionDatas[i].nPrimitiveNum);
					pTechnique->EndPass();
					pTechnique->End();
				}
			}
		}
	}

	return nGrassCount;
}


int RSpeedGrassGPUInstance::RenderGStage( RRENDERINGMETHOD method)
{
	RPFC_THISFUNC;
	if( m_pSpeedGrassShader)
	{
		RShaderFX* pShader = NULL;
		switch( method) 
		{
		case RENDERING_DEFERRED: 
			pShader = m_pSpeedGrassShader->GetShaderForGStageDeferred();
			break;
		case RENDERING_INFERRED: 
			pShader = m_pSpeedGrassShader->GetShaderForGStageInferred();
			break;
		}

		if( pShader)
		{
			return Render( pShader, &(pShader->GetTechniqueByIndex(0)));
		}
	}

	return false;

}


int RSpeedGrassGPUInstance::RenderMStage()
{
	if( m_pSpeedGrassShader)
	{
		RShaderFX* pShader = m_pSpeedGrassShader->GetShaderForMStageInferred();
		
		if( pShader)
		{
			return Render( pShader, &(pShader->GetTechniqueByIndex(0)));
		}
	}

	return false;	
}


void RSpeedGrassGPUInstance::RenderRaw()
{
	m_pDevice->SetVertexFormat(m_hGrassVertex);
	{
		// nRegion은 타일 기반의 분류고 그 안에 풀 데이터인 m_vBlades를 가진다.
		unsigned int uiNumRegions = 0;
		const SRegion* pRegions = GetRegions(uiNumRegions);

		// run through the regions and render those that aren't culled
		for (int i = 0; i < (int)uiNumRegions; ++i)
		{
			if (!pRegions[i].m_bCulled )	// region - 공간정보만, regiondata - 실제 렌더링시 필요한 정보
			{
				if ( m_vRegionDatas[i].nPrimitiveNum && m_vRegionDatas[i].nVertexNum )
				{
					// 정점 버퍼와 인덱스 버퍼 세트
					m_pDevice->SetVertexBuffer( m_vRegionDatas[i].hVertexBuffer, 0, 0);
					m_pDevice->SetIndexBuffer ( m_vRegionDatas[i].hIndexBuffer );
					m_pDevice->DrawIndexedPrimitive( RPT_TRIANGLELIST, 0, 0, m_vRegionDatas[i].nVertexNum, 0, m_vRegionDatas[i].nPrimitiveNum);
				}
			}
		}
	}
}

};
