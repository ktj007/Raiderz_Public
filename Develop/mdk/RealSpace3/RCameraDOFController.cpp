#include "StdAfx.h"
#include "RCameraDOFController.h"
#include "RCameraSceneNode.h"
#include "RVType.h"
#include "RShaderCompositeManager.h"


//#define GAUSSIAN_TEXTURE_SIZE	512

namespace rs3 {


class DOFControllerListener : public RListener
{
	RCameraDOFController* m_pDOFController;
public:
	DOFControllerListener(RCameraDOFController* pDOFController) : m_pDOFController(pDOFController) {}

	virtual void Update() override
	{
		m_pDOFController->Destroy();
		m_pDOFController->Create();
	}
};



RCameraDOFController::RCameraDOFController(  RCameraSceneNode* pCameraSceneNode )
: RCameraSceneNodeController( RCCT_POSTRENDER, "1_DOF", true )
, m_pCameraSceneNode(pCameraSceneNode)
//, m_pColorBuffer(NULL)
, m_pColorBufferOwn(NULL)
, m_pDepthBlurBufferBlured(NULL)
, m_pDepthBlurBufferBluredExpanded(NULL)
, m_pDepthBlurBuffer(NULL)
, m_pColorBufferOwnOrg(NULL)
, m_pDOFShader(NULL)
, m_fNearBlurDist(-1)
, m_fFarBlurDist(-1)
, m_fMaxBlurOfBehind(1)
, m_fFocusRange(0)
, m_fCoCSize(5)
, m_fFocusDist(500)
, m_bEdgeBlur(true)
{
	m_bActive	= true;

	m_eFiterType = DFT_COC_N_GAUSSIAN;
	
	m_dwWidth = 0;
	m_dwHeight = 0;

	m_dwWidthGaussian = 0;
	m_dwHeightGaussian = 0;

	m_nNumTaps = 0;
	
	// event 구독
	m_pEventListener = new DOFControllerListener(this);
	REngine::GetEventBroadcaster(RE_DEVICE_RESTORE)->AddListener( m_pEventListener );

	if( !Create())
		mlog3("DOF 카메라 생성 실패!\n");
}

RCameraDOFController::~RCameraDOFController(void)
{
	// event 해제
	REngine::GetEventBroadcaster(RE_DEVICE_RESTORE)->RemoveListener( m_pEventListener );

	SAFE_DELETE(m_pEventListener);

	Destroy();
}






bool RCameraDOFController::Create()
{
	// 속도 문제로 RFMT_A16B16G16R16F(64bit)를 사용 하지 않고 32bit짜리 사용 
	// 64bit짜리를 사용 하는 HDR과 순서를 잘 설정 해야 할 것이다..
	RFORMAT eDOFBufferFormat = RFMT_A8R8G8B8;
	RFORMAT eInfoFormat =  RFMT_G16R16F;

	m_dwWidth	= (DWORD)REngine::GetDevice().GetScreenWidth();
	m_dwHeight	= (DWORD)REngine::GetDevice().GetScreenHeight();

	m_dwWidthGaussian = m_dwWidth/4;
	m_dwHeightGaussian = m_dwHeight/4;

	if ( false == REngine::GetDevice().QueryFeature(RQF_PS20) )
		return false;

	if ( false == REngine::GetDevice().QueryFeature(RQF_PS30) )
		return false;

	// 사용 할지 안할지 모르지만 런타임 시 다른 부분(HDR등)에서 MRT 사용 유무가 바뀔 수 있으므로 우선 무조건 생성
	m_pColorBufferOwn = REngine::GetDevice().CreateRenderTargetTexture( m_dwWidth, m_dwHeight, eDOFBufferFormat);
	if(R_NONE==m_pColorBufferOwn) return false;

	m_pInfoBufferExpandBlurred = REngine::GetDevice().CreateRenderTargetTexture( m_dwWidth, m_dwHeight, eInfoFormat);
	if( R_NONE == m_pInfoBufferExpandBlurred) return false;

	m_pDepthBlurBuffer = REngine::GetDevice().CreateRenderTargetTexture( m_dwWidth, m_dwHeight, eInfoFormat);
	if( R_NONE == m_pDepthBlurBuffer) return false;

	m_pDepthBlurBufferBlured = REngine::GetDevice().CreateRenderTargetTexture( m_dwWidthGaussian, m_dwHeightGaussian, eInfoFormat);
	if( R_NONE == m_pDepthBlurBufferBlured) return false;

	m_pDepthBlurBufferBluredExpanded = REngine::GetDevice().CreateRenderTargetTexture( m_dwWidthGaussian, m_dwHeightGaussian, eInfoFormat);
	if( R_NONE == m_pDepthBlurBufferBluredExpanded) return false;

	m_pGaussianBuffer = REngine::GetDevice().CreateRenderTargetTexture( m_dwWidthGaussian, m_dwHeightGaussian, eDOFBufferFormat);
	if(R_NONE==m_pGaussianBuffer) return false;

	switch( m_eFiterType)
	{
	case DFT_COC:
		m_pDOFShader = REngine::GetDevice().CreateShaderFX("DOF_CoC.fx");
		break;
	case DFT_COC_N_GAUSSIAN:
		m_pDOFShader = REngine::GetDevice().CreateShaderFX("DOF_CoCnGaussian.fx");
		break;
	}
	

	// 랜더 타겟이 설정 되어 있지 않으면 설정 해준다. - 090617, OZ
	// HDR, DOF 둘이 stage0에 같은 포맷의 랜더 타겟을 사용 한다. 둘중 하나만 설정 되게 하고 같이 사용 하도록 한다.
	// 다른 것이 추가 되었는데 포맷이 다르면? MRT 사용 하는 것이 추가가 되면?? 아아~ 젠젠와까라나이~ ㅠㅠ 그건 그때가서~
	//if( m_pCameraSceneNode->GetRenderTarget(0) == RT_FRAMEBUFFER)
	//{
	//	m_pCameraSceneNode->SetRenderTarget( 0, m_pColorBufferOwn );
	//	m_pColorBuffer = m_pColorBufferOwn;
	//}
	//else
	//{
	//	m_pColorBuffer = m_pCameraSceneNode->GetRenderTarget(0);
	//}
	//
	//// stage 1 RT에 정보를 받는다.
	//m_pCameraSceneNode->SetRenderTarget( 1, m_pInfoBuffer );
	// 왜 글케 복잡하게 생각했는지..
	// OnPreRender()에서 기존 백업 하고 내꺼 설정 해주면 되는데... - 090617, OZ
	// 대신 조건이 OnPreRender()시와 OnPostRender()시의 컨트롤러 수행 순서가 바뀌어야 한다.


	// Setup filter taps
	SetupFilterKernel();

	// Compute viewport scale
	SetupViewportScale();


	return true;
}

void RCameraDOFController::Destroy()
{
	REngine::GetDevicePtr()->DeleteTexture( m_pColorBufferOwn);
	REngine::GetDevicePtr()->DeleteTexture( m_pInfoBufferExpandBlurred);
	REngine::GetDevicePtr()->DeleteTexture( m_pDepthBlurBuffer);
	REngine::GetDevicePtr()->DeleteTexture( m_pDepthBlurBufferBlured);
	REngine::GetDevicePtr()->DeleteTexture( m_pDepthBlurBufferBluredExpanded);
	REngine::GetDevicePtr()->DeleteTexture( m_pGaussianBuffer);

	if (m_pDOFShader)
		REngine::GetDevice().DeleteShaderFX(m_pDOFShader);
}


void RCameraDOFController::Restore()
{
	REngine::GetDevice().ColorFill( m_pColorBufferOwn );
	REngine::GetDevice().ColorFill( m_pInfoBufferExpandBlurred );
	REngine::GetDevice().ColorFill( m_pDepthBlurBuffer );
	REngine::GetDevice().ColorFill( m_pDepthBlurBufferBlured );
	REngine::GetDevice().ColorFill( m_pDepthBlurBufferBluredExpanded );
	REngine::GetDevice().ColorFill( m_pGaussianBuffer);
}


//-----------------------------------------------------------------------------
// Desc: Draw a properly aligned quad covering the entire render target
//-----------------------------------------------------------------------------
void RCameraDOFController::DrawFullScreenQuad(float fLeftU, float fTopV, float fRightU, float fBottomV, int bSmallSize)
{
	REngine::GetDevice().SetFillMode(RFILL_SOLID);

	// Ensure that we're directly mapping texels to pixels by offset by 0.5
	// For more info see the doc page titled "Directly Mapping Texels to Pixels"
	float fWidth5, fHeight5;

	if( bSmallSize == 1)
	{
		fWidth5		= 256;
		fHeight5	= 256;
// 		fWidth5		= (float)m_dwWidthGaussian;
// 		fHeight5	= (float)m_dwHeightGaussian;
	}
	else if( bSmallSize == -1)
	{
		fWidth5		= (float)m_dwWidthGaussian;
		fHeight5	= (float)m_dwHeightGaussian;
	}
	else
	{
		fWidth5		= (float)m_dwWidth;
		fHeight5	= (float)m_dwHeight;
	}

	// Draw the quad
	RVt_pos_tex_rhw vQuad[4];
	vQuad[0].vPos	= RVector4( -0.5f, -0.5f, 0.0f, 1.0f );
	vQuad[0].u		= fLeftU;
	vQuad[0].v		= fTopV;

	vQuad[1].vPos	= RVector4( -0.5f, fHeight5-0.5f, 0.0f, 1.0f );
	vQuad[1].u		= fLeftU;
	vQuad[1].v		= fBottomV;

	vQuad[2].vPos	= RVector4( fWidth5-0.5f, -0.5f, 0.0f, 1.0f );
	vQuad[2].u		= fRightU;
	vQuad[2].v		= fTopV;

	vQuad[3].vPos	= RVector4( fWidth5-0.5f, fHeight5-0.5f, 0.0f, 1.0f );
	vQuad[3].u		= fRightU;
	vQuad[3].v		= fBottomV;

	REngine::GetDevice().SetDepthEnable(false, false);
	REngine::GetDevice().SetFvF(RVt_pos_tex_rhw::FVF);
	REngine::GetDevice().DrawPrimitiveUP( RPT_TRIANGLESTRIP, 2, vQuad, sizeof( RVt_pos_tex_rhw ) );
	REngine::GetDevice().SetDepthEnable(true, true);
}


//-----------------------------------------------------------------------------
// Name: RenderDOF()
// Desc: Renders DoF effect use only CoC
//-----------------------------------------------------------------------------
void RCameraDOFController::RenderDOF_CoC()
{
	if(!GetInferredRenderingStrategy())
		return;


	REngine::GetDevice().SetAlphaTestEnable(false);



	RTechnique Technique = m_pDOFShader->GetTechnique("FilterDoF");
	REngine::GetDevice().SetTexture(0, m_pColorBufferOwn);
	REngine::GetDevice().SetTexture(1, GetInferredRenderingStrategy()->GetDepthBuffer());

	// Set max CoC size
	m_pDOFShader->GetConstant("maxCoC").SetFloat( m_fCoCSize / 2.0f);

	m_pDOFShader->GetConstant("fFocusRange").SetFloat( m_fFocusRange);

	REngine::GetDevice().SetTextureFilter( 0, RTF_LINEAR );
	REngine::GetDevice().SetTextureFilter( 1, RTF_LINEAR );

	REngine::GetDevice().SetBlending(RBLEND_NONE, RBLEND_NONE);
	REngine::GetDevice().SetAlphaTestEnable(false);

	UINT uiPass, uiPassCount;
	Technique.Begin(&uiPassCount, 0);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);

		DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f );

		Technique.EndPass();
	}
	Technique.End();

	REngine::GetDevice().SetTexture(0, NULL);
	REngine::GetDevice().SetTexture(1, NULL);

}



//-----------------------------------------------------------------------------
// Name: RenderDOF()
// Desc: Renders DoF effect use Gaussian and CoC
//-----------------------------------------------------------------------------
void RCameraDOFController::RenderDOF_CoC_Gaussian()
{
	if(!GetInferredRenderingStrategy())
		return;

	REngine::GetDevice().SetDepthEnable( false, false);
	REngine::GetDevice().SetTextureAddressClamp(0);
	REngine::GetDevice().SetTextureAddressClamp(1);
	REngine::GetDevice().SetTextureAddressClamp(2);
	REngine::GetDevice().SetTextureAddressClamp(3);
	REngine::GetDevice().SetTextureAddressClamp(4);
	REngine::GetDevice().SetBlending(RBLEND_NONE, RBLEND_NONE);
	REngine::GetDevice().SetAlphaTestEnable(false);
	REngine::GetDevice().SetTexture(0, NULL);
	REngine::GetDevice().SetTexture(1, NULL);
	REngine::GetDevice().SetTexture(2, NULL);
	REngine::GetDevice().SetTexture(3, NULL);
	REngine::GetDevice().SetTexture(4, NULL);


	// Set constant
	m_pDOFShader->GetConstant("maxCoC").SetFloat( m_fCoCSize / 2.0f);
	m_pDOFShader->GetConstant("g_fBlurFactor").SetFloat( m_bEdgeBlur ? 1.0f : 2.0f);
	RVector4 vDofParams( m_fNearBlurDist, m_fFocusDist, m_fFarBlurDist, m_fMaxBlurOfBehind);
	m_pDOFShader->GetConstant("g_vDofParams").SetVector4( vDofParams);


	// 가우시안 블러링 ----------------------------------------------------------
	// Save RT
	RTexture* pBeforeRenderTarget0 = REngine::GetDevice().GetRenderTarget(0);
	RTexture* pBeforeRenderTarget1 = REngine::GetDevice().GetRenderTarget(1);

	REngine::GetDevice().SetTextureFilter( 0, RTF_LINEAR);
	REngine::GetDevice().SetTextureFilter( 1, RTF_LINEAR);

	{
		UINT uiPass, uiPassCount;
		// 가우시안 테크닉 수행
		RTechnique TechniqueGaussian = m_pDOFShader->GetTechnique("GaussBlur5x5");
		// RGB 버퍼를 블러링
		// Set RT 4 가우시안
		REngine::GetDevice().SetRenderTarget( 0, m_pGaussianBuffer);
		REngine::GetDevice().SetRenderTarget( 1, R_NONE);
		// 변형 하지 않은 깨끗한 화면을 소스로
		REngine::GetDevice().SetTexture(0, m_pColorBufferOwn);
		TechniqueGaussian.Begin(&uiPassCount, 0);
		for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
		{
			TechniqueGaussian.BeginPass(uiPass);

			DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f, -1);

			TechniqueGaussian.EndPass();
		}
		TechniqueGaussian.End();
	}

	{
		UINT uiPass, uiPassCount;
		// 깊이 블러 정보 버퍼를 생성 ------------------------------
		RTechnique TechniqueMakeDepthBlur = m_pDOFShader->GetTechnique("MakeDepthBlur");
		REngine::GetDevice().SetRenderTarget( 0, m_pDepthBlurBuffer);
		// 씬 랜더에서 기록한 정보 버퍼를 입력으로
		REngine::GetDevice().SetTexture(0, GetInferredRenderingStrategy()->GetDepthBuffer());
		REngine::GetDevice().SetTextureFilter( 0, RTF_POINT);
		TechniqueMakeDepthBlur.Begin(&uiPassCount, 0);
		for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
		{
			TechniqueMakeDepthBlur.BeginPass(uiPass);
			DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f);
			TechniqueMakeDepthBlur.EndPass();
		}
		TechniqueMakeDepthBlur.End();
	}


	RTexture* pDepthBlurBuffer = m_pDepthBlurBuffer;

	// 정보 버퍼를 블러링
	if( m_bEdgeBlur)
	{
		UINT uiPass, uiPassCount;
		REngine::GetDevice().SetTextureFilter( 0, RTF_LINEAR);
		// 정보 버퍼의 블러 값을 확장 시킨다.
		RTechnique TechniqueExpand = m_pDOFShader->GetTechnique("Expand5x5");		
		// Set RT 4 가우시안 정보 버퍼를
		REngine::GetDevice().SetRenderTarget( 0, m_pDepthBlurBufferBlured);
		REngine::GetDevice().SetRenderTarget( 1, R_NONE);
		// 정보 버퍼를 소스로
		REngine::GetDevice().SetTexture(0, m_pDepthBlurBuffer);
		TechniqueExpand.Begin(&uiPassCount, 0);
		for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
		{
			TechniqueExpand.BeginPass(uiPass);
			DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f, -1);
			TechniqueExpand.EndPass();
		}
		TechniqueExpand.End();

		// 확장시킨 버퍼를 블러링
		REngine::GetDevice().SetTextureFilter( 0, RTF_POINT);
		RTechnique TechniqueGaussian = m_pDOFShader->GetTechnique("GaussBlur5x5");
		// Set RT 4 가우시안 정보 버퍼를
		REngine::GetDevice().SetRenderTarget( 0, m_pDepthBlurBufferBluredExpanded);
		REngine::GetDevice().SetRenderTarget( 1, NULL);
		// 정보 버퍼를 소스로
		REngine::GetDevice().SetTexture(0, m_pDepthBlurBufferBlured);
		TechniqueGaussian.Begin(&uiPassCount, 0);
		for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
		{
			TechniqueGaussian.BeginPass(uiPass);
			DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f, -1);
			TechniqueGaussian.EndPass();
		}
		TechniqueGaussian.End();

		pDepthBlurBuffer = m_pDepthBlurBufferBluredExpanded;
	}


	// RT 돌려놓기
	REngine::GetDevice().SetRenderTarget( 0, pBeforeRenderTarget0);
	REngine::GetDevice().SetRenderTarget( 1, pBeforeRenderTarget1);


	// DOF (CoC) ----------------------------------------------------------------
	UINT uiPass, uiPassCount;
	RTechnique TechniqueDoF;
	TechniqueDoF = m_pDOFShader->GetTechnique("FilterDoF");
	REngine::GetDevice().SetTexture(0, m_pColorBufferOwn);
	REngine::GetDevice().SetTexture(1, pDepthBlurBuffer);
	REngine::GetDevice().SetTexture(2, m_pGaussianBuffer);		// 가우시안 처리된 것을 S2로
	REngine::GetDevice().SetTexture(3, m_pDepthBlurBuffer);		// 깊이 값을 사용 하기 위해 정보 버퍼의 원본도 넘겨줌

	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT);
	// 블러 된 정보 버퍼를 사용 시에는 LINEAR필터로 사용 하여야 부드럽다.
	REngine::GetDevice().SetTextureFilter( 1, m_bEdgeBlur ? RTF_LINEAR : RTF_POINT);
	REngine::GetDevice().SetTextureFilter( 2, RTF_LINEAR);
	REngine::GetDevice().SetTextureFilter( 3, RTF_POINT);

	//UINT uiPass, uiPassCount;
	TechniqueDoF.Begin(&uiPassCount, 0);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		TechniqueDoF.BeginPass(uiPass);

		DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f );

		TechniqueDoF.EndPass();
	}
	TechniqueDoF.End();

	REngine::GetDevice().SetTextureFilter( 0, RTF_LINEAR);
	REngine::GetDevice().SetTextureFilter( 1, RTF_LINEAR);
	REngine::GetDevice().SetTextureFilter( 2, RTF_LINEAR);
	REngine::GetDevice().SetTextureFilter( 3, RTF_LINEAR);
	REngine::GetDevice().SetTextureFilter( 4, RTF_LINEAR);

	REngine::GetDevice().SetTexture(0, NULL);
	REngine::GetDevice().SetTexture(1, NULL);
	REngine::GetDevice().SetTexture(2, NULL);
	REngine::GetDevice().SetTexture(3, NULL);
	REngine::GetDevice().SetTexture(4, NULL);

	REngine::GetDevice().SetTextureAddressWrap(0);
	REngine::GetDevice().SetTextureAddressWrap(1);
	REngine::GetDevice().SetTextureAddressWrap(2);
	REngine::GetDevice().SetTextureAddressWrap(3);
	REngine::GetDevice().SetTextureAddressWrap(4);

	REngine::GetDevice().SetDepthEnable( true, true);

}


