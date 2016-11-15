#include "StdAfx.h"
#include "RSSAOProcessor.h"
#include "REngine.h"
#include "RVType.h"
#include "RRenderHelper.h"
#include "RShaderFX.h"

namespace rs3 {


RSSAOProcessor::RSSAOProcessor( int n)
: RGIProcessor( n)
, m_ptexNoise( NULL)
, m_pEdge( NULL)
, m_pShaderSSAO( NULL)
, m_pShaderEdgeDetect( NULL)
, m_pDepthExpand( NULL)
, m_ptexBlurX( NULL)
, m_ptexBlurY( NULL)
, m_bPointFilterBufferOnly(true)
, m_bPointFilterDepthOnly(true)
{
	m_pTarget[0] = NULL;
	m_pTarget[1] = NULL;
	m_nPatternPixel = 4;
	m_nGaussPixel = 5;
	m_dwWidthFull = m_dwHeightFull = 0;
}

RSSAOProcessor::~RSSAOProcessor(void)
{
	Destroy();
}


void RSSAOProcessor::Render( RCameraSceneNode* pCamera, int nWidth, int nHeight, bool bRenderDirect)
{
	if(!pCamera || !m_pShaderSSAO)
		return ;

	_ASSERT( m_dwWidthFull == nWidth);
	_ASSERT( m_dwHeightFull == nHeight);

	RMatrix matInvVP;

	bool bWillTargetWrite = false;

	UINT uiPass, uiPassCount;
	RTechnique Technique;

	RTexture* pRT0 = REngine::GetDevice().GetRenderTarget( 0);
	RTexture* pRT1 = REngine::GetDevice().GetRenderTarget( 1);
	RTexture* pRT2 = REngine::GetDevice().GetRenderTarget( 2);
	RTexture* pRT3 = REngine::GetDevice().GetRenderTarget( 3);

	RTexture* pTexOrg0 = REngine::GetDevice().GetTexture( 0);
	RTexture* pTexOrg1 = REngine::GetDevice().GetTexture( 1);
	RTexture* pTexOrg2 = REngine::GetDevice().GetTexture( 2);
	RTexture* pTexOrg3 = REngine::GetDevice().GetTexture( 3);

	REngine::GetDevice().SetDepthEnable(false, false);
	REngine::GetDevice().SetCullMode(RCULL_CCW);

	REngine::GetDevice().SetTextureAddressClamp( 0);
	REngine::GetDevice().SetTextureAddressWrap( 1);
	REngine::GetDevice().SetTextureAddressClamp( 2);
	REngine::GetDevice().SetTextureAddressClamp( 3);

	bool nTargetIdnx = 0;
	//RTexture* pTexDepthToUse = pTexDepth;

	RVector4 vZInvVP, vWInvVP, vViewDimensions;

	// -----------------depth expand ----------------------------------------------------
	// 고 해상도로 한다.

	// target : 확장 버퍼용
	REngine::GetDevice().SetRenderTarget( 0, m_pDepthExpand);
	// source : 입력 받은 깊이
	//REngine::GetDevice().SetTexture( 0, pTexDepth);
	if( m_bPointFilterDepthOnly)
		REngine::GetDevice().SetTextureFilter( 0, RTF_POINT);
	else
		REngine::GetDevice().SetTextureFilter( 0, RTF_BILINEAR_ANISO);
	// 인버스 매트릭스 관련 상수 설정
	pCamera->GetProjectionMatrix().GetInverse( &matInvVP);
	vZInvVP = RVector4( matInvVP._13, matInvVP._23, matInvVP._33 ,matInvVP._43);
	vWInvVP = RVector4( matInvVP._14, matInvVP._24, matInvVP._34 ,matInvVP._44);
	m_pShaderSSAO->GetConstant("g_vProjInvZ").SetVector4( vZInvVP);
	m_pShaderSSAO->GetConstant("g_vProjInvW").SetVector4( vWInvVP);
	// 뷰 디멘션 관련 설정 : w,h는 랜더 타겟의 것으로 셋팅 한다.
	vViewDimensions = RVector4( (float)m_dwWidthFull, (float)m_dwHeightFull, pCamera->GetNearZ(), pCamera->GetFarZ());
	m_pShaderSSAO->GetConstant("vViewDimensions").SetVector4( vViewDimensions);
	// 깊이 확장 처리 테크닉
	Technique = m_pShaderSSAO->GetTechnique( "TexpandBrightforce");
	Technique.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);
		//DrawFullScreenQuad( 0,0,1,1,true);
		RRenderHelper::RenderQuad(0, 0, (float)m_dwWidthFull, (float)m_dwHeightFull);
		Technique.EndPass();
	}
	Technique.End();
	//pTexDepthToUse = m_pDepthExpand;
	// ----------------------------------------------------------------------------------




	// ----------------------------------- Pass AO -------------------------------------
	// 우선은 AO정보만 랜더

	// 축소 타겟
	REngine::GetDevice().SetRenderTarget( 0, m_pTarget[ (int)nTargetIdnx]);
	// 뎁스 버퍼와 노이즈 버퍼를 입력으로
	//REngine::GetDevice().SetTexture( 0, pTexDepthToUse);
	//REngine::GetDevice().SetTexture( 1, m_ptexNoise);
	//REngine::GetDevice().SetTexture( 2, pTexMask);
	//REngine::GetDevice().SetTexture( 3, pTexNormal);
	
	REngine::GetDevice().SetTextureFilter( 1, RTF_POINT);
	REngine::GetDevice().SetTextureFilter( 2, RTF_POINT);
	REngine::GetDevice().SetTextureFilter( 3, RTF_POINT);
	
	// 인버스 매트릭스 관련 상수 설정
	pCamera->GetProjectionMatrix().GetInverse( &matInvVP);
	vZInvVP = RVector4( matInvVP._13, matInvVP._23, matInvVP._33 ,matInvVP._43);
	vWInvVP = RVector4( matInvVP._14, matInvVP._24, matInvVP._34 ,matInvVP._44);
	m_pShaderSSAO->GetConstant("g_vProjInvZ").SetVector4( vZInvVP);
	m_pShaderSSAO->GetConstant("g_vProjInvW").SetVector4( vWInvVP);
	// 뷰 디멘션 관련 설정 : w,h는 랜더 타겟의 것으로 셋팅 한다.
	vViewDimensions = RVector4( (float)m_nWidth, (float)m_nHeight, pCamera->GetNearZ(), pCamera->GetFarZ());
	m_pShaderSSAO->GetConstant("vViewDimensions").SetVector4( vViewDimensions);
	// AO 처리 테크닉
	Technique = m_pShaderSSAO->GetTechnique( "Tmain");
	//Technique = m_pShaderSSAO->GetTechnique( "TmainUseNormal");
	Technique.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);
		//DrawFullScreenQuad( 0,0,1,1,true);
		RRenderHelper::RenderQuad(0, 0, (float)m_nWidth, (float)m_nHeight);
		Technique.EndPass();
	}
	Technique.End();
	nTargetIdnx = !nTargetIdnx;



	// --------------- Edge Detect -------------------------------------------------------
	REngine::GetDevice().SetRenderTarget( 0, m_pEdge);
	//REngine::GetDevice().SetTexture( 0, pTexDepthToUse);
	pCamera->GetProjectionMatrix().GetInverse( &matInvVP);
	vZInvVP = RVector4( matInvVP._13, matInvVP._23, matInvVP._33 ,matInvVP._43);
	vWInvVP = RVector4( matInvVP._14, matInvVP._24, matInvVP._34 ,matInvVP._44);
	m_pShaderEdgeDetect->GetConstant("g_vProjInvZ").SetVector4( vZInvVP);
	m_pShaderEdgeDetect->GetConstant("g_vProjInvW").SetVector4( vWInvVP);
	// 뷰 디멘션 관련 설정 : w,h, depth limit, farZ
	vViewDimensions = RVector4( (float)nWidth, (float)nHeight, 40, pCamera->GetFarZ());
	m_pShaderEdgeDetect->GetConstant("vViewDimensions").SetVector4( vViewDimensions);
	// AO 처리 테크닉
	Technique = m_pShaderEdgeDetect->GetTechnique( "TLaplacian");
	Technique.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);
		//DrawFullScreenQuad( 0,0,1,1,true);
		RRenderHelper::RenderQuad(0, 0, (float)nWidth, (float)nHeight);
		Technique.EndPass();
	}
	Technique.End();




	
	bWillTargetWrite = true;
	// ----------------------------------- Pass denoise  -------------------------------------
	// 깊이 정보를 참고한다.
	//REngine::GetDevice().SetTexture( 0, pTexDepthToUse);
	// 깊이 정보를 참고한다.
	//REngine::GetDevice().SetTexture( 1, m_pEdge);
	// AO 정보만 있는 스몰 버퍼를 인풋으로
	//REngine::GetDevice().SetTexture( 2, m_pTarget[ (int)!nTargetIdnx]);

	if( bWillTargetWrite)
	{
		// 모든 라이트 처리가 되어 있는 버퍼에 SSAO 결과를 알파 합성으로 그릴 것이다.
		REngine::GetDevice().SetBlending(RBLEND_DESTCOLOR, RBLEND_ZERO);
		// 원래 버퍼에 출력
		REngine::GetDevice().SetRenderTarget( 0, pRT0);
		// 사용 텍스쳐 사이즈 관련 재 설정
		//vViewDimensions = RVector4( (float)m_dwWidthFull, (float)m_dwHeightFull, pCamera->GetNearZ(), pCamera->GetFarZ());
		vViewDimensions = RVector4( (float)m_nWidth, (float)m_nHeight, pCamera->GetNearZ(), pCamera->GetFarZ());

		if( m_bPointFilterDepthOnly)
			REngine::GetDevice().SetTextureFilter( 0, RTF_POINT);
		else
			REngine::GetDevice().SetTextureFilter( 0, RTF_LINEAR);

		if( m_bPointFilterBufferOnly)
		{
			REngine::GetDevice().SetTextureFilter( 1, RTF_POINT);
			REngine::GetDevice().SetTextureFilter( 2, RTF_POINT);
		}
		else
		{
			REngine::GetDevice().SetTextureFilter( 1, RTF_LINEAR/*RTF_BILINEAR_ANISO*/);
			REngine::GetDevice().SetTextureFilter( 2, RTF_LINEAR/*RTF_BILINEAR_ANISO*/);
		}
	}
	else
	{
		// 축소 타겟에 그린 AO 정보를 라이트 버퍼( 원래 설정 된 타겟)에 드로우

		// 다른 스몰 버퍼에 블러링 처리 하여 씀
		REngine::GetDevice().SetRenderTarget( 0, m_pTarget[ (int)nTargetIdnx]);
		// 사용 텍스쳐 사이즈 관련 재 설정
		vViewDimensions = RVector4( (float)m_nWidth, (float)m_nHeight, pCamera->GetNearZ(), pCamera->GetFarZ());

		if( m_bPointFilterDepthOnly)
			REngine::GetDevice().SetTextureFilter( 0, RTF_POINT);
		else
			REngine::GetDevice().SetTextureFilter( 0, RTF_BILINEAR_ANISO);

		if( m_bPointFilterBufferOnly)
		{
			REngine::GetDevice().SetTextureFilter( 1, RTF_POINT);
			REngine::GetDevice().SetTextureFilter( 2, RTF_POINT);
		}
		else
		{
			REngine::GetDevice().SetTextureFilter( 1, RTF_BILINEAR_ANISO);
			REngine::GetDevice().SetTextureFilter( 2, RTF_BILINEAR_ANISO);
		}
	}

	// 사용 텍스쳐 사이즈 셰이더에 적용
	m_pShaderSSAO->GetConstant("vViewDimensions").SetVector4( vViewDimensions);
	// 블러링 테크닉
	Technique = m_pShaderSSAO->GetTechnique( "Blur4x4BaiasEdge");
	Technique.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);
		RRenderHelper::RenderQuad(0, 0, bWillTargetWrite ? m_dwWidthFull : m_nWidth, bWillTargetWrite ? m_dwHeightFull : m_nHeight);
		Technique.EndPass();
	}
	Technique.End();
	nTargetIdnx = !nTargetIdnx;

	// -------------------------------------- restore ----------------------------------
	REngine::GetDevice().SetRenderTarget( 0, pRT0);
	REngine::GetDevice().SetRenderTarget( 1, pRT1);
	REngine::GetDevice().SetRenderTarget( 2, pRT2);
	REngine::GetDevice().SetRenderTarget( 3, pRT3);

	REngine::GetDevice().SetTexture( 0, pTexOrg0);
	REngine::GetDevice().SetTexture( 1, pTexOrg1);
	REngine::GetDevice().SetTexture( 2, pTexOrg2);
	REngine::GetDevice().SetTexture( 3, pTexOrg3);

	REngine::GetDevice().SetTextureFilter( 0, RTF_LINEAR);
	REngine::GetDevice().SetTextureFilter( 1, RTF_LINEAR);
	REngine::GetDevice().SetTextureFilter( 2, RTF_LINEAR);
	REngine::GetDevice().SetTextureFilter( 3, RTF_LINEAR);

	REngine::GetDevice().SetTextureAddressWrap( 0);
	REngine::GetDevice().SetTextureAddressWrap( 1);
	REngine::GetDevice().SetTextureAddressWrap( 2);
	REngine::GetDevice().SetTextureAddressWrap( 3);

	REngine::GetDevice().SetBlending(RBLEND_NONE);
	REngine::GetDevice().SetDepthEnable(true, true);
	REngine::GetDevice().SetCullMode(RCULL_CW);
}



