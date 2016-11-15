#include "StdAfx.h"
#include "RPostEffectInterface.h"
#include "RVType.h"
#include "RShaderComposite.h"
#include "RShaderFX.h"
#include "RShaderCompositeManager.h"


namespace rs3 {


RPostEffectInterface::RPostEffectInterface( POST_EFFECT_TYPE eType, RRENDERPASS maskingRenderPass )
: m_eType(eType)
, m_pMaskingTarget(NULL)
, m_maskingRenderPass(maskingRenderPass)
, m_bDebug(false)
, m_bActive(true)
, m_bLastPostEffect( false )
{

}

RPostEffectInterface::~RPostEffectInterface(void)
{
	Destroy();
}


bool RPostEffectInterface::Init( int nW, int nH )
{
	return Resize( nW, nH );
}

bool RPostEffectInterface::Resize( int nW, int nH )
{
	if( nW <= 0 || nH <= 0)
		return false;

	m_dwWidth = nW; 
	m_dwHeight = nH;

	return true;
}

void RPostEffectInterface::Destroy()
{
}

void RPostEffectInterface::Invalidate()
{
	DestroyBuffers();
}

void RPostEffectInterface::Restore()
{
	CreateBuffers();
}


void RPostEffectInterface::ClearMaskBuffer()
{
	RPFC_THISFUNC;
	if( m_pMaskingTarget)
		REngine::GetDevice().ColorFill( m_pMaskingTarget);
}


bool RPostEffectInterface::MakeMaskTarget( int nW, int nH, RFORMAT eBufferFormat)
{
	if(!m_pMaskingTarget)
		m_pMaskingTarget = REngine::GetDevice().CreateRenderTargetTexture( nW, nH, eBufferFormat);
	if( R_NONE == m_pMaskingTarget) 
		return false;
	else
		return true;
}


void RPostEffectInterface::DeleteMaskTarget()
{
	if( m_pMaskingTarget)
		REngine::GetDevicePtr()->DeleteTexture( m_pMaskingTarget);	
	m_pMaskingTarget = NULL;	
}


void RPostEffectInterface::DrawFullScreenQuad( DWORD dwX, DWORD dwY, DWORD dwW, DWORD dwH)
{
	RPFC_THISFUNC;
	// Ensure that we're directly mapping texels to pixels by offset by 0.5
	// For more info see the doc page titled "Directly Mapping Texels to Pixels"
	float fWidth, fHeight, fX, fY;
	fWidth		= (float)dwW;
	fHeight		= (float)dwH;
	fX			= (float)dwX;
	fY			= (float)dwY;
	

	// Draw the quad
	RVt_pos_tex_rhw vQuad[4];
	vQuad[0].vPos	= RVector4( -0.5f + fX,		-0.5f + fY,		0.0f, 1.0f );
	vQuad[0].u		= 0;
	vQuad[0].v		= 0;

	vQuad[1].vPos	= RVector4( -0.5f + fX,		-0.5f + fHeight + fY,	0.0f, 1.0f );
	vQuad[1].u		= 0;
	vQuad[1].v		= 1;

	vQuad[2].vPos	= RVector4( -0.5f + fWidth + fX,	-0.5f + fY,		0.0f, 1.0f );
	vQuad[2].u		= 1;
	vQuad[2].v		= 0;

	vQuad[3].vPos	= RVector4( -0.5f + fWidth + fX,	-0.5f + fHeight + fY,	0.0f, 1.0f );
	vQuad[3].u		= 1;
	vQuad[3].v		= 1;

	REngine::GetDevice().SetFogEnable(false);
	REngine::GetDevice().SetFillMode(RFILL_SOLID);
	REngine::GetDevice().SetCullMode(RCULL_CW);
	REngine::GetDevice().SetDepthEnable(false, false);
	REngine::GetDevice().SetVertexFormat(R_NONE);
	REngine::GetDevice().SetFvF(RVt_pos_tex_rhw::FVF);
	REngine::GetDevice().DrawPrimitiveUP( RPT_TRIANGLESTRIP, 2, vQuad, sizeof( RVt_pos_tex_rhw ) );
	REngine::GetDevice().SetDepthEnable(true, true);

}



//=====================================================================================
//RPostEffectVoid

RPostEffectVoid::RPostEffectVoid() 
: RPostEffectInterface(PE_VOID, RRP_NONE)
{
}

void RPostEffectVoid::Render( RCameraSceneNode* pCamera, RTexture* pColorBuffer, RTexture* pNormalBuffer, RTexture* pDepthBuffer)
{
	RPFC_THISFUNC;
	REngine::GetDevice().SetTexture(0, pColorBuffer);
	REngine::GetDevice().SetTextureFilter(0, RTF_POINT);
	
	RSCID rscComponents = RSC_POSITIONW | REngine::GetSceneManager().GetShaderCompositeManager()->GetVertexTexcoordComponent() | RSC_DIFFUSEMAP;
	RShaderFX* pShader = REngine::GetSceneManager().GetShaderCompositeManager()->GetShader(rscComponents, false);
	if( pShader )
	{
		RTechnique technique = pShader->GetTechniqueByIndex(0);
		technique.Begin(NULL, TECH_PRESERVE_NOT);
		technique.BeginPass();
		DrawFullScreenQuad(0, 0, m_dwWidth, m_dwHeight);
		technique.EndPass();
		technique.End();	
	}
}
//=====================================================================================

}