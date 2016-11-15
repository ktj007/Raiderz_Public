#include "StdAfx.h"

#include "RPostEffectHDR.h"
#include "RCameraSceneNode.h"
#include "RVType.h"
#include "RRenderHelper.h"
#include "RShaderFX.h"
#include "RGlareDef.h"
#include "REnvironmentSet.h"

namespace rs3 {

const RVector RPostEffectHDR::DEFAULT_BRIGHTPASS_THRESHOLD = RVector( 1.f, 0.9f, 0.9f );



RPostEffectHDR::RPostEffectHDR() 
: RPostEffectInterface( PE_HDR, RRP_NONE )
, m_pTexSceneScaled(NULL)
, m_pTexBrightPass(NULL)
, m_pTexBloomSource(NULL)
, m_pHDRShader(NULL)
, m_fBloomBlurDeviation(3.f)
, m_fBloomBlurMultiplier(2.f)
, m_bGaussian2Pass(false)
, m_bScaleAndBright1Pass(false)
{
	for( int i = 0; i < NUM_BLOOM_TEXTURES; ++i )
		m_apTexBloom[i] = NULL;

	m_vBrightPassThreshold = DEFAULT_BRIGHTPASS_THRESHOLD;

	m_bDebugRT = false;
}

RPostEffectHDR::~RPostEffectHDR()
{

}

bool RPostEffectHDR::Init( int nW, int nH)
{
	RPFC_THISFUNC;
	if(!RPostEffectInterface::Init(nW, nH))
		return false;

	if ( false == REngine::GetDevice().QueryFeature(RQF_PS20) )
		return false;

	if ( false == REngine::GetDevice().QueryFeature(RQF_PS30) )
		return false;

	Restore();
	
	if(!m_pHDRShader)
		m_pHDRShader = REngine::GetDevice().CreateShaderFX("HDRLight.fx");

	return true;
}

void RPostEffectHDR::Destroy()
{
	RPostEffectInterface::Destroy();

	Invalidate();

	if (m_pHDRShader)
	{
		REngine::GetDevice().DeleteShaderFX(m_pHDRShader);
		m_pHDRShader = NULL;
	}
}

void RPostEffectHDR::DestroyBuffers()
{
	REngine::GetDevicePtr()->SafeDeleteTexture( m_pTexSceneScaled );
	REngine::GetDevicePtr()->SafeDeleteTexture( m_pTexBrightPass );
	REngine::GetDevicePtr()->SafeDeleteTexture( m_pTexBloomSource );

	for( int i = 0; i < NUM_BLOOM_TEXTURES; ++i ) 
	{
		REngine::GetDevicePtr()->SafeDeleteTexture( m_apTexBloom[i] );
	}
}

void RPostEffectHDR::CreateBuffers()
{
	RFORMAT eHDRBufferFormat = RFMT_A16B16G16R16F;
	RFORMAT eDepthInfoBufferFormat = RFMT_G32R32F;

	if( false == REngine::GetDevice().QueryFeature(RQF_A16B16G16R16F) 
		||	false == REngine::GetDevice().QueryFeature(RQF_G32R32F))
	{
		eHDRBufferFormat = RFMT_A8R8G8B8;
		eDepthInfoBufferFormat = RFMT_G16R16F;

	}

	int nCropWidth, nCropHeight;

	GetCropSize(nCropWidth, nCropHeight);

	// Scaled version of the HDR scene texture
	if(!m_pTexSceneScaled)
		m_pTexSceneScaled = REngine::GetDevice().CreateRenderTargetTexture( nCropWidth / 4, nCropHeight / 4, eHDRBufferFormat);

	// bright-pass filter texture.  // Texture has a black border of single texel thickness to fake border 
	if(!m_pTexBrightPass)
		m_pTexBrightPass = REngine::GetDevice().CreateRenderTargetTexture( nCropWidth / 4, nCropHeight / 4, RFMT_A8R8G8B8);

	// source for the bloom effect
	if(!m_pTexBloomSource)
		m_pTexBloomSource = REngine::GetDevice().CreateRenderTargetTexture (nCropWidth / 8, nCropHeight / 8, RFMT_A8R8G8B8);

	// Textures with borders must be cleared since scissor rect testing will
	// be used to avoid rendering on top of the border
	REngine::GetDevice().ColorFill( m_pTexSceneScaled );
	REngine::GetDevice().ColorFill( m_pTexBloomSource );
	REngine::GetDevice().ColorFill( m_pTexBrightPass );



	// Create the temporary blooming effect textures
	for( int i = 0; i < NUM_BLOOM_TEXTURES; ++i ) 
	{
		if(!m_apTexBloom[i])
			m_apTexBloom[i] = REngine::GetDevice().CreateRenderTargetTexture( nCropWidth / 8, nCropHeight / 8, RFMT_A8R8G8B8);
	}

	for( int i = 0; i < NUM_BLOOM_TEXTURES; ++i ) 
	{
		REngine::GetDevice().ColorFill( m_apTexBloom[i] );
	}
}

//-----------------------------------------------------------------------------
// Desc: Get the texture coordinate offsets to be used inside the DownScale4x4
//       pixel shader.
//-----------------------------------------------------------------------------
bool RPostEffectHDR::GetSampleOffsets_DownScale4x4( int nWidth, int nHeight, RVector2 vSampleOffsets[] )
{
	if( NULL == vSampleOffsets )
		return false;

	float tU = 1.0f / nWidth;
	float tV = 1.0f / nHeight;

	// Sample from the 16 surrounding points. Since the center point will be in
	// the exact center of 16 texels, a 0.5f offset is needed to specify a texel center.
	int index = 0;
	for( int y = 0; y < 4; ++y )
	{
		for( int x = 0; x < 4; ++x )
		{
			vSampleOffsets[ index ].x = ( x - 1.5f ) * tU;
			vSampleOffsets[ index ].y = ( y - 1.5f ) * tV;
			++index;
		}
	}

	return true;
}


//-----------------------------------------------------------------------------
// Desc: Get the texture coordinate offsets to be used inside the DownScale2x2
//       pixel shader.
//-----------------------------------------------------------------------------
bool RPostEffectHDR::GetSampleOffsets_DownScale2x2( int nWidth, int nHeight, RVector2 vSampleOffsets[] )
{
	float tU = 1.0f / nWidth;
	float tV = 1.0f / nHeight;

	// 4점의 텍셀을 샘플링 해오지만 바로 이웃의 텍셀이 아닌 한 텍셀 더 건너서 샘플링 해 온다.
	// 반드시 입력 필터링을 LINEAR로 하여 두 텍셀이 보간되어 들어오게 할 것.
	int index = 0;
	for( int y = -1; y < 2; y+=2 )
	{
		for( int x = -1; x < 2; x+=2 )
		{
			vSampleOffsets[ index ].x = ( x * 1.5f ) * tU;
			vSampleOffsets[ index ].y = ( y * 1.5f ) * tV;
			index++;
		}
	}

	return true;
}


//-----------------------------------------------------------------------------
// Desc: Helper function for GetSampleOffsets function to compute the 
//       2 parameter Gaussian distrubution using the given standard deviation
//       rho
//-----------------------------------------------------------------------------
float RPostEffectHDR::GaussianDistribution( float x, float y, float rho )
{
	float g = 1.0f / sqrtf( 2.0f * MMath::PI * rho * rho );
	g *= expf( -( x * x + y * y ) / ( 2 * rho * rho ) );

	return g;
}

//-----------------------------------------------------------------------------
// Desc: Get the texture coordinate offsets to be used inside the GaussBlur5x5
//       pixel shader.
//-----------------------------------------------------------------------------
bool RPostEffectHDR::GetSampleOffsets_GaussBlur5x5( int nTexWidth, int nTexHeight, RVector2* vTexCoordOffset, RVector4* vSampleWeight, FLOAT fMultiplier )
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

bool RPostEffectHDR::GetSampleOffsets_Bloom(int nTexWidth, float afTexCoordOffset[15], RVector4* vColorWeight, float fDeviation, FLOAT fMultiplier )
{
	int i = 0;
	float tu = 1.0f / ( float )nTexWidth;

	// Fill the center texel
	float weight = fMultiplier * GaussianDistribution( 0, 0, fDeviation );
	vColorWeight[0] = RVector4( weight, weight, weight, 1.0f );

	afTexCoordOffset[0] = 0.0f;

	// Fill the first half
	for( i = 1; i < 8; i++ )
	{
		// Get the Gaussian intensity for this offset
		weight = fMultiplier * GaussianDistribution( ( float )i, 0, fDeviation );
		afTexCoordOffset[i] = i * tu;

		vColorWeight[i] = RVector4( weight, weight, weight, 1.0f );
	}

	// Mirror to the second half
	for( i = 8; i < 15; i++ )
	{
		vColorWeight[i] = vColorWeight[i - 7];
		afTexCoordOffset[i] = -afTexCoordOffset[i - 7];
	}

	return true;
}

bool RPostEffectHDR::GetSampleOffsets_Star( int nTexWidth, float afTexCoordOffset[15], RVector4* vColorWeight, float fDeviation )
{
	int i = 0;
	float tu = 1.0f / ( float )nTexWidth;

	// Fill the center texel
	float weight = 1.0f * GaussianDistribution( 0, 0, fDeviation );
	vColorWeight[0] = RVector4( weight, weight, weight, 1.0f );

	afTexCoordOffset[0] = 0.0f;

	// Fill the first half
	for( i = 1; i < 8; i++ )
	{
		// Get the Gaussian intensity for this offset
		weight = 1.0f * GaussianDistribution( ( float )i, 0, fDeviation );
		afTexCoordOffset[i] = i * tu;

		vColorWeight[i] = RVector4( weight, weight, weight, 1.0f );
	}

	// Mirror to the second half
	for( i = 8; i < 15; i++ )
	{
		vColorWeight[i] = vColorWeight[i - 7];
		afTexCoordOffset[i] = -afTexCoordOffset[i - 7];
	}

	return S_OK;
}

void RPostEffectHDR::GetCropSize(int& outWidth, int& outHeight)
{
	outWidth	= m_dwWidth - m_dwWidth % 8;
	outHeight	= m_dwHeight - m_dwHeight % 8;
}

//-----------------------------------------------------------------------------
// Desc: Scale down g_pTexScene by 1/4 x 1/4 and place the result in m_pTexSceneScaled
//-----------------------------------------------------------------------------
RTexture* RPostEffectHDR::SceneScale( RCameraSceneNode* pCamera, RTexture* pScrBuffer)
{
	RVector2 vSampleOffsets[MAX_SAMPLES];

	// Create a 1/4 x 1/4 scale copy of the HDR texture. Since bloom textures
	// are 1/8 x 1/8 scale, border texels of the HDR texture will be discarded 
	// to keep the dimensions evenly divisible by 8; this allows for precise 
	// control over sampling inside pixel shaders.
	RTechnique Technique = m_pHDRShader->GetTechnique("DownScale4x4");

	// Get the sample offsets used within the pixel shader
	GetSampleOffsets_DownScale4x4(  REngine::GetDevice().GetScreenWidth(), REngine::GetDevice().GetScreenHeight(), vSampleOffsets );
	m_pHDRShader->m_pEffect->SetValue("g_avSampleOffsets", vSampleOffsets, sizeof( vSampleOffsets ) );

	REngine::GetDevice().SetRenderTarget( 0, m_pTexSceneScaled );
	REngine::GetDevice().SetTexture(0, pScrBuffer);
	REngine::GetDevice().SetTextureFilter(0, RTF_POINT);

	UINT uiPassCount, uiPass;
	Technique.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);
		DrawFullScreenQuad( 0,0, m_pTexSceneScaled->GetInfo().nTextureWidth, m_pTexSceneScaled->GetInfo().nTextureHeight);
		Technique.EndPass();
	}
	Technique.End();

	return m_pTexSceneScaled;
}