void RSSAOProcessor::Init( int nW, int nH)
{
	if( nW <= 0 || nH <= 0)
		return;

	m_bPointFilterBufferOnly = true;
	m_bPointFilterDepthOnly = true;

	m_dwWidthFull = nW;
	m_dwHeightFull = nH;

	nW = nW / 2;
	nH = nH / 2;

	RGIProcessor::Init( nW, nH);

	bool bSupportRGB16 = REngine::GetDevice().QueryFeature(RQF_RGB16_RTF);
	bool bSupportR32F = REngine::GetDevice().QueryFeature(RQF_R32F_RTF);
	bool bSupportMaginot = REngine::GetDevice().QueryFeature(RQF_A8R8G8B8_RTF);

	m_pShaderSSAO = REngine::GetDevice().CreateShaderFX("SSAO.fx");
	m_pShaderEdgeDetect = REngine::GetDevice().CreateShaderFX("EdgeDetect.fx");
	m_ptexNoise = REngine::GetDevice().CreateTexture( "vector_noise.dds", RTF_POINT, 0);

	if( bSupportRGB16)
	{
		m_pTarget[0] = REngine::GetDevice().CreateRenderTargetTexture( m_nWidth, m_nHeight, RFMT_RGB16);
		m_pTarget[1] = REngine::GetDevice().CreateRenderTargetTexture( m_nWidth, m_nHeight, RFMT_RGB16);
		m_pEdge = REngine::GetDevice().CreateRenderTargetTexture( m_dwWidthFull, m_dwHeightFull, RFMT_RGB16);
		m_bPointFilterBufferOnly = false;
	}
	else
	{
		m_pTarget[0] = REngine::GetDevice().CreateRenderTargetTexture( m_nWidth, m_nHeight, RFMT_A8R8G8B8);
		m_pTarget[1] = REngine::GetDevice().CreateRenderTargetTexture( m_nWidth, m_nHeight, RFMT_A8R8G8B8);
		m_pEdge = REngine::GetDevice().CreateRenderTargetTexture( m_dwWidthFull, m_dwHeightFull, RFMT_A8R8G8B8);

		if( bSupportMaginot)
			m_bPointFilterBufferOnly = false;
		else
			m_bPointFilterBufferOnly = true;
	}

	m_pDepthExpand = REngine::GetDevice().CreateRenderTargetTexture( m_dwWidthFull, m_dwHeightFull, RFMT_R32F);
	if( bSupportR32F)
		m_bPointFilterDepthOnly = false;
	
	SetSampleOffsets_GaussBlur( 5, m_nWidth, m_nHeight);

	// X, Y 따로 하는 블러링 용
#define NUM_GAUSS	6
	DWORD i;
	FLOAT total=0;
	FLOAT tbl[NUM_GAUSS];
	for( i=0; i<NUM_GAUSS; i++ ){
		FLOAT pos = 1.0f+2.0f*(FLOAT)i;
		tbl[i] = expf(-0.5f*(FLOAT)(pos*pos)/ (25.0f));
		total += 2.0f*tbl[i];
	}
	// 정규화
	for( i=0; i<NUM_GAUSS; i++ ) 
		tbl[i] /= total;

	if( m_pShaderSSAO)
		m_pShaderSSAO->m_pEffect->SetValue("g_fWeight", tbl, sizeof( float) * NUM_GAUSS);


	// X먼저 처리하고 Y를 처리한다
	//m_ptexBlurX = REngine::GetDevice().CreateRenderTargetTexture( m_dwWidthFull, nH, RFMT_RGB16 /*RFMT_A8R8G8B8*/);
	//m_ptexBlurY = REngine::GetDevice().CreateRenderTargetTexture( m_dwWidthFull, m_dwHeightFull, RFMT_A8R8G8B8 /*RFMT_A8R8G8B8*/);
}


