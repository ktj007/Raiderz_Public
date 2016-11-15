#include "StdAfx.h"

#include "RCameraHDRController.h"
#include "RCameraSceneNode.h"
#include "RVType.h"
#include "RRenderHelper.h"

namespace rs3 {

class HDRControllerListener : public RListener
{
	RCameraHDRController* m_pHDRController;
public:
	HDRControllerListener(RCameraHDRController* pHDRController) : m_pHDRController(pHDRController) {}

	virtual void Update() override
	{
		m_pHDRController->Destroy();
		m_pHDRController->Create();
	}
};


RCameraHDRController::RCameraHDRController(RCameraSceneNode* pCameraSceneNode) 
	: RCameraSceneNodeController( RCCT_POSTRENDER, "0_HDR", true )
	, m_pCameraSceneNode(pCameraSceneNode)
	, m_pHDRBuffer(NULL)
	, m_pTexOcclusionQuery(NULL)
	, m_pTexSceneScaled(NULL)
	, m_pTexBrightPass(NULL)
	, m_pTexStarSource(NULL)
	, m_pTexBloomSource(NULL)
	, m_pHDRShader(NULL)
	, m_bAdaptationInvalid(true)
{
	for( int i = 0; i < NUM_BLOOM_TEXTURES; ++i )
		m_apTexBloom[i] = NULL;

	for( int i = 0; i < NUM_STAR_TEXTURES; ++i )
		m_apTexStar[i] = NULL;

	m_eGlareType = ( EGLARELIBTYPE )GLT_FILTER_CROSSSCREEN;
	m_GlareDef.Initialize( m_eGlareType );

	m_bHDR					= true;
	m_bToneMap				= true;
	//	m_bBlueShift			= false;//true;
	m_bDebugRT				= false;
	m_bBloom				= true;
	m_fBloomScale			= 15.0f;
	m_bGlare				= false;//true;
	m_fGlareScale			= 5.0f;

	m_nCurrentFrame 		= 0;
	float fBucketWidth = MAX_LUMINANCE / NUM_MAX_BUCKETS;
	float fValue = 0.0f;
	for( int i = 0; i < NUM_MAX_BUCKETS; ++i ) {
		m_arrBucket[i].m_nCountPixels	= 0;
		m_arrBucket[i].m_nFrame			= i;
		m_arrBucket[i].m_vMinMax.x		= fValue;
		fValue += fBucketWidth;
		m_arrBucket[i].m_vMinMax.y		= fValue;
	}
	m_arrBucket[NUM_MAX_BUCKETS-1].m_vMinMax.y = 10000.0f;

	m_fTonemapMin = 0.0f;
	m_fTonemapMax = MAX_LUMINANCE;

	m_fDarkBarrierRatio		= 0.00f;
	m_fBrightBarrierRatio	= 0.95f;

	m_fLimitMin = 0.20f * MAX_LUMINANCE;
	m_fLimitMax = 0.35f * MAX_LUMINANCE;

	m_bFullFrame = false;
	m_fAdaptationSpeed = 20.f;

	// event 구독
	m_pEventListener = new HDRControllerListener(this);
	REngine::GetEventBroadcaster(RE_DEVICE_RESTORE)->AddListener( m_pEventListener );

	m_strConfigPath = "Data/system/Default.hcf";
	LoadConfig();
}

RCameraHDRController::~RCameraHDRController()
{
	// event 해제
	REngine::GetEventBroadcaster(RE_DEVICE_RESTORE)->RemoveListener( m_pEventListener );

	SAFE_DELETE(m_pEventListener);

	Destroy();
}

void RCameraHDRController::SetConfigPath(string strPath)
{
	m_strConfigPath = strPath;
	LoadConfig();
}

void RCameraHDRController::LoadConfig()
{
	MXml kXml;
	if ( !kXml.LoadFile( m_strConfigPath.c_str() ) )
		return ;

	MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("HDR_CONFIG");
	if(pRootElement == NULL)
		return ;

	// 무조건 Default hcf 파일을 읽어 들인다.
	LoadToXML(*pRootElement);	
}

bool RCameraHDRController::Create()
{
	RFORMAT eHDRBufferFormat = RFMT_A16B16G16R16F;
	RFORMAT eDepthInfoBufferFormat = RFMT_G32R32F;
	if( false == REngine::GetDevice().QueryFeature(RQF_A16B16G16R16F) 
		||	false == REngine::GetDevice().QueryFeature(RQF_G32R32F))
	{
		eHDRBufferFormat = RFMT_A8R8G8B8;
		eDepthInfoBufferFormat = RFMT_G16R16F;
	}

	int nScreenWidth	= REngine::GetDevice().GetScreenWidth();
	int nScreenHeight	= REngine::GetDevice().GetScreenHeight();

	m_nCropWidth	= nScreenWidth - nScreenWidth % 8;
	m_nCropHeight	= nScreenHeight - nScreenHeight % 8;

	if ( false == REngine::GetDevice().QueryFeature(RQF_PS20) )
		return false;

	if ( false == REngine::GetDevice().QueryFeature(RQF_PS30) )
		return false;

	m_pHDRBuffer = REngine::GetDevice().CreateRenderTargetTexture(nScreenWidth, nScreenHeight, eHDRBufferFormat);
	if(R_NONE==m_pHDRBuffer) return false;

	// Scaled version of the HDR scene texture
	m_pTexSceneScaled = REngine::GetDevice().CreateRenderTargetTexture(m_nCropWidth / 4, m_nCropHeight / 4, eHDRBufferFormat);
	if(R_NONE==m_pTexSceneScaled) return false;
	// bright-pass filter texture.  // Texture has a black border of single texel thickness to fake border 
	m_pTexBrightPass = REngine::GetDevice().CreateRenderTargetTexture(m_nCropWidth / 4 + 2, m_nCropHeight / 4 + 2, RFMT_A8R8G8B8);
	if(R_NONE==m_pTexBrightPass) return false;
	// source for the star effect
	m_pTexStarSource = REngine::GetDevice().CreateRenderTargetTexture(m_nCropWidth / 4 + 2, m_nCropHeight / 4 + 2, RFMT_A8R8G8B8);
	if(R_NONE==m_pTexStarSource) return false;
	// source for the bloom effect
	m_pTexBloomSource = REngine::GetDevice().CreateRenderTargetTexture(m_nCropWidth / 8 + 2, m_nCropHeight / 8 + 2, RFMT_A8R8G8B8);
	if(R_NONE==m_pTexBloomSource) return false;
	// use to occlusion query's texture
	m_pTexOcclusionQuery = REngine::GetDevice().CreateRenderTargetTexture(m_nCropWidth / 4, m_nCropHeight / 4, eHDRBufferFormat);
	if(R_NONE==m_pTexOcclusionQuery) return false;

	// Create the temporary blooming effect textures
	for( int i = 1; i < NUM_BLOOM_TEXTURES; ++i ) {
		m_apTexBloom[i] = REngine::GetDevice().CreateRenderTargetTexture(m_nCropWidth / 8 + 2, m_nCropHeight / 8 + 2, RFMT_A8R8G8B8);
		if(R_NONE==m_apTexBloom[i]) return false;
	}
	// Create the star effect textures
	for( int i = 0; i < NUM_STAR_TEXTURES; ++i ) {
		m_apTexStar[i] = REngine::GetDevice().CreateRenderTargetTexture(m_nCropWidth / 4, m_nCropHeight / 4, eHDRBufferFormat);
		if(R_NONE==m_apTexStar[i]) return false;
	}
	// Create the final blooming effect texture
	m_apTexBloom[0] = REngine::GetDevice().CreateRenderTargetTexture(m_nCropWidth / 8, m_nCropHeight / 8, RFMT_A8R8G8B8);
	if(R_NONE==m_apTexBloom[0]) return false;
	//////////////////////////////////////////////////////////////////////////

	// Textures with borders must be cleared since scissor rect testing will
	// be used to avoid rendering on top of the border
	REngine::GetDevice().ColorFill( m_pHDRBuffer );		// 이건 왜 안하고있었지
	REngine::GetDevice().ColorFill( m_pTexSceneScaled );
	REngine::GetDevice().ColorFill( m_pTexOcclusionQuery );
	REngine::GetDevice().ColorFill( m_pTexBloomSource );
	REngine::GetDevice().ColorFill( m_pTexBrightPass );
	REngine::GetDevice().ColorFill( m_pTexStarSource );
	for( int i = 0; i < NUM_BLOOM_TEXTURES; ++i ) {
		REngine::GetDevice().ColorFill( m_apTexBloom[i] );
	}

	for( int i = 0; i < NUM_STAR_TEXTURES; ++i ) {
		REngine::GetDevice().ColorFill( m_apTexStar[i] );	
	}

	m_pHDRShader = REngine::GetDevice().CreateShaderFX("HDRLight.fx");

	return true;
}

void RCameraHDRController::Destroy()
{
	if( m_pHDRBuffer)
		REngine::GetDevicePtr()->DeleteTexture( m_pHDRBuffer);

	if( m_pTexOcclusionQuery)
		REngine::GetDevicePtr()->DeleteTexture( m_pTexOcclusionQuery);

	if( m_pTexSceneScaled)
		REngine::GetDevicePtr()->DeleteTexture( m_pTexSceneScaled);

	if( m_pTexBrightPass)
		REngine::GetDevicePtr()->DeleteTexture( m_pTexBrightPass);

	if( m_pTexStarSource)
		REngine::GetDevicePtr()->DeleteTexture( m_pTexStarSource);

	if( m_pTexBloomSource)
		REngine::GetDevicePtr()->DeleteTexture( m_pTexBloomSource);

	for( int i = 0; i < NUM_BLOOM_TEXTURES; i++ ) {
		if( m_apTexBloom[i])
			REngine::GetDevicePtr()->DeleteTexture( m_apTexBloom[i] );
	}

	for( int i = 0; i < NUM_STAR_TEXTURES; i++ ) {
		if( m_apTexStar[i])
			REngine::GetDevicePtr()->DeleteTexture( m_apTexStar[i] );
	}

	if (m_pHDRShader)
		REngine::GetDevice().DeleteShaderFX(m_pHDRShader);
}

void RCameraHDRController::Restore()
{
	REngine::GetDevice().ColorFill( m_pHDRBuffer);
	REngine::GetDevice().ColorFill( m_pTexSceneScaled );
	REngine::GetDevice().ColorFill( m_pTexOcclusionQuery );
	REngine::GetDevice().ColorFill( m_pTexBloomSource );
	REngine::GetDevice().ColorFill( m_pTexBrightPass );
	REngine::GetDevice().ColorFill( m_pTexStarSource );
	for( int i = 0; i < NUM_BLOOM_TEXTURES; ++i ) {
		REngine::GetDevice().ColorFill( m_apTexBloom[i] );
	}
	for( int i = 0; i < NUM_STAR_TEXTURES; ++i ) {
		REngine::GetDevice().ColorFill( m_apTexStar[i] );	
	}
}

//-----------------------------------------------------------------------------
// Desc: Draw a properly aligned quad covering the entire render target
//-----------------------------------------------------------------------------
void RCameraHDRController::DrawFullScreenQuad(float fLeftU, float fTopV, float fRightU, float fBottomV, bool bSmallSize)
{
	REngine::GetDevice().SetFillMode(RFILL_SOLID);

	// Ensure that we're directly mapping texels to pixels by offset by 0.5
	// For more info see the doc page titled "Directly Mapping Texels to Pixels"
	float fWidth5, fHeight5;

	RTexture* pRT = REngine::GetDevice().GetRenderTarget( 0 );
	// 다른 후처리 이펙트가 생기면서 RT가 변동된다. 스크린 사이즈에 적용 하는 것에 대한 플래그를 인자로 받게 변경 - 090617, OZ
	//if(pRT == RT_FRAMEBUFFER)
	if(!bSmallSize)
	{
		fWidth5		= REngine::GetDevice().GetScreenWidth();
		fHeight5	= REngine::GetDevice().GetScreenHeight();
	}
	else
	{
		fWidth5		= pRT->GetInfo().nTextureWidth - 0.5f;
		fHeight5	= pRT->GetInfo().nTextureHeight - 0.5f;
	}

	// Draw the quad
	RVt_pos_tex_rhw vQuad[4];
	vQuad[0].vPos	= RVector4( -0.5f, -0.5f, 0.0f, 1.0f );
	vQuad[0].u		= fLeftU;
	vQuad[0].v		= fTopV;

	vQuad[1].vPos	= RVector4( -0.5f, fHeight5, 0.0f, 1.0f );
	vQuad[1].u		= fLeftU;
	vQuad[1].v		= fBottomV;

	vQuad[2].vPos	= RVector4( fWidth5, -0.5f, 0.0f, 1.0f );
	vQuad[2].u		= fRightU;
	vQuad[2].v		= fTopV;

	vQuad[3].vPos	= RVector4( fWidth5, fHeight5, 0.0f, 1.0f );
	vQuad[3].u		= fRightU;
	vQuad[3].v		= fBottomV;

	REngine::GetDevice().SetDepthEnable(false, false);
	REngine::GetDevice().SetFvF(RVt_pos_tex_rhw::FVF);
	REngine::GetDevice().DrawPrimitiveUP( RPT_TRIANGLESTRIP, 2, vQuad, sizeof( RVt_pos_tex_rhw ) );
	REngine::GetDevice().SetDepthEnable(true, true);
}

//-----------------------------------------------------------------------------
// Desc: Get the texture coordinates to use when rendering into the destination
//       texture, given the source and destination rectangles
//-----------------------------------------------------------------------------
bool RCameraHDRController::GetTextureCoords( RTexture* pTexSrc, RECT* pRectSrc, RTexture* pTexDest, RECT* pRectDest, RVector4* pCoords )
{
	float tU, tV;

	// Validate arguments
	if( pTexSrc == NULL || pTexDest == NULL || pCoords == NULL )
		return false;

	// Start with a default mapping of the complete source surface to complete 
	// destination surface
	pCoords->x = 0.0f;
	pCoords->y = 0.0f;
	pCoords->z = 1.0f;
	pCoords->w = 1.0f;

	// If not using the complete source surface, adjust the coordinates
	if( pRectSrc != NULL )
	{
		// Get destination texture description
		RTextureInfo Info = pTexSrc->GetInfo();

		// These delta values are the distance between source texel centers in 
		// texture address space
		tU = 1.0f / Info.nTextureWidth;
		tV = 1.0f / Info.nTextureHeight;

		pCoords->x += pRectSrc->left * tU;
		pCoords->y += pRectSrc->top * tV;
		pCoords->z -= ( Info.nTextureWidth - pRectSrc->right ) * tU;
		pCoords->w -= ( Info.nTextureHeight - pRectSrc->bottom ) * tV;
	}

	// If not drawing to the complete destination surface, adjust the coordinates
	if( pRectDest != NULL )
	{
		// Get source texture description
		RTextureInfo Info = pTexDest->GetInfo();

		// These delta values are the distance between source texel centers in 
		// texture address space
		tU = 1.0f / Info.nTextureWidth;
		tV = 1.0f / Info.nTextureHeight;

		pCoords->x -= pRectDest->left * tU;
		pCoords->y -= pRectDest->top * tV;
		pCoords->z += ( Info.nTextureWidth - pRectDest->right ) * tU;
		pCoords->w += ( Info.nTextureHeight - pRectDest->bottom ) * tV;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Desc: Get the texture coordinate offsets to be used inside the DownScale2x2
//       pixel shader.
//-----------------------------------------------------------------------------
bool RCameraHDRController::GetSampleOffsets_DownScale2x2( int nWidth, int nHeight, RVector2 vSampleOffsets[] )
{
	float tU = 1.0f / nWidth;
	float tV = 1.0f / nHeight;

	// Sample from the 4 surrounding points. Since the center point will be in
	// the exact center of 4 texels, a 0.5f offset is needed to specify a texel center.
	int index = 0;
	for( int y = 0; y < 2; y++ )
	{
		for( int x = 0; x < 2; x++ )
		{
			vSampleOffsets[ index ].x = ( x - 0.5f ) * tU;
			vSampleOffsets[ index ].y = ( y - 0.5f ) * tV;

			index++;
		}
	}

	return true;
}


//-----------------------------------------------------------------------------
// Desc: Get the texture coordinate offsets to be used inside the DownScale4x4
//       pixel shader.
//-----------------------------------------------------------------------------
bool RCameraHDRController::GetSampleOffsets_DownScale4x4( int nWidth, int nHeight, RVector2 vSampleOffsets[] )
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
// Desc: Helper function for GetSampleOffsets function to compute the 
//       2 parameter Gaussian distrubution using the given standard deviation
//       rho
//-----------------------------------------------------------------------------
float RCameraHDRController::GaussianDistribution( float x, float y, float rho )
{
	float g = 1.0f / sqrtf( 2.0f * MMath::PI * rho * rho );
	g *= expf( -( x * x + y * y ) / ( 2 * rho * rho ) );

	return g;
}

//-----------------------------------------------------------------------------
// Desc: Get the texture coordinate offsets to be used inside the GaussBlur5x5
//       pixel shader.
//-----------------------------------------------------------------------------
bool RCameraHDRController::GetSampleOffsets_GaussBlur5x5( int nTexWidth, int nTexHeight, RVector2* vTexCoordOffset, RVector4* vSampleWeight, FLOAT fMultiplier )
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

bool RCameraHDRController::GetSampleOffsets_Bloom(int nTexWidth, float afTexCoordOffset[15], RVector4* vColorWeight, float fDeviation, FLOAT fMultiplier )
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

bool RCameraHDRController::GetSampleOffsets_Star( int nTexWidth, float afTexCoordOffset[15], RVector4* vColorWeight, float fDeviation )
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

//-----------------------------------------------------------------------------
// Desc: Scale down g_pTexScene by 1/4 x 1/4 and place the result in m_pTexSceneScaled
//-----------------------------------------------------------------------------
bool RCameraHDRController::Scene_To_SceneScaled()
{
	RVector2 vSampleOffsets[MAX_SAMPLES];

	// Create a 1/4 x 1/4 scale copy of the HDR texture. Since bloom textures
	// are 1/8 x 1/8 scale, border texels of the HDR texture will be discarded 
	// to keep the dimensions evenly divisible by 8; this allows for precise 
	// control over sampling inside pixel shaders.
	RTechnique Technique = m_pHDRShader->GetTechnique("DownScale4x4");

	// Place the rectangle in the center of the back buffer surface
	RECT rectSrc;
	rectSrc.left	= ( REngine::GetDevice().GetScreenWidth() - m_nCropWidth ) / 2;
	rectSrc.top		= ( REngine::GetDevice().GetScreenHeight() - m_nCropHeight ) / 2;
	rectSrc.right	= rectSrc.left + m_nCropWidth;
	rectSrc.bottom	= rectSrc.top + m_nCropHeight;

	// Get the texture coordinates for the render target
	RVector4 vCoords;
	GetTextureCoords( m_pHDRBuffer, &rectSrc, m_pTexSceneScaled, NULL, &vCoords );

	// Get the sample offsets used within the pixel shader
	GetSampleOffsets_DownScale4x4(  REngine::GetDevice().GetScreenWidth(), REngine::GetDevice().GetScreenHeight(), vSampleOffsets );
	m_pHDRShader->m_pEffect->SetValue("g_avSampleOffsets", vSampleOffsets, sizeof( vSampleOffsets ) );

	REngine::GetDevice().SetRenderTarget( 0, m_pTexSceneScaled );
	REngine::GetDevice().SetTexture(0, m_pHDRBuffer);
	REngine::GetDevice().SetTextureFilter(0, RTF_POINT);
	REngine::GetDevice().SetTextureAddressClamp(0);

	UINT uiPassCount, uiPass;
	Technique.Begin(&uiPassCount, 0);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);
		DrawFullScreenQuad(vCoords);
		Technique.EndPass();
	}
	Technique.End();

	return true;
}

//-----------------------------------------------------------------------------
// Desc: 
//-----------------------------------------------------------------------------
bool RCameraHDRController::OcclusionQueryTonemap()
{
	UINT uiPass, uiPassCount;

	RVector2 vSampleOffsets[MAX_SAMPLES];
	RVector4 vSampleWeights[MAX_SAMPLES];

	// Get the rectangle describing the sampled portion of the source texture.
	// Decrease the rectangle to adjust for the single pixel black border.
	RECT rectSrc;
	RTextureInfo Info = m_pTexSceneScaled->GetInfo();
	rectSrc.left	= 0;
	rectSrc.top		= 0;
	rectSrc.right	= Info.nTextureWidth;
	rectSrc.bottom	= Info.nTextureHeight;

	InflateRect( &rectSrc, -1, -1 );

	// Get the destination rectangle.
	// Decrease the rectangle to adjust for the single pixel black border.
	RECT rectDest;
	Info = m_pTexOcclusionQuery->GetInfo();
	rectDest.left	= 0;
	rectDest.top	= 0;
	rectDest.right	= Info.nTextureWidth;
	rectDest.bottom	= Info.nTextureHeight;
	InflateRect( &rectDest, -1, -1 );

	// Get the correct texture coordinates to apply to the rendered quad in order 
	// to sample from the source rectangle and render into the destination rectangle
	RVector4 vCoords;
	GetTextureCoords( m_pTexSceneScaled, &rectSrc, m_pTexOcclusionQuery, &rectDest, &vCoords );

	RTechnique Technique = m_pHDRShader->GetTechnique("OcclusionQuery");
	m_pHDRShader->m_pEffect->SetValue("g_avSampleOffsets", vSampleOffsets, sizeof( vSampleOffsets ) );

	REngine::GetDevice().SetRenderTarget( 0, m_pTexOcclusionQuery );
	REngine::GetDevice().SetTexture( 0, m_pTexSceneScaled );
	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT );

	if( m_bFullFrame )
	{
		// Full Frame
		for( int i = 0; i < NUM_MAX_BUCKETS; ++i )
		{
			m_pHDRShader->GetConstant("g_fBucketMin").SetFloat(m_arrBucket[i].m_vMinMax.x);
			m_pHDRShader->GetConstant("g_fBucketMax").SetFloat(m_arrBucket[i].m_vMinMax.y);

			// Occlusion Query
			REngine::GetDevice().Issue(RISSUE_BEGIN);

			Technique.Begin(&uiPassCount, 0);
			for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
			{
				Technique.BeginPass(uiPass);

				DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f );

				Technique.EndPass();
			}
			Technique.End();

			REngine::GetDevice().Issue(RISSUE_END);

			// Loop until the data becomes available
			DWORD dwPixelsVisible = 0;
			while (REngine::GetDevice().GetQueryData((void *)&dwPixelsVisible, sizeof(DWORD), D3DGETDATA_FLUSH) == false) {}

			m_arrBucket[i].m_nCountPixels = dwPixelsVisible;
		}
	}
	else
	{
		// Per 16 Frame
		m_pHDRShader->GetConstant("g_fBucketMin").SetFloat(m_arrBucket[m_nCurrentFrame].m_vMinMax.x);
		m_pHDRShader->GetConstant("g_fBucketMax").SetFloat(m_arrBucket[m_nCurrentFrame].m_vMinMax.y);

		// Occlusion Query
		REngine::GetDevice().Issue(RISSUE_BEGIN);

		Technique.Begin(&uiPassCount, 0);
		for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
		{
			Technique.BeginPass(uiPass);

			DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f );

			Technique.EndPass();
		}
		Technique.End();

		REngine::GetDevice().Issue(RISSUE_END);

		// Loop until the data becomes available
		DWORD dwPixelsVisible = 0;
		while (REngine::GetDevice().GetQueryData((void *)&dwPixelsVisible, sizeof(DWORD), D3DGETDATA_FLUSH) == false) {}

		m_arrBucket[m_nCurrentFrame].m_nCountPixels = dwPixelsVisible;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Desc: Improved Tone map