//-----------------------------------------------------------------------------
// Name: RenderOverlay()
// Desc: Visializes one of the surfaces in the overlay
//-----------------------------------------------------------------------------
void RCameraDOFController::RenderOverlay()
{
	if(!GetInferredRenderingStrategy())
		return;

	//RTechnique Technique = m_pDOFShader->GetTechnique( "DepthBlurOverlay");
	RTechnique Technique = m_pDOFShader->GetTechnique( "BlurOverlay");
	//RTechnique Technique = m_pDOFShader->GetTechnique( "DepthOverlay");

	//m_pEffect->SetFloat("depthScale", 0.03f);
	m_pDOFShader->GetConstant("depthScale").SetFloat( 0.0005f);

	REngine::GetDevice().SetTexture(0, NULL);
	REngine::GetDevice().SetTexture(1, m_bEdgeBlur ? m_pDepthBlurBufferBluredExpanded : m_pDepthBlurBuffer);

	REngine::GetDevice().SetBlending(RBLEND_NONE, RBLEND_NONE);
	REngine::GetDevice().SetAlphaTestEnable(false);

	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT );

	REngine::GetDevice().SetDepthEnable( false, false);	// 090625, OZ

	//m_pEffect->Begin(&cPasses, 0);
	//for (iPass = 0; iPass < cPasses; iPass++)
	//{
	//	m_pEffect->BeginPass(iPass);
	//	m_pEffect->SetTexture("tBlurDepth", m_pTex[1]);
	//	m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &m_Overlay, sizeof(VERTEX2D));
	//	//m_pEffect->SetTexture("tBlurDepth", m_pTexLockSrc);
	//	//m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &m_OverlayTest, sizeof(VERTEX2D));
	//	m_pEffect->EndPass();
	//}
	//m_pEffect->End();
	UINT uiPass, uiPassCount;
	Technique.Begin(&uiPassCount, 0);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);

		DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f, 1);

		Technique.EndPass();
	}
	Technique.End();

	REngine::GetDevice().SetTexture(0, NULL);
	REngine::GetDevice().SetTexture(1, NULL);

}


