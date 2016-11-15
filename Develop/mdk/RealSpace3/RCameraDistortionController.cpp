#include "StdAfx.h"

#include "RCameraDistortionController.h"
#include "RCameraSceneNode.h"
#include "RVType.h"
#include "RActor.h"
#include "RActorNode.h"
#include "REffectManager.h"
#include "RXMLTemplate.h"
#include "RShaderCompositeManager.h"
#include "RMeshNode.h"

namespace rs3 {

	//=======================================================================================================
	//=======================================================================================================
	// RDistortionParameter

	RDistortionParameter::RDistortionParameter()
	{
		fDir0X = -0.05f;
		fDir0Y = 0.02f;
		fDir1X = 0.03f;
		fDir1Y = 0.014f;
		fScale =0.02f;
		fSpeed0 =0.1f;
		fSpeed1 =0.1f;
		bRender = true;
		strcpy_s(strNormalMapFileName, DISTORTION_DEFAUL_NORMALMAP);
		pNormalMap = NULL;
		fRange =0.0f;
		fVisibility =1.0f;
		fAttenuationBegin =0.0f;
	}

	RDistortionParameter::~RDistortionParameter()
	{
		REngine::GetDevice().DeleteTexture(pNormalMap);
	}

	RDistortionParameter& RDistortionParameter::operator=(const RDistortionParameter& param)
	{
		fDir0X = param.fDir0X;
		fDir0Y = param.fDir0Y;
		fDir1X = param.fDir1X;
		fDir1Y = param.fDir1Y;
		fScale = param.fScale;
		fSpeed0 = param.fSpeed0;
		fSpeed1 = param.fSpeed1;
		bRender = param.bRender;
		strcpy_s(strNormalMapFileName, param.strNormalMapFileName);

		if( param.pNormalMap==NULL )
			pNormalMap =NULL;
		else
			LoadNormalMap(false);

		fRange = param.fRange;
		fVisibility = param.fVisibility;
		fAttenuationBegin = param.fAttenuationBegin;

		return *this;
	}

	void RDistortionParameter::LoadNormalMap(bool bBackground)
	{
		if( strcmp(strNormalMapFileName, DISTORTION_DEFAUL_NORMALMAP) != 0 )
			pNormalMap = REngine::GetDevice().CreateTextureForBuildLater(strNormalMapFileName, RTF_LINEAR, RCT_AUTOGENMIPMAP, RFMT_NONE, bBackground);		
	}

	void RDistortionParameter::ReplaceNormalMap()
	{
		if( strcmp(strNormalMapFileName, DISTORTION_DEFAUL_NORMALMAP) != 0 )
		{
			REngine::GetDevice().DeleteTexture(pNormalMap);
			pNormalMap=REngine::GetDevice().CreateTexture( strNormalMapFileName );
		}		
	}
 
	void RDistortionParameter::CreateToXML(MXmlElement &element)
	{
		using namespace TXML;

		if( MXmlElement *pDistortionAttribute = element.FirstChildElement("DISTORTION_ATTRIBUTE") )
		{
			fDir0X = _GETElement<float>::Value(pDistortionAttribute,"DIRECTION0_X","float");
			fDir0Y = _GETElement<float>::Value(pDistortionAttribute,"DIRECTION0_Y","float");
			fDir1X = _GETElement<float>::Value(pDistortionAttribute,"DIRECTION1_X","float");
			fDir1Y = _GETElement<float>::Value(pDistortionAttribute,"DIRECTION1_Y","float");
			fScale = _GETElement<float>::Value(pDistortionAttribute,"SCALE","float");
			fSpeed0 = _GETElement<float>::Value(pDistortionAttribute,"SPEED0","float");
			fSpeed1 = _GETElement<float>::Value(pDistortionAttribute,"SPEED1","float");
			bRender = _GETElement<int>::Value(pDistortionAttribute,"RENDERABLE","int") ? true : false;
			_Contents(strNormalMapFileName, pDistortionAttribute->FirstChildElement("NORMALMAP"));
			fRange = _GETElement<float>::Value(pDistortionAttribute,"RANGE","float");
			fAttenuationBegin = _GETElement<float>::Value(pDistortionAttribute,"ATTENUATIONBEGIN","float");
		}		
	}