//-----------------------------------------------------------------------------
bool RCameraHDRController::CalculateTonemap()
{
	float fBucketWidth = MAX_LUMINANCE / NUM_MAX_BUCKETS;

	float fMaxPixelCount= 0;	// 전체 픽셀 개수 ( 모든 히스토그램 버켓의 합 )
	float fSumPixel		= 0;	// 중간 계산용으로 사용될 누적 픽셀 개수

	//RBucket	arrCalcBucket[NUM_MAX_BUCKETS];	// 계산용으로 사용될 버켓 데이터
	//for( int i = 0; i < NUM_MAX_BUCKETS; ++i ) {
	//	// 복사 ( 원본은 사용하지 않는다 )
	//	arrCalcBucket[i].m_nFrame		= m_arrBucket[i].m_nFrame;
	//	arrCalcBucket[i].m_nCountPixels = m_arrBucket[i].m_nCountPixels;
	//	arrCalcBucket[i].m_vMinMax		= m_arrBucket[i].m_vMinMax;
	//}

	//////////////////////////////////////////////////////////////////////////
	// Dark Barrier : 전체 체적의 DarkBarrierRatio(def:50%)에 해당하는 TonemapMin을 구한다
	//////////////////////////////////////////////////////////////////////////
	/*
	for( int i = 0; i < NUM_MAX_BUCKETS; ++i ) 
	fMaxPixelCount += m_arrBucket[i].m_nCountPixels;

	float			fDarkPixels			= fMaxPixelCount * m_fDarkBarrierRatio;	// 전체 픽셀 대비 시작 영역(def:50%)에 해당하는 픽셀 개수
	m_nDarkBucketFrame					= -1;

	for( int i = 0; i < NUM_MAX_BUCKETS; ++i ) 
	{
	fSumPixel += m_arrBucket[i].m_nCountPixels;

	if( m_nDarkBucketFrame == -1 && fDarkPixels <= fSumPixel )
	m_nDarkBucketFrame = i;
	}

	if( m_nDarkBucketFrame >= 0 )	// 50%에 해당하는 픽셀이 포함된 버켓의 인덱스
	{
	int nPrevCount = 0;
	for( int i = 0; i < m_nDarkBucketFrame; ++i )
	nPrevCount += m_arrBucket[i].m_nCountPixels;

	float fRatio = 0;
	if( fDarkPixels > 0)
	fRatio	= ( fDarkPixels - (float)nPrevCount ) / (float)m_arrBucket[m_nDarkBucketFrame].m_nCountPixels;

	// TonemapMin 값은 히스토그램을 그리고 Secondary Rule을 계산하기 위한 용도로만 사용되고 실제 쉐이더 상수로는 항상 '0'이 들어간다
	m_fTonemapMin	= m_arrBucket[m_nDarkBucketFrame].m_vMinMax.x + (fRatio * fBucketWidth);

	//	arrCalcBucket[m_nDarkBucketFrame].m_nCountPixels -= ( fDarkPixels - (float)nPrevCount );
	}
	*/
	m_nDarkBucketFrame	= 0;
	m_fTonemapMin		= 0;

	//////////////////////////////////////////////////////////////////////////
	// Primary Rule : Brightness threshold
	// Bright Barrier : 전체 체적의 BrightBarrierRatio(def:95%)에 해당하는 TonemapMax를 구한다
	//////////////////////////////////////////////////////////////////////////
	fMaxPixelCount	= 0;
	fSumPixel		= 0;

	for( int i = 0; i < NUM_MAX_BUCKETS; ++i )
		fMaxPixelCount += m_arrBucket[i].m_nCountPixels;

	m_nBrightBucketFrame			= -1;
	float		fBrightPixels		= fMaxPixelCount * m_fBrightBarrierRatio;

	for( int i = 0; i < NUM_MAX_BUCKETS; ++i )
	{
		fSumPixel += m_arrBucket[i].m_nCountPixels;

		if( m_nBrightBucketFrame == -1 && fBrightPixels <= fSumPixel )
			m_nBrightBucketFrame = i;
	}

	float fCurrentMax = 0.0f;	// 임시 값으로 가지고 있는다
	if( m_nBrightBucketFrame >= 0 )
	{
		int nPrevCount = 0;
		for( int i = 0; i < m_nBrightBucketFrame; ++i )
			nPrevCount += m_arrBucket[i].m_nCountPixels;

		float fRatio = 0;
		if( fBrightPixels > 0)
			fRatio	= ( fBrightPixels - (float)nPrevCount ) / (float)m_arrBucket[m_nBrightBucketFrame].m_nCountPixels;

		fCurrentMax	= m_arrBucket[m_nBrightBucketFrame].m_vMinMax.x + (fRatio * fBucketWidth);

		//	arrCalcBucket[m_nBrightBucketFrame].m_nCountPixels -= ( fBrightPixels - (float)nPrevCount );
	}

	//////////////////////////////////////////////////////////////////////////
	// Secondary Rule : Use median luminance as a darkness barrier
	// 위에서 구한 Dark 부터 Bright 까지의 체적중 BrightBarrierRatio(95%)를 취한다
	//////////////////////////////////////////////////////////////////////////
	//fMaxPixelCount	= 0;
	//fSumPixel		= 0;

	//// m_nDarkBucketFrame 에서 m_nBrightBucketFrame 까지의 픽셀 이므로 루프종결문에 + 1
	//for( int i = m_nDarkBucketFrame; i < m_nBrightBucketFrame + 1; ++i )
	//	fMaxPixelCount += arrCalcBucket[i].m_nCountPixels;

	//int			nSecBrightBucketFrame	= -1;	// 
	//float		fSecBrightPixels		= fMaxPixelCount * m_fBrightBarrierRatio;	// 
	//
	//for( int i = m_nDarkBucketFrame; i < m_nBrightBucketFrame + 1; ++i )
	//{
	//	fSumPixel += arrCalcBucket[i].m_nCountPixels;

	//	if( nSecBrightBucketFrame == -1 && fSecBrightPixels <= fSumPixel )
	//		nSecBrightBucketFrame = i;
	//}

	//if( nSecBrightBucketFrame >= 0 )
	//{
	//	int nPrevCount = 0;
	//	for( int i = m_nDarkBucketFrame; i < nSecBrightBucketFrame; ++i )
	//		nPrevCount += arrCalcBucket[i].m_nCountPixels;

	//	float fRatio = 0;
	//	if( fSecBrightPixels > 0)
	//		fRatio	= ( fSecBrightPixels - (float)nPrevCount ) / (float)arrCalcBucket[nSecBrightBucketFrame].m_nCountPixels;

	//	fCurrentMax	= arrCalcBucket[nSecBrightBucketFrame].m_vMinMax.x + (fRatio * fBucketWidth);
	//}

	float fElapsed = REngine::GetTimer().GetElapsedTime() * 0.001f;
	m_fTonemapMax = m_fTonemapMax + (fCurrentMax - m_fTonemapMax) * ( 1 - pow( 0.98f, m_fAdaptationSpeed * fElapsed ) );
	if( m_fTonemapMax <= m_fLimitMin ) {
		m_fTonemapMax = m_fLimitMin;
	}

	if( m_fTonemapMax >= m_fLimitMax) {
		m_fTonemapMax = m_fLimitMax;
	}

	m_nCurrentFrame++;
	if( m_nCurrentFrame >= NUM_MAX_BUCKETS )
		m_nCurrentFrame = 0;

	return true;
}