void RCameraDOFController::OnRender()
{
	if(!GetInferredRenderingStrategy())
		return;

	REngine::GetDevice().SetFogEnable(false);

	RMatrix matInvVP;
	m_pCameraSceneNode->GetProjectionMatrix().GetInverse( &matInvVP);
	RVector4 vZInvVP( matInvVP._13, matInvVP._23, matInvVP._33 ,matInvVP._43);
	RVector4 vWInvVP( matInvVP._14, matInvVP._24, matInvVP._34 ,matInvVP._44);
	m_pDOFShader->GetConstant("g_vProjInvZ").SetVector4( vZInvVP);
	m_pDOFShader->GetConstant("g_vProjInvW").SetVector4( vWInvVP);

	switch( m_eFiterType)
	{
	case DFT_COC:
		RenderDOF_CoC();
		break;
	case DFT_COC_N_GAUSSIAN:
		RenderDOF_CoC_Gaussian();
		break;
	}

#ifdef _DEBUG
	//RenderOverlay();
#endif
}


void RCameraDOFController::OnPostRender()
{
	if(!GetInferredRenderingStrategy())
		return;

	// 기존 백업용으로 돌려놓는다. - 090617, OZ
	REngine::GetDevice().SetRenderTarget( 0, m_pColorBufferOwnOrg );

	OnRender();
}