//-----------------------------------------------------------------------------
// Desc: Run the bright-pass filter on m_pTexSceneScaled and place the result
//       in m_pTexBrightPass
//-----------------------------------------------------------------------------
RTexture* RPostEffectHDR::BrightPass( RCameraSceneNode* pCamera, RTexture* pScrBuffer)
{
	// The bright-pass filter removes everything from the scene except lights and
	// bright reflections
	RTechnique Technique = m_pHDRShader->GetTechnique("BrightPassFilter");

	RVector vHDRBrightPassThreshold = m_vBrightPassThreshold;
	// 환경이 있으면 환경값을 사용한다.
	REnvironmentSet* pEnvironmentSet = REngine::GetSceneManager().GetCurrentRootSceneNode(false)->GetEnvironmentSet();
	if( pEnvironmentSet )
	{
		vHDRBrightPassThreshold = pEnvironmentSet->GetHDRBrightPassThreshold();
	}
	m_pHDRShader->GetConstant("g_vHDRBrightPassThreshold").SetVector3(vHDRBrightPassThreshold);

	REngine::GetDevice().SetRenderTarget( 0, m_pTexBrightPass );
	REngine::GetDevice().SetTexture( 0, pScrBuffer);
	//REngine::GetDevice().SetTextureFilter( 0, RTF_POINT );
	REngine::GetDevice().SetTextureFilter( 0, RTF_LINEAR);

	UINT uiPassCount, uiPass;
	Technique.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);
		DrawFullScreenQuad( 0,0, m_pTexBrightPass->GetInfo().nTextureWidth, m_pTexBrightPass->GetInfo().nTextureHeight);
		Technique.EndPass();
	}
	Technique.End();

	return m_pTexBrightPass;
}