void RSSAOProcessor::Destroy()
{
	if (m_pShaderSSAO)
		REngine::GetDevice().DeleteShaderFX(m_pShaderSSAO);
	if (m_pShaderEdgeDetect)
		REngine::GetDevice().DeleteShaderFX(m_pShaderEdgeDetect);
	if( m_ptexNoise)
		REngine::GetDevice().DeleteTexture( m_ptexNoise);
	if( m_pTarget[0])
		REngine::GetDevice().DeleteTexture( m_pTarget[0]);
	if( m_pTarget[1])
		REngine::GetDevice().DeleteTexture( m_pTarget[1]);
	if( m_pEdge)
		REngine::GetDevice().DeleteTexture( m_pEdge);
	if( m_pDepthExpand)
		REngine::GetDevice().DeleteTexture( m_pDepthExpand);
	if( m_ptexBlurX)
		REngine::GetDevice().DeleteTexture( m_ptexBlurX);
	if( m_ptexBlurY)
		REngine::GetDevice().DeleteTexture( m_ptexBlurY);

	m_pShaderSSAO = NULL;
	m_pShaderEdgeDetect = NULL;
	m_ptexNoise = NULL;
	m_pTarget[0] = NULL;
	m_pTarget[1] = NULL;
	m_pEdge = NULL;
	m_pDepthExpand = NULL;
	m_ptexBlurX = NULL;
	m_ptexBlurY = NULL;

}


