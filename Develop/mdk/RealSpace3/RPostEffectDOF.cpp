#include "StdAfx.h"
#include "RPostEffectDOF.h"
#include "RCameraSceneNode.h"
#include "RVType.h"
#include "RShaderCompositeManager.h"
#include "RShaderFX.h"

#define SATURATED_LINEAR_DEPTH	1
//#define BUFFER_FORMAT_16BIT		1

namespace rs3 {


RPostEffectDOF::RPostEffectDOF()
: RPostEffectInterface( PE_DOF, RRP_NONE )
, m_pDepthBlurBufferBlured(NULL)
, m_pDepthBlurBufferBluredExpanded(NULL)
, m_pDepthBlurBuffer(NULL)
, m_ptexNoise(NULL)
, m_pDOFShader(NULL)
, m_fNearBlurDist(0)
, m_fFarBlurDist(2000)
, m_fMaxBlurOfBehind(1)
, m_fFocusRange(0)
, m_fCoCSize(5)
, m_nNumTaps(6)		// max 13
, m_fFocusDist(0)
, m_bEdgeBlur(false)
, m_bUseNoise(false)
{
	m_bActive	= true;

	m_eFiterType = DFT_COC_N_GAUSSIAN;
	
	m_dwWidth = 0;
	m_dwHeight = 0;

	m_dwWidthGaussian = 0;
	m_dwHeightGaussian = 0;

	m_pGaussianBuffer[0] = NULL;
	m_pGaussianBuffer[1] = NULL;

}

RPostEffectDOF::~RPostEffectDOF(void)
{
}






bool RPostEffectDOF::Init(  int nW, int nH)
{
	if(!RPostEffectInterface::Init(nW, nH))
		return false;

	m_dwWidthGaussian = m_dwWidth/4;
	m_dwHeightGaussian = m_dwHeight/4;

	if ( false == REngine::GetDevice().QueryFeature(RQF_PS20) )
		return false;

	if ( false == REngine::GetDevice().QueryFeature(RQF_PS30) )
		return false;

	Restore();
	if( R_NONE == m_pDepthBlurBuffer) return false;
	if( R_NONE == m_pDepthBlurBufferBlured) return false;
	if( R_NONE == m_pDepthBlurBufferBluredExpanded) return false;
	if(R_NONE==m_pGaussianBuffer) return false;

	if(!m_ptexNoise)
		m_ptexNoise = REngine::GetDevice().CreateTexture( "vector_noise.dds", RTF_POINT, 0);

	switch( m_eFiterType)
	{
	case DFT_COC:
		m_pDOFShader = REngine::GetDevice().CreateShaderFX("DOF_CoC.fx");
		break;
	case DFT_COC_N_GAUSSIAN:
		m_pDOFShader = REngine::GetDevice().CreateShaderFX("DOF_CoCnGaussian.fx");
		break;
	}

	// Setup filter taps
	SetupFilterKernel();

	// Compute viewport scale
	SetupViewportScale();


	return true;
}

void RPostEffectDOF::Destroy()
{
	RPostEffectInterface::Destroy();

	Invalidate();
	m_ptexNoise = NULL;
	if ( m_pDOFShader)
		REngine::GetDevice().DeleteShaderFX(m_pDOFShader);
	m_pDOFShader = NULL;
}

void RPostEffectDOF::CreateBuffers()
{
	RFORMAT eGaussianBufferFormat = RFMT_RGB16;
	RFORMAT eInfoFormat = RFMT_RGB16;
#ifdef BUFFER_FORMAT_16BIT
	if ( false == REngine::GetDevice().QueryFeature(RQF_RGB16) )
	{
		eGaussianBufferFormat = RFMT_A8R8G8B8;
		eInfoFormat = RFMT_G16R16F;
	}
#else
	eGaussianBufferFormat = RFMT_A8R8G8B8;
	eInfoFormat = RFMT_G16R16F;
#endif

	if(!m_pDepthBlurBuffer)
		m_pDepthBlurBuffer = REngine::GetDevice().CreateRenderTargetTexture( m_dwWidth, m_dwHeight, eInfoFormat);

	if(!m_pDepthBlurBufferBlured)
		m_pDepthBlurBufferBlured = REngine::GetDevice().CreateRenderTargetTexture( m_dwWidthGaussian, m_dwHeightGaussian, eInfoFormat);

	if(!m_pDepthBlurBufferBluredExpanded)
		m_pDepthBlurBufferBluredExpanded = REngine::GetDevice().CreateRenderTargetTexture( m_dwWidthGaussian, m_dwHeightGaussian, eInfoFormat);

	if(!m_pGaussianBuffer[0])
		m_pGaussianBuffer[0] = REngine::GetDevice().CreateRenderTargetTexture( m_dwWidthGaussian, m_dwHeightGaussian, eGaussianBufferFormat);

	if(!m_pGaussianBuffer[1])
		m_pGaussianBuffer[1] = REngine::GetDevice().CreateRenderTargetTexture( m_dwWidthGaussian, m_dwHeightGaussian, RFMT_A16B16G16R16F);

	REngine::GetDevice().ColorFill( m_pDepthBlurBuffer );
	REngine::GetDevice().ColorFill( m_pDepthBlurBufferBlured );
	REngine::GetDevice().ColorFill( m_pDepthBlurBufferBluredExpanded );
	REngine::GetDevice().ColorFill( m_pGaussianBuffer[0]);
	REngine::GetDevice().ColorFill( m_pGaussianBuffer[1]);
}

void RPostEffectDOF::DestroyBuffers()
{
	REngine::GetDevicePtr()->SafeDeleteTexture( m_pDepthBlurBuffer );
	REngine::GetDevicePtr()->SafeDeleteTexture( m_pDepthBlurBufferBlured );
	REngine::GetDevicePtr()->SafeDeleteTexture( m_pDepthBlurBufferBluredExpanded );
	REngine::GetDevicePtr()->SafeDeleteTexture( m_pGaussianBuffer[0] );
	REngine::GetDevicePtr()->SafeDeleteTexture( m_pGaussianBuffer[1] );
	REngine::GetDevicePtr()->SafeDeleteTexture( m_ptexNoise );
}

//-----------------------------------------------------------------------------
// Name: RenderDOF()
// Desc: Renders DoF effect use only CoC
//-----------------------------------------------------------------------------
void RPostEffectDOF::RenderDOF_CoC( RCameraSceneNode* pCamera, RTexture* pColorBuffer, RTexture* pNormalBuffer, RTexture* pDepthBuffer)
{
	RPFC_THISFUNC;
	RTechnique Technique = m_pDOFShader->GetTechnique("FilterDoF");
	REngine::GetDevice().SetTexture(0, pColorBuffer);
	REngine::GetDevice().SetTexture(1, pDepthBuffer);

	// Set max CoC size
	m_pDOFShader->GetConstant("g_fmaxCoC").SetFloat( m_fCoCSize / 2.0f);
	m_pDOFShader->GetConstant("g_nNumTaps").SetInt( m_nNumTaps);

	REngine::GetDevice().SetTextureFilter( 0, RTF_LINEAR );
	REngine::GetDevice().SetTextureFilter( 1, RTF_LINEAR );


	UINT uiPass, uiPassCount;
	Technique.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);