//-----------------------------------------------------------------------------
// Desc: Run the bright-pass filter on m_pTexSceneScaled and place the result
//       in m_pTexBrightPass
//-----------------------------------------------------------------------------
bool RCameraHDRController::SceneScaled_To_BrightPass()
{
	RVector2 vSampleOffsets[MAX_SAMPLES];
	RVector4 vSampleWeights[MAX_SAMPLES];

	// Get the rectangle describing the sampled portion of the source texture.
	// Decrease the rectangle to adjust for the single pixel black border.
	RECT rectSrc;
	RTextureInfo Info = m_pTexSceneScaled->GetInfo();
	rectSrc.left	= 0;
	rectSrc.top		= 0;
	rectSrc.right	= Info.nTextureWidth;
	rectSrc.bottom	= Info.nTextureHeight;

	InflateRect( &rectSrc, -1, -1 );

	// Get the destination rectangle.
	// Decrease the rectangle to adjust for the single pixel black border.
	RECT rectDest;
	Info = m_pTexBrightPass->GetInfo();
	rectDest.left	= 0;
	rectDest.top	= 0;
	rectDest.right	= Info.nTextureWidth;
	rectDest.bottom	= Info.nTextureHeight;
	InflateRect( &rectDest, -1, -1 );

	// Get the correct texture coordinates to apply to the rendered quad in order 
	// to sample from the source rectangle and render into the destination rectangle
	RVector4 vCoords;
	GetTextureCoords( m_pTexSceneScaled, &rectSrc, m_pTexBrightPass, &rectDest, &vCoords );

	// The bright-pass filter removes everything from the scene except lights and
	// bright reflections
	RTechnique Technique = m_pHDRShader->GetTechnique("BrightPassFilter");

	REngine::GetDevice().SetRenderTarget( 0, m_pTexBrightPass );
	REngine::GetDevice().SetTexture( 0, m_pTexSceneScaled );
	REngine::GetDevice().SetScissorTestEnable(true);
	REngine::GetDevice().SetScissorRect( &rectDest );
	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT );
	REngine::GetDevice().SetTextureFilter( 1, RTF_POINT );

	UINT uiPassCount, uiPass;
	Technique.Begin(&uiPassCount, 0);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);

		DrawFullScreenQuad( vCoords );

		Technique.EndPass();
	}
	Technique.End();

	REngine::GetDevice().SetScissorTestEnable(false);

	return true;
}

