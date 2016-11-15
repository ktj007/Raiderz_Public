#include "StdAfx.h"

#include "RPostEffectDistortion.h"
#include "RPostEffectDistortionParameter.h"
#include "RCameraSceneNode.h"
#include "RVType.h"
#include "RActor.h"
#include "RActorNode.h"
#include "REffectManager.h"
#include "RXMLTemplate.h"
#include "RShaderCompositeManager.h"
#include "RMeshNode.h"
#include "RShaderFX.h"

namespace rs3 {

RPostEffectDistortion::RPostEffectDistortion() 
: RPostEffectInterface( PE_DISTORTION, RRP_NONE )
, m_pDistortedRT(NULL)
, m_pNormalMap(NULL)
, m_pDistortionShader(NULL)
, m_bValid(false)
, m_fCullLength(2000)
{
	m_fCullLengthSq	= m_fCullLength * m_fCullLength;
}

RPostEffectDistortion::~RPostEffectDistortion()
{
	//Destroy()는 베이스의 소멸자에서 호출 해 줌. 
}

bool RPostEffectDistortion::Init(  int nW, int nH)  // 기존의 Create()
{
	if(!RPostEffectInterface::Init(nW, nH))
		return false;

	//쉐이더 생성
	if(!m_pDistortionShader)
		m_pDistortionShader = REngine::GetDevice().CreateShaderFX("Distortion.fx");

	
	//---------------------------------------------------------------------
	// Texture 생성
	if ( false == REngine::GetDevice().QueryFeature(RQF_PS20) )
		return false;

	//NormalMap
	if(!m_pNormalMap)
		m_pNormalMap =  REngine::GetDevice().CreateTexture( DISTORTION_DEFAUL_NORMALMAP, RTF_LINEAR, RCT_AUTOGENMIPMAP | RCT_UNREDUCIBLE );
	//---------------------------------------------------------------------

	//RenderTarget
	Restore();

	if(!m_pDistortionShader || !GetMaskBuffer() || !m_pDistortedRT || !m_pNormalMap)
	{
		mlog3("Distortion 생성 실패!\n");

		return false;
	}
	

	//-------------------------------------------------------
	// shader 관련
	m_cDirection = m_pDistortionShader->GetConstant("g_dir");
	m_cInfo = m_pDistortionShader->GetConstant("g_Info");
	m_cTime = m_pDistortionShader->GetConstant("g_fTime");
	m_cWVP = m_pDistortionShader->GetConstant("g_matWVP");
	m_cResolution = m_pDistortionShader->GetConstant("g_vResolution");
	m_cBlendMatrices = m_pDistortionShader->GetConstant("g_blendMatrices");
	m_cPhysique = m_pDistortionShader->GetConstant("g_bPhysique");
	m_cRenderTarget = m_pDistortionShader->GetConstant("g_tRenderTarget");
	m_cNormalMap = m_pDistortionShader->GetConstant("g_tNormalMap");
	m_cMaskingMap = m_pDistortionShader->GetConstant("g_tMaskingMap");
	m_cMaskingMap.SetTexture( GetMaskBuffer(), REngine::GetDevicePtr());
	//-------------------------------------------------------

	m_bValid = true;
	
	return m_bValid;
}

void RPostEffectDistortion::Destroy()
{
	RPostEffectInterface::Destroy();

	REngine::GetDevicePtr()->DeleteTexture(m_pNormalMap);
	m_pNormalMap = NULL;

	Invalidate();

	if (m_pDistortionShader)
		REngine::GetDevice().DeleteShaderFX(m_pDistortionShader);
	m_pDistortionShader = NULL;
}

void RPostEffectDistortion::CreateBuffers()
{
	RFORMAT eRenderTargetFormat = RFMT_A8R8G8B8;

	MakeMaskTarget( m_dwWidth, m_dwHeight, eRenderTargetFormat);
	
	if(!m_pDistortedRT)
		m_pDistortedRT =REngine::GetDevice().CreateRenderTargetTexture(m_dwWidth, m_dwHeight, eRenderTargetFormat);	
}

void RPostEffectDistortion::DestroyBuffers()
{
	REngine::GetDevicePtr()->SafeDeleteTexture(m_pDistortedRT);
	DeleteMaskTarget();
}

void RPostEffectDistortion::Render( RCameraSceneNode* pCamera, RTexture* pColorBuffer, RTexture* pNormalBuffer, RTexture* pDepthBuffer)
{
	RPFC_THISFUNC;
	RenderTextureToFullScreen( pColorBuffer);
}

bool RPostEffectDistortion::CheckRenderTargetFormat()
{
	RPFC_THISFUNC;
	RTexture* pRT0 = REngine::GetDevice().GetRenderTarget(0);
	if(pRT0==RT_FRAMEBUFFER || m_pDistortedRT==NULL)
	{
		_ASSERT(0 && "Fail to Check RenderTarget for Distortion!");
		return false;
	}

	if( m_pDistortedRT->GetInfo().pixelFormat != pRT0->GetInfo().pixelFormat )
	{
		REngine::GetDevice().DeleteTexture(m_pDistortedRT);
		DeleteMaskTarget();

		int nScreenWidth	= m_dwWidth;
		int nScreenHeight	= m_dwHeight;
		RFORMAT eRenderTargetFormat = pRT0->GetInfo().pixelFormat;

		MakeMaskTarget(nScreenWidth, nScreenHeight, eRenderTargetFormat);
		m_pDistortedRT =REngine::GetDevice().CreateRenderTargetTexture(nScreenWidth, nScreenHeight, eRenderTargetFormat);

		m_cMaskingMap.SetTexture( GetMaskBuffer(), REngine::GetDevicePtr());
	}

	return true;
}

void RPostEffectDistortion::SetShaderConstant(const RMatrix& matViewProj, const RDistortionParameter& param)
{
	RVector2 vDir0(param.fDir0X, param.fDir0Y); vDir0.Normalize();
	RVector2 vDir1(param.fDir1X, param.fDir1Y); vDir1.Normalize();

	m_cDirection.SetVector4( RVector4(vDir0.x, vDir0.y, vDir1.x, vDir1.y) );
	m_cInfo.SetVector4( param.fSpeed0, param.fSpeed1, param.fScale * param.fVisibility, param.fRange );
	m_cTime.SetFloat(REngine::GetTimer().GetTime() /1000.0f);
	m_cWVP.SetMatrix(matViewProj);

	if( strcmp(param.strNormalMapFileName, DISTORTION_DEFAUL_NORMALMAP) != 0 )
		m_cNormalMap.SetTexture(param.pNormalMap, REngine::GetDevicePtr());
	else
		m_cNormalMap.SetTexture(m_pNormalMap, REngine::GetDevicePtr());

	m_cMaskingMap.SetTexture( GetMaskBuffer(), REngine::GetDevicePtr());

	m_cResolution.SetVector4( RVector4( (float)m_dwWidth, (float)m_dwHeight, 0.5f/(float)m_dwWidth, 0.5f/(float)m_dwHeight));
}

void RPostEffectDistortion::RenderTextureToFullScreen(RTexture* pTexture)
{
	RPFC_THISFUNC;

	m_cRenderTarget.SetTexture(pTexture, REngine::GetDevicePtr());

	RTechnique Technique = m_pDistortionShader->GetTechnique("NoDistortion");
	Technique.Begin();
	Technique.BeginPass();
	DrawFullScreenQuad(0, 0, m_dwWidth, m_dwHeight);
	Technique.EndPass();
	Technique.End();
}

void RPostEffectDistortion::RenderFullScreenDistortion(float fDistortionScale)
{
	if(CheckRenderTargetFormat() == false)
		return;

	//-------------------------------------------------------------------------------
	//Set RenderState
	REngine::GetDevice().SetDepthEnable(false, false);

	RTexture* pRT0 = REngine::GetDevice().GetRenderTarget(0);

	m_cRenderTarget.SetTexture(pRT0, REngine::GetDevicePtr());
	//-------------------------------------------------------------------------------


	//-------------------------------------------------------------------------------
	//Set Shader constant
	RDistortionParameter Param;
	Param.fSpeed0 *=0.2f;
	Param.fSpeed1 *=0.2f;
	Param.fScale *= 0.5;

	SetShaderConstant(RMatrix::IDENTITY, Param);
	//-------------------------------------------------------------------------------


	//-------------------------------------------------------------------------------
	//Render distorted Scene to Temp RenderTarget
	REngine::GetDevice().SetRenderTarget(0, m_pDistortedRT);
	RTechnique Technique = m_pDistortionShader->GetTechnique("FullScreenDistortion");
	Technique.Begin();
	Technique.BeginPass();
	DrawFullScreenQuad(0, 0, m_dwWidth, m_dwHeight);
	Technique.EndPass();
	Technique.End();
	//-------------------------------------------------------------------------------


	//-------------------------------------------------------------------------------
	//Restore Render State
	REngine::GetDevice().SetRenderTarget(0, pRT0);
	REngine::GetDevice().SetDepthEnable(true, true);
	//-------------------------------------------------------------------------------



	//Render Temp RenderTarget to current RenderTarget
	RenderTextureToFullScreen(m_pDistortedRT);	
}

void RPostEffectDistortion::RenderEffectDistortion(RCameraSceneNode* pCamera, RDistortionParameter& param, bool bTriangleStrip)
{
	RPFC_THISFUNC;
	SetShaderConstant(pCamera->GetViewProjectionMatrix(), param);
	
	REngine::GetDevice().ColorFill(m_pDistortedRT);
	
	//------------------------------------------------------------------------------------
	//Render distorted Emitter to Temp RenderTarget


	//Set RenderState
	REngine::GetDevice().SetDepthEnable(true, false);
	REngine::GetDevice().SetCullMode(RCULL_NONE);


	RTexture* pRT0 = REngine::GetDevice().SetRenderTarget(0, m_pDistortedRT);
	
	m_cRenderTarget.SetTexture(pRT0, REngine::GetDevicePtr());

	RTechnique Technique = m_pDistortionShader->GetTechnique("Emitter_Distortion");
	Technique.Begin();
	Technique.BeginPass(0);

	int nPrimitive=0;
	if(bTriangleStrip)
		nPrimitive = REffectManager::DrawIndexedPrimitiveTriangleStrip();
	else
		nPrimitive = REffectManager::DrawPrimitiveTriangleList();
	REngine::GetResourceProfiler().AddPolygon(PPT_PARTICLE, nPrimitive );
	
	Technique.EndPass();
	Technique.End();
	//------------------------------------------------------------------------------------

	

	//------------------------------------------------------------------------------------
	//Draw Temp RenderTarget over current RenderTarget
	REngine::GetDevice().SetRenderTarget(0, pRT0);
	RenderTextureToFullScreen(m_pDistortedRT);
	//------------------------------------------------------------------------------------


	//Restore RenderState
	REngine::GetDevice().SetDepthEnable(true, true);
	REngine::GetDevice().SetCullMode(RCULL_CW);
}

void RPostEffectDistortion::RenderActorNodeDistortion(RCameraSceneNode* pCamera, RActorNode* pActorNode, int nMtrIndex)
{
	RPFC_THISFUNC;

	_ASSERT( pActorNode );
	_ASSERT( pActorNode->m_pActor );
	_ASSERT( pActorNode->m_pActor->IsDistortion() );

	if(CheckRenderTargetFormat() == false)
		return;

	
	REngine::GetDevice().ColorFill(m_pDistortedRT);
	ClearMaskBuffer();


	RTexture* pRT0 = REngine::GetDevice().GetRenderTarget(0);
	REngine::GetDevice().SetDepthEnable(true, false);
	REngine::GetDevice().SetCullMode(RCULL_NONE);


	RDistortionParameter& param = *pActorNode->m_pActor->GetDistortionParam();
	param.fVisibility = param.fAttenuationBegin==0 ? 1 : min(pActorNode->GetVisibilityResult() /param.fAttenuationBegin, 1);//pActorNode->GetVisibilityResult();
	SetShaderConstant(pCamera->GetViewProjectionMatrix(), param);

	SetActorNodeState(pActorNode, pCamera->GetViewProjectionMatrix());


	RTechnique Technique = m_pDistortionShader->GetTechnique("Actor_Distortion");
	Technique.Begin();
	{
		//------------------------------------------------------------------------------------
		//Pass0: Render EffectActor to Masking RenderTarget
		BeginMasking(0);
		{
			Technique.BeginPass(0);
			{
				if(nMtrIndex<0)
					pActorNode->RenderAllNodePrimitive();
				else
					pActorNode->RenderPrimitive(nMtrIndex);
			}
			Technique.EndPass();
		}
		EndMasking();
		//------------------------------------------------------------------------------------


		//------------------------------------------------------------------------------------
		//Pass1: Render distorted EffectActor to Temp RenderTarget

		REngine::GetDevice().SetRenderTarget(0, m_pDistortedRT);
		m_cRenderTarget.SetTexture(pRT0, REngine::GetDevicePtr());

		Technique.BeginPass(1);
		{
			if(nMtrIndex<0)
				pActorNode->RenderAllNodePrimitive();
			else
				pActorNode->RenderPrimitive(nMtrIndex);
		}
		Technique.EndPass();
		//------------------------------------------------------------------------------------
	}
	Technique.End();


	//------------------------------------------------------------------------------------
	//Restore RenderState
	REngine::GetDevice().SetRenderTarget(0, pRT0);
	REngine::GetDevice().SetDepthEnable(true, true);
	REngine::GetDevice().SetCullMode(RCULL_CW);
	//------------------------------------------------------------------------------------



	//Render Temp RenderTarget to current RenderTarget
	RenderTextureToFullScreen(m_pDistortedRT);
}

void RPostEffectDistortion::BeginMasking(int nRT)
{
	m_nMaskRT =nRT;
	m_pMaskingMapOrg =REngine::GetDevice().SetRenderTarget(nRT, GetMaskBuffer());
}

void RPostEffectDistortion::EndMasking()
{
	RPFC_THISFUNC;
	REngine::GetDevice().SetRenderTarget(m_nMaskRT, m_pMaskingMapOrg);
}

void RPostEffectDistortion::SetActorNodeState(RActorNode* pActorNode, const RMatrix& matViewProj)
{
	RPFC_THISFUNC;
 	RMatrix matWorld = pActorNode->GetWorldTransform();

	RMeshNode* pMeshNode = pActorNode->m_pMeshNode;
	bool bPhysiue = pMeshNode->m_nPhysiqueCnt && !pMeshNode->isClothMesh();
	
	if(bPhysiue)
	{
		m_cBlendMatrices.SetMatrixArray(&pActorNode->m_matBones[0],pMeshNode->m_nBoneTableCnt);

		RActor* pParentActor = pActorNode->m_pActor;
		if(pParentActor)
			matWorld = pParentActor->GetWorldTransform();
	}

	m_cPhysique.SetInt(bPhysiue);
	m_cWVP.SetMatrix(matWorld * matViewProj);

	pActorNode->SetVertexStream();
}

bool RPostEffectDistortion::IsValid()
{
	return ( m_bValid && IsActive() && REngine::GetSceneManager().GetRenderingStrategy()->GetRenderingMode() != RRM_DIFFUSE_ONLY );
}

bool RPostEffectDistortion::IsCulledVisible(RSceneNode* pNode, const RVector3& vCameraWorldPosition)
{
	if( pNode==NULL )
		return false;

	float fR = pNode->GetSphere().radius + m_fCullLength;
	fR = fR * fR;
	float fL = (pNode->GetSphere().center - vCameraWorldPosition).LengthSq();
	
	if( fL > fR)	/// 기준보다 멀리 있으면
		return false;
	else
		return true;
}
	
}