//-----------------------------------------------------------------------------
// Desc: Scale down g_pTexScene by 1/4 x 1/4 and place the result in m_pTexSceneScaled
//-----------------------------------------------------------------------------
RTexture* RPostEffectHDR::SceneScaleAndBrightPass( RCameraSceneNode* pCamera, RTexture* pScrBuffer)
{
	RTechnique Technique = m_pHDRShader->GetTechnique("DownScale4x4nBrightPassFilter");

	// 환경이 있으면 환경값을 사용한다.
	RVector vHDRBrightPassThreshold = m_vBrightPassThreshold;
	REnvironmentSet* pEnvironmentSet = REngine::GetSceneManager().GetCurrentRootSceneNode(false)->GetEnvironmentSet();
	if( pEnvironmentSet )
	{
		vHDRBrightPassThreshold = pEnvironmentSet->GetHDRBrightPassThreshold();
	}
	m_pHDRShader->GetConstant("g_vHDRBrightPassThreshold").SetVector3(vHDRBrightPassThreshold);

	// Get the sample offsets used within the pixel shader
	RVector2 vSampleOffsets[MAX_SAMPLES];
	GetSampleOffsets_DownScale4x4(  REngine::GetDevice().GetScreenWidth(), REngine::GetDevice().GetScreenHeight(), vSampleOffsets );
	m_pHDRShader->m_pEffect->SetValue("g_avSampleOffsets", vSampleOffsets, sizeof( vSampleOffsets ) );

	REngine::GetDevice().SetRenderTarget( 0, m_pTexBrightPass );
	REngine::GetDevice().SetTexture( 0, pScrBuffer);
	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT);

	UINT uiPassCount, uiPass;
	Technique.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);
		DrawFullScreenQuad( 0,0, m_pTexBrightPass->GetInfo().nTextureWidth, m_pTexBrightPass->GetInfo().nTextureHeight);
		Technique.EndPass();
	}
	Technique.End();

	return m_pTexBrightPass;
}

