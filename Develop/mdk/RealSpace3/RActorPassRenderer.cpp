#include "stdafx.h"

#include "RActorPassRenderer.h"
#include "RSceneManager.h"
#include "RActor.h"
#include "RMeshNode.h"
#include "RMaterial.h"
#include "RActorNode.h"
#include "RCameraSceneNode.h"
#include "RProfileInfo.h"
#include "RShaderCompositeManager.h"
#include "RShaderFX.h"
#include "RDirectionalLightSceneNode.h"
#include "mmsystem.h"
#include "RPostEffectDistortion.h"
#include "RShadowStrategy.h"
#include "RShaderEffectController.h"
#include "RUtil.h"
#include "RActorNodeRenderStrategy.h"


namespace rs3 {

RActorNodeInstancingBuffer	RActorPassRenderer::m_InstancingBuffer;	// 우선은 모든 인스턴스가 하나 공유
RMatrix*	RActorPassRenderer::m_pInstancingData = NULL;


RActorPassRenderer::RActorPassRenderer()
: m_bRender(true)
{
}

void RActorPassRenderer::Init()
{
	m_InstancingBuffer.Create( REngine::GetDevicePtr());

	if(!m_pInstancingData)
		m_pInstancingData = new RMatrix[ m_InstancingBuffer.GetMaxSize()];
}

void RActorPassRenderer::Destroy()
{
	m_InstancingBuffer.Destroy();

	SAFE_DELETE_ARRAY( m_pInstancingData);
}

void RActorPassRenderer::RenderDepthOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RShadowStrategy* pShadowStrategy )
{
	RPFC_THISFUNC;
	if ( m_bRender == false )
		return;

	_ASSERT(pShadowStrategy);

	RenderShadowMap(pCurrentCamera, pShadowStrategy, false);
}

void RActorPassRenderer::RenderDoubleShadowDepthOnPass( RCameraSceneNode* pCurrentCamera, RShadowStrategy* pShadowStrategy )
{
	RPFC_THISFUNC;

	if ( m_bRender == false )
		return;

	_ASSERT(pShadowStrategy);

	RenderShadowMap(pCurrentCamera, pShadowStrategy, true);
}

void RActorPassRenderer::RenderShadowMap(RCameraSceneNode* pCurrentCamera, RShadowStrategy* pShadowStrategy, bool bDoubleShadowPass )
{
	RPFC_THISFUNC;

	RActorNodeRenderStrategyShadowMap DepthShadowRenderStrategy;
	RActorNodeRenderStrategyProjectionShadowMap ProjectionRenderStrategy;

	RActorNodeRenderStrategyShadowMap* pRenderStrategy = 
		pShadowStrategy->GetShadowDesc().eMethod==SHADOW_PROJECTION ?
		&ProjectionRenderStrategy : &DepthShadowRenderStrategy;

	pRenderStrategy->SetShadowStrategy(pShadowStrategy);

	if(bDoubleShadowPass)
		RenderOnPass(pCurrentCamera, pRenderStrategy, pCurrentCamera->GetViewFrustumCullingMgr()->GetActorNodeDoubleShadowChain());
	else
		RenderOnPass(pCurrentCamera, pRenderStrategy, pCurrentCamera->GetViewFrustumCullingMgr()->GetActorNodeShadowChain());
}

void RActorPassRenderer::RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	// 	TIMEPROFILE_THIS_FUNCTION;
	RPFC_THISFUNC;

	RRenderChain<RActorNodeKey>* pActorNodeRenderChain = pCurrentCamera->GetViewFrustumCullingMgr()->GetActorNodeChain();

	switch( renderMethod)
	{
	case RENDERING_INFERRED:
		{
			RActorNodeRenderStrategyMStageInferred RenderStrategy;
			RenderOnPass( pCurrentCamera, &RenderStrategy, pActorNodeRenderChain);
		}
		break;
	case RENDERING_DEFERRED:
		{
			RActorNodeRenderStrategyMStageAddColor RenderStrategy;
			RenderOnPass( pCurrentCamera, &RenderStrategy, pActorNodeRenderChain);
		}
		break;
	}

	RPFC_GPU_CALL;
}