// nPixel은 홀수이어야 한다.
int RSSAOProcessor::SetSampleOffsets_GaussBlur( int nPixel, int nTexWidth, int nTexHeight, FLOAT fMultiplier )
{
	_ASSERT( nPixel <= 5);

	if(!m_pShaderSSAO)
		return false;

	float tu = 1.0f / ( float )nTexWidth;
	float tv = 1.0f / ( float )nTexHeight;

	// nPixel:9 -> nHalfPixel:4
	int nHalfPixel = nPixel / 2;
	// nPixel X nPixel 텍셀 사각형에서 다이아몬드 꼴 만큼 샘플링에 쓰이는 총 픽셀 수를 얻는다.
	// XXXOXXX -.
	// XXOOOXX  | nHalfPixel*nHalfPixel
	// XOOOOOX -'
	// OOOOOOO <- nPixel
	// XOOOOOX -.
	// XXOOOXX  | nHalfPixel*nHalfPixel
	// XXXOXXX -'
	int nPixelCnt = (nHalfPixel*nHalfPixel) * 2 + nPixel;

	RVector2* vTexCoordOffset = new RVector2[nPixelCnt]; 
	RVector4* vSampleWeight = new RVector4[nPixelCnt];
	RVector4 vWhite( 1.0f, 1.0f, 1.0f, 1.0f );

	float totalWeight = 0.0f;
	int index = 0;
	for( int x = -nHalfPixel; x <= nHalfPixel; x++ )
	{
		for( int y = -nHalfPixel; y <= nHalfPixel; y++ )
		{
			// Exclude pixels with a block distance greater than 2. This will
			// create a kernel which approximates a 5x5 kernel using only 13
			// sample points instead of 25; this is necessary since 2.0 shaders
			// only support 16 texture grabs.
			if( abs( x ) + abs( y ) > nHalfPixel )
				continue;

			// Get the unscaled Gaussian intensity for this offset
			vTexCoordOffset[index] = RVector2( x * tu, y * tv );
			vSampleWeight[index] = vWhite * GaussianDistribution( ( float )x, ( float )y, 1.0f );
			totalWeight += vSampleWeight[index].x;

			index++;
		}
	}

	_ASSERT( nPixelCnt == index);

	// Divide the current weight by the total weight of Zall the samples; Gaussian
	// blur kernels add to 1.0f to ensure that the intensity of the image isn't
	// changed when the blur occurs. An optional multiplier variable is used to
	// add or remove image intensity during the blur.
	for( int i = 0; i < index; i++ )
	{
		vSampleWeight[i] /= totalWeight;
		vSampleWeight[i] *= fMultiplier;
	}

	m_pShaderSSAO->m_pEffect->SetValue("g_avSampleOffsets", vTexCoordOffset, sizeof( RVector2) * nPixelCnt);
	m_pShaderSSAO->m_pEffect->SetValue("g_avSampleWeights", vSampleWeight, sizeof( RVector4) * nPixelCnt);
	m_pShaderSSAO->m_pEffect->SetInt("g_nSampleCount", nPixelCnt);

	SAFE_DELETE_ARRAY( vTexCoordOffset);
	SAFE_DELETE_ARRAY( vSampleWeight);

	return index;
}