// 랜더 타겟을 관리 - 090617, OZ
void RCameraDOFController::OnPreRender( RInferredRenderingStrategy* pInferredRenderingStrategy)
{
	RCameraSceneNodeController::OnPreRender( pInferredRenderingStrategy);

	if(!GetInferredRenderingStrategy())
		return;

	// OnPreRender()에서 기존 백업 하고 내꺼 설정 해주면 되는데...
	// 대신 조건이 OnPreRender()시와 OnPostRender()시의 컨트롤러 수행 순서가 바뀌어야 한다.

	// 스카이박스등에서는 RT1에 그리지 않을것이다. 그를 대비하여 뭔가 색을 채워 넣어야 한다. - 090625, OZ
	//REngine::GetDevice().ColorFill( m_pInfoBuffer,  D3DCOLOR_ARGB( 255,255,255,255));

	m_pColorBufferOwnOrg = REngine::GetDevice().GetRenderTarget(0);

	REngine::GetDevice().SetRenderTarget( 0, m_pColorBufferOwn );


	// DOF에 관련 된 이펙트 값들도 설정
	RVector4 vDofParams( m_fNearBlurDist, m_fFocusDist, m_fFarBlurDist, m_fMaxBlurOfBehind);
	RShaderCompositeManager::m_scDofParams.SetVector4(vDofParams);
	RShaderCompositeManager::m_cFarZ.SetFloat( m_pCameraSceneNode->GetFarZ());

	// 스피드 트리 등에서 참조 할 수 있게 디바이스에 적용
	REngine::GetDevice().SetDofParams( vDofParams);

}