void RActorPassRenderer::RenderGStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	// 	TIMEPROFILE_THIS_FUNCTION;
	RPFC_THISFUNC;

	RRenderChain<RActorNodeKey>* pActorNodeRenderChain = pCurrentCamera->GetViewFrustumCullingMgr()->GetActorNodeChain();

	switch( renderMethod)
	{
	case RENDERING_INFERRED:
		{
			RActorNodeRenderStrategyGStageInferred RenderStrategy;
			RenderOnPass( pCurrentCamera, &RenderStrategy, pActorNodeRenderChain);
		}
		break;
	case RENDERING_DEFERRED:
		{
			if(REngine::GetSceneManager().GetRenderingStrategy()->GetRenderingMode()==RRM_DIFFUSE_ONLY)
			{
				RActorNodeRenderStrategyDiffuseOnly RenderStrategy;
				RenderOnPass( pCurrentCamera, &RenderStrategy, pActorNodeRenderChain);
			}
			else
			{
				RActorNodeRenderStrategyGStageDeferred RenderStrategy;
				RenderOnPass( pCurrentCamera, &RenderStrategy, pActorNodeRenderChain);
			}
		}
		break;
	}

	RPFC_GPU_CALL;
}


// 인스턴싱 정보 상수들 업데이트 하기
void RActorPassRenderer::SetInstancingConstant(int nCnt)
{
	RShaderCompositeManager::m_cInstancingMatrices.SetMatrixArray( m_pInstancingData, nCnt);
}


void RActorPassRenderer::SetActorNodeInstancingData(int nIdx, RActorNode* pNode)
{
	PFC_THISFUNC;

	if(!pNode || !m_pInstancingData || nIdx >= m_InstancingBuffer.GetMaxSize() || nIdx < 0)
		return;

	RMatrix mxWorld = pNode->GetWorldTransform();
	RVector4 vColor = RVector4( pNode->m_dwPartsColor);


	// 데이터에 셋팅. 매트릭스의 14,24,34,44는 0,0,0,1이라 전제 하고 그 자리에 컬러 값으로 채움
	mxWorld._14 = vColor.x;
	mxWorld._24 = vColor.y;
	mxWorld._34 = vColor.z;
	mxWorld._44 = pNode->m_pActor->GetAO() ? 0.0f : 1.0f ;
	m_pInstancingData[nIdx] = mxWorld;
}


void RActorPassRenderer::RenderInstancingNode( RActorNode* pActorNode, RCameraSceneNode* pCurrentCamera, RActorNodeRenderStrategy* pActorRenderStrategy, 
									 RTechnique& currentTechnique, int& nInstanceCnt,
									 RActorNode** ppInstancingActorNode, int index)
{
	PFC_THISFUNC;

	// 인스턴싱이 가능한 것은 인스턴싱 버퍼에 데이터를 저장하고
	// 버퍼의 공간이 모자라면 랜더 후 버퍼 비움( 실제 비우지 않고 카운터만 초기화).
	SetActorNodeInstancingData(nInstanceCnt, pActorNode);
	++nInstanceCnt;
	*ppInstancingActorNode = pActorNode;

	if( nInstanceCnt >= m_InstancingBuffer.GetMaxSize())
	{
		RenderNode(pCurrentCamera, pActorRenderStrategy, pActorNode, nInstanceCnt, currentTechnique, index, true);

		nInstanceCnt = 0;
		*ppInstancingActorNode = NULL;
	}
}