	void RDistortionParameter::SaveToXML(MXmlElement &element)
	{
		using namespace TXML;

		MXmlElement *pDistortionElement = new MXmlElement("DISTORTION_ATTRIBUTE");
		element.LinkEndChild(pDistortionElement);
		_SetElementValue(pDistortionElement,"DIRECTION0_X", "float", fDir0X);
		_SetElementValue(pDistortionElement,"DIRECTION0_Y", "float", fDir0Y);
		_SetElementValue(pDistortionElement,"DIRECTION1_X", "float", fDir1X);
		_SetElementValue(pDistortionElement,"DIRECTION1_Y", "float", fDir1Y);
		_SetElementValue(pDistortionElement,"SCALE", "float", fScale);
		_SetElementValue(pDistortionElement,"SPEED0", "float", fSpeed0);
		_SetElementValue(pDistortionElement,"SPEED1", "float", fSpeed1);
		_SetElementValue(pDistortionElement,"RENDERABLE", "bool", bRender);
		_SetElementValue(pDistortionElement,"NORMALMAP", "string", strNormalMapFileName);
		_SetElementValue(pDistortionElement,"RANGE", "float", fRange);
		_SetElementValue(pDistortionElement,"ATTENUATIONBEGIN", "float", fAttenuationBegin);
	}

	// RDistortionParameter
	//=======================================================================================================
	//=======================================================================================================
	



	
	//=======================================================================================================
	//=======================================================================================================
	// DistortionControllerListener

	class DistortionControllerListener : public RListener
	{
		RCameraDistortionController* m_pDistortionController;
	public:
		DistortionControllerListener(RCameraDistortionController* pDistortionController) 
			: m_pDistortionController(pDistortionController) {}

		virtual void Update() override
		{
			m_pDistortionController->Destroy();
			m_pDistortionController->Create();
		}
	};

	// DistortionControllerListener
	//=======================================================================================================
	//=======================================================================================================
	




	//=======================================================================================================
	//=======================================================================================================
	// RCameraDistortionController

	RCameraDistortionController::RCameraDistortionController(RCameraSceneNode* pCameraSceneNode, bool& bValid) 
		: RCameraSceneNodeController( RCCT_POSTRENDER, "11_Distortion")
		, m_pCameraSceneNode(pCameraSceneNode)
		, m_pRenderTarget(NULL)
		, m_pDistortredRT(NULL)
		, m_pRenderTargetOrg(NULL)
		, m_pNormalMap(NULL)
		, m_pMaskingMap(NULL)
		, m_pMaskingMapOrg(NULL)
		, m_pDistortionShader(NULL)
		, m_bValid(false)
	{
		// event 구독
		m_pEventListener = new DistortionControllerListener(this);
		REngine::GetEventBroadcaster(RE_DEVICE_RESTORE)->AddListener( m_pEventListener );

		bValid = Create();		
	}

	RCameraDistortionController::~RCameraDistortionController()
	{
		// event 해제
		REngine::GetEventBroadcaster(RE_DEVICE_RESTORE)->RemoveListener( m_pEventListener );

		SAFE_DELETE(m_pEventListener);

		Destroy();
	}

	bool RCameraDistortionController::Create()
	{
		//쉐이더 생성
		m_pDistortionShader = REngine::GetDevice().CreateShaderFX("Distortion.fx");

		
		//---------------------------------------------------------------------
		// Texture 생성
		RFORMAT eRenderTargetFormat = RFMT_A8R8G8B8;

		int nScreenWidth	= REngine::GetDevice().GetScreenWidth();
		int nScreenHeight	= REngine::GetDevice().GetScreenHeight();

		if ( false == REngine::GetDevice().QueryFeature(RQF_PS20) )
			return false;

		//RenderTarget
		m_pRenderTarget = REngine::GetDevice().CreateRenderTargetTexture(nScreenWidth, nScreenHeight, eRenderTargetFormat);
		m_pMaskingMap= REngine::GetDevice().CreateRenderTargetTexture(nScreenWidth, nScreenHeight, eRenderTargetFormat);
		m_pDistortredRT =REngine::GetDevice().CreateRenderTargetTexture(nScreenWidth, nScreenHeight, eRenderTargetFormat);

		//NormalMap
		m_pNormalMap =  REngine::GetDevice().CreateTexture( DISTORTION_DEFAUL_NORMALMAP );
		//---------------------------------------------------------------------


		if(!m_pDistortionShader || !m_pRenderTarget || !m_pMaskingMap || !m_pDistortredRT || !m_pNormalMap)
		{
			m_bValid =false;

			mlog3("Distortion 카메라 생성 실패!\n");

			return m_bValid;
		}
		else
			m_bValid =true;


		//-------------------------------------------------------
		// shader 관련
		m_pDistortionShader->GetConstant("g_tMaskingMap").SetTexture(m_pMaskingMap, REngine::GetDevicePtr());
		m_pDistortionShader->GetConstant("g_fDistortionScale").SetFloat(0.03f);

		RMatrix matRTTex= RMatrix::IDENTITY;
		matRTTex.SetTranslation(RVector(0.5f +0.5f/nScreenWidth, 0.5f+0.5f/nScreenHeight, 0.0f));
		matRTTex.SetScale(RVector(0.5f, -0.5f, 1.0f));

		m_pDistortionShader->GetConstant("g_matRTTex").SetMatrix(matRTTex);
		//-------------------------------------------------------

		return m_bValid;
	}