		DrawFullScreenQuad( 0.0f, 0.0f, m_dwWidth, m_dwHeight);

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
void RPostEffectDOF::RenderDOF_CoC_Gaussian( RCameraSceneNode* pCamera, RTexture* pColorBuffer, RTexture* pNormalBuffer, RTexture* pDepthBuffer)
{
	if( !m_pGaussianBuffer || !pCamera)
		return;

	RPFC_THISFUNC;

	REngine::GetDevice().SetDepthEnable( false, false);
	REngine::GetDevice().SetTextureAddressClamp(0);
	REngine::GetDevice().SetTextureAddressClamp(1);
	REngine::GetDevice().SetTextureAddressClamp(2);
	REngine::GetDevice().SetTextureAddressClamp(3);
	REngine::GetDevice().SetTextureAddressWrap(4);
	REngine::GetDevice().SetTexture(0, NULL);
	REngine::GetDevice().SetTexture(1, NULL);
	REngine::GetDevice().SetTexture(2, NULL);
	REngine::GetDevice().SetTexture(3, NULL);
	REngine::GetDevice().SetTexture(4, NULL);


	// Set constant
	m_pDOFShader->GetConstant("g_fmaxCoC").SetFloat( m_fCoCSize / 2.0f);
	m_pDOFShader->GetConstant("g_fBlurFactor").SetFloat( m_bEdgeBlur ? 1.0f : 2.0f);
#ifdef SATURATED_LINEAR_DEPTH
	float fFarZ = pCamera->GetFarZ();
	RVector4 vDofParams( m_fNearBlurDist/fFarZ, m_fFocusDist/fFarZ, m_fFarBlurDist/fFarZ, m_fMaxBlurOfBehind);
#else
	RVector4 vDofParams( m_fNearBlurDist, m_fFocusDist, m_fFarBlurDist, m_fMaxBlurOfBehind);
#endif
	m_pDOFShader->GetConstant("g_vDofParams").SetVector4( vDofParams);
	m_pDOFShader->GetConstant("g_nNumTaps").SetInt( m_nNumTaps);


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

		REngine::GetDevice().SetRenderTarget( 0, m_pGaussianBuffer[ REngine::GetSceneManager().GetRenderingStrategy()->GetGammaCorrect() ]);
		REngine::GetDevice().SetRenderTarget( 1, R_NONE);
		// 변형 하지 않은 깨끗한 화면을 소스로
		REngine::GetDevice().SetTexture(0, pColorBuffer);
		TechniqueGaussian.Begin(&uiPassCount, TECH_PRESERVE_NOT);
		for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
		{
			TechniqueGaussian.BeginPass(uiPass);

			DrawFullScreenQuad( 0.0f, 0.0f, m_dwWidthGaussian, m_dwHeightGaussian);

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
		REngine::GetDevice().SetTexture(0, pDepthBuffer);
		REngine::GetDevice().SetTextureFilter( 0, RTF_POINT);
		TechniqueMakeDepthBlur.Begin(&uiPassCount, TECH_PRESERVE_NOT);
		for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
		{
			TechniqueMakeDepthBlur.BeginPass(uiPass);
			DrawFullScreenQuad( 0.0f, 0.0f, m_dwWidth, m_dwHeight);
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
		TechniqueExpand.Begin(&uiPassCount, TECH_PRESERVE_NOT);
		for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
		{
			TechniqueExpand.BeginPass(uiPass);
			DrawFullScreenQuad( 0.0f, 0.0f, m_dwWidthGaussian, m_dwHeightGaussian);
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
		TechniqueGaussian.Begin(&uiPassCount, TECH_PRESERVE_NOT);
		for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
		{
			TechniqueGaussian.BeginPass(uiPass);
			DrawFullScreenQuad( 0.0f, 0.0f, m_dwWidthGaussian, m_dwHeightGaussian);
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
	if( m_bUseNoise)
		TechniqueDoF = m_pDOFShader->GetTechnique("FilterDoF_noise");
	else
		TechniqueDoF = m_pDOFShader->GetTechnique("FilterDoF");
	REngine::GetDevice().SetTexture(0, pColorBuffer);
	REngine::GetDevice().SetTexture(1, pDepthBlurBuffer);
	REngine::GetDevice().SetTexture(2, m_pGaussianBuffer[ REngine::GetSceneManager().GetRenderingStrategy()->GetGammaCorrect() ]);		// 가우시안 처리된 것을 S2로
	REngine::GetDevice().SetTexture(3, m_pDepthBlurBuffer);		// 깊이 값을 사용 하기 위해 정보 버퍼의 원본도 넘겨줌
	REngine::GetDevice().SetTexture(4, m_ptexNoise);			// noise vector

	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT);
	// 블러 된 정보 버퍼를 사용 시에는 LINEAR필터로 사용 하여야 부드럽다.
	REngine::GetDevice().SetTextureFilter( 1, m_bEdgeBlur ? RTF_LINEAR : RTF_POINT);
	REngine::GetDevice().SetTextureFilter( 2, RTF_LINEAR);
	REngine::GetDevice().SetTextureFilter( 3, RTF_POINT);
	REngine::GetDevice().SetTextureFilter( 4, RTF_POINT);

	//UINT uiPass, uiPassCount;
	TechniqueDoF.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		TechniqueDoF.BeginPass(uiPass);

		DrawFullScreenQuad( 0.0f, 0.0f, m_dwWidth, m_dwHeight);

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
void RPostEffectDOF::RenderOverlay()
{

	//RTechnique Technique = m_pDOFShader->GetTechnique( "DepthBlurOverlay");
	RTechnique Technique = m_pDOFShader->GetTechnique( "BlurOverlay");
	//RTechnique Technique = m_pDOFShader->GetTechnique( "DepthOverlay");

	//m_pEffect->SetFloat("depthScale", 0.03f);
	m_pDOFShader->GetConstant("depthScale").SetFloat( 0.0005f);

	REngine::GetDevice().SetTexture(0, NULL);
	REngine::GetDevice().SetTexture(1, m_bEdgeBlur ? m_pDepthBlurBufferBluredExpanded : m_pDepthBlurBuffer);

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
	Technique.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);

		DrawFullScreenQuad( 0.0f, 0.0f, m_dwWidthGaussian, m_dwHeightGaussian);

		Technique.EndPass();
	}
	Technique.End();

	REngine::GetDevice().SetTexture(0, NULL);
	REngine::GetDevice().SetTexture(1, NULL);

}


void RPostEffectDOF::Render( RCameraSceneNode* pCamera, RTexture* pColorBuffer, RTexture* pNormalBuffer, RTexture* pDepthBuffer)
{
	RPFC_THISFUNC;
	RMatrix matInvVP;
	pCamera->GetProjectionMatrix().GetInverse( &matInvVP);
	RVector4 vZInvVP( matInvVP._13, matInvVP._23, matInvVP._33 ,matInvVP._43);
	RVector4 vWInvVP( matInvVP._14, matInvVP._24, matInvVP._34 ,matInvVP._44);
	m_pDOFShader->GetConstant("g_vProjInvZ").SetVector4( vZInvVP);
	m_pDOFShader->GetConstant("g_vProjInvW").SetVector4( vWInvVP);

	RMatrix matView = pCamera->GetViewMatrix();
	RVector3 vP = pCamera->GetViewFrustum().GetPoints()[0];
	matView.TransformVect(vP);
	m_pDOFShader->GetConstant("g_vFarClipCornerInView").SetVector3(RVector3(abs(vP.x),-abs(vP.y),vP.z));


	switch( m_eFiterType)
	{
	case DFT_COC:
		RenderDOF_CoC( pCamera, pColorBuffer, pNormalBuffer, pDepthBuffer);
		break;
	case DFT_COC_N_GAUSSIAN:
		RenderDOF_CoC_Gaussian( pCamera, pColorBuffer, pNormalBuffer, pDepthBuffer);
		break;
	}

#ifdef _DEBUG
	//RenderOverlay();
#endif
}



//-----------------------------------------------------------------------------
// Name: SetupViewportScale()
// Desc: Sets proper viewport scale
//-----------------------------------------------------------------------------
void RPostEffectDOF::SetupViewportScale()
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
void RPostEffectDOF::SetupFilterKernel()
{
	RVector4 v[13];
	int nNumTaps = 0;
	FLOAT dx = 1.0f / (FLOAT)m_dwWidth;
	FLOAT dy = 1.0f / (FLOAT)m_dwHeight;

	// 그림 파일이 있으면 그림 파일에서 CoC 필터를 생성한다.
	if( RTexture* texCoC = REngine::GetDevice().CreateTexture( "CoC.bmp", RTF_POINT, 0))
	{
		// 그림파일로 부터 어레이 생성
		int nW = texCoC->GetInfo().nTextureWidth;
		int nH = texCoC->GetInfo().nTextureHeight;
		RLOCKED_RECT lockedRect;
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

		// 가운데 탭.
		v[ 0] = RVector4( 0, 0, 0.0f, 0.0f);
		v[ 1]  = RVector4(-0.326212f * dx, -0.405805f * dy, 0.0f, 0.0f);
		v[ 2]  = RVector4(0.519456f * dx, 0.767022f * dy, 0.0f, 0.0f);
		v[ 3]  = RVector4(-0.695914f * dx, 0.457137f * dy, 0.0f, 0.0f);
		v[ 4]  = RVector4(0.473434f * dx, -0.480026f * dy, 0.0f, 0.0f);
		v[ 5]  = RVector4(-0.840144f * dx, -0.07358f * dy, 0.0f, 0.0f);
		v[ 6]  = RVector4(0.507431f * dx, 0.064425f * dy, 0.0f, 0.0f);
		v[ 7]  = RVector4(0.185461f * dx, -0.893124f * dy, 0.0f, 0.0f);
		v[ 8]  = RVector4(-0.203345f * dx, 0.620716f * dy, 0.0f, 0.0f);
		v[ 9]  = RVector4(0.96234f * dx, -0.194983f * dy, 0.0f, 0.0f);
		v[10]  = RVector4(0.89642f * dx, 0.412458f * dy, 0.0f, 0.0f);
		v[11] = RVector4(-0.32194f * dx, -0.932615f * dy, 0.0f, 0.0f);
		v[12] = RVector4(-0.791559f * dx, -0.597705f * dy, 0.0f, 0.0f);
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
float RPostEffectDOF::GaussianDistribution( float x, float y, float rho )
{
	float g = 1.0f / sqrtf( 2.0f * MMath::PI * rho * rho );
	g *= expf( -( x * x + y * y ) / ( 2 * rho * rho ) );

	return g;
}

bool RPostEffectDOF::GetSampleOffsets_GaussBlur5x5( int nTexWidth, int nTexHeight, RVector2* vTexCoordOffset, RVector4* vSampleWeight, FLOAT fMultiplier )
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



}