//-----------------------------------------------------------------------------
// Name: SetupViewportScale()
// Desc: Sets proper viewport scale
//-----------------------------------------------------------------------------
void RCameraDOFController::SetupViewportScale()
{
	RVector4 v;

	RVector4 vVPScale = RVector4(2.0f / (FLOAT)m_dwWidth, -2.0f / (FLOAT)m_dwHeight, 1.0f, 1.0f);
	//m_pEffect->SetVector("viewportScale", &vVPScale);
	m_pDOFShader->GetConstant("viewportScale").SetVector4( vVPScale);

	RVector4 vVPBias = RVector4(-1.0f, 1.0f, 0.0f, 0.0f);
	//m_pEffect->SetVector("viewportBias", &vVPBias);
	m_pDOFShader->GetConstant("viewportBias").SetVector4( vVPBias);
}

//-----------------------------------------------------------------------------
// Name: SetupFilterKernel()
// Desc: Scale and set filter kernel tap offsets
//-----------------------------------------------------------------------------
void RCameraDOFController::SetupFilterKernel()
{
	RVector4 v[13];
	int nNumTaps = 0;
	FLOAT dx = 1.0f / (FLOAT)m_dwWidth;
	FLOAT dy = 1.0f / (FLOAT)m_dwHeight;

	// 그림 파일이 있으면 그림 파일에서 CoC 필터를 생성한다.
	if( RTexture* texCoC = REngine::GetDevice().CreateTexture( "CoC.bmp", RTF_POINT))
	{
		// 그림파일로 부터 어레이 생성
		int nW = texCoC->GetInfo().nTextureWidth;
		int nH = texCoC->GetInfo().nTextureHeight;
		D3DLOCKED_RECT lockedRect;
		if( SUCCEEDED( REngine::GetDevice().LockRect(texCoC, 0, &lockedRect, 0, D3DLOCK_READONLY)))
		{
			BYTE* pHeightMap = (BYTE*)lockedRect.pBits;
			for( int y = 0 ; y < nH; y++ )
			{
				for( int x = 0 ; x < nW; x++ )
				{
					int iClr = pHeightMap[(y*lockedRect.Pitch)+(x*4)];
					if( iClr < 10)
					{
						float fX = (float)x / (float)nW;	// 0 ~ 1
						fX = fX * 2.0f - 1.0f;				// -1 ~ 1
						float fY = (float)y / (float)nH;	// 0 ~ 1
						fY = fY * 2.0f - 1.0f;				// -1 ~ 1

						if(nNumTaps < 12)
						{
							v[nNumTaps] = RVector4( fX * dx, fY * dy, 0.0f, 0.0f);
							nNumTaps++;
						}
						else
						{
							mlog( " CoC Filter Tab Count Over 12.\n");
						}
					}
				}
			}
			REngine::GetDevice().UnlockRect( texCoC, 0);
		}
		// 가운데 탭.
		v[12] = RVector4( 0, 0, 0.0f, 0.0f);
		// 셰이더에 적용
		m_pDOFShader->GetConstant("filterTaps").SetVector4Array( (RVector4*)(&v[0]), nNumTaps+1);
		//m_pDOFShader->GetConstant("g_nNumTaps").SetInt( nNumTaps);	// 레지스터 부족

		// 그림 지워!
		REngine::GetDevice().DeleteTexture( texCoC);
	}
	
	// 12개가 설정 안 되어 있으면 안된다.
	if( nNumTaps < 12)
	{
		nNumTaps = 12;
		v[0]  = RVector4(-0.326212f * dx, -0.405805f * dy, 0.0f, 0.0f);
		v[1]  = RVector4(-0.840144f * dx, -0.07358f * dy, 0.0f, 0.0f);
		v[2]  = RVector4(-0.695914f * dx, 0.457137f * dy, 0.0f, 0.0f);
		v[3]  = RVector4(-0.203345f * dx, 0.620716f * dy, 0.0f, 0.0f);
		v[4]  = RVector4(0.96234f * dx, -0.194983f * dy, 0.0f, 0.0f);
		v[5]  = RVector4(0.473434f * dx, -0.480026f * dy, 0.0f, 0.0f);
		v[6]  = RVector4(0.519456f * dx, 0.767022f * dy, 0.0f, 0.0f);
		v[7]  = RVector4(0.185461f * dx, -0.893124f * dy, 0.0f, 0.0f);
		v[8]  = RVector4(0.507431f * dx, 0.064425f * dy, 0.0f, 0.0f);
		v[9]  = RVector4(0.89642f * dx, 0.412458f * dy, 0.0f, 0.0f);
		v[10] = RVector4(-0.32194f * dx, -0.932615f * dy, 0.0f, 0.0f);
		v[11] = RVector4(-0.791559f * dx, -0.597705f * dy, 0.0f, 0.0f);
		// 가운데 탭.
		v[12] = RVector4( 0, 0, 0.0f, 0.0f);

		m_pDOFShader->GetConstant("filterTaps").SetVector4Array( v, nNumTaps+1);
		//m_pDOFShader->GetConstant("g_nNumTaps").SetInt( nNumTaps);	// 레지스터 부족
	}

	// 가우시안 적용
	if( m_eFiterType == DFT_COC_N_GAUSSIAN)
	{
		RVector2 vSampleOffsets[13];
		RVector4 vSampleWeights[13];
		GetSampleOffsets_GaussBlur5x5( m_dwWidthGaussian, m_dwHeightGaussian, vSampleOffsets, vSampleWeights );
		m_pDOFShader->m_pEffect->SetValue("g_avSampleOffsets", vSampleOffsets, sizeof( vSampleOffsets ) );
		m_pDOFShader->m_pEffect->SetValue("g_avSampleWeights", vSampleWeights, sizeof( vSampleWeights ) );
	}

}