	void RCameraDistortionController::Destroy()
	{
		REngine::GetDevicePtr()->DeleteTexture(m_pRenderTarget);
		REngine::GetDevicePtr()->DeleteTexture(m_pNormalMap);
		REngine::GetDevicePtr()->DeleteTexture(m_pMaskingMap);
		REngine::GetDevicePtr()->DeleteTexture(m_pDistortredRT);

		if (m_pDistortionShader)
			REngine::GetDevice().DeleteShaderFX(m_pDistortionShader);
	}

	void RCameraDistortionController::Restore()
	{
		REngine::GetDevice().ColorFill( m_pRenderTarget );
	}

	void RCameraDistortionController::DrawFullScreenQuad()
	{
		REngine::GetDevice().SetFillMode(RFILL_SOLID);

		// Ensure that we're directly mapping texels to pixels by offset by 0.5
		// For more info see the doc page titled "Directly Mapping Texels to Pixels"
		float fWidth5, fHeight5;

		fWidth5		= REngine::GetDevice().GetScreenWidth();
		fHeight5	= REngine::GetDevice().GetScreenHeight();

		// Draw the quad
		RVt_pos_tex_rhw vQuad[4];
		vQuad[0].vPos	= RVector4( -0.5f, -0.5f, 0.0f, 1.0f );
		vQuad[0].u		= 0;
		vQuad[0].v		= 0;

		vQuad[1].vPos	= RVector4( -0.5f, fHeight5 -0.5f, 0.0f, 1.0f );
		vQuad[1].u		= 0;
		vQuad[1].v		= 1;

		vQuad[2].vPos	= RVector4( fWidth5 -0.5f, -0.5f, 0.0f, 1.0f );
		vQuad[2].u		= 1;
		vQuad[2].v		= 0;

		vQuad[3].vPos	= RVector4( fWidth5 -0.5f, fHeight5 -0.5f, 0.0f, 1.0f );
		vQuad[3].u		= 1;
		vQuad[3].v		= 1;
		

		REngine::GetDevice().SetDepthEnable(false, false);
		REngine::GetDevice().SetFvF(RVt_pos_tex_rhw::FVF);
		REngine::GetDevice().DrawPrimitiveUP( RPT_TRIANGLESTRIP, 2, vQuad, sizeof( RVt_pos_tex_rhw ) );
		REngine::GetDevice().SetDepthEnable(true, true);
	}

	void RCameraDistortionController::OnPreRender( RInferredRenderingStrategy* pInferredRenderingStrategy)
	{
		RCameraSceneNodeController::OnPreRender( pInferredRenderingStrategy);

		m_pRenderTargetOrg = REngine::GetDevice().SetRenderTarget( 0, m_pRenderTarget);
	}

	void RCameraDistortionController::OnClear()
	{
		REngine::GetDevice().Clear( true, true, 
			m_pCameraSceneNode->isHaveStencil(), 
			m_pCameraSceneNode->GetClearColor(),
			m_pCameraSceneNode->GetClearDepth(),
			m_pCameraSceneNode->GetClearStencil() );
	}

