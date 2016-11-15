#include "StdAfx.h"
#include "RPostEffectEdge.h"
#include "RCameraSceneNode.h"
#include "RVType.h"
#include "RShaderCompositeManager.h"
#include "RShaderFX.h"
#include "RSUtil.h"


namespace rs3
{

RPostEffectEdge::RPostEffectEdge()
: RPostEffectInterface( PE_EDGE, RRP_POSTEFFECT_EDGE )
,m_fEdgePower( 0.f )
,m_fTickNess( 0.f )
,m_bProjection( false )
,m_pTexEdgeH(NULL)
,m_pTexEdgeV(NULL)
,m_fBulrFactor( 0 )
,m_iBlurCount( 0 )
,m_iScreenDiv( 0 )
,m_iPEEdgeCount( 0 )
{
	SetActive( false );
}


RPostEffectEdge::~RPostEffectEdge()
{
	RPostEffectInterface::Destroy();

	if (m_pEdgeShader)
		REngine::GetDevice().DeleteShaderFX(m_pEdgeShader);
	m_pEdgeShader = NULL;
}


bool RPostEffectEdge::Init( int nW, int nH )
{
	if(!RPostEffectInterface::Init(nW, nH))
		return false;

	if ( false == REngine::GetDevice().QueryFeature(RQF_PS20) )
		return false;

	if ( false == REngine::GetDevice().QueryFeature(RQF_PS30) )
		return false;

	m_pEdgeShader = REngine::GetDevice().CreateShaderFX("PostEdge.fx");

	Restore();

	return true;
}


void RPostEffectEdge::Destroy()
{
	RPostEffectInterface::Destroy();

	if (m_pEdgeShader)
		REngine::GetDevice().DeleteShaderFX(m_pEdgeShader);
	m_pEdgeShader = NULL;

	Invalidate();
}

void RPostEffectEdge::CreateBuffers()
{
	RFORMAT eVelocityBufferFormat = RFMT_A8R8G8B8;
	// 마스킹 타겟
	MakeMaskTarget( m_dwWidth , m_dwHeight , eVelocityBufferFormat);

	PEEdgeQuality(m_iBlurCount, m_iScreenDiv);
}

void RPostEffectEdge::DestroyBuffers()
{
	DeleteMaskTarget();

	REngine::GetDevicePtr()->SafeDeleteTexture( m_pTexEdgeH );
	REngine::GetDevicePtr()->SafeDeleteTexture( m_pTexEdgeV );
}

void RPostEffectEdge::SetFactorTickNess( const float _fTickness, const float _fBlurFactor, const float _fColorPower )
{
	m_fTickNess		= _fTickness + ( m_iScreenDiv / 10.f );
	m_fBulrFactor	= _fBlurFactor + ( m_iScreenDiv / 10.f ) * m_iScreenDiv;// + (float)m_iScreenDiv - 1.f;
	m_fEdgePower	= _fColorPower + (float)m_iScreenDiv / 10.f * (float)m_iScreenDiv;		// 알파 값만 사용하고 있다.

	m_pEdgeShader->GetConstant("g_vEdgeTickNess").SetFloat( m_fTickNess );
	m_pEdgeShader->GetConstant("g_fFactor").SetFloat( m_fBulrFactor );	
	m_pEdgeShader->GetConstant("g_fEdgePower").SetFloat( m_fEdgePower );
}


void RPostEffectEdge::EnablePEEdge( bool _bProjection )
{
	SetActive( true );
	m_bProjection	= _bProjection;
}


void RPostEffectEdge::DisablePEEdge()
{
	SetActive( false );
	m_bProjection	= false;
	m_iPEEdgeCount = 0;
}


void RPostEffectEdge::AddCount()
{
	m_iPEEdgeCount++;
	if( !IsActive() )
		SetActive( true );
}


void RPostEffectEdge::MinusCount()
{
	m_iPEEdgeCount--;
	
	if( 0 >= m_iPEEdgeCount )
	{
		m_iPEEdgeCount = 0;
		if( IsActive() )
			SetActive( false );
	}
}


void RPostEffectEdge::PEEdgeQuality(int _iBlurCount, int _iScreenSize)
{
	if( _iBlurCount == 0 && _iScreenSize == 0 )
		return;

	m_iBlurCount	= _iBlurCount;
	m_iScreenDiv	= _iScreenSize;

	int nScreenWidth	= REngine::GetDevice().GetScreenWidth();
	int nScreenHeight	= REngine::GetDevice().GetScreenHeight();

	if(!m_pTexEdgeH)
		m_pTexEdgeH = REngine::GetDevice().CreateRenderTargetTexture( nScreenWidth / m_iScreenDiv, nScreenHeight / m_iScreenDiv, RFMT_A8R8G8B8 );
	else
	{
		REngine::GetDevicePtr()->DeleteTexture( m_pTexEdgeH );
		m_pTexEdgeH = NULL;
		m_pTexEdgeH = REngine::GetDevice().CreateRenderTargetTexture( nScreenWidth / m_iScreenDiv, nScreenHeight / m_iScreenDiv, RFMT_A8R8G8B8 );
	}


	if(!m_pTexEdgeV)
		m_pTexEdgeV = REngine::GetDevice().CreateRenderTargetTexture( nScreenWidth / m_iScreenDiv, nScreenHeight / m_iScreenDiv, RFMT_A8R8G8B8 );
	{
		REngine::GetDevicePtr()->DeleteTexture( m_pTexEdgeV );
		m_pTexEdgeV = NULL;
		m_pTexEdgeV = REngine::GetDevice().CreateRenderTargetTexture( nScreenWidth / m_iScreenDiv, nScreenHeight / m_iScreenDiv, RFMT_A8R8G8B8 );
	}

	REngine::GetDevice().ColorFill( m_pTexEdgeH );
	REngine::GetDevice().ColorFill( m_pTexEdgeV );


	m_pEdgeShader->GetConstant("g_txSrcEdge").SetTexture(GetMaskBuffer(), REngine::GetDevicePtr());
	m_pEdgeShader->GetConstant("g_fScreenWidth").SetFloat( (float)REngine::GetDevice().GetScreenWidth());
	m_pEdgeShader->GetConstant("g_fScreenHeight").SetFloat( (float)REngine::GetDevice().GetScreenHeight());

	// 값 설정해 준다.
	m_pEdgeShader->GetConstant("g_vEdgeTickNess").SetFloat( m_fTickNess );
	m_pEdgeShader->GetConstant("g_fFactor").SetFloat( m_fBulrFactor );	
	m_pEdgeShader->GetConstant("g_fEdgePower").SetFloat( m_fEdgePower );
}


void RPostEffectEdge::EdgeBlurV()
{
	RTechnique	Technique;
	UINT		uiPassCount;

	REngine::GetDevice().ColorFill( m_pTexEdgeV, D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
	REngine::GetDevice().SetRenderTarget( 0, m_pTexEdgeV );


	Technique = m_pEdgeShader->GetTechnique("PostEdgeBlur");
	m_pEdgeShader->GetConstant("g_txSrcEdgeTest").SetTexture(m_pTexEdgeH, REngine::GetDevicePtr());

	Technique.Begin(&uiPassCount);
	Technique.BeginPass(0);
	DrawFullScreenQuad( 0.0f, 0.0f, m_pTexEdgeV->GetInfo().nTextureWidth, m_pTexEdgeV->GetInfo().nTextureHeight );
	Technique.EndPass();
	Technique.End();
}


void RPostEffectEdge::EdgeBlurH()
{
	RTechnique	Technique;
	UINT		uiPassCount;

	REngine::GetDevice().ColorFill( m_pTexEdgeH, D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
	REngine::GetDevice().SetRenderTarget( 0, m_pTexEdgeH );

	Technique = m_pEdgeShader->GetTechnique("PostEdgeBlur");
	m_pEdgeShader->GetConstant("g_txSrcEdgeTest").SetTexture(m_pTexEdgeV, REngine::GetDevicePtr());

	Technique.Begin(&uiPassCount);
	Technique.BeginPass(1);
	DrawFullScreenQuad( 0.0f, 0.0f, m_pTexEdgeV->GetInfo().nTextureWidth, m_pTexEdgeV->GetInfo().nTextureHeight );
	Technique.EndPass();
	Technique.End();

}


void RPostEffectEdge::Render( RCameraSceneNode* pCamera, RTexture* pColorBuffer, RTexture* pNormalBuffer, RTexture* pDepthBuffer)
{
	RPFC_B("RPostEffectEdge::Render");

	float fFirMipMap = REngine::GetDevice().GetTextureMipmapLodBias(0);
	float fSecMipMap = REngine::GetDevice().GetTextureMipmapLodBias(1);

	REngine::GetDevice().SetTextureMipmapLodBias( 0, 0 );
	REngine::GetDevice().SetTextureMipmapLodBias( 1, 0 );
	

	RTexture* pBeforeRenderTarget0 = REngine::GetDevice().GetRenderTarget(0);	// Low dynamic range surface for final output

	REngine::GetDevice().ColorFill( m_pTexEdgeH, D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
	REngine::GetDevice().SetRenderTarget( 0, m_pTexEdgeH );

	RTechnique Technique;
	UINT uiPass, uiPassCount;

	m_pEdgeShader->GetConstant("g_txSrcColor").SetTexture(pColorBuffer, REngine::GetDevicePtr());

	Technique = m_pEdgeShader->GetTechnique("PostEdge");

	Technique.Begin(&uiPassCount);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);
		DrawFullScreenQuad( 0.0f, 0.0f, m_pTexEdgeH->GetInfo().nTextureWidth, m_pTexEdgeH->GetInfo().nTextureHeight);
		Technique.EndPass();
	}
	Technique.End();


	for( int i = 0 ; i < m_iBlurCount ; i++)
	{
		EdgeBlurV();
		EdgeBlurH();
	}

	REngine::GetDevice().SetRenderTarget( 0, pBeforeRenderTarget0);

	if( GetLastPostEffect() )
	{
		Technique = m_pEdgeShader->GetTechnique("PostEdgeSceneCopy");

		Technique.Begin(&uiPassCount);

		Technique.BeginPass(0);
		DrawFullScreenQuad( 0.0f, 0.0f, m_dwWidth, m_dwHeight);
		Technique.EndPass();

		Technique.End();
	}

	Technique = m_pEdgeShader->GetTechnique("PostEdgeAddScene");
	m_pEdgeShader->GetConstant("g_txSrcEdgeTest").SetTexture(m_pTexEdgeH, REngine::GetDevicePtr());

	REngine::GetDevice().SetStencilEnable(true);
	REngine::GetDevice().SetStencilFunc( RCMP_EQUAL );
	REngine::GetDevice().SetStencilOp(RSTENCILOP_KEEP, RSTENCILOP_KEEP, RSTENCILOP_KEEP);

	Technique.Begin(&uiPassCount);

	if( !REngine::GetSceneManager().GetRenderingStrategy()->GetGammaCorrect() )
		Technique.BeginPass(0);
	else
		Technique.BeginPass(1);

	DrawFullScreenQuad( 0.0f, 0.0f, m_dwWidth, m_dwHeight );

	Technique.EndPass();
	Technique.End();


	//Restore Render State
	{
		REngine::GetDevice().SetStencilEnable(false);
		REngine::GetDevice().SetStencilFunc( RCMP_ALWAYS );

		REngine::GetDevice().SetTextureMipmapLodBias( 0, fFirMipMap );
		REngine::GetDevice().SetTextureMipmapLodBias( 1, fSecMipMap );
	}
	
	RPFC_E;
}


}