void RActorPassRenderer::RenderNode(RCameraSceneNode* pCamera, RActorNodeRenderStrategy* pActorRenderStrategy, 
												 RActorNode* pActorNode, int nInstanceCnt, RTechnique& currentTechnique, int index, bool bAbleInstancing)
{
	RPFC_THISFUNC;
	pActorNode->SetVertexStream(false);

	if(bAbleInstancing)
	{
		pActorRenderStrategy->ResetNodeRenderState();

		// 데이터가 담긴 배열을 셰이더 상수로 전달 및 commit
		SetInstancingConstant( min( nInstanceCnt, m_InstancingBuffer.GetMaxSize()));
	}
	else
	{
		// 인스턴싱 안할 시는 노드 별 랜더 스테이트 설정 따로
		RMaterial* pMaterial = index<0 ? NULL : pActorNode->GetSubMaterial(index);
		pActorRenderStrategy->SetNodeRenderState( pCamera, pActorNode, pMaterial);
	}


	SetupForRenderInstancing( &m_InstancingBuffer, nInstanceCnt);

	RShaderFX* pShader = pActorRenderStrategy->GetShader(pActorNode, index, bAbleInstancing);
	if( BeginTechnique(currentTechnique, pShader) )
	{
		currentTechnique.CommitChanges();

		if(index<0)
			pActorNode->RenderAllNodePrimitive();
		else
			pActorNode->RenderPrimitive( index );
	}
}

template<class KEY_TYPE>
void RActorPassRenderer::RenderOnPass(RCameraSceneNode* pCurrentCamera, RActorNodeRenderStrategy* pActorRenderStrategy, RRenderChain<KEY_TYPE>* pActorNodeRenderChain)
{
	// 	TIMEPROFILE_THIS_FUNCTION;
	

	if (m_bRender == false)
		return;

	if( !pActorNodeRenderChain)
		return;

	_ASSERT( pCurrentCamera);
	_ASSERT(pActorRenderStrategy);
	if(pActorRenderStrategy==NULL)
		return;

	RPFC_B("RActorPassRenderer::RenderOnPass Chain Loop");

	//Timer Reset & Start
	REngine::GetResourceProfiler().StartMeshProfiler();

	RPFC_THISFUNC;

	pActorRenderStrategy->Begin();
	{
		RTechnique currentTechnique;
		//////////////////////////////////////////////////
		// 렌더링 가능한 메시노드별로 순회
		RRenderChainIterator<KEY_TYPE>	Iter;
		for( Iter = pActorNodeRenderChain->Begin(); Iter != pActorNodeRenderChain->End() ; Iter.Next() )
		{
			SCENE_SET* pSet = Iter.GetSceneSet();
			if( pSet->empty() )	continue;

			KEY_TYPE key = Iter.GetKey();


			//Set per-chain state
			pActorRenderStrategy->SetMaterialRenderState((RActorNode*)(*pSet->begin()), key.m_pMaterial);


			RActorNode* pInstancingActorNode = NULL;
			int nInstanceCnt = 0;

			for( SCENE_SET::iterator ActorNodeIter = pSet->begin() ; ActorNodeIter != pSet->end() ; ++ActorNodeIter)
			{
				RActorNode* pActorNode = (RActorNode*) (*ActorNodeIter);

				if(pActorNode->CheckDoRender()==false)
					continue;

				if(pActorNode->GetSubMaterialMask(key.m_nSubMaterial)==false)
					continue;

				if( pActorRenderStrategy->IsRender(pCurrentCamera, pActorNode, key.m_nSubMaterial) == false )
					continue;


				if( pActorRenderStrategy->CheckInstancing(pActorNode, key.m_pMaterial) && (pSet->size() > 1) )
					RenderInstancingNode( pActorNode, pCurrentCamera, pActorRenderStrategy, currentTechnique, nInstanceCnt, &pInstancingActorNode, key.m_nSubMaterial);
				else
					RenderNode(pCurrentCamera, pActorRenderStrategy, pActorNode, 1, currentTechnique, key.m_nSubMaterial, false);
			}

			// 인스턴싱 버퍼에 남아있는 것이 있으면 랜더.
			if( nInstanceCnt > 0 && pInstancingActorNode)
			{
				RenderNode(pCurrentCamera, pActorRenderStrategy, pInstancingActorNode, nInstanceCnt, currentTechnique, key.m_nSubMaterial, true);
			}
		}
		RPFC_E;

		if(currentTechnique.m_bUsing)
			EndTechnique( currentTechnique);

		_ASSERT(RTechnique::m_bUsing==false);


		//////////////////////////////////////////////////////////////////////////
		// state 복구
		RestoreForRenderInstancing();
	}
	pActorRenderStrategy->End();	
}