	void RCameraDistortionController::OnPostRender()
	{
		REngine::GetDevice().SetRenderTarget( 0, m_pRenderTargetOrg);
		REngine::GetDevice().SetAlphaTestEnable(false);

		RenderTextureToFullScreen(m_pRenderTarget);
	}

	void RCameraDistortionController::SetFog()
	{
		float FogNear, FogFar;
		bool FogEnable;

		FogEnable = m_pCameraSceneNode->GetFogEnable();
		FogNear = m_pCameraSceneNode->GetFogNear();
		FogFar = m_pCameraSceneNode->GetFogFar();

		if (FogEnable)
		{
			m_pDistortionShader->GetConstant("g_vFogParams").SetVector3(FogNear, FogFar, FogFar - FogNear);
		}
		else
			m_pDistortionShader->GetConstant("g_vFogParams").SetVector3(1000000, 10000001, 1);

	}

	void RCameraDistortionController::CheckRenderTargetFormat()
	{
		RTexture* pRT0 = REngine::GetDevice().GetRenderTarget(0);

		if( m_pDistortredRT->GetInfo().pixelFormat != pRT0->GetInfo().pixelFormat )
		{
			REngine::GetDevice().DeleteTexture(m_pDistortredRT);
			REngine::GetDevice().DeleteTexture(m_pMaskingMap);

			int nScreenWidth	= REngine::GetDevice().GetScreenWidth();
			int nScreenHeight	= REngine::GetDevice().GetScreenHeight();
			RFORMAT eRenderTargetFormat = pRT0->GetInfo().pixelFormat;

			m_pDistortredRT =REngine::GetDevice().CreateRenderTargetTexture(nScreenWidth, nScreenHeight, eRenderTargetFormat);
			m_pMaskingMap =REngine::GetDevice().CreateRenderTargetTexture(nScreenWidth, nScreenHeight, eRenderTargetFormat);

			m_pDistortionShader->GetConstant("g_tMaskingMap").SetTexture(m_pMaskingMap, REngine::GetDevicePtr());
		}
	}

	void RCameraDistortionController::SetShaderConstant(const RDistortionParameter& param)
	{
		RVector2 vDir0(param.fDir0X, param.fDir0Y), vDir1(param.fDir1X, param.fDir1Y);

		m_pDistortionShader->GetConstant("g_dir0").SetVector2(vDir0.Normalize());
		m_pDistortionShader->GetConstant("g_dir1").SetVector2(vDir1.Normalize());
		m_pDistortionShader->GetConstant("g_fDistortionScale").SetFloat(param.fScale * param.fVisibility);
		m_pDistortionShader->GetConstant("g_fSpeed0").SetFloat(param.fSpeed0);
		m_pDistortionShader->GetConstant("g_fSpeed1").SetFloat(param.fSpeed1);
		m_pDistortionShader->GetConstant("g_bRender").SetInt(param.bRender);
		m_pDistortionShader->GetConstant("g_fRange").SetFloat(param.fRange);
		m_pDistortionShader->GetConstant("g_fAttenuationBegin").SetFloat(param.fAttenuationBegin);		
		m_pDistortionShader->GetConstant("g_matWVP").SetMatrix(m_pCameraSceneNode->GetViewProjectionMatrix());				
		m_pDistortionShader->GetConstant("g_fTime").SetFloat(REngine::GetTimer().GetTime() /1000.0f);

		if( strcmp(param.strNormalMapFileName, DISTORTION_DEFAUL_NORMALMAP) != 0 )
			m_pDistortionShader->GetConstant("g_tNormalMap").SetTexture(param.pNormalMap, REngine::GetDevicePtr());
		else
			m_pDistortionShader->GetConstant("g_tNormalMap").SetTexture(m_pNormalMap, REngine::GetDevicePtr());
	}

	void RCameraDistortionController::RenderTextureToFullScreen(RTexture* pTexture)
	{
		m_pDistortionShader->GetConstant("g_tRenderTarget").SetTexture(pTexture, REngine::GetDevicePtr());
		m_pDistortionShader->GetConstant("g_tNormalMap").SetTexture(m_pNormalMap, REngine::GetDevicePtr());

		//Set RenderState
		REngine::GetDevice().SetFogEnable(false);
		REngine::GetDevice().SetDepthEnable(false, false);
		REngine::GetDevice().SetBlending(RBLEND_NONE);

		RTechnique Technique = m_pDistortionShader->GetTechnique("NoDistortion");
		Technique.Begin();
		Technique.BeginPass();
		DrawFullScreenQuad();
		Technique.EndPass();
		Technique.End();
	}