//-----------------------------------------------------------------------------
// Name: BloomSource
// Desc: Scale down m_pTexStarSource by 1/2 x 1/2 and place the result in 
//       m_pTexBloomSource
//-----------------------------------------------------------------------------
RTexture* RPostEffectHDR::BloomSource( RCameraSceneNode* pCamera, RTexture* pScrBuffer)
{
	RVector2 vSampleOffsets[MAX_SAMPLES];
	RVector4 vSampleWeights[MAX_SAMPLES];

	// Get the sample offsets used within the pixel shader
	RTextureInfo Info = m_pTexBrightPass->GetInfo();

	GetSampleOffsets_GaussBlur5x5( Info.nTextureWidth, Info.nTextureHeight, vSampleOffsets, vSampleWeights );
	m_pHDRShader->m_pEffect->SetValue("g_avSampleOffsets", vSampleOffsets, sizeof( vSampleOffsets ) );
	m_pHDRShader->m_pEffect->SetValue("g_avSampleWeights", vSampleWeights, sizeof( vSampleWeights ) );

	// The gaussian blur smooths out rough edges to avoid aliasing effects
	// when the star effect is run
	RTechnique Technique = m_pHDRShader->GetTechnique("GaussBlur5x5");

	REngine::GetDevice().SetRenderTarget( 0, m_pTexBloomSource );
	REngine::GetDevice().SetTexture( 0, pScrBuffer );
	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT );

	UINT uiPassCount, uiPass;
	Technique.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);
		DrawFullScreenQuad( 0,0, m_pTexBloomSource->GetInfo().nTextureWidth, m_pTexBloomSource->GetInfo().nTextureHeight);
		Technique.EndPass();
	}
	Technique.End();

	return m_pTexBloomSource;
}