bool RActorPassRenderer::BeginTechnique(RTechnique& currentTechnique, RShaderFX* pShader)
{
	RPFC_THISFUNC;

	if( pShader==NULL )
		return false;
	
	RTechnique technique = pShader->GetTechniqueByIndex(0);
	if( false == technique.IsEqual(currentTechnique) )
	{
		if(currentTechnique.m_bUsing)
		{
			currentTechnique.EndPass();
			currentTechnique.End();
		}

		currentTechnique = technique;

		currentTechnique.Begin(NULL, TECH_PRESERVE_NOT );
		currentTechnique.BeginPass();
	}

	return true;
}


bool RActorPassRenderer::EndTechnique( RTechnique& currentTechnique)
{
	currentTechnique.EndPass();
	currentTechnique.End();
	currentTechnique.m_hTechnique=0;

	return true;
}


void RActorPassRenderer::SetupForRenderInstancing( RActorNodeInstancingBuffer* pInstancingBuffer, UINT nCnt)
{
	RPFC_THISFUNC;
	REngine::GetDevice().SetupForRenderInstancing( pInstancingBuffer ? pInstancingBuffer->GetBuffer() : R_NONE, nCnt);
}


void RActorPassRenderer::RestoreForRenderInstancing()
{
	RPFC_THISFUNC;
	REngine::GetDevice().RestoreForRenderInstancing();
}

bool RActorPassRenderer::CheckCastShadow(RActor* pActor, RShadowStrategy* pShadowStrategy, RCameraSceneNode* pCurrentCamera)
{
	if( pActor->GetAlpha2PassRendering() || pActor->GetNoiseRefResult() < 1)
		return false;

	return pShadowStrategy->IsCastShadow(pCurrentCamera->GetViewMatrix(), pCurrentCamera->GetViewProjectionMatrix(), pActor->GetSphere());
}

void RActorPassRenderer::AddShaderEffectShaderIDForGStage(RActorNode* pActorNode, RSCID& rscComponents)
{
	RActor* pActor = pActorNode->m_pActor;

	bool bPointColor = pActorNode->IsUsePointColor();

	if( pActor->GetShaderEffectController())
	{
		rscComponents = pActor->GetShaderEffectController()->CommitShaderGStage( &rscComponents, bPointColor, pActor->m_bUseShade, pActor->m_fDistanceToCamera );
	}
}

void RActorPassRenderer::AddShaderEffectShaderIDForMStage(RActorNode* pActorNode, RSCID& rscComponents)
{
	RActor* pActor = pActorNode->m_pActor;

	bool bPointColor = pActorNode->IsUsePointColor();

	if( pActor->GetShaderEffectController())
	{
		rscComponents = pActor->GetShaderEffectController()->CommitShaderMStage( &rscComponents, bPointColor, pActor->m_bUseShade);
	}
}