	void RCameraDistortionController::RenderFullScreenDistortion(float fDistortionScale)
	{
		CheckRenderTargetFormat();

		//Set RenderState
		REngine::GetDevice().SetFogEnable(false);
		REngine::GetDevice().SetAlphaTestEnable(false);
		REngine::GetDevice().SetDepthEnable(false, false);
		REngine::GetDevice().SetBlending(RBLEND_NONE);

		RTexture* pRT0 = REngine::GetDevice().GetRenderTarget(0);

		m_pDistortionShader->GetConstant("g_tRenderTarget").SetTexture(pRT0, REngine::GetDevicePtr());
		m_pDistortionShader->GetConstant("g_fDistortionScale").SetFloat(fDistortionScale);
		m_pDistortionShader->GetConstant("g_fSpeed0").SetFloat(0.5f);
		m_pDistortionShader->GetConstant("g_fSpeed1").SetFloat(0.5f);
		m_pDistortionShader->GetConstant("g_fTime").SetFloat(REngine::GetTimer().GetTime() /1000.0f);

		//-------------------------------------------------------------------------------
		//Render distorted Scene to Temp RenderTarget
		REngine::GetDevice().SetRenderTarget(0, m_pDistortredRT);
		RTechnique Technique = m_pDistortionShader->GetTechnique("FullScreenDistortion");
		Technique.Begin();
		Technique.BeginPass();
		DrawFullScreenQuad();
		Technique.EndPass();
		Technique.End();
		//-------------------------------------------------------------------------------

		//-------------------------------------------------------------------------------
		//Render Temp RenderTarget to current RenderTarget
		REngine::GetDevice().SetRenderTarget(0, pRT0);
		REngine::GetDevice().SetAlphaTestEnable(false);

		RenderTextureToFullScreen(m_pDistortredRT);
		//-------------------------------------------------------------------------------
	}

	void RCameraDistortionController::RenderEmitterDistortion(int nParticleCount, RDistortionParameter& param)
	{
		SetShaderConstant(param);
		
		SetFog();

		REngine::GetDevice().ColorFill(m_pDistortredRT);
		
		
		//------------------------------------------------------------------------------------
		//Render distorted Emitter to Temp RenderTarget

		//Set RenderState
		REngine::GetDevice().SetFogEnable(false);
		REngine::GetDevice().SetBlending(RBLEND_NONE);

		RTexture* pRT0 = REngine::GetDevice().SetRenderTarget(0, m_pDistortredRT);
		
		m_pDistortionShader->GetConstant("g_tRenderTarget").SetTexture(pRT0, REngine::GetDevicePtr());

		RTechnique Technique = m_pDistortionShader->GetTechnique("Emitter_Distortion");
		Technique.Begin();
		Technique.BeginPass(0);
		if( REffectManager::DrawPrimitive( RPT_TRIANGLELIST, nParticleCount*2) )
		{
			REngine::GetProfileInfo().AddPolygon(PPT_PARTICLE, nParticleCount*2 );
		}
		Technique.EndPass();
		Technique.End();
		//------------------------------------------------------------------------------------

		

		//------------------------------------------------------------------------------------
		//Draw Temp RenderTarget over current RenderTarget
 		REngine::GetDevice().SetRenderTarget(0, pRT0);
		RenderResultToRenderTarget();		
		//------------------------------------------------------------------------------------
	}

