#include "stdafx.h"
#include "RWaterPassRenderer.h"
#include "RRenderHelper.h"
#include "RTerrain.h"
#include "RStaticMeshSceneNode.h"
#include "RDirectionalLightSceneNode.h"
#include "RWaterSceneNode.h"
#include "RPostEffectDistortion.h"
#include "RShaderCompositeManager.h"
#include "RShaderComposite.h"
#include "RShaderFX.h"


namespace rs3 {


//////////////////////////////////////////////////////////////////////////
// WaterPassRendererListener
class WaterPassRendererListener : public RListener
{
public:
	WaterPassRendererListener(RWaterPassRenderer* pWaterRenderer) : m_pWaterRenderer( pWaterRenderer) {}

	virtual void Update() override
	{
		m_pWaterRenderer->RecreateBuffer();
	}

	RWaterPassRenderer* m_pWaterRenderer;
};
// WaterPassRendererListener
//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////
// WaterPassRenderer

RWaterPassRenderer::RWaterPassRenderer() : 
	m_pWaterShader(NULL), m_fTime(0),
	m_bRenderDisable(false),
	m_pRefractionMap(NULL),
	m_pMakingMap(NULL)
{
	m_pEventListener = new WaterPassRendererListener(this);
	REngine::GetEventBroadcaster(RE_DEVICE_RESTORE)->AddListener( m_pEventListener );
}

RWaterPassRenderer::~RWaterPassRenderer()
{
	REngine::GetEventBroadcaster(RE_DEVICE_RESTORE)->RemoveListener( m_pEventListener );
	SAFE_DELETE(m_pEventListener);
}

void RWaterPassRenderer::CreateVertexFormat()
{
	RVertexAttribute waterattribs[] = 
	{ 
		TYPE_VERTEX, FORMAT_FLOAT, 3,
		TYPE_TEXCOORD, FORMAT_FLOAT, 1,
	};

	m_hVertexFormat = REngine::GetDevice().CreateVertexFormat(waterattribs, _countof(waterattribs));
}

void RWaterPassRenderer::CreateShader()
{
	m_pWaterShader = REngine::GetDevice().CreateShaderFX("Water.fx");

	m_WaterTechnique = m_pWaterShader->GetTechnique("Water");
	m_WaterTechniqueLow = m_pWaterShader->GetTechnique("WaterLow");
	m_UnderWaterTechnique = m_pWaterShader->GetTechnique("UnderWater");
	
	m_scViewProjection = m_pWaterShader->GetConstant("g_mModelViewProj");
	m_scViewInv = m_pWaterShader->GetConstant("g_mModelViewInv");
	m_scRefractionTextureMat = m_pWaterShader->GetConstant("g_mTextureTrans");
	m_scResolution = m_pWaterShader->GetConstant("g_vResolution");
	m_scViewProjInvMat= m_pWaterShader->GetConstant("g_matViewProjInv");

	m_scFogColor = m_pWaterShader->GetConstant("g_vFogColor");
	m_scFogParam = m_pWaterShader->GetConstant("g_vFogParams");
	m_scRefractionTexture = m_pWaterShader->GetConstant("g_tDiffuseMap");	
	m_scMaskingTexture = m_pWaterShader->GetConstant("g_tMasking");
	m_scDepthBuffer = m_pWaterShader->GetConstant("g_tDepthBuffer");

	m_scDepthLimit = m_pWaterShader->GetConstant("g_fLimitDepth");

	m_scBumpUVScale = m_pWaterShader->GetConstant("g_vBumpUVScale");

	m_scBumpMove1 = m_pWaterShader->GetConstant("g_vBumpMove1");
	m_scBumpMove2 = m_pWaterShader->GetConstant("g_vBumpMove2");

	m_scBumpAdjustFactor = m_pWaterShader->GetConstant("g_fBumpAdjustFactor");

	m_scBumpWeight1 = m_pWaterShader->GetConstant("g_vBumpWeight1");
	m_scBumpWeight2 = m_pWaterShader->GetConstant("g_vBumpWeight2");
	m_scBumpWeight3 = m_pWaterShader->GetConstant("g_vBumpWeight3");
	m_scBumpWeight4 = m_pWaterShader->GetConstant("g_vBumpWeight4");
	m_scBumpColorToVector = m_pWaterShader->GetConstant("g_vBumpColorToVector");

	m_scTime = m_pWaterShader->GetConstant("g_fTime");
	m_scCameraPos = m_pWaterShader->GetConstant("g_vCameraPos");

	m_scBumpTexture1 = m_pWaterShader->GetConstant("g_tBumpMap");
	m_scBumpTexture2 = m_pWaterShader->GetConstant("g_tBumpMap2");
	m_scShoreTexture = m_pWaterShader->GetConstant("g_tWaterShore");

	m_scWaterRGBHalflife = m_pWaterShader->GetConstant("g_fWaterRGBHalflife");

	m_scFresnelConstant = m_pWaterShader->GetConstant("g_fFresnelConstant");
	m_scReflectAdjust = m_pWaterShader->GetConstant("g_fReflectAdjust");

	m_scHalfVector = m_pWaterShader->GetConstant("g_vHalfVector");
	m_scDiffuseColor = m_pWaterShader->GetConstant("g_vDiffuseColor");
	m_scWaterMaterial = m_pWaterShader->GetConstant("g_vWaterMaterial");

	m_scScale =m_pWaterShader->GetConstant("g_fScale");
	m_scAmplitude1 =m_pWaterShader->GetConstant("g_fAmplitude1");
	m_scPhase1 =m_pWaterShader->GetConstant("g_fPhase1");

	m_scFarClipCornerInView = m_pWaterShader->GetConstant( "g_vFarClipCornerInView" );

	m_scShoreInfo = m_pWaterShader->GetConstant( "g_vShoreInfo" );

	m_scRefractAttenuation = m_pWaterShader->GetConstant( "g_fRefractAttenuation" );
}

void RWaterPassRenderer::CreateBuffer()
{
	RSceneManager* pSceneManager = REngine::GetSceneManagerPtr();
	_ASSERT( pSceneManager );
	RRenderingStrategy* pCurrentRenderingStrategy = pSceneManager->GetRenderingStrategy();
	_ASSERT( pCurrentRenderingStrategy );
	DWORD dwWidth = pCurrentRenderingStrategy->GetWidth();
	DWORD dwHeight = pCurrentRenderingStrategy->GetHeight();

	m_pRefractionMap = REngine::GetDevice().CreateRenderTargetTexture(dwWidth, dwHeight, RFMT_A8R8G8B8);
	m_pMakingMap = REngine::GetDevice().CreateRenderTargetTexture(dwWidth, dwHeight, RFMT_A8R8G8B8);
}

void RWaterPassRenderer::DestroyBuffer()
{
	REngine::GetDevice().DeleteTexture(m_pRefractionMap);
	REngine::GetDevice().DeleteTexture(m_pMakingMap);

	m_pRefractionMap =NULL;
	m_pMakingMap =NULL;
}

void RWaterPassRenderer::RecreateBuffer()
{
	DestroyBuffer();
	CreateBuffer();
}

void RWaterPassRenderer::Reset()
{
	m_fTime = 0;
}

void RWaterPassRenderer::Init()
{
	Reset();

	CreateVertexFormat();
	CreateShader();
	CreateBuffer();
}

void RWaterPassRenderer::Destroy()
{
	if (REngine::GetDevicePtr())
		REngine::GetDevice().DeleteShaderFX(m_pWaterShader);

	m_pWaterShader = NULL;

	DestroyBuffer();
}

void RWaterPassRenderer::Update(float timegap)
{
	m_fTime += timegap;
}

void RWaterPassRenderer::UploadShaderConst(RCameraSceneNode* pCamera, RRENDERINGMETHOD renderMethod)
{
	RPFC_THISFUNC;
	m_scCameraPos.SetVector3(pCamera->GetPosition());
	m_scViewProjection.SetMatrix(pCamera->GetViewProjectionMatrix());
	
	RMatrix matView, matViewInv;
	matView = pCamera->GetViewMatrix();
	matView.GetInverse(&matViewInv);
	m_scViewInv.SetMatrix(matViewInv);
	
	m_scTime.SetFloat(m_fTime);


	RDirectionalLightSceneNode* Light = REngine::GetSceneManager().GetDefaultLight();
	if (Light != NULL)
	{
		RVector L = -Light->GetDirection();

		m_scHalfVector.SetVector3(L);
		m_scDiffuseColor.SetVector4(Light->GetDiffuse() * Light->GetPower());
	}

	RSceneManager* pSceneManager = REngine::GetSceneManagerPtr();
	_ASSERT( pSceneManager );
	RRenderingStrategy* pCurrentRenderingStrategy = pSceneManager->GetRenderingStrategy();
	_ASSERT( pCurrentRenderingStrategy );
	DWORD dwScreenWidth = pCurrentRenderingStrategy->GetWidth();
	DWORD dwScreenHeight = pCurrentRenderingStrategy->GetHeight();

	RMatrix matWorldToScreen;
	RMatrix matProjectionToScreen;
	matProjectionToScreen.SetScreenSpaceMatrix(dwScreenWidth, dwScreenHeight);
	matWorldToScreen = pCamera->GetViewProjectionMatrix() * matProjectionToScreen;

	m_scRefractionTextureMat.SetMatrix(matWorldToScreen);
	m_scResolution.SetVector4( RVector4( (float)dwScreenWidth, (float)dwScreenHeight, 0.5f/(float)dwScreenWidth, 0.5f/(float)dwScreenHeight));

	RMatrix matViewProjInv;
	pCamera->GetViewProjectionMatrix().GetInverse(&matViewProjInv);
	m_scViewProjInvMat.SetMatrix(matViewProjInv);

	if( renderMethod == RENDERING_DEFERRED || renderMethod == RENDERING_INFERRED)
	{
		RTexture* pDepthBuffer = REngine::GetSceneManager().GetRenderingStrategy()->GetDepthBuffer();
		m_scDepthBuffer.SetTexture(pDepthBuffer,REngine::GetDevicePtr());
	}
	else
		m_scDepthBuffer.SetTexture(NULL,REngine::GetDevicePtr());

	m_scFarClipCornerInView.SetVector3(pCamera->GetFarClipCornerInView());
}

void RWaterPassRenderer::UploadShaderConstFromWPResource( RCameraSceneNode* pCurrentCamera, RWaterPlane* pWaterPlane)
{
	RPFC_THISFUNC;
	RVector4 FogColor;
	float FogNear, FogFar;
	bool FogEnable;

	RWaterPlane::SWaterPlaneProperty* pProperty = pWaterPlane->GetWaterPlaneProperty();

	if (pProperty->m_bUseCustomFog)
	{
		FogColor.Set(pProperty->m_vFogColor.x, pProperty->m_vFogColor.y, pProperty->m_vFogColor.z, 1);
		FogEnable = true;
		FogNear = pProperty->m_fFogNear;
		FogFar = pProperty->m_fFogFar;
	}
	else
	{
		FogColor = RVector4( pCurrentCamera->GetFogColor() );
		FogEnable = pCurrentCamera->GetFogEnable();
		FogNear = pCurrentCamera->GetFogNear();
		FogFar = pCurrentCamera->GetFogFar();
	}

	m_scFogColor.SetVector4(FogColor);
	if (FogEnable)
	{
		m_scFogParam.SetVector3(FogNear, FogFar, FogFar - FogNear);
	}
	else
		m_scFogParam.SetVector3(1000000, 10000001, 1);

	m_scBumpTexture1.SetTexture(pWaterPlane->GetBumpTexture1(), REngine::GetDevicePtr());
	m_scBumpTexture2.SetTexture(pWaterPlane->GetBumpTexture2(), REngine::GetDevicePtr());
	m_scShoreTexture.SetTexture(pWaterPlane->GetShoreTexture(), REngine::GetDevicePtr());

	m_scDepthLimit.SetFloat(1.0f/pProperty->m_fDepthLimit);
	m_scBumpUVScale.SetVector4(RVector4(1.0f/pProperty->m_fBumpUVScale1, 1.0f/pProperty->m_fBumpUVScale2, 1.0f/pProperty->m_fBumpUVScale3, 1.0f/pProperty->m_fBumpUVScale4));
	m_scWaterRGBHalflife.SetVector3(RVector(1.0f/pProperty->m_vWaterRGBHalflife.x, 1.0f/pProperty->m_vWaterRGBHalflife.y, 1.0f/pProperty->m_vWaterRGBHalflife.z));

	m_scBumpMove1.SetVector4(RVector4(pProperty->m_vBumpMove1.x, pProperty->m_vBumpMove1.y, pProperty->m_vBumpMove2.x, pProperty->m_vBumpMove2.y));
	m_scBumpMove2.SetVector4(RVector4(pProperty->m_vBumpMove3.x, pProperty->m_vBumpMove3.y, pProperty->m_vBumpMove4.x, pProperty->m_vBumpMove4.y));

	RVector4 w1, w2, w3, w4, ctv;
	w1.Set(pProperty->m_fBumpWeight1, pProperty->m_fBumpWeight1, 2.0f, 0.0f);
	w2.Set(pProperty->m_fBumpWeight2, pProperty->m_fBumpWeight2, 2.0f, 0.0f);
	w3.Set(pProperty->m_fBumpWeight3, pProperty->m_fBumpWeight3, 2.0f, 0.0f);
	w4.Set(pProperty->m_fBumpWeight4, pProperty->m_fBumpWeight4, 2.0f, 0.0f);
	ctv = (w1 + w2 + w3 + w4) * -0.5f;
	m_scBumpWeight1.SetVector4(w1);
	m_scBumpWeight2.SetVector4(w2);
	m_scBumpWeight3.SetVector4(w3);
	m_scBumpWeight4.SetVector4(w4);
	m_scBumpColorToVector.SetVector4(ctv);
	m_scBumpAdjustFactor.SetFloat(pProperty->m_fBumpAdjustFactor);
	m_scFresnelConstant.SetFloat(pProperty->m_fFresnelConstant);
	m_scReflectAdjust.SetFloat(pProperty->m_fReflectAdjust);
	
	m_scWaterMaterial.SetVector3(pProperty->m_vDiffuseColor);

	m_scScale.SetFloat(pProperty->m_fScale);
	m_scAmplitude1.SetFloat(pProperty->m_fAmplitude1);
	m_scPhase1.SetFloat(pProperty->m_fPhase1);

	m_scShoreInfo.SetVector4(RVector4(pProperty->m_vShoreUVScale.x, pProperty->m_vShoreUVScale.y, pProperty->m_fShoreIntensity, pProperty->m_fBlending));

	m_scRefractAttenuation.SetVector2(RVector2(pProperty->m_fRefractAttenuationDist,pProperty->m_fRefractAttenuationFactor));

	//m_scResolution.SetVector4( RVector4( (float)dwScreenWidth, (float)dwScreenHeight, 0.5f/(float)dwScreenWidth, 0.5f/(float)dwScreenHeight));
}

void RWaterPassRenderer::RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	RPFC_THISFUNC;