void RActorPassRenderer::RenderTypeOnPass(RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, bool bUseBlending, RRENDERPASS _passType)
{
	// 	TIMEPROFILE_THIS_FUNCTION;
	RPFC_THISFUNC;
	if(!IsRenderEnabled() || !pRenderingList->size())
		return;

	RShaderCompositeManager* pCompositeManager = REngine::GetSceneManagerPtr()->GetShaderCompositeManager();
	RDevice* pDevice = REngine::GetDevicePtr();
	if( !pCompositeManager || !pDevice)
		return;


	REngine::GetResourceProfiler().StartMeshProfiler();

	SCENE_SET::iterator	iter;
	for( iter = pRenderingList->begin() ; iter != pRenderingList->end() ; ++iter)
	{
		RSceneNode *pSceneNode = (*iter);
		if( !pSceneNode->GetVisible() || !pSceneNode->GetUsable() || pSceneNode->GetOcclued() )	continue;

		RActor* pActor = MDynamicCast( RActor, pSceneNode);

		// 액터 노드 그리기
		for(size_t i=0;i<pActor->m_sortedRenderableActorNode.size();++i)
		{
			RActorNode* pActorNode = pActor->m_sortedRenderableActorNode[i];
			if( !pActorNode->IsRenderable())
				continue;

			pActorNode->SetVertexStream();

			for (int index = 0; index < pActorNode->m_pMeshNode->m_nMtrlCnt ; ++index)
			{
				if( pActorNode->GetSubMaterialMask(index) == false )
					continue;


				RSCID rscComponents = pCompositeManager->MakeComponentBitflagForMStage(pActorNode->GetMaskingPassComponentDesc(index));
				UpdateShaderIDForMStage( rscComponents, false, false, false, pActorNode->m_pActor->IsUseMultiplyColor() );

				/// 픽셀 모션 블러나 누적 버퍼 블러 시는 마스킹 용도이므로 특수 효과는 적용 되지 않을 것이고, 버텍스 모션 블러 시는 특수 효과를 적용 하지 않기로 하자.
				/// 그러므로 AddShaderEffectShaderIDForMStage()는 호출하지 않는다.

				if( RRP_POSTEFFECT_EDGE == _passType )	
				{
					// 알파 옵션이 있는 오브젝트에는 외곽선이 생기면 안된다.
					if( pActorNode->IsAlpha() || pActorNode->HasAlphaMaterial())
					{
						continue;
					}
				}

				RShaderFX* pShader = pCompositeManager->GetShader( rscComponents, true);
				if( NULL == pShader )
					continue;

				// 쉐이더
				pCompositeManager->SetMaterialRenderState( pActorNode->GetSubMaterial(index), false, false, false );
				pActorNode->SetNodeRenderState( pCurrentCamera->GetViewMatrix(), pCurrentCamera->GetViewProjectionMatrix() );

				switch( _passType )
				{
				case RRP_VBLUR:
					pActorNode->SetNodeRenderStateForBlur( pCurrentCamera);
					break;

				case RRP_POSTEFFECT_EDGE:
					{
						pDevice->SetStencilEnable(true);
						pDevice->SetStencilOp(RSTENCILOP_KEEP, RSTENCILOP_KEEP, RSTENCILOP_INCR);

						RShaderCompositeManager::m_cPEEdgeColor.SetVector3( pActor->GetEdgeColor() );
					}
					break;

				case RRP_EDGE:
					{
						REngine::GetDevice().SetCullMode(RCULL_CCW);	// 뒤집어서 그린다.

						//float fTickNess = thickness X camera dist X fovx / screen width;
						RVector vPos = pActorNode->GetWorldPosition();
						float fDist = pCurrentCamera->GetDistanceTo( vPos );

						RVector2 TickNess;
						TickNess.x = pActor->GetEdgeTickNess() * fDist * pCurrentCamera->GetFovX() / (float)REngine::GetDevice().GetScreenWidth();
						TickNess.y = pActor->GetEdgeTickNess() * fDist * pCurrentCamera->GetFovY() / (float)REngine::GetDevice().GetScreenHeight();
						RShaderCompositeManager::m_cTickNess.SetVector2( TickNess );
					}
					break;
				}


				if(bUseBlending)
					pDevice->SetBlending( RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);
				else
					pDevice->SetBlending( RBLEND_NONE, RBLEND_NONE);


				if( _passType == RRP_POSTEFFECT_EDGE && pActor->GetPEEdgeProjection() )
					pDevice->SetDepthEnable(false, false);
				else
					pDevice->SetDepthEnable(true, false);


				// 쉐이더 테크닉 얻기	
				RTechnique technique = pShader->GetTechniqueByIndex(0);
				technique.Begin(NULL, TECH_PRESERVE_NOT);
				technique.BeginPass();
				pActorNode->RenderPrimitive(index);
				technique.EndPass();
				technique.End();
			}
		}
	}

	// 렌더 스테이트 복구
	pDevice->SetDepthEnable(true, true);
	pDevice->SetBlending(RBLEND_NONE);
	pDevice->SetCullMode(RCULL_CW);

	pDevice->SetStencilEnable(false);	
	pDevice->SetStencilOp(RSTENCILOP_KEEP, RSTENCILOP_KEEP, RSTENCILOP_KEEP);
}

}
