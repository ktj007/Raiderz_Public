#include "StdAfx.h"
#include "RSpotLightPassRenderer.h"
#include "RShaderCompositeManager.h"
#include "RShaderFX.h"
#include "RRenderHelper.h"
#include "RLightingShader.h"

namespace rs3 {

RSpotLightPassRenderer::RSpotLightPassRenderer(void)
{
#ifdef USE_VERTEXBUFFER
	RVertexAttributeEx Attribs[] = 
	{
		RVertexAttributeEx(0, TYPE_VERTEX, 0, FORMAT_FLOAT, 4),
	};

	m_VertexFormat = REngine::GetDevice().CreateVertexFormat(Attribs, 1);

	m_VertexBuffer =REngine::GetDevice().CreateVertexBuffer(sizeof(RVector4)*5*MAX_INSTANCE, sizeof(RVector4));




	const WORD BaseIndexArray[] =
	{
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 1,
		1, 3, 2,
		1, 4, 3
	};

	WORD pIndexData[18*MAX_INSTANCE];

	for (int iLight=0; iLight<MAX_INSTANCE; ++iLight)
	{
		for(int iIndex=0; iIndex<18; ++iIndex)
		{
			pIndexData[iLight*18 +iIndex] = BaseIndexArray[iIndex] +iLight*5;
		}
	}
	m_IndexBuffer =REngine::GetDevice().CreateIndexBuffer(18*MAX_INSTANCE, RFMT_INDEX16, pIndexData);
#endif
}

RSpotLightPassRenderer::~RSpotLightPassRenderer(void)
{
}

void RSpotLightPassRenderer::CategorizeLight(RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, 
											 RSceneNodeList* outInstancing, RSceneNodeList* outNoInstancing, 
											 RSceneNodeList* outInstancingWithCamera, RSceneNodeList* outNoInstancingWithCamera)
{
	SCENE_SET::iterator iter;
	for( iter = pCulledList->begin() ; iter != pCulledList->end() ; ++iter)
	{
		RSceneNode* pSceneNode = (*iter);

		RSpotLightSceneNode* pSpotLight= MDynamicCast(RSpotLightSceneNode, pSceneNode);
		if(pSpotLight==NULL)
			continue;
		
		bool bInstancing = CheckInstancing(pSpotLight);
		bool bCameraInLight = pSpotLight->CheckCameraInLight(pCurrentCamera);

		if(bInstancing)
		{
			if(bCameraInLight)
				outInstancingWithCamera->push_back(pSpotLight);
			else
				outInstancing->push_back(pSpotLight);
		}
		else
		{
			if(bCameraInLight)
				outNoInstancingWithCamera->push_back(pSpotLight);
			else
				outNoInstancing->push_back(pSpotLight);

		}
	}
}

bool RSpotLightPassRenderer::CheckInstancing(RSpotLightSceneNode* pSpotLight)
{
	return ( (!pSpotLight->GetShadowCast()) && (pSpotLight->GetSpotLightTexture()==NULL) );
}

void RSpotLightPassRenderer::RenderSpotLightingToLBuffer(RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RLightingShader* pLightingShader, RRENDERING_MODE eRRM)
{
	if( pCulledList->empty() )	return;

	RSceneNodeList InstancingList, NoInstancingList, InstancingWithCameraList, NoInstancingWithCameraList;

	CategorizeLight(pCurrentCamera, pCulledList, &InstancingList, &NoInstancingList, &InstancingWithCameraList, &NoInstancingWithCameraList);


	REngine::GetDevice().SetDepthEnable(true, false);
	REngine::GetDevice().SetBlending(RBLEND_ONE, RBLEND_ONE);


	//Light 범위 밖에 카메라가 있는 Light 렌더링
	{
		REngine::GetDevice().SetCullMode(RCULL_CCW);
		REngine::GetDevice().SetDepthFunc(RCMP_LESSEQUAL);

		if(!InstancingList.empty())
			RenderLight(pCurrentCamera, &InstancingList, pLightingShader, eRRM);

		if(!NoInstancingList.empty())
			RenderLightOnebyOne(pCurrentCamera, &NoInstancingList, pLightingShader, eRRM);
	}


	//Light 범위 안에 카메라가 있는 Light 렌더링
	{
		REngine::GetDevice().SetCullMode(RCULL_CW);
		REngine::GetDevice().SetDepthFunc(RCMP_GREATER);

		if(!InstancingWithCameraList.empty())
			RenderLight(pCurrentCamera, &InstancingWithCameraList, pLightingShader, eRRM);

		if(!NoInstancingWithCameraList.empty())
			RenderLightOnebyOne(pCurrentCamera, &NoInstancingWithCameraList, pLightingShader, eRRM);
	}
	

	//Restore Render State
	REngine::GetDevice().SetDepthEnable(true, true);
	REngine::GetDevice().SetBlending(RBLEND_NONE);
	REngine::GetDevice().SetCullMode(RCULL_CW);
	REngine::GetDevice().SetDepthFunc(RCMP_LESSEQUAL);
}

void RSpotLightPassRenderer::SetInstanceData(RCameraSceneNode* pCurrentCamera, RSpotLightSceneNode* pSpotLightSceneNode, int nIndex)
{
	RVector3 vPos(pSpotLightSceneNode->GetWorldPosition()); 
	pCurrentCamera->GetViewMatrix().TransformVect(vPos);

	RVector3 vDirection(pSpotLightSceneNode->GetWorldDirection()); 
	pCurrentCamera->GetViewMatrix().TransformNormal(vDirection);
	float fcosFOV = cosf(pSpotLightSceneNode->GetFov()*(MMath::PI/180.0f)*0.5f);


	RVector3 vScale = pSpotLightSceneNode->GetScaleFromWorldTransform();
	float fScale = max(max(vScale.x, vScale.y), vScale.z);
	RVector2 vAttenuationInfo(pSpotLightSceneNode->GetFar()*fScale, pSpotLightSceneNode->GetAttenuationStart()*fScale);
	float fVisibility = pSpotLightSceneNode->GetVisibilityResult() *pSpotLightSceneNode->GetApplyIntensity();

	m_InstanceData.vPosition[nIndex] = vPos;
	m_InstanceData.vPositionInWorld[nIndex] = pSpotLightSceneNode->GetWorldPosition();
	m_InstanceData.vLightInfo[nIndex] = RVector3(vAttenuationInfo.x, vAttenuationInfo.y, fVisibility);
	m_InstanceData.vLightDirectionFOV[nIndex]=RVector4(vDirection, fcosFOV);
	m_InstanceData.vColor[nIndex] = pSpotLightSceneNode->m_color;
	m_InstanceData.ViewFrustum[nIndex]= pSpotLightSceneNode->GetLightFrustum();	
	m_InstanceData.bCameraInLight[nIndex] = pSpotLightSceneNode->CheckCameraInLight(pCurrentCamera);
}

void RSpotLightPassRenderer::RenderLight(RCameraSceneNode* pCurrentCamera, RSceneNodeList* pLightList, RLightingShader* pLightingShader, RRENDERING_MODE eRRM)
{
	int nIndex =0;

	for( RSceneNodeList::iterator iter = pLightList->begin() ; iter != pLightList->end() ; ++iter )
	{
		RSpotLightSceneNode* pSpotLightSceneNode = MDynamicCast(RSpotLightSceneNode, (*iter));

		if( pSpotLightSceneNode->GetVisible() == false || pSpotLightSceneNode->GetOcclued() )
			continue;

		SetInstanceData(pCurrentCamera, pSpotLightSceneNode, nIndex++);

		if(nIndex==MAX_INSTANCE)
		{
			RenderInstancing(nIndex, pLightingShader, eRRM);

			nIndex =0;
		}
	}

	if(nIndex>0)
		RenderInstancing(nIndex, pLightingShader, eRRM);
}

void RSpotLightPassRenderer::RenderLightOnebyOne(RCameraSceneNode* pCurrentCamera, RSceneNodeList* pLightList, RLightingShader* pLightingShader, RRENDERING_MODE eRRM)
{
	bool bSpecular = eRRM != RRM_NO_SPECULAR_RIM_LIGHT; 
	for( RSceneNodeList::iterator iter = pLightList->begin() ; iter != pLightList->end() ; ++iter )
	{
		RSpotLightSceneNode* pSpotLightSceneNode = MDynamicCast(RSpotLightSceneNode, (*iter));
		RenderSingleSpotLight(pCurrentCamera, pSpotLightSceneNode, pLightingShader, eRRM);
	}
}

void RSpotLightPassRenderer::RenderInstancing(int nCount, RLightingShader* pLightingShader, RRENDERING_MODE eRRM)
{
	//--------------------------------------------------------------------------------------------
	//Set Vertex&Index Buffer
#ifdef USE_VERTEXBUFFER
	RVector4* pVertexData= (RVector4*)REngine::GetDevice().LockVertexBuffer(m_VertexBuffer, D3DLOCK_DISCARD);
	if(pVertexData==NULL)
		return;

	for (int iLight=0; iLight<nCount; ++iLight)
	{
		pVertexData[iLight*5+ 0] = RVector4(m_InstanceData.vPositionInWorld[iLight], iLight);
		pVertexData[iLight*5+ 1] = RVector4(m_InstanceData.ViewFrustum[iLight].GetPoints()[0], iLight);
		pVertexData[iLight*5+ 2] = RVector4(m_InstanceData.ViewFrustum[iLight].GetPoints()[2], iLight);
		pVertexData[iLight*5+ 3] = RVector4(m_InstanceData.ViewFrustum[iLight].GetPoints()[6], iLight);
		pVertexData[iLight*5+ 4] = RVector4(m_InstanceData.ViewFrustum[iLight].GetPoints()[4], iLight);
	}

	REngine::GetDevice().UnlockVertexBuffer(m_VertexBuffer);
#else
	const static WORD BaseIndexArray[] =
	{
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 1,
		1, 3, 2,
		1, 4, 3
	};

	for (int iLight=0; iLight<nCount; ++iLight)
	{
		m_VertexBuffer[iLight*5+ 0] = RVector4(m_InstanceData.vPositionInWorld[iLight], iLight);
		m_VertexBuffer[iLight*5+ 1] = RVector4(m_InstanceData.ViewFrustum[iLight].GetPoints()[0], iLight);
		m_VertexBuffer[iLight*5+ 2] = RVector4(m_InstanceData.ViewFrustum[iLight].GetPoints()[2], iLight);
		m_VertexBuffer[iLight*5+ 3] = RVector4(m_InstanceData.ViewFrustum[iLight].GetPoints()[6], iLight);
		m_VertexBuffer[iLight*5+ 4] = RVector4(m_InstanceData.ViewFrustum[iLight].GetPoints()[4], iLight);

		for(int iIndex=0; iIndex<18; ++iIndex)
		{
			m_IndexBuffer[iLight*18 +iIndex] = BaseIndexArray[iIndex] +iLight*5;
		}
	}
#endif
	//--------------------------------------------------------------------------------------------

	bool bSpecular = eRRM != RRM_NO_SPECULAR_RIM_LIGHT;

	RShaderFX* pEffect = pLightingShader->CreateShaderForSpotLightInstancing( bSpecular);

	pLightingShader->m_scLightPositionArray.SetVector3Array(m_InstanceData.vPosition, nCount);
	pLightingShader->m_scLightColorArray.SetVector3Array(m_InstanceData.vColor, nCount);
	pLightingShader->m_scLightInfoArray.SetVector3Array(m_InstanceData.vLightInfo, nCount);
	pLightingShader->m_scLightDirectionFOVArray.SetVector4Array(m_InstanceData.vLightDirectionFOV, nCount);	

	//Set Render State
	REngine::GetDevice().SetFvF(D3DFVF_XYZW);
#ifdef USE_VERTEXBUFFER
	REngine::GetDevice().SetVertexBuffer(m_VertexBuffer, 0, 0);
	REngine::GetDevice().SetIndexBuffer(m_IndexBuffer);
	REngine::GetDevice().SetVertexFormat(m_VertexFormat);
#endif


	RTechnique technique = pEffect->GetTechnique( "SpotLightTechnique" );
	technique.Begin(NULL, TECH_PRESERVE_NOT);
	technique.BeginPass();
#ifdef USE_VERTEXBUFFER
	REngine::GetDevice().DrawIndexedPrimitive(RPT_TRIANGLELIST, 0, 0, 5*nCount, 0, 6*nCount);
#else
	REngine::GetDevice().DrawIndexedPrimitiveUP( RPT_TRIANGLELIST, 0, 5*nCount, 6*nCount,(void*)m_IndexBuffer, (void*)m_VertexBuffer, sizeof(RVector4));
#endif
	
	technique.EndPass();
	technique.End();	
}

void RSpotLightPassRenderer::RenderSingleSpotLight(RCameraSceneNode* pCurrentCamera, RSpotLightSceneNode* pSpotLight, RLightingShader* pLightingShader, RRENDERING_MODE eRRM)
{
	bool bShadowCast = REngine::GetSceneManager().GetRealtimeShadowMethod() != SHADOW_NONE && pSpotLight->GetShadowCast();
	bool bUseSpotMap = pSpotLight->GetSpotLightTexture()!=NULL;
	bool bSpecular = eRRM != RRM_NO_SPECULAR_RIM_LIGHT;
	//----------------------------------------------------
	RShaderFX* pEffect = pLightingShader->CreateShaderForSpotLight( bShadowCast, bUseSpotMap, bSpecular);
	//----------------------------------------------------

	//----------------------------------------------------
	//Set Shader Constant
	RVector3 vPos(pSpotLight->GetWorldPosition()); 
	pCurrentCamera->GetViewMatrix().TransformVect(vPos);

	RVector3 vDirection(pSpotLight->GetWorldDirection());
	vDirection.Normalize();
	pCurrentCamera->GetViewMatrix().TransformNormal(vDirection);
	float fcosFOV = 1;
	if(bUseSpotMap)
		fcosFOV =cosf(179.0f*(MMath::PI/180.0f)*0.5f);
	else
		fcosFOV =cosf(pSpotLight->GetFov()*(MMath::PI/180.0f)*0.5f);
		

	RVector3 vScale = pSpotLight->GetScaleFromWorldTransform();
	float fScale = max(max(vScale.x, vScale.y), vScale.z);
	RVector2 vAttenuationInfo(pSpotLight->GetFar()*fScale, pSpotLight->GetAttenuationStart()*fScale);

	pLightingShader->m_scLightPosition.SetVector3(vPos);
	pLightingShader->m_scAttenuationInfo.SetVector2(vAttenuationInfo);
	pLightingShader->m_scVisibility.SetFloat(pSpotLight->GetVisibilityResult() *pSpotLight->GetApplyIntensity());
	pLightingShader->m_scLightDirectionFOV.SetVector4(RVector4(vDirection, fcosFOV));
	pLightingShader->m_scLightColor.SetVector3(pSpotLight->m_color);


	RShadowStrategy* pShadowStrategy = pSpotLight->GetShadowStrategy();

	RMatrix matView = pCurrentCamera->GetViewMatrix();
	RMatrix matViewInv;
	matView.GetInverse(&matViewInv);
	pLightingShader->m_scLightTransform.SetMatrix( matViewInv * pSpotLight->GetLightTransform());
	
	RMatrix matLightView = matViewInv * pSpotLight->GetLightViewTransform();
	pLightingShader->m_scLightViewTransform.SetVector4(RVector4(matLightView._13, matLightView._23, matLightView._33, matLightView._43));
	pLightingShader->m_scDepthTexelSize.SetFloat( 1.f / float(pShadowStrategy->GetCurrentDepthMapSize()) );
	//----------------------------------------------------



	//----------------------------------------------------
	//Set RenderState
	if(bUseSpotMap)
	{
		REngine::GetDevice().SetTexture(AS_SPOTMAP, pSpotLight->GetSpotLightTexture());
		REngine::GetDevice().SetTextureFilter( AS_SPOTMAP, RTF_LINEAR );
		REngine::GetDevice().SetTextureBorderColor(AS_SPOTMAP, 0);
		REngine::GetDevice().SetTextureAddress(AS_SPOTMAP, RTADDRESS_BORDER, RTADDRESS_BORDER, RTADDRESS_BORDER);		
	}
	
	if(bShadowCast)
		REngine::GetDevice().SetTexture( AS_DEPTHMAP ,pShadowStrategy->GetCurrentDepthMap());
	//------------------------------------------------------------------------------------



	//----------------------------------------------------
	//Rendering
	RTechnique technique = pEffect->GetTechnique( "SpotLightTechnique" );
	technique.Begin(NULL, TECH_PRESERVE_NOT);
	technique.BeginPass();
	rs3::RRenderHelper::RenderPyramid(pSpotLight->GetWorldPosition(), pSpotLight->GetLightFrustum(), 0xFFFFFFFF, false);
	technique.EndPass();
	technique.End();	
	//----------------------------------------------------
}

}