//-----------------------------------------------------------------------------
// Name: BrightPass_To_BloomSource
// Desc: Scale down m_pTexStarSource by 1/2 x 1/2 and place the result in 
//       m_pTexBloomSource
//-----------------------------------------------------------------------------
bool RCameraHDRController::BrightPass_To_BloomSource()
{
	RVector2 vSampleOffsets[MAX_SAMPLES];
	RVector4 vSampleWeights[MAX_SAMPLES];

	// Get the destination rectangle.
	// Decrease the rectangle to adjust for the single pixel black border.
	RECT rectDest;
	RTextureInfo Info = m_pTexBloomSource->GetInfo();
	rectDest.left	= 0;
	rectDest.top	= 0;
	rectDest.right	= Info.nTextureWidth;
	rectDest.bottom	= Info.nTextureHeight;
	InflateRect( &rectDest, -1, -1 );

	// Get the correct texture coordinates to apply to the rendered quad in order 
	// to sample from the source rectangle and render into the destination rectangle
	RVector4 vCoords;
	GetTextureCoords( m_pTexBrightPass, NULL, m_pTexBloomSource, &rectDest, &vCoords );

	// Get the sample offsets used within the pixel shader
	Info = m_pTexBrightPass->GetInfo();

	GetSampleOffsets_GaussBlur5x5( Info.nTextureWidth, Info.nTextureHeight, vSampleOffsets, vSampleWeights );
	m_pHDRShader->m_pEffect->SetValue("g_avSampleOffsets", vSampleOffsets, sizeof( vSampleOffsets ) );
	m_pHDRShader->m_pEffect->SetValue("g_avSampleWeights", vSampleWeights, sizeof( vSampleWeights ) );

	// The gaussian blur smooths out rough edges to avoid aliasing effects
	// when the star effect is run
	RTechnique Technique = m_pHDRShader->GetTechnique("GaussBlur5x5");

	REngine::GetDevice().SetRenderTarget( 0, m_pTexBloomSource );
	REngine::GetDevice().SetTexture( 0, m_pTexBrightPass );
	REngine::GetDevice().SetScissorRect( &rectDest );
	REngine::GetDevice().SetScissorTestEnable(true);
	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT );
	REngine::GetDevice().SetTextureAddress( 0, RTADDRESS_CLAMP, RTADDRESS_CLAMP, RTADDRESS_CLAMP );

	UINT uiPassCount, uiPass;
	Technique.Begin(&uiPassCount, 0);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);

		DrawFullScreenQuad( vCoords );

		Technique.EndPass();
	}
	Technique.End();

	REngine::GetDevice().SetScissorTestEnable(false);

	return true;
}

//-----------------------------------------------------------------------------
// Name: BrightPass_To_StarSource
// Desc: Perform a 5x5 gaussian blur on m_pTexBrightPass and place the result
//       in m_pTexStarSource. The bright-pass filtered image is blurred before
//       being used for star operations to avoid aliasing artifacts.
//-----------------------------------------------------------------------------
bool RCameraHDRController::BrightPass_To_StarSource()
{
	RVector2 vSampleOffsets[MAX_SAMPLES];
	RVector4 vSampleWeights[MAX_SAMPLES];

	// Get the destination rectangle.
	// Decrease the rectangle to adjust for the single pixel black border.
	RECT rectDest;
	RTextureInfo Info = m_pTexStarSource->GetInfo();
	rectDest.left	= 0;
	rectDest.top	= 0;
	rectDest.right	= Info.nTextureWidth;
	rectDest.bottom	= Info.nTextureHeight;
	InflateRect( &rectDest, -1, -1 );

	// Get the correct texture coordinates to apply to the rendered quad in order 
	// to sample from the source rectangle and render into the destination rectangle
	RVector4 vCoords;
	GetTextureCoords( m_pTexBrightPass, NULL, m_pTexStarSource, &rectDest, &vCoords );

	// Get the sample offsets used within the pixel shader
	Info = m_pTexBrightPass->GetInfo();

	GetSampleOffsets_GaussBlur5x5( Info.nTextureWidth, Info.nTextureHeight, vSampleOffsets, vSampleWeights );
	m_pHDRShader->m_pEffect->SetValue("g_avSampleOffsets", vSampleOffsets, sizeof( vSampleOffsets ) );
	m_pHDRShader->m_pEffect->SetValue("g_avSampleWeights", vSampleWeights, sizeof( vSampleWeights ) );

	// The gaussian blur smooths out rough edges to avoid aliasing effects
	// when the star effect is run
	RTechnique Technique = m_pHDRShader->GetTechnique("GaussBlur5x5");

	REngine::GetDevice().SetRenderTarget( 0, m_pTexStarSource );
	REngine::GetDevice().SetTexture( 0, m_pTexBrightPass );
	REngine::GetDevice().SetScissorRect( &rectDest );
	REngine::GetDevice().SetScissorTestEnable(true);
	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT );
	REngine::GetDevice().SetTextureAddress( 0, RTADDRESS_CLAMP, RTADDRESS_CLAMP, RTADDRESS_CLAMP );

	UINT uiPassCount, uiPass;
	Technique.Begin(&uiPassCount, 0);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);

		DrawFullScreenQuad( vCoords );

		Technique.EndPass();
	}
	Technique.End();

	REngine::GetDevice().SetScissorTestEnable(false);

	return true;
}

//-----------------------------------------------------------------------------
// Name: StarSource_To_BloomSource
// Desc: Scale down m_pTexStarSource by 1/2 x 1/2 and place the result in 
//       m_pTexBloomSource
//-----------------------------------------------------------------------------
bool RCameraHDRController::StarSource_To_BloomSource()
{
	HRESULT hr = S_OK;

	RVector2 vSampleOffsets[MAX_SAMPLES];

	// Get the rectangle describing the sampled portion of the source texture.
	// Decrease the rectangle to adjust for the single pixel black border.
	RECT rectSrc;
	RTextureInfo Info = m_pTexStarSource->GetInfo();
	rectSrc.left	= 0;
	rectSrc.top		= 0;
	rectSrc.right	= Info.nTextureWidth;
	rectSrc.bottom	= Info.nTextureHeight;
	InflateRect( &rectSrc, -1, -1 );

	// Get the destination rectangle.
	// Decrease the rectangle to adjust for the single pixel black border.
	RECT rectDest;
	Info = m_pTexBloomSource->GetInfo();
	rectDest.left	= 0;
	rectDest.top	= 0;
	rectDest.right	= Info.nTextureWidth;
	rectDest.bottom	= Info.nTextureHeight;
	InflateRect( &rectDest, -1, -1 );

	// Get the correct texture coordinates to apply to the rendered quad in order 
	// to sample from the source rectangle and render into the destination rectangle
	RVector4 vCoords;
	GetTextureCoords( m_pTexStarSource, &rectSrc, m_pTexBloomSource, &rectDest, &vCoords );

	// Get the sample offsets used within the pixel shader
	Info = m_pTexBrightPass->GetInfo();
	GetSampleOffsets_DownScale2x2( Info.nTextureWidth, Info.nTextureHeight, vSampleOffsets );
	m_pHDRShader->m_pEffect->SetValue("g_avSampleOffsets", vSampleOffsets, sizeof( vSampleOffsets ) );

	// Create an exact 1/2 x 1/2 copy of the source texture
	RTechnique Technique = m_pHDRShader->GetTechnique("DownScale2x2");

	REngine::GetDevice().SetRenderTarget( 0, m_pTexBloomSource );
	REngine::GetDevice().SetTexture( 0, m_pTexStarSource );
	REngine::GetDevice().SetScissorRect( &rectDest );
	REngine::GetDevice().SetScissorTestEnable(true);
	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT );
	REngine::GetDevice().SetTextureAddress( 0, RTADDRESS_CLAMP, RTADDRESS_CLAMP, RTADDRESS_CLAMP );

	UINT uiPassCount, uiPass;
	Technique.Begin(&uiPassCount, 0);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);

		DrawFullScreenQuad( vCoords );

		Technique.EndPass();
	}
	Technique.End();

	REngine::GetDevice().SetScissorTestEnable(false);

	return true;
}