	void RCameraDistortionController::RenderEffectActorDistortion(const vector<RActorNode*>& ActorNode, RDistortionParameter& param)
	{
		CheckRenderTargetFormat();

		SetShaderConstant(param);

		REngine::GetDevice().ColorFill(m_pDistortredRT);
		REngine::GetDevice().ColorFill(m_pMaskingMap);

		RTechnique Technique = m_pDistortionShader->GetTechnique("Actor_Distortion");
		Technique.Begin();

		//------------------------------------------------------------------------------------
		//Pass0: Render EffectActor to Masking RenderTarget

		//Set RenderState
		m_pMaskingMapOrg = REngine::GetDevice().SetRenderTarget(1, m_pMaskingMap);
		REngine::GetDevice().SetBlending(RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);
		REngine::GetDevice().SetDepthEnable(true, false);
		REngine::GetDevice().SetCullMode(RCULL_NONE);

		Technique.BeginPass(0);
		for(size_t i=0;i<ActorNode.size();i++)
		{
			RActorNode* pActorNode = ActorNode[i];

			RenderEffecActorNode(pActorNode, Technique);
		}
		Technique.EndPass();
		//------------------------------------------------------------------------------------

		//------------------------------------------------------------------------------------
		//Pass1: Render distorted EffectActor to Temp RenderTarget

		//Set RenderState
		REngine::GetDevice().SetFogEnable(false);
		REngine::GetDevice().SetBlending(RBLEND_NONE);

		RTexture* pRT0 = REngine::GetDevice().SetRenderTarget(0, m_pDistortredRT);
		REngine::GetDevice().SetRenderTarget(1, m_pMaskingMapOrg);

		m_pDistortionShader->GetConstant("g_tRenderTarget").SetTexture(pRT0, REngine::GetDevicePtr());

		Technique.BeginPass(1);
		for(size_t i=0;i<ActorNode.size();i++)
		{
			RActorNode* pActorNode = ActorNode[i];
			
			RenderEffecActorNode(pActorNode, Technique);
		}
		Technique.EndPass();
		//------------------------------------------------------------------------------------

		Technique.End();

		//------------------------------------------------------------------------------------
		//Render Temp RenderTarget to current RenderTarget
		REngine::GetDevice().SetRenderTarget(0, pRT0);
		RenderResultToRenderTarget();
		//------------------------------------------------------------------------------------
	}

	void RCameraDistortionController::BeginMasking(RDistortionParameter& parameter)
	{
		CheckRenderTargetFormat();

		REngine::GetDevice().ColorFill(m_pMaskingMap);
		m_pMaskingMapOrg =REngine::GetDevice().SetRenderTarget(1, m_pMaskingMap);

		RShaderCompositeManager::m_cAttenuationBegin.SetFloat(parameter.fAttenuationBegin);

		if( !parameter.bRender)
			REngine::GetDevice().SetColorWriteEnable(false);
	}

	void RCameraDistortionController::EndMasking()
	{
		REngine::GetDevice().SetRenderTarget(1, m_pMaskingMapOrg);
		REngine::GetDevice().SetColorWriteEnable(true);
	}

	void RCameraDistortionController::RenderEffecActorNode(RActorNode* pActorNode, RTechnique technique)
	{
		pActorNode->UpdateTransform();

		RMatrix matWorld = pActorNode->GetWorldTransform();

		RMeshNode* pMeshNode = pActorNode->m_pMeshNode;
		bool bPhysiue = pMeshNode->m_nPhysiqueCnt && !pMeshNode->isClothMesh();
		
		if(bPhysiue)
		{
			m_pDistortionShader->GetConstant("g_blendMatrices").SetMatrixArray(&pActorNode->m_matBones[0],pMeshNode->m_nBoneTableCnt);

			RActor* pParentActor = pActorNode->m_pActor;
			if(pParentActor)
				matWorld = pParentActor->GetWorldTransform();
		}

		m_pDistortionShader->GetConstant("g_bPhysique").SetInt(bPhysiue);
		m_pDistortionShader->GetConstant("g_matWVP").SetMatrix(matWorld * m_pCameraSceneNode->GetViewProjectionMatrix());

		technique.CommitChanges();

		pActorNode->RenderOnlyMesh();
	}

	void RCameraDistortionController::RenderResultToRenderTarget()
	{
		RCMPFUNC AlphaFunc = REngine::GetDevice().GetAlphaFunc();
		DWORD AlphaRef = REngine::GetDevice().GetAlphaRef();
		REngine::GetDevice().SetAlphaTestEnable(true);
		REngine::GetDevice().SetAlphaFunc(RCMP_GREATER);
		REngine::GetDevice().SetAlphaRef(0);

		RenderTextureToFullScreen(m_pDistortredRT);

		REngine::GetDevice().SetAlphaTestEnable(false);
		REngine::GetDevice().SetAlphaFunc(AlphaFunc);
		REngine::GetDevice().SetAlphaRef(AlphaRef);
	}

	// RCameraDistortionController
	//=======================================================================================================
	//=======================================================================================================
	
}