//-----------------------------------------------------------------------------
// Name: BrightPass_To_BloomSourc2Pass
// Desc: Scale down m_pTexStarSource by 1/2 x 1/2 and place the result in 
//       m_pTexBloomSource
//-----------------------------------------------------------------------------
RTexture* RPostEffectHDR::BloomSource_2Pass( RCameraSceneNode* pCamera, RTexture* pScrBuffer)
{
	// Get the sample offsets used within the pixel shader
	RTextureInfo Info = m_pTexBrightPass->GetInfo();

	// The gaussian blur smooths out rough edges to avoid aliasing effects
	// when the star effect is run
	RTechnique Technique = m_pHDRShader->GetTechnique("QuickGaussBlur");

	REngine::GetDevice().SetRenderTarget( 0, m_apTexBloom[0] );
	REngine::GetDevice().SetTexture( 0, m_pTexBrightPass );
	REngine::GetDevice().SetTextureFilter( 0, RTF_LINEAR );

	UINT uiPassCount, uiPass;
	Technique.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		switch( uiPass)
		{
		case 0:
			REngine::GetDevice().SetRenderTarget( 0, m_apTexBloom[0]);
			REngine::GetDevice().SetTexture( 0, pScrBuffer);
			break;
		case 1:
			REngine::GetDevice().SetRenderTarget( 0, m_pTexBloomSource);
			REngine::GetDevice().SetTexture( 0, m_apTexBloom[0]);
			break;
		}
		Technique.BeginPass(uiPass);
		DrawFullScreenQuad( 0,0, m_pTexBloomSource->GetInfo().nTextureWidth, m_pTexBloomSource->GetInfo().nTextureHeight);
		Technique.EndPass();
	}
	Technique.End();

	return m_pTexBloomSource;
}