//-----------------------------------------------------------------------------
// Name: RenderBloom()
// Desc: Render the blooming effect
//-----------------------------------------------------------------------------
bool RCameraHDRController::RenderBloom()
{
	UINT uiPassCount, uiPass;
	int i = 0;

	RVector2 vSampleOffsets[MAX_SAMPLES];
	FLOAT afSampleOffsets[MAX_SAMPLES];
	RVector4 vSampleWeights[MAX_SAMPLES];

	// Clear the bloom texture
	REngine::GetDevice().ColorFill( m_apTexBloom[0], D3DCOLOR_ARGB( 0, 0, 0, 0 ) );

	if( m_GlareDef.m_fGlareLuminance <= 0.0f ||	m_GlareDef.m_fBloomLuminance <= 0.0f )
		return false;

	RECT rectSrc;
	RTextureInfo Info = m_pTexBloomSource->GetInfo();
	rectSrc.left	= 0;
	rectSrc.top		= 0;
	rectSrc.right	= Info.nTextureWidth;
	rectSrc.bottom	= Info.nTextureHeight;
	InflateRect( &rectSrc, -1, -1 );

	RECT rectDest;
	Info = m_apTexBloom[2]->GetInfo();
	rectDest.left	= 0;
	rectDest.top	= 0;
	rectDest.right	= Info.nTextureWidth;
	rectDest.bottom	= Info.nTextureHeight;
	InflateRect( &rectDest, -1, -1 );

	RVector4 vCoords;
	GetTextureCoords( m_pTexBloomSource, &rectSrc, m_apTexBloom[2], &rectDest, &vCoords );

	Info = m_pTexBloomSource->GetInfo();

	RTechnique Technique = m_pHDRShader->GetTechnique("GaussBlur5x5");
	GetSampleOffsets_GaussBlur5x5( Info.nTextureWidth, Info.nTextureHeight, vSampleOffsets, vSampleWeights, 1.0f );

	m_pHDRShader->m_pEffect->SetValue("g_avSampleOffsets", vSampleOffsets, sizeof( vSampleOffsets ) );
	m_pHDRShader->m_pEffect->SetValue("g_avSampleWeights", vSampleWeights, sizeof( vSampleWeights ) );

	REngine::GetDevice().SetRenderTarget( 0, m_apTexBloom[2] );
	REngine::GetDevice().SetTexture( 0, m_pTexBloomSource );
	REngine::GetDevice().SetScissorRect( &rectDest );
	REngine::GetDevice().SetScissorTestEnable(true);
	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT );

	Technique.Begin(&uiPassCount, 0);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);

		DrawFullScreenQuad( vCoords );

		Technique.EndPass();
	}
	Technique.End();

	REngine::GetDevice().SetScissorTestEnable(false);

	Info = m_apTexBloom[2]->GetInfo();

	GetSampleOffsets_Bloom( Info.nTextureWidth, afSampleOffsets, vSampleWeights, 3.0f, 2.0f );
	for( i = 0; i < MAX_SAMPLES; i++ ) {
		vSampleOffsets[i] = RVector2( afSampleOffsets[i], 0.0f );
	}

	Technique = m_pHDRShader->GetTechnique("Bloom");
	m_pHDRShader->m_pEffect->SetValue("g_avSampleOffsets", vSampleOffsets, sizeof( vSampleOffsets ) );
	m_pHDRShader->m_pEffect->SetValue("g_avSampleWeights", vSampleWeights, sizeof( vSampleWeights ) );

	REngine::GetDevice().SetRenderTarget( 0, m_apTexBloom[1] );
	REngine::GetDevice().SetTexture( 0, m_apTexBloom[2] );
	REngine::GetDevice().SetScissorRect( &rectDest );
	REngine::GetDevice().SetScissorTestEnable(true);
	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT );

	Technique.Begin(&uiPassCount, 0);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);

		DrawFullScreenQuad( vCoords );

		Technique.EndPass();
	}
	Technique.End();

	REngine::GetDevice().SetScissorTestEnable(false);

	Info = m_apTexBloom[1]->GetInfo();
	GetSampleOffsets_Bloom( Info.nTextureHeight, afSampleOffsets, vSampleWeights, 3.0f, 2.0f );
	for( i = 0; i < MAX_SAMPLES; i++ ) {
		vSampleOffsets[i] = RVector2( 0.0f, afSampleOffsets[i] );
	}

	Info = m_apTexBloom[1]->GetInfo();
	rectSrc.left	= 0;
	rectSrc.top		= 0;
	rectSrc.right	= Info.nTextureWidth;
	rectSrc.bottom	= Info.nTextureHeight;
	InflateRect( &rectSrc, -1, -1 );

	GetTextureCoords( m_apTexBloom[1], &rectSrc, m_apTexBloom[0], NULL, &vCoords );

	Technique = m_pHDRShader->GetTechnique("Bloom");
	m_pHDRShader->m_pEffect->SetValue("g_avSampleOffsets", vSampleOffsets, sizeof( vSampleOffsets ) );
	m_pHDRShader->m_pEffect->SetValue("g_avSampleWeights", vSampleWeights, sizeof( vSampleWeights ) );

	REngine::GetDevice().SetRenderTarget( 0, m_apTexBloom[0] );
	REngine::GetDevice().SetTexture( 0, m_apTexBloom[1] );
	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT );

	Technique.Begin(&uiPassCount, 0);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);

		DrawFullScreenQuad( vCoords );

		Technique.EndPass();
	}
	Technique.End();

	return true;
}

