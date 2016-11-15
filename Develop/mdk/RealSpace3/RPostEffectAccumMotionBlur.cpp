#include "StdAfx.h"
#include "RPostEffectAccumMotionBlur.h"
#include "RCameraSceneNode.h"
#include "RShaderCompositeManager.h"
#include "RShaderFX.h"

#pragma warning(disable:4800)


namespace rs3 {



RPostEffectAccumMotionBlur::RPostEffectAccumMotionBlur()
: RPostEffectInterface( PE_ACCUM_MOTION_BLUR, RRP_ABLUR )
, m_pShader(NULL)
, m_fDecreaseAlpha(0.2f)
, m_dwTimeAccum(0)
, m_fLastAlphaValue(0)
, m_id(0)
{
	m_pAccumBuffer[0] = NULL;
	m_pAccumBuffer[1] = NULL;

	SetAccumRate( 0.02f);

	SetActive( false);	// 기본으로 꺼둔다.
}

RPostEffectAccumMotionBlur::~RPostEffectAccumMotionBlur(void)
{
}



bool RPostEffectAccumMotionBlur::Init(  int nW, int nH)
{
	if(!RPostEffectInterface::Init(nW, nH))
		return false;

	if ( false == REngine::GetDevice().QueryFeature(RQF_PS20) )
		return false;

	Restore();

	if(R_NONE==m_pAccumBuffer[0]) return false;
	if(R_NONE==m_pAccumBuffer[1]) return false;
	REngine::GetDevice().ColorFill( m_pAccumBuffer[1]);
	
	if(!m_pShader)
		m_pShader = REngine::GetDevice().CreateShaderFX("AccumMotionBlur.fx");

	m_fLastAlphaValue = 0;

	return true;
}

void RPostEffectAccumMotionBlur::Destroy()
{
	RPostEffectInterface::Destroy();

	Invalidate();

	if (m_pShader)
		REngine::GetDevice().DeleteShaderFX(m_pShader);

	m_pAccumBuffer[0] = NULL;
	m_pAccumBuffer[1] = NULL;
	m_pShader = NULL;

	m_fLastAlphaValue = 0;
}

void RPostEffectAccumMotionBlur::CreateBuffers()
{
	// 속도 문제로 RFMT_A16B16G16R16F(64bit)를 사용 하지 않고 32bit짜리 사용 
	// 64bit짜리를 사용 하는 HDR과 순서를 잘 설정 해야 할 것이다..
	RFORMAT eBufferFormat = RFMT_A8R8G8B8;

	// 마스킹 타겟
	MakeMaskTarget( m_dwWidth, m_dwHeight, eBufferFormat);

	// 누적 버퍼
	if(!m_pAccumBuffer[0])
		m_pAccumBuffer[0] = REngine::GetDevice().CreateRenderTargetTexture( m_dwWidth, m_dwHeight, eBufferFormat);
	if(!m_pAccumBuffer[1])
		m_pAccumBuffer[1] = REngine::GetDevice().CreateRenderTargetTexture( m_dwWidth, m_dwHeight, eBufferFormat);

	REngine::GetDevice().ColorFill( m_pAccumBuffer[0]);
	REngine::GetDevice().ColorFill( m_pAccumBuffer[1]);
}

void RPostEffectAccumMotionBlur::DestroyBuffers()
{
	DeleteMaskTarget();

	REngine::GetDevicePtr()->SafeDeleteTexture(m_pAccumBuffer[0]);
	REngine::GetDevicePtr()->SafeDeleteTexture(m_pAccumBuffer[1]);
}


//-----------------------------------------------------------------------------
// Name: RenderDOF()
// Desc: Renders Blur effect
//-----------------------------------------------------------------------------
void RPostEffectAccumMotionBlur::Render( RCameraSceneNode* pCamera, RTexture* pColorBuffer, RTexture* pNormalBuffer, RTexture* pDepthBuffer)
{
	RPFC_THISFUNC;
	// 2D 랜더를 위한 상태 설정
	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT );
	REngine::GetDevice().SetTextureFilter( 1, RTF_POINT );
	REngine::GetDevice().SetTextureFilter( 2, RTF_POINT );


	REngine::GetDevice().SetDepthEnable( false, false);

	REngine::GetDevice().SetTextureAddressClamp( 0);
	REngine::GetDevice().SetTextureAddressClamp( 1);
	REngine::GetDevice().SetTextureAddressClamp( 2);

	RTechnique Technique;
	UINT uiPass, uiPassCount;

