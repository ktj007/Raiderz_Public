#include "StdAfx.h"
#include "RActorNodeRenderStrategy.h"
#include "RActorNode.h"
#include "RShaderCompositeManager.h"
#include "RPostEffectDistortion.h"
#include "RShadowStrategy.h"
#include "RDirectionalLightSceneNode.h"

namespace rs3
{

void RActorNodeRenderStrategy::SetMaterialRenderState(RActorNode* pActorNode, RMaterial* pMaterial)
{
	RPFC_THISFUNC;
	_ASSERT(pActorNode);
	_ASSERT(pMaterial);

	REngine::GetSceneManagerPtr()->GetShaderCompositeManager()->SetMaterialRenderState( pMaterial, true, true, pActorNode->IsAlpha() );
}

void RActorNodeRenderStrategy::SetDiffuseStatePerNode(RActorNode* pActorNode, RMaterial* pMaterial)
{
	pActorNode->SetBlendMaterialRenderState();

	if(pMaterial->GetCompositeLayer(RTT_DIFFUSE)==NULL)
		RShaderCompositeManager::SetMaterialDecalRenderState( pActorNode->m_pActor->GetMaterialDecal()->GetDecalLayer(pMaterial->GetName()) );
}

void RActorNodeRenderStrategy::SetNodeRenderState( RCameraSceneNode* pCurrentCamera, RActorNode* pActorNode, RMaterial* pMaterial )
{
	_ASSERT(pCurrentCamera);
	_ASSERT(pActorNode);
	_ASSERT(pMaterial);

	pActorNode->SetNodeRenderState( pCurrentCamera->GetViewMatrix(), pCurrentCamera->GetViewProjectionMatrix() );


	SetDiffuseStatePerNode(pActorNode, pMaterial);
}

void RActorNodeRenderStrategy::End()
{	
	RPFC_THISFUNC;
	REngine::GetDevice().SetCullMode(RCULL_CW);
	REngine::GetDevice().SetBlending(RBLEND_NONE);
}

RShaderFX* RActorNodeRenderStrategy::GetShaderForStage(RActorNode* pActorNode, int nMtrIndex, bool bMstage)
{
	_ASSERT(pActorNode);

	RShaderCompositeManager* pShaderCompositeManager = REngine::GetSceneManagerPtr()->GetShaderCompositeManager();
	RShaderFX* pShaderGStage = pShaderCompositeManager->GetShader( pActorNode->GetShaderIDGStage(nMtrIndex), REngine::GetConfig().m_bBackgroundCreateShaderComposite );
	RShaderFX* pShaderMStage = pShaderCompositeManager->GetShader( pActorNode->GetShaderIDMStage(nMtrIndex), REngine::GetConfig().m_bBackgroundCreateShaderComposite );

	if( NULL == pShaderGStage || NULL == pShaderMStage )
		return NULL;

	return bMstage ? pShaderMStage : pShaderGStage;
}

bool RActorNodeRenderStrategy::CheckInstancing(RActorNode* pActorNode, RMaterial* pMaterial)
{
	_ASSERT(pActorNode);

	bool bInstancing = pActorNode->CheckInstancingCondition();

	bInstancing &= (pActorNode->m_pActor->GetMaterialDecal()->HaveDecalLayer(pMaterial->GetName())==false);

	return bInstancing;
}


//////////////////////////////////////////////////////////////////////////
// 2-pass alpha render strategies

void RActorNodeRenderStrategyAlphaDepthOnly::SetMaterialRenderState(RActorNode* pActorNode, RMaterial* pMaterial)
{
	_ASSERT(pActorNode);
	_ASSERT(pMaterial);

	RPFC_THISFUNC;

	RShaderCompositeManager::SetCullMode(pMaterial->CheckFlag(RM_FLAG_TWOSIDED));
	//Because ColorWrite is disable, Blending setting is not necessary.


	if(pMaterial->CheckFlag(RM_FLAG_USEALPHATEST))
	{
		RShaderCompositeManager::SetTextureByType(pMaterial->GetTextureTypeForAlphaUsage(ALPHAUSAGE_ALPHA), pMaterial);

		RShaderCompositeManager::m_cAlphaRef.SetFloat( ceilf(pMaterial->GetAlphaRefValue() *pActorNode->GetVisibilityResult()) / 255.0f);
	}

}
void RActorNodeRenderStrategyAlphaDepthOnly::Begin()
{
	REngine::GetDevice().SetColorWriteEnable( false );
}

void RActorNodeRenderStrategyAlphaDepthOnly::End()
{
	RPFC_THISFUNC;
	RActorNodeRenderStrategy::End();

	REngine::GetDevice().SetColorWriteEnable( true );
}

RShaderFX* RActorNodeRenderStrategyAlphaDepthOnly::GetShader(RActorNode* pActorNode, int nMtrIndex, bool bAbleInstancing)
{
	RPFC_THISFUNC;
	_ASSERT(pActorNode);
	_ASSERT(nMtrIndex>=0);

	pActorNode->CheckAndCreateTechniqueTableOnMaterialForMStage(RENDERING_INFERRED, nMtrIndex, bAbleInstancing, pActorNode->IsApplyNoise(), REngine::GetConfig().m_bBackgroundCreateShaderComposite);
	return GetShaderForStage(pActorNode, nMtrIndex, true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// for M Stage render strategy
void RActorNodeRenderStrategyAlpha2PassMStage::SetMaterialRenderState(RActorNode* pActorNode, RMaterial* pMaterial)
{
	_ASSERT(pActorNode);
	_ASSERT(pMaterial);

	RPFC_THISFUNC;

	//Set Render State
	{
		RShaderCompositeManager::SetCullMode(pMaterial->CheckFlag(RM_FLAG_TWOSIDED));
		RShaderCompositeManager::SetBlending(pMaterial->GetFlag(), pActorNode->IsAlpha());
	}


	//Set Texture
	{
		RShaderCompositeManager::SetTextureByType(RTT_DIFFUSE, pMaterial);
		RShaderCompositeManager::SetTextureByType(RTT_SELFILLUMINATION, pMaterial);

		if(pMaterial->IsUseAlpha() && pMaterial->GetTextureTypeForAlphaUsage(ALPHAUSAGE_ALPHA)!=RTT_DIFFUSE)
			RShaderCompositeManager::SetTextureByType(pMaterial->GetTextureTypeForAlphaUsage(ALPHAUSAGE_ALPHA), pMaterial);
	}

	//Set Shader Constant
	REngine::GetSceneManager().GetShaderCompositeManager()->SetMaterialShaderConstant(pMaterial, false, pActorNode->GetVisibilityResult(), pActorNode->GetIlluminationScale());
	RShaderCompositeManager::SetFogColorForBlending(pMaterial->CheckFlag(RM_FLAG_ADDITIVE));
}

void RActorNodeRenderStrategyAlpha2PassMStage::Begin()
{
	RPFC_THISFUNC;
	REngine::GetDevice().SetDepthEnable( true, false );
	REngine::GetDevice().SetDepthFunc( RCMP_EQUAL );
}

void RActorNodeRenderStrategyAlpha2PassMStage::End()
{
	RPFC_THISFUNC;
	RActorNodeRenderStrategy::End();

	REngine::GetDevice().SetDepthEnable( true, true );
	REngine::GetDevice().SetDepthFunc( RCMP_LESSEQUAL );
	RShaderCompositeManager::RestoreFogColor();
}

RShaderFX* RActorNodeRenderStrategyAlpha2PassMStage::GetShader(RActorNode* pActorNode, int nMtrIndex, bool bAbleInstancing)
{
	RPFC_THISFUNC;
	_ASSERT(pActorNode);
	_ASSERT(nMtrIndex>=0);

	pActorNode->CheckAndCreateTechniqueTableOnMaterialForMStage(RENDERING_INFERRED, nMtrIndex, bAbleInstancing, pActorNode->IsApplyNoise(), REngine::GetConfig().m_bBackgroundCreateShaderComposite);
	return GetShaderForStage(pActorNode, nMtrIndex, true);
}

void RActorNodeRenderStrategyAlpha2PassMStage::ApplyDistortion(RCameraSceneNode* pCamera, RActorNode* pActorNode, int nMtrIndex)
{
	_ASSERT(pCamera);
	_ASSERT(pActorNode);

	RPostEffectDistortion* pPostEffectDistortion = (RPostEffectDistortion*)REngine::GetSceneManager().GetPostEffect(PE_DISTORTION);
	if( pPostEffectDistortion==NULL || pPostEffectDistortion->IsValid()==false)
		return;

	if( pActorNode->m_pActor->IsDistortion() && pPostEffectDistortion->IsCulledVisible( pActorNode->m_pActor, pCamera->GetWorldPosition()))
	{
		pPostEffectDistortion->RenderActorNodeDistortion( pCamera, pActorNode, nMtrIndex );
	}
}


//////////////////////////////////////////////////////////////////////////
// Depth render strategy
bool RActorNodeRenderStrategyShadowMap::IsRender(RCameraSceneNode* pCamera, RActorNode* pActorNode, int nMtrIndex)
{
	RPFC_THISFUNC;
	_ASSERT(pCamera);
	_ASSERT(pActorNode);

	RActor* pActor = pActorNode->m_pActor;
	_ASSERT(pActor);

	if( IsRenderReceiver(pActor->IsShadowReceiver())==false )
		return false;


	if(m_pShadowStrategy->IsCastShadow(pCamera->GetViewMatrix(), pCamera->GetViewProjectionMatrix(), pActor->GetSphere()) ==false)
		return false;


	return true;
}

bool RActorNodeRenderStrategyShadowMap::IsRenderReceiver(bool bReceiver)
{
	return bReceiver==false;
}

RShaderFX* RActorNodeRenderStrategyShadowMap::GetShader(RActorNode* pActorNode, int nMtrIndex, bool bAbleInstancing)
{
	_ASSERT(pActorNode);

	RPFC_THISFUNC;

	RSCID rscComponents = pActorNode->GetBaseComponent() | REngine::GetSceneManager().GetShaderCompositeManager()->GetVertexTexcoordComponent() | m_pShadowStrategy->GetDepthWriteComponent();

	if(bAbleInstancing)
		rscComponents |= RSC_INSTANCING;

	if( !REngine::m_bUseSkinning)
		rscComponents &= ~RSC_PHYSIQUE;

	bool bAlphaTest = (nMtrIndex>=0) && pActorNode->GetSubMaterial(nMtrIndex)->CheckFlag(RM_FLAG_USEALPHATEST);
	if(bAlphaTest)
		rscComponents |= RSC_PPL_CLIP_ALPHA;	

	return REngine::GetSceneManagerPtr()->GetShaderCompositeManager()->GetShader(rscComponents, REngine::GetConfig().m_bBackgroundCreateShaderComposite);
}

void RActorNodeRenderStrategyShadowMap::SetMaterialRenderState(RActorNode* pActorNode, RMaterial* pMaterial)
{
	_ASSERT(pActorNode);
	_ASSERT(pMaterial);
	_ASSERT(pMaterial->CheckFlag(RM_FLAG_USEOPACITY)==false);

	RPFC_THISFUNC;

	if(pMaterial->CheckFlag(RM_FLAG_USEALPHATEST))
	{
		RShaderCompositeManager::SetTexture(AS_DIFFUSE, pMaterial->GetTexLayerByType(pMaterial->GetTextureTypeForAlphaUsage(ALPHAUSAGE_ALPHA)));

		_ASSERT(pActorNode->GetVisibilityResult()==1);
		RShaderCompositeManager::m_cAlphaRef.SetFloat( ceilf(pMaterial->GetAlphaRefValue()*pActorNode->GetVisibilityResult()) / 255.0f);
	}
	else
	{
		RShaderCompositeManager::SetTexture(AS_DIFFUSE, NULL);
	}
}

void RActorNodeRenderStrategyShadowMap::SetNodeRenderState(  RCameraSceneNode* pCurrentCamera, RActorNode* pActorNode, RMaterial* pMaterial )
{
	_ASSERT(pCurrentCamera);
	_ASSERT(pActorNode);

	RPFC_THISFUNC;

	pActorNode->SetShaderTransformConstant(pCurrentCamera->GetViewMatrix(), pCurrentCamera->GetViewProjectionMatrix());
}

void RActorNodeRenderStrategyShadowMap::Begin()
{
	RPFC_THISFUNC;
	REngine::GetDevice().SetCullMode(RCULL_NONE);
}


bool RActorNodeRenderStrategyProjectionShadowMap::IsRenderReceiver(bool bReceiver)
{
	return true;
}

void RActorNodeRenderStrategyProjectionShadowMap::SetNodeRenderState(  RCameraSceneNode* pCurrentCamera, RActorNode* pActorNode, RMaterial* pMaterial )
{
	_ASSERT(pCurrentCamera);
	_ASSERT(pActorNode);

	pActorNode->SetShaderTransformConstant(pCurrentCamera->GetViewMatrix(), pCurrentCamera->GetViewProjectionMatrix());


	if(pActorNode->m_pActor->IsShadowReceiver())
	{
		RShaderCompositeManager::m_cShadowValue.SetFloat(1);
	}
	else
	{
		ResetNodeRenderState();
	}
}

void RActorNodeRenderStrategyProjectionShadowMap::ResetNodeRenderState()
{
	RPFC_THISFUNC;
	float fShadowValue = DEFAULT_SHADOW_LUMINOSITY;

	if(REngine::GetSceneManager().GetDefaultLight())
		fShadowValue = REngine::GetSceneManager().GetDefaultLight()->GetShadowLuminosity();

	RShaderCompositeManager::m_cShadowValue.SetFloat(fShadowValue);
}

bool RActorNodeRenderStrategyProjectionShadowMap::CheckInstancing(RActorNode* pActorNode, RMaterial* pMaterial)
{
	if(pActorNode->m_pActor->IsShadowReceiver())
		return false;

	return pActorNode->CanInstancing();
}


////////////////////////// DEFERRED ////////////////////////////////////////////////

// G Stage render strategy


void RActorNodeRenderStrategyGStageDeferred::SetTextureByEnvironment( RActorNode* _pActorNode, RMaterial* _pMaterial )
{
	// RTT_SPECULAR 부분이 RTT_ENVIRONMENT로 바껴야 한다.
	RMtrlTexInfo* pTexInfo = _pMaterial->GetTexLayerByType( RTT_REFLECT );	// 타입에 해당하는 텍스쳐가 있어야 한다.
	if(pTexInfo == NULL)
		return;

	RTexture* pTexture = _pActorNode->m_pActor->GetEnvironmentTexture();

	if( !pTexture )
	{
		// 기본 텍스쳐.
		_pActorNode->m_pActor->SetEnvironmentTexture( "grassenvmap1024.dds" );
		pTexture = _pActorNode->m_pActor->GetEnvironmentTexture();
	}

	REngine::GetDevice().SetTexture( AS_REFLECT, pTexture );
	REngine::GetDevice().SetTextureFilter( AS_REFLECT, RTF_BILINEAR );
	REngine::GetDevice().SetTextureAddress( AS_REFLECT, pTexInfo->GetTextureAddressU(), pTexInfo->GetTextureAddressV(), pTexInfo->GetTextureAddressU());

}

void RActorNodeRenderStrategyGStageDeferred::SetMaterialRenderState(RActorNode* pActorNode, RMaterial* pMaterial)
{
	_ASSERT(pActorNode);
	_ASSERT(pMaterial);

	RPFC_THISFUNC;

	//Set Render State
	{
		RShaderCompositeManager::SetCullMode(pMaterial->CheckFlag(RM_FLAG_TWOSIDED));

		_ASSERT(!pMaterial->CheckFlag(RM_FLAG_USEOPACITY) && !pActorNode->IsAlpha());
	}


	//Set Texture
	{
		RShaderCompositeManager::SetTextureByType(RTT_DIFFUSE, pMaterial);
		RShaderCompositeManager::SetTextureByType(RTT_SPECULAR, pMaterial);
		SetTextureByEnvironment( pActorNode, pMaterial );

		if( REngine::GetSceneManager().GetShaderCompositeManager()->GetUseNormalMap() || 
			pMaterial->GetTextureTypeForAlphaUsage(ALPHAUSAGE_ALPHA)==RTT_NORMAL)
			RShaderCompositeManager::SetTextureByType(RTT_NORMAL, pMaterial);

		// 매터리얼 별로 노이즈 텍스쳐가 따로 있는 경우를 대비하여 이곳에서 노이즈 텍스쳐를 셋팅한다. 
		// 중복 체크가 되므로 매번 셋팅 하여도 상관 없을 것이다.
		REngine::GetDevice().SetTexture( AS_NOISEMAP, REngine::GetDevice().GetDefaultNoiseTexture());
	}


	//Set Shader Constant
	REngine::GetSceneManager().GetShaderCompositeManager()->SetMaterialShaderConstant(pMaterial, true, pActorNode->GetVisibilityResult(), pActorNode->GetIlluminationScale());
}

void RActorNodeRenderStrategyGStageDeferred::SetNodeRenderState(RCameraSceneNode* pCurrentCamera, RActorNode* pActorNode, RMaterial* pMaterial )
{
	RPFC_THISFUNC;
	RActorNodeRenderStrategy::SetNodeRenderState(pCurrentCamera, pActorNode, pMaterial);

	if(REngine::GetSceneManager().GetRealtimeShadowMethod()==SHADOW_PROJECTION)
	{
		if(pActorNode->m_pActor->IsShadowReceiver())
			REngine::GetDevice().SetStencilOp(RSTENCILOP_INCR,RSTENCILOP_KEEP,RSTENCILOP_KEEP);
		else
			ResetNodeRenderState();
	}
}

void RActorNodeRenderStrategyGStageDeferred::ResetNodeRenderState()
{
	RPFC_THISFUNC;
	REngine::GetDevice().SetStencilOp(RSTENCILOP_ZERO,RSTENCILOP_KEEP,RSTENCILOP_KEEP);
}

void RActorNodeRenderStrategyGStageDeferred::End()
{
	if(REngine::GetSceneManager().GetRealtimeShadowMethod()==SHADOW_PROJECTION)
		REngine::GetDevice().SetStencilOp(RSTENCILOP_KEEP,RSTENCILOP_KEEP,RSTENCILOP_KEEP);	
}

RShaderFX* RActorNodeRenderStrategyGStageDeferred::GetShader(RActorNode* pActorNode, int nMtrIndex, bool bAbleInstancing)
{
	RPFC_THISFUNC;
	_ASSERT(pActorNode);
	_ASSERT(nMtrIndex>=0);

	pActorNode->CheckAndCreateTechniqueTableOnMaterialForGStage(RENDERING_DEFERRED, nMtrIndex, bAbleInstancing, pActorNode->IsApplyNoise(), REngine::GetConfig().m_bBackgroundCreateShaderComposite);
	return GetShaderForStage(pActorNode, nMtrIndex, false);
}

bool RActorNodeRenderStrategyGStageDeferred::CheckInstancing(RActorNode* pActorNode, RMaterial* pMaterial)
{
	_ASSERT(pActorNode);

	bool bInstancing = RActorNodeRenderStrategy::CheckInstancing(pActorNode, pMaterial);

	if(REngine::GetSceneManager().GetRealtimeShadowMethod()==SHADOW_PROJECTION)
	{
		if(pActorNode->m_pActor->IsShadowReceiver())
			bInstancing = false;
	}

	return bInstancing;
}



void RActorNodeRenderStrategyDiffuseOnly::SetMaterialRenderState(RActorNode* pActorNode, RMaterial* pMaterial)
{
	RPFC_THISFUNC;
	_ASSERT(pActorNode && pMaterial);

	//Set Render State
	{
		RShaderCompositeManager::SetCullMode(pMaterial->CheckFlag(RM_FLAG_TWOSIDED));

		_ASSERT(!pMaterial->CheckFlag(RM_FLAG_USEOPACITY) && !pActorNode->IsAlpha());
	}


	//Set Texture
	{
		RShaderCompositeManager::SetTextureByType(RTT_DIFFUSE, pMaterial);

		if(pMaterial->CheckFlag(RM_FLAG_USEALPHATEST) && pMaterial->GetTextureTypeForAlphaUsage(ALPHAUSAGE_ALPHA)!=RTT_DIFFUSE)
			RShaderCompositeManager::SetTextureByType(pMaterial->GetTextureTypeForAlphaUsage(ALPHAUSAGE_ALPHA), pMaterial);


		// 매터리얼 별로 노이즈 텍스쳐가 따로 있는 경우를 대비하여 이곳에서 노이즈 텍스쳐를 셋팅한다. 
		// 중복 체크가 되므로 매번 셋팅 하여도 상관 없을 것이다.
		REngine::GetDevice().SetTexture( AS_NOISEMAP, REngine::GetDevice().GetDefaultNoiseTexture());
	}


	//Set Shader Constant
	REngine::GetSceneManager().GetShaderCompositeManager()->SetMaterialShaderConstant(pMaterial, false, pActorNode->GetVisibilityResult(), pActorNode->GetIlluminationScale());
}

RShaderFX* RActorNodeRenderStrategyDiffuseOnly::GetShader(RActorNode* pActorNode, int nMtrIndex, bool bAbleInstancing)
{
	_ASSERT(pActorNode);
	_ASSERT(nMtrIndex>=0);

	RPFC_THISFUNC;

	RMaterial* pMtrl = pActorNode->GetSubMaterial(nMtrIndex);
	_ASSERT(pMtrl);

	RShaderCompositeManager* pShaderCompositeManager = REngine::GetSceneManager().GetShaderCompositeManager();


	RSCID rscComponents = pActorNode->GetBaseComponent() | pShaderCompositeManager->GetVertexTexcoordComponent() | RSC_TRANSFORM_CONSTANT | RSC_TRANSFORM;

	if(bAbleInstancing)
		rscComponents |= RSC_INSTANCING;

	if( !REngine::m_bUseSkinning)
		rscComponents &= ~RSC_PHYSIQUE;

	rscComponents |= pShaderCompositeManager->GetDiffuseComponent( pMtrl);

	pShaderCompositeManager->ApplyAlphaComponent( rscComponents, pMtrl, pActorNode->IsUsePointColor());

	if( pMtrl && pMtrl->CheckFlag(RM_FLAG_USEPARTSCOLOR) && pActorNode->IsUseTexCoord())
	{
		rscComponents |= RSC_DIFFUSE_PARTSCOLOR_CONSTANT | RSC_DIFFUSE_PARTSCOLOR;
		if( bAbleInstancing)
			rscComponents |= RSC_INSTANCE_PARTSCOLOR;
	}
	// 라이팅이 먹지 않는 상황에서만 텍스쳐 팩터를 적용.
	if( pActorNode->m_pActor && pActorNode->m_pActor->IsUseMultiplyColor())
		rscComponents |= RSC_FACTOR;
	
	return pShaderCompositeManager->GetShader(rscComponents, REngine::GetConfig().m_bBackgroundCreateShaderComposite);
}



void RActorNodeRenderStrategyAlpha::SetMaterialRenderState(RActorNode* pActorNode, RMaterial* pMaterial)
{
	_ASSERT(pActorNode && pMaterial);


	RPFC_THISFUNC;


	//Set Render State
	{
		RShaderCompositeManager::SetCullMode(pMaterial->CheckFlag(RM_FLAG_TWOSIDED));

		_ASSERT(pMaterial->CheckFlag(RM_FLAG_USEOPACITY) || pActorNode->IsAlpha());
		RShaderCompositeManager::SetBlending(pMaterial->GetFlag(), pActorNode->IsAlpha());
	}


	//Set Texture
	{
		RShaderCompositeManager::SetTextureByType(RTT_DIFFUSE, pMaterial);
		RShaderCompositeManager::SetTextureByType(RTT_SELFILLUMINATION, pMaterial);

		if(pMaterial->GetTextureTypeForAlphaUsage(ALPHAUSAGE_ALPHA) != RTT_DIFFUSE)
			RShaderCompositeManager::SetTextureByType(pMaterial->GetTextureTypeForAlphaUsage(ALPHAUSAGE_ALPHA), pMaterial);


		// 매터리얼 별로 노이즈 텍스쳐가 따로 있는 경우를 대비하여 이곳에서 노이즈 텍스쳐를 셋팅한다. 
		// 중복 체크가 되므로 매번 셋팅 하여도 상관 없을 것이다.
		REngine::GetDevice().SetTexture( AS_NOISEMAP, REngine::GetDevice().GetDefaultNoiseTexture());
	}


	//Set Shader Constant
	REngine::GetSceneManager().GetShaderCompositeManager()->SetMaterialShaderConstant(pMaterial, false, pActorNode->GetVisibilityResult(), pActorNode->GetIlluminationScale());
	RShaderCompositeManager::SetFogColorForBlending(pMaterial->CheckFlag(RM_FLAG_ADDITIVE));
}

void RActorNodeRenderStrategyAlpha::Begin()
{
	RPFC_THISFUNC;
	REngine::GetDevice().SetDepthEnable(true, false);
}

void RActorNodeRenderStrategyAlpha::End()
{
	RPFC_THISFUNC;
	RActorNodeRenderStrategy::End();

	REngine::GetDevice().SetDepthEnable(true, true);
	RShaderCompositeManager::RestoreFogColor();
}

RShaderFX* RActorNodeRenderStrategyAlpha::GetShader(RActorNode* pActorNode, int nMtrIndex, bool bAbleInstancing)
{
	RPFC_THISFUNC;
	_ASSERT(pActorNode);
	_ASSERT(nMtrIndex>=0);

	pActorNode->CheckAndCreateTechniqueTableOnMaterialForMStage(RENDERING_INFERRED, nMtrIndex, bAbleInstancing, pActorNode->IsApplyNoise(), REngine::GetConfig().m_bBackgroundCreateShaderComposite);
	return GetShaderForStage(pActorNode, nMtrIndex, true);
}

void RActorNodeRenderStrategyAlpha::ApplyDistortion(RCameraSceneNode* pCamera, RActorNode* pActorNode, int nMtrIndex)
{
	RPFC_THISFUNC;
	_ASSERT(pCamera);
	_ASSERT(pActorNode);

	RPostEffectDistortion* pPostEffectDistortion = (RPostEffectDistortion*)REngine::GetSceneManager().GetPostEffect(PE_DISTORTION);
	if( pPostEffectDistortion==NULL || pPostEffectDistortion->IsValid()==false)
		return;

	if( pActorNode->m_pActor->IsDistortion() && pPostEffectDistortion->IsCulledVisible( pActorNode->m_pActor, pCamera->GetWorldPosition()))
	{
		pPostEffectDistortion->RenderActorNodeDistortion( pCamera, pActorNode, nMtrIndex );
	}
}

///////////////////////////////////// ADD COLOR /////////////////////////////////////
/// 디퍼드의 매터리얼 스테이지는 ADD color패스이다.
void RActorNodeRenderStrategyMStageAddColor::SetMaterialRenderState(RActorNode* pActorNode, RMaterial* pMaterial) 
{
	_ASSERT(pActorNode);
	_ASSERT(pMaterial);

	RPFC_THISFUNC;

	//Set Render State
	{
		RShaderCompositeManager::SetCullMode(pMaterial->CheckFlag(RM_FLAG_TWOSIDED));

		if(pActorNode->IsNoShade())
			REngine::GetDevice().SetBlending(RBLEND_NONE);
		else
			REngine::GetDevice().SetBlending(RBLEND_ONE, RBLEND_ONE);	
	}


	//Set Texture
	{
		RShaderCompositeManager::SetTextureByType(RTT_SELFILLUMINATION, pMaterial, RTF_BILINEAR);

		if(pActorNode->IsNoShade())
		{
			RShaderCompositeManager::SetTextureByType(RTT_DIFFUSE, pMaterial);
			if(pMaterial->CheckFlag(RM_FLAG_USEALPHATEST) && pMaterial->GetTextureTypeForAlphaUsage(ALPHAUSAGE_ALPHA)!=RTT_DIFFUSE)
				RShaderCompositeManager::SetTextureByType(pMaterial->GetTextureTypeForAlphaUsage(ALPHAUSAGE_ALPHA), pMaterial);
		}

		REngine::GetDevice().SetTexture( AS_NOISEMAP, REngine::GetDevice().GetDefaultNoiseTexture());
	}	


	//Set Shader Constant
	REngine::GetSceneManager().GetShaderCompositeManager()->SetMaterialShaderConstant(pMaterial, false, pActorNode->GetVisibilityResult(), pActorNode->GetIlluminationScale());
	RShaderCompositeManager::SetFogColorForBlending(!pActorNode->IsNoShade());
}

void RActorNodeRenderStrategyMStageAddColor::SetNodeRenderState( RCameraSceneNode* pCurrentCamera, RActorNode* pActorNode, RMaterial* pMaterial )
{
	_ASSERT(pCurrentCamera && pActorNode && pMaterial);

	pActorNode->SetNodeRenderState( pCurrentCamera->GetViewMatrix(), pCurrentCamera->GetViewProjectionMatrix() );


	if(pActorNode->IsNoShade())
	{
		SetDiffuseStatePerNode(pActorNode, pMaterial);
	}
}

void RActorNodeRenderStrategyMStageAddColor::Begin()
{
	REngine::GetDevice().SetDepthEnable(true, false);
	REngine::GetDevice().SetDepthFunc( RCMP_EQUAL );
}

void RActorNodeRenderStrategyMStageAddColor::End()
{
	RPFC_THISFUNC;

	RActorNodeRenderStrategy::End();

	REngine::GetDevice().SetDepthEnable(true, true);
	REngine::GetDevice().SetDepthFunc( RCMP_LESSEQUAL );
	RShaderCompositeManager::RestoreFogColor();
}

bool RActorNodeRenderStrategyMStageAddColor::IsRender(RCameraSceneNode* pCamera, RActorNode* pActorNode, int nMtrIndex)
{
	RPFC_THISFUNC;
	_ASSERT(pActorNode);

	if( !pActorNode->IsUseAddColor( pActorNode->GetSubMaterial(nMtrIndex) ) && 
		!pActorNode->IsNoShade() &&
		!pActorNode->GetSubMaterial(nMtrIndex)->GetTexLayerByType(RTT_REFLECT) )
		return false;

	return true;
}

RShaderFX* RActorNodeRenderStrategyMStageAddColor::GetShader(RActorNode* pActorNode, int nMtrIndex, bool bAbleInstancing)
{
	RPFC_THISFUNC;
	_ASSERT(pActorNode);
	_ASSERT(nMtrIndex>=0);

	pActorNode->CheckAndCreateTechniqueTableOnMaterialForMStage(RENDERING_DEFERRED, nMtrIndex, bAbleInstancing, pActorNode->IsApplyNoise(), REngine::GetConfig().m_bBackgroundCreateShaderComposite);
	return GetShaderForStage(pActorNode, nMtrIndex, true);
}




void RActorNodeRenderStrategyGStageInferred::SetMaterialRenderState( RActorNode* pActorNode, RMaterial* pMaterial)
{
	_ASSERT(pActorNode);
	_ASSERT(pMaterial);

	RPFC_THISFUNC;

	//Set Render State
	{
		RShaderCompositeManager::SetCullMode(pMaterial->CheckFlag(RM_FLAG_TWOSIDED));

		_ASSERT(pMaterial->CheckFlag(RM_FLAG_USEOPACITY) && !pActorNode->IsAlpha());
	}


	//Set Texture
	{
		if(REngine::GetSceneManager().GetShaderCompositeManager()->GetUseNormalMap() || pMaterial->GetTextureTypeForAlphaUsage(ALPHAUSAGE_ALPHA)==RTT_NORMAL)
			RShaderCompositeManager::SetTextureByType(RTT_NORMAL, pMaterial);

		RShaderCompositeManager::SetTextureByType(RTT_SPECULAR, pMaterial);

		if( pMaterial->CheckFlag(RM_FLAG_USEALPHATEST) && pMaterial->GetTextureTypeForAlphaUsage(ALPHAUSAGE_ALPHA)==RTT_DIFFUSE)
			RShaderCompositeManager::SetTextureByType(pMaterial->GetTextureTypeForAlphaUsage(ALPHAUSAGE_ALPHA), pMaterial);

		// 매터리얼 별로 노이즈 텍스쳐가 따로 있는 경우를 대비하여 이곳에서 노이즈 텍스쳐를 셋팅한다. 
		// 중복 체크가 되므로 매번 셋팅 하여도 상관 없을 것이다.
		REngine::GetDevice().SetTexture( AS_NOISEMAP, REngine::GetDevice().GetDefaultNoiseTexture());
	}


	//Set Shader constant
	REngine::GetSceneManager().GetShaderCompositeManager()->SetMaterialShaderConstant(pMaterial, true, pActorNode->GetVisibilityResult(), pActorNode->GetIlluminationScale());
}

void RActorNodeRenderStrategyGStageInferred::SetNodeRenderState(RCameraSceneNode* pCurrentCamera, RActorNode* pActorNode, RMaterial* pMaterial )
{
	RPFC_THISFUNC;
	_ASSERT(pCurrentCamera);
	_ASSERT(pActorNode);

	pActorNode->SetNodeRenderState( pCurrentCamera->GetViewMatrix(), pCurrentCamera->GetViewProjectionMatrix() );

	if(REngine::GetSceneManager().GetRealtimeShadowMethod()==SHADOW_PROJECTION)
	{
		if(pActorNode->m_pActor->IsShadowReceiver())
			REngine::GetDevice().SetStencilOp(RSTENCILOP_INCR,RSTENCILOP_KEEP,RSTENCILOP_KEEP);
		else
			ResetNodeRenderState();
	}
}

void RActorNodeRenderStrategyGStageInferred::ResetNodeRenderState()
{
	RPFC_THISFUNC;
	REngine::GetDevice().SetStencilOp(RSTENCILOP_ZERO,RSTENCILOP_KEEP,RSTENCILOP_KEEP);
}

void RActorNodeRenderStrategyGStageInferred::End()
{
	if(REngine::GetSceneManager().GetRealtimeShadowMethod()==SHADOW_PROJECTION)
		REngine::GetDevice().SetStencilOp(RSTENCILOP_KEEP,RSTENCILOP_KEEP,RSTENCILOP_KEEP);	
}

RShaderFX* RActorNodeRenderStrategyGStageInferred::GetShader(RActorNode* pActorNode, int nMtrIndex, bool bAbleInstancing)
{
	RPFC_THISFUNC;
	_ASSERT(pActorNode);
	_ASSERT(nMtrIndex>=0);

	pActorNode->CheckAndCreateTechniqueTableOnMaterialForGStage(RENDERING_INFERRED, nMtrIndex, bAbleInstancing, pActorNode->IsApplyNoise(), REngine::GetConfig().m_bBackgroundCreateShaderComposite);
	return GetShaderForStage(pActorNode, nMtrIndex, false);
}

bool RActorNodeRenderStrategyGStageInferred::CheckInstancing(RActorNode* pActorNode, RMaterial* pMaterial)
{
	_ASSERT(pActorNode);

	bool bInstancing = RActorNodeRenderStrategy::CheckInstancing(pActorNode, pMaterial);

	if(REngine::GetSceneManager().GetRealtimeShadowMethod()==SHADOW_PROJECTION)
	{
		if(pActorNode->m_pActor->IsShadowReceiver())
			bInstancing = false;
	}

	return bInstancing;
}




void RActorNodeRenderStrategyMStageInferred::SetMaterialRenderState(RActorNode* pActorNode, RMaterial* pMaterial)
{
	_ASSERT(pActorNode);
	_ASSERT(pMaterial);

	RPFC_THISFUNC;


	//Set Render State
	{
		RShaderCompositeManager::SetCullMode(pMaterial->CheckFlag(RM_FLAG_TWOSIDED));
		RShaderCompositeManager::SetBlending(pMaterial->GetFlag(), pActorNode->IsAlpha());
	}


	//Set Texture
	{
		RShaderCompositeManager::SetTextureByType(RTT_DIFFUSE, pMaterial);
		RShaderCompositeManager::SetTextureByType(RTT_SELFILLUMINATION, pMaterial);
		RShaderCompositeManager::SetTextureByType(RTT_SPECULAR, pMaterial);

		if(REngine::GetSceneManager().GetShaderCompositeManager()->GetUseNormalMap() || pMaterial->GetTextureTypeForAlphaUsage(ALPHAUSAGE_ALPHA)==RTT_NORMAL)
			RShaderCompositeManager::SetTextureByType(RTT_NORMAL, pMaterial);


		// 매터리얼 별로 노이즈 텍스쳐가 따로 있는 경우를 대비하여 이곳에서 노이즈 텍스쳐를 셋팅한다. 
		// 중복 체크가 되므로 매번 셋팅 하여도 상관 없을 것이다.
		REngine::GetDevice().SetTexture( AS_NOISEMAP, REngine::GetDevice().GetDefaultNoiseTexture());
	}


	//Set Shader constant
	REngine::GetSceneManager().GetShaderCompositeManager()->SetMaterialShaderConstant(pMaterial, true, pActorNode->GetVisibilityResult(), pActorNode->GetIlluminationScale());
	RShaderCompositeManager::SetFogColorForBlending(pMaterial->CheckFlag(RM_FLAG_ADDITIVE));
}

void RActorNodeRenderStrategyMStageInferred::Begin()
{
	RPFC_THISFUNC;
	REngine::GetDevice().SetDepthEnable(true, false);
}

void RActorNodeRenderStrategyMStageInferred::End()
{
	RPFC_THISFUNC;
	RActorNodeRenderStrategy::End();

	REngine::GetDevice().SetDepthEnable(true, true);
	RShaderCompositeManager::RestoreFogColor();
}

RShaderFX* RActorNodeRenderStrategyMStageInferred::GetShader(RActorNode* pActorNode, int nMtrIndex, bool bAbleInstancing)
{
	RPFC_THISFUNC;
	_ASSERT(pActorNode);
	_ASSERT(nMtrIndex>=0);

	pActorNode->CheckAndCreateTechniqueTableOnMaterialForMStage(RENDERING_INFERRED, nMtrIndex, bAbleInstancing, pActorNode->IsApplyNoise(), REngine::GetConfig().m_bBackgroundCreateShaderComposite);
	return GetShaderForStage(pActorNode, nMtrIndex, true);
}


//----------------------------------------------------------------------------------
//Sky RenderStrategy
void RActorNodeRenderStrategySky::SetMaterialRenderState(RActorNode* pActorNode, RMaterial* pMaterial)
{
	_ASSERT(pActorNode);
	_ASSERT(pMaterial);

	RPFC_THISFUNC;

	//Set Render State
	{
		RShaderCompositeManager::SetCullMode(pMaterial->CheckFlag(RM_FLAG_TWOSIDED));
		RShaderCompositeManager::SetBlending(pMaterial->GetFlag(), pActorNode->IsAlpha());
	}


	//Set Texture
	{
		RShaderCompositeManager::SetTextureByType(RTT_DIFFUSE, pMaterial, RTF_BILINEAR);

		if(pMaterial->IsUseAlpha() && pMaterial->GetTextureTypeForAlphaUsage(ALPHAUSAGE_ALPHA)!=RTT_DIFFUSE)
			RShaderCompositeManager::SetTextureByType(pMaterial->GetTextureTypeForAlphaUsage(ALPHAUSAGE_ALPHA), pMaterial);
	}


	//Set Shader constant
	REngine::GetSceneManager().GetShaderCompositeManager()->SetMaterialShaderConstant(pMaterial, true, pActorNode->GetVisibilityResult(), pActorNode->GetIlluminationScale());
}

void RActorNodeRenderStrategySky::Begin()
{
	REngine::GetDevice().SetDepthEnable(false, false);
}

void RActorNodeRenderStrategySky::End()
{
	RActorNodeRenderStrategy::End();

	REngine::GetDevice().SetDepthEnable(true, true);
}

RShaderFX* RActorNodeRenderStrategySky::GetShader(RActorNode* pActorNode, int nMtrIndex, bool bAbleInstancing)
{
	RPFC_THISFUNC;
	_ASSERT(pActorNode);
	_ASSERT(nMtrIndex>=0);

	pActorNode->CheckAndCreateTechniqueTableOnMaterialForMStage(RENDERING_INFERRED, nMtrIndex, bAbleInstancing, pActorNode->IsApplyNoise(), REngine::GetConfig().m_bBackgroundCreateShaderComposite);
	return GetShaderForStage(pActorNode, nMtrIndex, true);
}

}