//-----------------------------------------------------------------------------
// Name: RenderStar()
// Desc: Render the blooming effect
//-----------------------------------------------------------------------------
bool RCameraHDRController::RenderStar()
{
	UINT uiPassCount, uiPass;
	int i, d, p, s; // Loop variables

	// Clear the star texture
	REngine::GetDevice().ColorFill( m_apTexStar[0], D3DCOLOR_ARGB( 0, 0, 0, 0 ) );

	// Avoid rendering the star if it's not being used in the current glare
	if( m_GlareDef.m_fGlareLuminance <= 0.0f || m_GlareDef.m_fStarLuminance <= 0.0f )
		return true;

	// Initialize the constants used during the effect
	const CStarDef& starDef = m_GlareDef.m_starDef;
	const float fTanFoV = atanf( MMath::PI / 8 );
	const RVector4 vWhite( 1.0f, 1.0f, 1.0f, 1.0f );
	static const int s_maxPasses = 3;
	static const int nSamples = 8;
	static RVector4 s_aaColor[s_maxPasses][8];
	RVector4 s_colorWhite( 0.63f, 0.63f, 0.63f, 0.0f );

	RVector4 vSampleWeights[MAX_SAMPLES];
	RVector2 vSampleOffsets[MAX_SAMPLES];

	REngine::GetDevice().SetBlending(RBLEND_ONE, RBLEND_ONE);

	RTextureInfo Info = m_pTexStarSource->GetInfo();
	float srcW;
	srcW = ( FLOAT )Info.nTextureWidth;
	float srcH;
	srcH = ( FLOAT )Info.nTextureHeight;

	for( p = 0; p < s_maxPasses; p ++ )
	{
		float ratio;
		ratio = ( float )( p + 1 ) / ( float )s_maxPasses;

		for( s = 0; s < nSamples; s ++ )
		{
			RVector4 chromaticAberrColor;

			RVector4 vColor = RVector4( CStarDef::GetChromaticAberrationColor(s).r,
				CStarDef::GetChromaticAberrationColor(s).g,
				CStarDef::GetChromaticAberrationColor(s).b,
				CStarDef::GetChromaticAberrationColor(s).a );
			chromaticAberrColor = MMath::LinearInterpolation( vColor, s_colorWhite, ratio);
			s_aaColor[p][s] = MMath::LinearInterpolation(s_colorWhite, chromaticAberrColor, m_GlareDef.m_fChromaticAberration);
		}
	}

	float radOffset;
	radOffset = m_GlareDef.m_fStarInclination + starDef.m_fInclination;

	RTexture* pTexSource;
	RTexture* pSurfDest;
	// Direction loop
	for( d = 0; d < starDef.m_nStarLines; d ++ )
	{
		CONST STARLINE& starLine = starDef.m_pStarLine[d];

		pTexSource = m_pTexStarSource;

		float rad;
		rad = radOffset + starLine.fInclination;
		float sn, cs;
		sn = sinf( rad ), cs = cosf( rad );
		RVector2 vtStepUV;
		vtStepUV.x = sn / srcW * starLine.fSampleLength;
		vtStepUV.y = cs / srcH * starLine.fSampleLength;

		float attnPowScale;
		attnPowScale = ( fTanFoV + 0.1f ) * 1.0f *
			( 160.0f + 120.0f ) / ( srcW + srcH ) * 1.2f;

		// 1 direction expansion loop
		REngine::GetDevice().SetBlending( RBLEND_NONE, RBLEND_NONE );

		int iWorkTexture;
		iWorkTexture = 1;
		for( p = 0; p < starLine.nPasses; p ++ )
		{
			if( p == starLine.nPasses - 1 )
			{
				// Last pass move to other work buffer
				pSurfDest = m_apTexStar[d + 4];
			}
			else
			{
				pSurfDest = m_apTexStar[iWorkTexture];
			}

			// Sampling configration for each stage
			for( i = 0; i < nSamples; i ++ )
			{
				float lum;
				lum = powf( starLine.fAttenuation, attnPowScale * i );

				vSampleWeights[i] = s_aaColor[starLine.nPasses - 1 - p][i] *
					lum * ( p + 1.0f ) * 0.5f;


				// Offset of sampling coordinate
				vSampleOffsets[i].x = vtStepUV.x * i;
				vSampleOffsets[i].y = vtStepUV.y * i;
				if( fabs( vSampleOffsets[i].x ) >= 0.9f ||
					fabs( vSampleOffsets[i].y ) >= 0.9f )
				{
					vSampleOffsets[i].x = 0.0f;
					vSampleOffsets[i].y = 0.0f;
					vSampleWeights[i] *= 0.0f;
				}

			}

			RTechnique Technique = m_pHDRShader->GetTechnique("Star");
			m_pHDRShader->m_pEffect->SetValue("g_avSampleOffsets", vSampleOffsets, sizeof( vSampleOffsets ) );
			m_pHDRShader->GetConstant("g_avSampleWeights").SetVector4Array(vSampleWeights, nSamples);

			REngine::GetDevice().SetRenderTarget( 0, pSurfDest );
			REngine::GetDevice().SetTexture( 0, pTexSource );
			REngine::GetDevice().SetTextureFilter( 0, RTF_LINEAR );

			Technique.Begin(&uiPassCount, 0);
			for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
			{
				Technique.BeginPass(uiPass);
				DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f  );
				Technique.EndPass();
			}
			Technique.End();

			// Setup next expansion
			vtStepUV *= nSamples;
			attnPowScale *= nSamples;

			// Set the work drawn just before to next texture source.
			pTexSource = m_apTexStar[iWorkTexture];

			iWorkTexture += 1;
			if( iWorkTexture > 2 )
			{
				iWorkTexture = 1;
			}
		}
	}

	pSurfDest = m_apTexStar[0];

	for( i = 0; i < starDef.m_nStarLines; i++ )
	{
		REngine::GetDevice().SetTexture( i, m_apTexStar[i + 4] );
		REngine::GetDevice().SetTextureFilter( i, RTF_LINEAR );

		vSampleWeights[i] = vWhite * 1.0f / ( FLOAT )starDef.m_nStarLines;
	}

	char strTechnique[256];
	sprintf_s(strTechnique, 256, "MergeTextures_%d", starDef.m_nStarLines);
	//StringCchPrintfA( strTechnique, 256, "MergeTextures_%d", starDef.m_nStarLines );

	RTechnique Technique = m_pHDRShader->GetTechnique(strTechnique);

	m_pHDRShader->GetConstant("g_avSampleWeights").SetVector4Array(vSampleWeights, starDef.m_nStarLines);

	REngine::GetDevice().SetRenderTarget( 0, pSurfDest );

	Technique.Begin(&uiPassCount, 0);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);
		DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f  );
		Technique.EndPass();
	}
	Technique.End();

	for( i = 0; i < starDef.m_nStarLines; i++ )
		REngine::GetDevice().SetTexture( i, NULL );

	return true;
}

// HDR Scene을 구성한다
void RCameraHDRController::OnRender()
{
	RTexture* pBeforeRenderTarget0 = REngine::GetDevice().GetRenderTarget(0);	// Low dynamic range surface for final output
	RTexture* pBeforeRenderTarget1 = REngine::GetDevice().SetRenderTarget(1, NULL);	// Off RT1

	REngine::GetDevice().SetTextureStageState(0,RTSS_COLOROP	,RTOP_SELECTARG1 );
	REngine::GetDevice().SetTextureStageState(0,RTSS_COLORARG1 ,RTA_TEXTURE );
	REngine::GetDevice().SetTextureStageState(0,RTSS_ALPHAOP	,RTOP_DISABLE );
	REngine::GetDevice().SetTextureStageState(1,RTSS_COLOROP	,RTOP_DISABLE);
	REngine::GetDevice().SetTextureStageState(1,RTSS_ALPHAOP	,RTOP_DISABLE );

	REngine::GetDevice().SetAlphaTestEnable(false);
	REngine::GetDevice().SetFogEnable(false);
	REngine::GetDevice().SetBlending(RBLEND_NONE, RBLEND_NONE);
	REngine::GetDevice().SetCullMode(RCULL_NONE);
	REngine::GetDevice().SetDepthEnable(false,false);
	REngine::GetDevice().SetTextureFactor(0);
	REngine::GetDevice().SetLighting(false);

	REngine::GetDevice().SetTexture(0, NULL);
	REngine::GetDevice().SetTexture(1, NULL);
	REngine::GetDevice().SetTexture(2, NULL);
	REngine::GetDevice().SetTexture(3, NULL);

	// Create a scaled copy of the scene
	Scene_To_SceneScaled();

	// Setup tone mapping technique
	if( m_bToneMap )
	{
		OcclusionQueryTonemap();
		CalculateTonemap();
	}

	if(m_bToneMap)
		// Now that luminance information has been gathered, the scene can be bright-pass filtered
		// to remove everything except bright lights and reflections.
		SceneScaled_To_BrightPass();

	if( m_bBloom )
	{
		BrightPass_To_BloomSource();
		RenderBloom();
	}

	if( m_bGlare )
	{
		BrightPass_To_StarSource();	
		RenderStar();
	}

	REngine::GetDevice().SetDepthEnable( true, true);
	REngine::GetDevice().SetRenderTarget( 0, pBeforeRenderTarget0);
	REngine::GetDevice().SetRenderTarget( 1, pBeforeRenderTarget1);
}