	// 일정 주기마다 누적 버퍼를 갱신한다.
	if( m_dwTimeAccum > m_dwAccumRate)
	{
		m_dwTimeAccum = 0;

		// 누적버퍼에 현재 임시 버퍼(액터의 모습들이 담김)을 축적시킨다. ------------------------------------------------
		// 한 프레임 씩 밀려넣어서 할 것이므로 최종 화면에 랜더 후 축적 시킨다.
		RTexture* pOrg = REngine::GetDevice().SetRenderTarget( 0, m_pAccumBuffer[(int)(1-m_id)]);

		Technique = m_pShader->GetTechnique("Accum");
		m_pShader->GetConstant("g_fDecreaseAlpha").SetFloat( m_fDecreaseAlpha);

		REngine::GetDevice().SetTexture(0, m_pAccumBuffer[(int)m_id]);
		REngine::GetDevice().SetTexture(1, GetMaskBuffer());
		REngine::GetDevice().SetTexture(2, pColorBuffer);

		Technique.Begin(&uiPassCount, TECH_PRESERVE_NOT);
		for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
		{
			Technique.BeginPass(uiPass);

			DrawFullScreenQuad( 0,0, (float)m_dwWidth, (float)m_dwHeight);

			Technique.EndPass();
		}
		Technique.End();
		REngine::GetDevice().SetRenderTarget( 0, pOrg);

		m_fLastAlphaValue -= m_fDecreaseAlpha;

		// idx swap
		m_id = (bool)(1-m_id);
	}

	// 누적 버퍼의 내용을 최종 화면에 반영하여 최종 블러. -----------------------------------------------------------------------
	Technique = m_pShader->GetTechnique("Blur");
	REngine::GetDevice().SetTexture(0, pColorBuffer);
	REngine::GetDevice().SetTexture(1, m_pAccumBuffer[(int)m_id]);
	REngine::GetDevice().SetTexture(2, NULL);
	Technique.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);

		DrawFullScreenQuad( 0,0, (float)m_dwWidth, (float)m_dwHeight );

		Technique.EndPass();
	}
	Technique.End();

	// 상태 돌려 놓기 ----------------------------------------------------------------------------------------------------------
	REngine::GetDevice().SetTextureFilter( 0, RTF_LINEAR );
	REngine::GetDevice().SetTextureFilter( 1, RTF_LINEAR );
	REngine::GetDevice().SetTextureFilter( 2, RTF_LINEAR );
	REngine::GetDevice().SetTextureAddressWrap( 0);
	REngine::GetDevice().SetTextureAddressWrap( 1);
	REngine::GetDevice().SetTextureAddressWrap( 2);
	REngine::GetDevice().SetTexture( 0, NULL);
	REngine::GetDevice().SetTexture( 1, NULL);
	REngine::GetDevice().SetTexture( 2, NULL);

	m_dwTimeAccum += REngine::GetTimer().GetElapsedTime();

	if( m_bDebug)
		RenderOverlay();


	REngine::GetDevice().SetDepthEnable( true, true);
}


//-----------------------------------------------------------------------------
// Name: RenderOverlay()
// Desc: Visializes one of the surfaces in the overlay
//-----------------------------------------------------------------------------
void RPostEffectAccumMotionBlur::RenderOverlay()
{
	RTechnique Technique;
	
	Technique = m_pShader->GetTechnique( "MaskInfo");

	//REngine::GetDevice().SetTexture(0, m_pAccumBuffer[(int)m_id]);
	REngine::GetDevice().SetTexture(0, GetMaskBuffer());


	REngine::GetDevice().SetDepthEnable( false, false);

	UINT uiPass, uiPassCount;
	Technique.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);

		DrawFullScreenQuad( 0,0, (float)256, (float)256 );

		Technique.EndPass();
	}
	Technique.End();


// 	REngine::GetDevice().SetTexture(0, m_pAccumBuffer[(int)m_id]);
// 	Technique = m_pShader->GetTechnique( "Overlay");
// 	Technique.Begin(&uiPassCount, 0);
// 	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
// 	{
// 		Technique.BeginPass(uiPass);
// 
// 		DrawFullScreenQuad( 0,256, (float)256, (float)256 );
// 
// 		Technique.EndPass();
// 	}
// 	Technique.End();
// 
// 
// 	REngine::GetDevice().SetRenderTarget( 0, m_pAccumBuffer[(int)(1-m_id)]);
// 	Technique = m_pShader->GetTechnique( "Overlay");
// 	Technique.Begin(&uiPassCount, 0);
// 	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
// 	{
// 		Technique.BeginPass(uiPass);
// 
// 		DrawFullScreenQuad( 0,512, (float)256, (float)256 );
// 
// 		Technique.EndPass();
// 	}
// 	Technique.End();



	REngine::GetDevice().SetTexture(0, NULL);

}



}