float RSSAOProcessor::GaussianDistribution( float x, float y, float rho )
{
	float g = 1.0f / sqrtf( 2.0f * MMath::PI * rho * rho );
	g *= expf( -( x * x + y * y ) / ( 2 * rho * rho ) );

	return g;
}



//-----------------------------------------------------------------------------
// Desc: Draw a properly aligned quad covering the entire render target
//-----------------------------------------------------------------------------
void RSSAOProcessor::DrawFullScreenQuad(float fLeftU, float fTopV, float fRightU, float fBottomV, int bSmallSize)
{
	REngine::GetDevice().SetFillMode(RFILL_SOLID);

	// Ensure that we're directly mapping texels to pixels by offset by 0.5
	// For more info see the doc page titled "Directly Mapping Texels to Pixels"
	float fWidth, fHeight;

	if( bSmallSize)
	{
		fWidth		= (float)m_nWidth;
		fHeight		= (float)m_nHeight;
	}
	else
	{
		fWidth		= (float)m_dwWidthFull;
		fHeight		= (float)m_dwHeightFull;
	}
	
	// Draw the quad
	RVt_pos_tex_rhw vQuad[4];
	vQuad[0].vPos	= RVector4( -0.5f, -0.5f, 0.0f, 1.0f );
	vQuad[0].u		= fLeftU;
	vQuad[0].v		= fTopV;

	vQuad[1].vPos	= RVector4( -0.5f, -0.5f + fHeight, 0.0f, 1.0f );
	vQuad[1].u		= fLeftU;
	vQuad[1].v		= fBottomV;

	vQuad[2].vPos	= RVector4( -0.5f + fWidth, -0.5f, 0.0f, 1.0f );
	vQuad[2].u		= fRightU;
	vQuad[2].v		= fTopV;

	vQuad[3].vPos	= RVector4( -0.5f + fWidth, -0.5f + fHeight, 0.0f, 1.0f );
	vQuad[3].u		= fRightU;
	vQuad[3].v		= fBottomV;

	REngine::GetDevice().SetDepthEnable(false, false);
	REngine::GetDevice().SetFvF(RVt_pos_tex_rhw::FVF);
	REngine::GetDevice().DrawPrimitiveUP( RPT_TRIANGLESTRIP, 2, vQuad, sizeof( RVt_pos_tex_rhw ) );
}

}