//-----------------------------------------------------------------------------
// Name: RenderBloom()
// Desc: Render the blooming effect
//-----------------------------------------------------------------------------
RTexture* RPostEffectHDR::RenderBloom( RCameraSceneNode* pCamera, RTexture* pScrBuffer)
{
	UINT uiPassCount, uiPass;
	int i = 0;

	RVector2 vSampleOffsets[MAX_SAMPLES];
	FLOAT afSampleOffsets[MAX_SAMPLES];
	RVector4 vSampleWeights[MAX_SAMPLES];

	// Clear the bloom texture
	REngine::GetDevice().ColorFill( m_apTexBloom[0], D3DCOLOR_ARGB( 0, 0, 0, 0 ) );

	float fBloomBlurDeviation = m_fBloomBlurDeviation;
	float fBloomBlurMultiplier = m_fBloomBlurMultiplier;
	REnvironmentSet* pEnvironmentSet = REngine::GetSceneManager().GetCurrentRootSceneNode(false)->GetEnvironmentSet();
	if( pEnvironmentSet )
	{
		fBloomBlurDeviation = pEnvironmentSet->GetHDRBloomBlurDeviation();
		fBloomBlurMultiplier = pEnvironmentSet->GetHDRBloomBlurMultiplier();
	}

	RTextureInfo Info = m_pTexBloomSource->GetInfo();

	RTechnique Technique = m_pHDRShader->GetTechnique("GaussBlur5x5");
	GetSampleOffsets_GaussBlur5x5( Info.nTextureWidth, Info.nTextureHeight, vSampleOffsets, vSampleWeights, 1.0f );

	m_pHDRShader->m_pEffect->SetValue("g_avSampleOffsets", vSampleOffsets, sizeof( vSampleOffsets ) );
	m_pHDRShader->m_pEffect->SetValue("g_avSampleWeights", vSampleWeights, sizeof( vSampleWeights ) );

	REngine::GetDevice().SetRenderTarget( 0, m_apTexBloom[2] );
	REngine::GetDevice().SetTexture( 0, pScrBuffer );
	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT );

	Technique.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);
		DrawFullScreenQuad( 0,0, m_apTexBloom[2]->GetInfo().nTextureWidth, m_apTexBloom[2]->GetInfo().nTextureHeight);
		Technique.EndPass();
	}
	Technique.End();

	Info = m_apTexBloom[2]->GetInfo();

	GetSampleOffsets_Bloom( Info.nTextureWidth, afSampleOffsets, vSampleWeights, fBloomBlurDeviation, fBloomBlurMultiplier );
	for( i = 0; i < MAX_SAMPLES; i++ ) {
		vSampleOffsets[i] = RVector2( afSampleOffsets[i], 0.0f );
	}

	Technique = m_pHDRShader->GetTechnique("Bloom");
	m_pHDRShader->m_pEffect->SetValue("g_avSampleOffsets", vSampleOffsets, sizeof( vSampleOffsets ) );
	m_pHDRShader->m_pEffect->SetValue("g_avSampleWeights", vSampleWeights, sizeof( vSampleWeights ) );

	REngine::GetDevice().SetRenderTarget( 0, m_apTexBloom[1] );
	REngine::GetDevice().SetTexture( 0, m_apTexBloom[2] );
	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT );

	Technique.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);
		DrawFullScreenQuad( 0,0, m_apTexBloom[1]->GetInfo().nTextureWidth, m_apTexBloom[1]->GetInfo().nTextureHeight);
		Technique.EndPass();
	}
	Technique.End();

	Info = m_apTexBloom[1]->GetInfo();
	GetSampleOffsets_Bloom( Info.nTextureHeight, afSampleOffsets, vSampleWeights, fBloomBlurDeviation, fBloomBlurMultiplier );
	for( i = 0; i < MAX_SAMPLES; i++ ) {
		vSampleOffsets[i] = RVector2( 0.0f, afSampleOffsets[i] );
	}

	Info = m_apTexBloom[1]->GetInfo();

	Technique = m_pHDRShader->GetTechnique("Bloom");
	m_pHDRShader->m_pEffect->SetValue("g_avSampleOffsets", vSampleOffsets, sizeof( vSampleOffsets ) );
	m_pHDRShader->m_pEffect->SetValue("g_avSampleWeights", vSampleWeights, sizeof( vSampleWeights ) );

	REngine::GetDevice().SetRenderTarget( 0, m_apTexBloom[0] );
	REngine::GetDevice().SetTexture( 0, m_apTexBloom[1] );
	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT );

	Technique.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);
		DrawFullScreenQuad( 0,0, m_apTexBloom[0]->GetInfo().nTextureWidth, m_apTexBloom[0]->GetInfo().nTextureHeight);
		Technique.EndPass();
	}
	Technique.End();

	return m_apTexBloom[0];
}