	if (m_bRenderDisable)
		return;

	UploadShaderConst( pCurrentCamera, renderMethod);
	SetUnderWaterEffect(pCurrentCamera, pRenderingList);

	SCENE_SET::iterator iter;
	int count=0;
	for( iter = pRenderingList->begin(); iter != pRenderingList->end(); ++iter)
	{
		RWaterPlane* pWater = (RWaterPlane*)(*iter);
		if( !pWater->GetVisible() || !pWater->GetUsable() || pWater->GetOcclued() )	continue;
		RenderWaterPlaneMStage( pCurrentCamera, pWater, count );
		count++;
	}

	RPFC_GPU_CALL;
}

void RWaterPassRenderer::RenderWaterPlaneMStage( RCameraSceneNode* pCurrentCamera, RWaterPlane* wp, int index )
{
	RPFC_THISFUNC;
	UploadShaderConstFromWPResource( pCurrentCamera, wp);
	
	//-------------------------------------------------------------------------
	//Set Texture
	RTexture* pRT0 = REngine::GetDevice().GetRenderTarget(0);
	if(pRT0!=RT_FRAMEBUFFER)
	{
		RenderMaskingMap(wp);

		REngine::GetDevice().SetRenderTarget(0, m_pRefractionMap);
		REngine::GetDevice().Clear(true, false);

		m_scRefractionTexture.SetTexture(pRT0, REngine::GetDevicePtr());
		m_scMaskingTexture.SetTexture(m_pMakingMap, REngine::GetDevicePtr());
	}
	else
	{
		REngine::GetDevice().SetBlending( RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);
		m_scRefractionTexture.SetTexture(NULL, REngine::GetDevicePtr());
	}
	//-------------------------------------------------------------------------
	


	//---------------------------------------------------------
	//Set Render State
	REngine::GetDevice().SetCullMode(RCULL_NONE);
	//---------------------------------------------------------



	//--------------------------------------------------------------------------
	//Render
	bool bUnderWater =wp->IsUnderWater(pCurrentCamera->GetWorldPosition());
	RTechnique nowTech = bUnderWater ? m_UnderWaterTechnique : ( pRT0!=RT_FRAMEBUFFER ? m_WaterTechnique : m_WaterTechniqueLow);

	REngine::GetDevice().SetVertexFormat(m_hVertexFormat);

	nowTech.Begin();
	nowTech.BeginPass();	
	wp->Render();
	nowTech.EndPass();
	nowTech.End();
	//--------------------------------------------------------------------------


	//Restore state
	REngine::GetDevice().SetCullMode(RCULL_CW);
	REngine::GetDevice().SetBlending( RBLEND_NONE);


	if(pRT0!=RT_FRAMEBUFFER)
	{
		REngine::GetDevice().SetRenderTarget(0, pRT0);
		RenderResultToScreen();
	}


	//--------------------------------------------------------------------------
	//Apply UnderWater Effect
	if( wp->IsApplyUnderWaterEffect(pCurrentCamera->GetWorldPosition()) )
	{
		RPostEffectDistortion* pPostEffectDistortion = (RPostEffectDistortion*)REngine::GetSceneManager().GetPostEffect(PE_DISTORTION);
		bool bDistortion = pPostEffectDistortion && pPostEffectDistortion->IsValid();
		if(bDistortion)
			pPostEffectDistortion->RenderFullScreenDistortion(wp->GetWaterPlaneProperty()->m_fUnderWaterBumpUVScale*0.001f);
	}
	//--------------------------------------------------------------------------	
}