//-----------------------------------------------------------------------------
// Desc: Get the texture coordinate offsets to be used inside the GaussBlur5x5
//       pixel shader.
//-----------------------------------------------------------------------------
float RCameraDOFController::GaussianDistribution( float x, float y, float rho )
{
	float g = 1.0f / sqrtf( 2.0f * MMath::PI * rho * rho );
	g *= expf( -( x * x + y * y ) / ( 2 * rho * rho ) );

	return g;
}

bool RCameraDOFController::GetSampleOffsets_GaussBlur5x5( int nTexWidth, int nTexHeight, RVector2* vTexCoordOffset, RVector4* vSampleWeight, FLOAT fMultiplier )
{
	float tu = 1.0f / ( float )nTexWidth;
	float tv = 1.0f / ( float )nTexHeight;

	RVector4 vWhite( 1.0f, 1.0f, 1.0f, 1.0f );

	float totalWeight = 0.0f;
	int index = 0;
	for( int x = -2; x <= 2; x++ )
	{
		for( int y = -2; y <= 2; y++ )
		{
			// Exclude pixels with a block distance greater than 2. This will
			// create a kernel which approximates a 5x5 kernel using only 13
			// sample points instead of 25; this is necessary since 2.0 shaders
			// only support 16 texture grabs.
			if( abs( x ) + abs( y ) > 2 )
				continue;

			// Get the unscaled Gaussian intensity for this offset
			vTexCoordOffset[index] = RVector2( x * tu, y * tv );
			vSampleWeight[index] = vWhite * GaussianDistribution( ( float )x, ( float )y, 1.0f );
			totalWeight += vSampleWeight[index].x;

			index++;
		}
	}

	// Divide the current weight by the total weight of all the samples; Gaussian
	// blur kernels add to 1.0f to ensure that the intensity of the image isn't
	// changed when the blur occurs. An optional multiplier variable is used to
	// add or remove image intensity during the blur.
	for( int i = 0; i < index; i++ )
	{
		vSampleWeight[i] /= totalWeight;
		vSampleWeight[i] *= fMultiplier;
	}

	return true;
}



void RCameraDOFController::OnRemoveFromCamera()
{
	//카메라 씬 노드의 정보를 건드리지 않게 되었다 - 090617, OZ
	//m_pCameraSceneNode->SetRenderTarget( 0, RT_FRAMEBUFFER );
	//m_pCameraSceneNode->SetRenderTarget( 1, NULL);
}

void RCameraDOFController::OnClear()
{
	REngine::GetDevice().Clear( true, true, 
		m_pCameraSceneNode->isHaveStencil(), 
		//D3DCOLOR_ARGB( 255, 255,255,255),		// 스카이박스등에서는 RT1에 안칠하므로 특정 정보를 가진 색으로 클리어 - 090626
		m_pCameraSceneNode->GetClearColor(),
		m_pCameraSceneNode->GetClearDepth(),
		m_pCameraSceneNode->GetClearStencil() );
}



}