// HDR 포멧을 LDR(Framebuffer) 포멧으로 변환한다
// 백버퍼가 AX 이므로 쉐이더 내부에서 Alpha 값은 1로 설정해준다.
void RCameraHDRController::Flip()
{
	REngine::GetDevice().SetTextureStageState(0,RTSS_COLOROP	,RTOP_SELECTARG1 );
	REngine::GetDevice().SetTextureStageState(0,RTSS_COLORARG1	,RTA_TEXTURE );
	REngine::GetDevice().SetTextureStageState(0,RTSS_ALPHAOP	,RTOP_DISABLE );
	REngine::GetDevice().SetTextureStageState(1,RTSS_COLOROP	,RTOP_DISABLE);
	REngine::GetDevice().SetTextureStageState(1,RTSS_ALPHAOP	,RTOP_DISABLE );
	REngine::GetDevice().SetBlending(RBLEND_NONE,RBLEND_NONE);
	REngine::GetDevice().SetDepthEnable(false,false);
	//REngine::GetDevice().SetFvF(D3DFVF_XYZRHW | D3DFVF_TEX1 );

	RTechnique Technique = m_pHDRShader->GetTechnique("FinalScenePass");

	// Set frame shader constants
	m_pHDRShader->GetConstant("g_bEnableToneMap").SetInt(m_bToneMap);
	//	m_pHDRShader->GetConstant("g_bEnableBlueShift").SetInt(m_bBlueShift);

	if(m_bBloom)
		m_pHDRShader->GetConstant("g_fBloomScale").SetFloat(m_fBloomScale);
	else
		m_pHDRShader->GetConstant("g_fBloomScale").SetFloat(0.0f);

	if(m_bGlare)
		m_pHDRShader->GetConstant("g_fStarScale").SetFloat(m_fGlareScale);
	else
		m_pHDRShader->GetConstant("g_fStarScale").SetFloat(0.0f);

	m_pHDRShader->GetConstant("g_bHDR").SetFloat(m_bHDR);

	m_pHDRShader->GetConstant("g_fTonemapMin").SetFloat(0.0f);//m_fTonemapMin);
	m_pHDRShader->GetConstant("g_fTonemapMax").SetFloat(m_fTonemapMax);

	REngine::GetDevice().SetTexture(0, m_pHDRBuffer);
	REngine::GetDevice().SetTexture(1, m_apTexBloom[0]);
	REngine::GetDevice().SetTexture(2, m_apTexStar[0]);
	REngine::GetDevice().SetTexture(3, NULL);

	REngine::GetDevice().SetTextureFilter( 0, RTF_POINT );
	REngine::GetDevice().SetTextureFilter( 1, RTF_LINEAR );
	REngine::GetDevice().SetTextureFilter( 2, RTF_LINEAR );
	REngine::GetDevice().SetTextureFilter( 3, RTF_POINT );

	UINT uiPassCount, uiPass;
	m_pHDRShader->CommitChanges();
	Technique.Begin(&uiPassCount, 0);
	for (uiPass = 0; uiPass < uiPassCount; ++uiPass) 
	{
		Technique.BeginPass(uiPass);
		DrawFullScreenQuad(0, 0, 1, 1, false);
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

	if(m_bDebugRT)
	{
		RRenderHelper::SetRenderState();
		RRenderHelper::RenderTexture(0, 250-250, 200, 150, m_pHDRBuffer);
		RRenderHelper::RenderTexture(0, 410-250, 200, 150, m_pTexBrightPass);///m_apTexToneMap[NUM_TONEMAP_TEXTURES - 1]);
		RRenderHelper::RenderTexture(0, 570-250, 200, 150, m_apTexBloom[0]);
		RRenderHelper::EndRenderState();
	}

	REngine::GetDevice().SetDepthEnable(true,true);
	REngine::GetDevice().SetBlending(RBLEND_ONE,RBLEND_NONE);
}

void RCameraHDRController::SaveToXML(MXmlElement &element)
{
	MXmlElement *pElement = NULL;

	pElement = new MXmlElement("HDRENABLE");
	_SetContents(pElement, m_bHDR);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("DEBUGVIEWENABLE");
	_SetContents(pElement, m_bDebugRT);
	element.LinkEndChild(pElement);

	//	pElement = new MXmlElement("BLUESHIFTENABLE");
	//	_SetContents(pElement, m_bBlueShift);
	//	element.LinkEndChild(pElement);

	pElement = new MXmlElement("BLOOMENABLE");
	_SetContents(pElement, m_bBloom);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("BLOOMSCALE");
	_SetContents(pElement, m_fBloomScale);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("GLAREENABLE");
	_SetContents(pElement, m_bGlare);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("GLARETYPE");
	_SetContents(pElement, (int)m_eGlareType);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("GLARESCALE");
	_SetContents(pElement, m_fGlareScale);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("TONEMAPENABLE");
	_SetContents(pElement, m_bToneMap);
	element.LinkEndChild(pElement);

	//pElement = new MXmlElement("DARKRATIO");
	//_SetContents(pElement, m_fDarkBarrierRatio);
	//element.LinkEndChild(pElement);

	pElement = new MXmlElement("BRIGHTRATIO");
	_SetContents(pElement, m_fBrightBarrierRatio * 100);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("LIMITMIN");
	_SetContents(pElement, m_fLimitMin / MAX_LUMINANCE);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("LIMITMAX");
	_SetContents(pElement, m_fLimitMax / MAX_LUMINANCE);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("ADAPTATIONSPEED");
	_SetContents(pElement, m_fAdaptationSpeed);
	element.LinkEndChild(pElement);
}

void RCameraHDRController::LoadToXML(MXmlElement &element)
{
	MXmlElement *pElement = NULL;

	if ( pElement = element.FirstChildElement("HDRENABLE") ) {
		_Contents(&m_bHDR, pElement);
	}

	if ( pElement = element.FirstChildElement("DEBUGVIEWENABLE") ) {
		_Contents(&m_bDebugRT, pElement);
	}

	//	if ( pElement = element.FirstChildElement("BLUESHIFTENABLE") ) {
	//		_Contents(&m_bBlueShift, pElement);
	//	}

	if ( pElement = element.FirstChildElement("BLOOMENABLE") ) {
		_Contents(&m_bBloom, pElement);
	}

	if ( pElement = element.FirstChildElement("BLOOMSCALE") ) {
		_Contents(&m_fBloomScale, pElement);
	}

	if ( pElement = element.FirstChildElement("GLAREENABLE") ) {
		_Contents(&m_bGlare, pElement);
	}

	if ( pElement = element.FirstChildElement("GLARETYPE") ) {
		int nType;
		_Contents(&nType, pElement);
		m_eGlareType = (EGLARELIBTYPE)nType;
	}

	if ( pElement = element.FirstChildElement("GLARESCALE") ) {
		_Contents(&m_fGlareScale, pElement);
	}


	if ( pElement = element.FirstChildElement("TONEMAPENABLE") ) {
		_Contents(&m_bToneMap, pElement);
	}

	//if ( pElement = element.FirstChildElement("DARKRATIO") ) {
	//	_Contents(&m_fDarkBarrierRatio, pElement);
	//}

	if ( pElement = element.FirstChildElement("BRIGHTRATIO") ) {
		_Contents(&m_fBrightBarrierRatio, pElement);
		m_fBrightBarrierRatio *= 0.01f;
	}

	if ( pElement = element.FirstChildElement("LIMITMIN") ) {
		_Contents(&m_fLimitMin, pElement);
		m_fLimitMin *= MAX_LUMINANCE;
	}

	if ( pElement = element.FirstChildElement("LIMITMAX") ) {
		_Contents(&m_fLimitMax, pElement);
		m_fLimitMax *= MAX_LUMINANCE;
	}

	if ( pElement = element.FirstChildElement("ADAPTATIONSPEED") ) {
		_Contents(&m_fAdaptationSpeed, pElement);
	}
}

void RCameraHDRController::OnRemoveFromCamera()
{
	//카메라 씬 노드의 정보를 건드리지 않는다.
}

void RCameraHDRController::OnClear()
{
	REngine::GetDevice().Clear( true, true, 
		m_pCameraSceneNode->isHaveStencil(), 
		m_pCameraSceneNode->GetClearColor(),
		m_pCameraSceneNode->GetClearDepth(),
		m_pCameraSceneNode->GetClearStencil() );
}

// HDR(R16G16B16A16F) -> FrameBuffer(AXR8G8B8)
void RCameraHDRController::OnPostRender()
{
	// 기존 백업용으로 돌려놓는다. - 090617, OZ
	REngine::GetDevice().SetRenderTarget( 0, m_pHDRBufferOrg);

	OnRender();
	Flip();
}


// 랜더 타겟을 관리
void RCameraHDRController::OnPreRender( RInferredRenderingStrategy* pInferredRenderingStrategy)
{
	RCameraSceneNodeController::OnPreRender( pInferredRenderingStrategy);

	m_pHDRBufferOrg = REngine::GetDevice().GetRenderTarget(0);

	REngine::GetDevice().SetRenderTarget( 0, m_pHDRBuffer);
}



}