//-----------------------------------------------------------------------------
// Name: RenderBloom()
// Desc: Render the blooming effect
//-----------------------------------------------------------------------------
RTexture* RPostEffectHDR::RenderBloom_2Pass( RCameraSceneNode* pCamera, RTexture* pScrBuffer)
{
	UINT uiPassCount, uiPass;
	int i = 0;

	RTechnique Technique = m_pHDRShader->GetTechnique("QuickGaussBlur");
	REngine::GetDevice().SetRenderTarget( 0, m_apTexBloom[2] );
	REngine::GetDevice().SetTexture( 0, m_pTexBloomSource );
	REngine::GetDevice().SetTextureFilter( 0, RTF_LINEAR );

	Technique.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		switch( uiPass)
		{
		case 0:
			REngine::GetDevice().SetRenderTarget( 0, m_apTexBloom[1]);
			REngine::GetDevice().SetTexture( 0, pScrBuffer);
			break;
		case 1:
			REngine::GetDevice().SetRenderTarget( 0, m_apTexBloom[2]);
			REngine::GetDevice().SetTexture( 0, m_apTexBloom[1]);
			break;
		}
		Technique.BeginPass(uiPass);
		DrawFullScreenQuad( 0,0, m_apTexBloom[2]->GetInfo().nTextureWidth, m_apTexBloom[2]->GetInfo().nTextureHeight);
		Technique.EndPass();
	}
	Technique.End();

	RVector2 vSampleOffsets[MAX_SAMPLES];
	FLOAT afSampleOffsets[MAX_SAMPLES];
	RVector4 vSampleWeights[MAX_SAMPLES];

	// Clear the bloom texture
	REngine::GetDevice().ColorFill( m_apTexBloom[0], D3DCOLOR_ARGB( 0, 0, 0, 0 ) );

	float fBloomBlurDeviation = m_fBloomBlurDeviation;
	float fBloomBlurMultiplier = m_fBloomBlurMultiplier;
	REnvironmentSet* pEnvironmentSet = REngine::GetSceneManager().GetCurrentRootSceneNode(false)->GetEnvironmentSet();
	if( pEnvironmentSet )
	{
		fBloomBlurDeviation = pEnvironmentSet->GetHDRBloomBlurDeviation();
		fBloomBlurMultiplier = pEnvironmentSet->GetHDRBloomBlurMultiplier();
	}

	RTextureInfo Info = m_apTexBloom[2]->GetInfo();

	GetSampleOffsets_Bloom( Info.nTextureWidth, afSampleOffsets, vSampleWeights, fBloomBlurDeviation, fBloomBlurMultiplier );
	for( i = 0; i < MAX_SAMPLES; i++ ) {
		vSampleOffsets[i] = RVector2( afSampleOffsets[i], 0.0f );
	}

	Technique = m_pHDRShader->GetTechnique("Bloom");
	m_pHDRShader->m_pEffect->SetValue("g_avSampleOffsets", vSampleOffsets, sizeof( vSampleOffsets ) );
	m_pHDRShader->m_pEffect->SetValue("g_avSampleWeights", vSampleWeights, sizeof( vSampleWeights ) );

	REngine::GetDevice().SetRenderTarget( 0, m_apTexBloom[1] );
	REngine::GetDevice().SetTexture( 0, m_apTexBloom[2] );
	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT );

	Technique.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);
		DrawFullScreenQuad( 0,0, m_apTexBloom[1]->GetInfo().nTextureWidth, m_apTexBloom[1]->GetInfo().nTextureHeight);
		Technique.EndPass();
	}
	Technique.End();

	Info = m_apTexBloom[1]->GetInfo();
	GetSampleOffsets_Bloom( Info.nTextureHeight, afSampleOffsets, vSampleWeights, fBloomBlurDeviation, fBloomBlurMultiplier );
	for( i = 0; i < MAX_SAMPLES; i++ ) {
		vSampleOffsets[i] = RVector2( 0.0f, afSampleOffsets[i] );
	}

	Info = m_apTexBloom[1]->GetInfo();

	Technique = m_pHDRShader->GetTechnique("Bloom");
	m_pHDRShader->m_pEffect->SetValue("g_avSampleOffsets", vSampleOffsets, sizeof( vSampleOffsets ) );
	m_pHDRShader->m_pEffect->SetValue("g_avSampleWeights", vSampleWeights, sizeof( vSampleWeights ) );

	REngine::GetDevice().SetRenderTarget( 0, m_apTexBloom[0] );
	REngine::GetDevice().SetTexture( 0, m_apTexBloom[1] );
	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT );

	Technique.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);
		DrawFullScreenQuad( 0,0, m_apTexBloom[0]->GetInfo().nTextureWidth, m_apTexBloom[0]->GetInfo().nTextureHeight);
		Technique.EndPass();
	}
	Technique.End();

	return m_apTexBloom[0];
}