void RWaterPassRenderer::SetUnderWaterEffect( RCameraSceneNode* cam, SCENE_SET* pRenderingList )
{
	RPFC_THISFUNC;
	SCENE_SET::iterator iter;
	for( iter = pRenderingList->begin(); iter != pRenderingList->end(); ++iter)
	{
		RWaterPlane* pWater = (RWaterPlane*)(*iter);
		MVector3 vCameraPosition = cam->GetWorldPosition();

		bool bUnderWater = pWater->IsApplyUnderWaterEffect(vCameraPosition);
		if(!bUnderWater)
			continue;
	
		float d = 7000;
		RWaterPlane::SWaterPlaneProperty* pWaterProperty = pWater->GetWaterPlaneProperty();
		RVector vWaterRGBHalflife =pWaterProperty->m_vWaterRGBHalflife;
		RVector vWaterDiffuseColor =pWaterProperty->m_vDiffuseColor;

		int R = exp(-d/vWaterRGBHalflife.x)*vWaterDiffuseColor.x *255;
		int G = exp(-d/vWaterRGBHalflife.y)*vWaterDiffuseColor.y *255;
		int B = exp(-d/vWaterRGBHalflife.z)*vWaterDiffuseColor.z *255; 	

		DWORD dwUnderWaterFogColor = D3DCOLOR_RGBA( R, G, B, 255);

		cam->SetUnderWaterFogEnable(true);
		cam->SetUnderWaterFog(pWaterProperty->m_fUnderWaterFogNear, pWaterProperty->m_fUnderWaterFogFar, dwUnderWaterFogColor );		

		return;
	}

	//Not UnderWater
	cam->SetUnderWaterFogEnable(false);		
}

