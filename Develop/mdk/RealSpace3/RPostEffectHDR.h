#pragma once

#include "RPostEffectInterface.h"

namespace rs3 {


#define NUM_MAX_BUCKETS		16
#define MAX_LUMINANCE		4.f

struct RBucket
{
	int			m_nFrame;		// 프레임
	int			m_nCountPixels;	// 픽셀 개수
	RVector2	m_vMinMax;		// 영역
};

class RS_API RPostEffectHDR : public RPostEffectInterface
{
public:

#define MAX_SAMPLES           16      // Maximum number of texture grabs
#define NUM_BLOOM_TEXTURES    3       // Number of textures used for the bloom post-processing effect

	static const RVector DEFAULT_BRIGHTPASS_THRESHOLD;

public:
	RPostEffectHDR();
	virtual ~RPostEffectHDR();

	RShaderFX*		m_pHDRShader;	

	// Render Target Texture
	RTexture* 		m_pTexSceneScaled;			// Scaled copy of the HDR scene
	RTexture* 		m_pTexBrightPass;			// Bright-pass filtered copy of the scene
	RTexture* 		m_pTexBloomSource;			// Bloom effect source texture

	RTexture*		m_apTexBloom[NUM_BLOOM_TEXTURES];		// Blooming effect working textures

	RVector			m_vBrightPassThreshold;
	float			m_fBloomBlurDeviation;
	float			m_fBloomBlurMultiplier;

	bool			m_bGaussian2Pass;
	bool			m_bScaleAndBright1Pass;
	bool			m_bDebugRT;

	RListener*		m_pEventListener;			///< OnRestoreDevice / OnLostDevice 이벤트 수신객체

public:
	RCameraSceneNode*		m_pCameraSceneNode;

	RVector			GetBrightPassThreshold() { return m_vBrightPassThreshold; }
	void			SetBrightPassThreshold( RVector value ) { m_vBrightPassThreshold = value; }

	float			GetBloomBlurDeviation() { return m_fBloomBlurDeviation; }
	void			SetBloomBlurDeviation( float value ) { m_fBloomBlurDeviation = value; }

	float			GetBloomBlurMultiplier() { return m_fBloomBlurMultiplier; }
	void			SetBloomBlurMultiplier( float value ) { m_fBloomBlurMultiplier = value; }

	bool 			GetDebugViewEnable() { return m_bDebugRT; }
	void 			SetDebugViewEnable(bool bEnable) { m_bDebugRT = bEnable; }

	virtual bool	Init(  int nW, int nH);
	virtual void	Destroy();

	virtual void	Render( RCameraSceneNode* pCamera, RTexture* pColorBuffer, RTexture* pNormalBuffer, RTexture* pDepthBuffer);

	virtual bool 	CreateFromXML(MXmlElement &element, bool bBackgroundCreation = false){ return true; }
	virtual bool 	FrustumTest(const RViewFrustum& Frustum, RFRUSTUMTESTMETHOD method /* = RFTM_FAST */) { return true; }

protected:
	virtual void	CreateBuffers()  override;
	virtual void	DestroyBuffers()  override;

private:
	bool 			GetSampleOffsets_DownScale4x4( int nWidth, int nHeight, RVector2 vSampleOffsets[] );
	bool 			GetSampleOffsets_DownScale2x2( int nWidth, int nHeight, RVector2 vSampleOffsets[] );
	float			GaussianDistribution( float x, float y, float rho );
	bool 			GetSampleOffsets_GaussBlur5x5( int nTexWidth, int nTexHeight, RVector2* vTexCoordOffset, RVector4* vSampleWeight, FLOAT fMultiplier = 1.0f );
	bool 			GetSampleOffsets_Bloom(int nTexWidth, float afTexCoordOffset[15], RVector4* vColorWeight, float fDeviation, FLOAT fMultiplier=1.0f );
	bool 			GetSampleOffsets_Star( int nTexWidth, float afTexCoordOffset[15], RVector4* vColorWeight, float fDeviation );
	void			GetCropSize(int& outWidth, int& outHeight);

	// Post-processing source textures creation
	RTexture*		SceneScale( RCameraSceneNode* pCamera, RTexture* pSrcBuffer);
	RTexture* 		BrightPass( RCameraSceneNode* pCamera, RTexture* pSrcBuffer);
	RTexture*		SceneScalenBrightPass( RCameraSceneNode* pCamera, RTexture* pSrcBuffer);
	RTexture*		SceneScaleAndBrightPass( RCameraSceneNode* pCamera, RTexture* pSrcBuffer);
	RTexture*		BloomSource( RCameraSceneNode* pCamera, RTexture* pSrcBuffer);
	RTexture*		BloomSource_2Pass( RCameraSceneNode* pCamera, RTexture* pSrcBuffer);
	RTexture*		RenderBloom( RCameraSceneNode* pCamera, RTexture* pSrcBuffer);
	RTexture*		RenderBloom_2Pass( RCameraSceneNode* pCamera, RTexture* pSrcBuffer);
	void 			Flip( RCameraSceneNode* pCamera, RTexture* pColorBuffer);
};

}