// HDR Scene을 구성한다
void RPostEffectHDR::Render( RCameraSceneNode* pCamera, RTexture* pColorBuffer, RTexture* pNormalBuffer, RTexture* pDepthBuffer)
{
	RPFC_THISFUNC;
	RTexture* pBeforeRenderTarget0 = REngine::GetDevice().GetRenderTarget(0);	// Low dynamic range surface for final output
	RTexture* pBeforeRenderTarget1 = REngine::GetDevice().SetRenderTarget(1, NULL);	// Off RT1

	REngine::GetDevice().SetCullMode(RCULL_NONE);
	REngine::GetDevice().SetDepthEnable(false,false);

	REngine::GetDevice().SetTexture(0, NULL);
	REngine::GetDevice().SetTexture(1, NULL);
	REngine::GetDevice().SetTexture(2, NULL);
	REngine::GetDevice().SetTexture(3, NULL);

	REngine::GetDevice().SetTextureAddressClamp(0);
	REngine::GetDevice().SetTextureAddressClamp(1);
	REngine::GetDevice().SetTextureAddressClamp(2);
	REngine::GetDevice().SetTextureAddressClamp(3);

	RTexture* pTargetBuffer = pColorBuffer;

	// Create a scaled copy of the scene
	// Now that luminance information has been gathered, the scene can be bright-pass filtered
	// to remove everything except bright lights and reflections.
	if( m_bScaleAndBright1Pass)
	{
		pTargetBuffer = SceneScaleAndBrightPass( pCamera, pTargetBuffer);
	}
	else
	{
		pTargetBuffer = SceneScale( pCamera, pTargetBuffer);
		pTargetBuffer = BrightPass( pCamera, pTargetBuffer);
	}

	if( m_bGaussian2Pass)
	{
		pTargetBuffer = BloomSource_2Pass( pCamera, pTargetBuffer);
		pTargetBuffer = RenderBloom_2Pass( pCamera, pTargetBuffer);
	}
	else
	{
		pTargetBuffer = BloomSource( pCamera, pTargetBuffer);
		pTargetBuffer = RenderBloom( pCamera, pTargetBuffer);
	}
	

	REngine::GetDevice().SetDepthEnable( true, true);
	REngine::GetDevice().SetRenderTarget( 0, pBeforeRenderTarget0);
	REngine::GetDevice().SetRenderTarget( 1, pBeforeRenderTarget1);


	// ---------------------- flip -------------------------------------------------
	REngine::GetDevice().SetDepthEnable(false,false);

	RTechnique Technique = m_pHDRShader->GetTechnique("FinalScenePass");

	REngine::GetDevice().SetTexture(0, pColorBuffer);
	REngine::GetDevice().SetTexture(1, pTargetBuffer);
	REngine::GetDevice().SetTexture(2, NULL);
	REngine::GetDevice().SetTexture(3, NULL);

	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT );
	REngine::GetDevice().SetTextureFilter( 1, RTF_LINEAR );
	REngine::GetDevice().SetTextureFilter( 2, RTF_LINEAR );
	REngine::GetDevice().SetTextureFilter( 3, RTF_POINT );

	UINT uiPassCount, uiPass;
	Technique.Begin(&uiPassCount, TECH_PRESERVE_NOT);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);
		DrawFullScreenQuad( 0,0, m_dwWidth, m_dwHeight);
		Technique.EndPass();
	}
	Technique.End();

	REngine::GetDevice().SetTexture( 0, NULL );
	REngine::GetDevice().SetTexture( 1, NULL );
	REngine::GetDevice().SetTexture( 2, NULL );
	REngine::GetDevice().SetTexture( 3, NULL );
	REngine::GetDevice().SetTextureFilter( 0, RTF_LINEAR );
	REngine::GetDevice().SetTextureFilter( 1, RTF_LINEAR );
	REngine::GetDevice().SetTextureFilter( 2, RTF_LINEAR );
	REngine::GetDevice().SetTextureFilter( 3, RTF_LINEAR );

	REngine::GetDevice().SetTextureAddressWrap(0);
	REngine::GetDevice().SetTextureAddressWrap(1);
	REngine::GetDevice().SetTextureAddressWrap(2);
	REngine::GetDevice().SetTextureAddressWrap(3);

	if(m_bDebugRT)
	{
		RRenderHelper::SetRenderState();
		RRenderHelper::RenderTexture(0, 250-250, 200, 150, pColorBuffer);
		RRenderHelper::RenderTexture(0, 410-250, 200, 150, m_pTexBrightPass);///m_apTexToneMap[NUM_TONEMAP_TEXTURES - 1]);
		RRenderHelper::RenderTexture(0, 570-250, 200, 150, m_apTexBloom[0]);
		RRenderHelper::EndRenderState();
	}

	REngine::GetDevice().SetDepthEnable(true,true);
	REngine::GetDevice().SetCullMode(RCULL_CW);
}

}