void RWaterPassRenderer::RenderMaskingMap(RWaterPlane* pWaterPlane)
{
	RPFC_THISFUNC;
	REngine::GetDevice().SetCullMode(RCULL_NONE);

	REngine::GetDevice().SetRenderTarget(0, m_pMakingMap);
	REngine::GetDevice().Clear(true, false);


	RTechnique MaskingTech = m_pWaterShader->GetTechnique("Masking");
	MaskingTech.Begin();
	MaskingTech.BeginPass();
	REngine::GetDevice().SetVertexFormat(m_hVertexFormat);
	pWaterPlane->Render();
	MaskingTech.EndPass();
	MaskingTech.End();
}

void RWaterPassRenderer::RenderResultToScreen()
{
	RDevice* pDevice =REngine::GetDevicePtr();

	RSceneManager* pSceneManager = REngine::GetSceneManagerPtr();
	_ASSERT( pSceneManager );
	RRenderingStrategy* pCurrentRenderingStrategy = pSceneManager->GetRenderingStrategy();
	_ASSERT( pCurrentRenderingStrategy );
	DWORD dwScreenWidth = pCurrentRenderingStrategy->GetWidth();
	DWORD dwScreenHeight = pCurrentRenderingStrategy->GetHeight();

	pDevice->SetDepthEnable(false, false);
	pDevice->SetBlending(RBLEND_SRCALPHA,RBLEND_INVSRCALPHA);

	m_pWaterShader->GetConstant("g_tMasking").SetTexture(m_pRefractionMap, REngine::GetDevicePtr());

	RTechnique RenderResultTech = m_pWaterShader->GetTechnique("RenderResultToScreen");
	RenderResultTech.Begin();
	RenderResultTech.BeginPass();
	RRenderHelper::RenderQuad(0, 0, dwScreenWidth, dwScreenHeight);
	RenderResultTech.EndPass();
	RenderResultTech.End();

	pDevice->SetDepthEnable(true, true);
	pDevice->SetBlending(RBLEND_NONE);
}

}
