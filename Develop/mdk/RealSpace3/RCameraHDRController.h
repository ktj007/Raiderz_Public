#ifndef _RCAMERA_HDR_CONTROLLER_H
#define _RCAMERA_HDR_CONTROLLER_H

#include "RCameraSceneNodeController.h"
#include "RShaderFX.h"
#include "RGlareDef.h"
#include "RListener.h"

namespace rs3 {


#define NUM_MAX_BUCKETS 16
#define MAX_LUMINANCE	4.f

struct RBucket
{
	int			m_nFrame;		// 프레임
	int			m_nCountPixels;	// 픽셀 개수
	RVector2	m_vMinMax;		// 영역
};

class RS_API RCameraHDRController : public RCameraSceneNodeController, public RListener
{
public:

#define MAX_SAMPLES           16      // Maximum number of texture grabs
#define NUM_STAR_TEXTURES     12      // Number of textures used for the star post-processing effect
#define NUM_BLOOM_TEXTURES    3       // Number of textures used for the bloom post-processing effect

public:
	RCameraHDRController( RCameraSceneNode* pCameraSceneNode );
	virtual ~RCameraHDRController();

	RShaderFX*		m_pHDRShader;	

	// Render Target Texture
	// 카메라 컨트롤러가 인퍼드에서만 정상 작동 하게 한다.
	RTexture* 		m_pHDRBuffer;				// 참조 혹은 생성한 것으로 참조
	RTexture* 		m_pHDRBufferOrg;			// 복원용
	RTexture* 		m_pTexOcclusionQuery;		// Occlusion Query Testing texture
	RTexture* 		m_pTexSceneScaled;			// Scaled copy of the HDR scene
	RTexture* 		m_pTexBrightPass;			// Bright-pass filtered copy of the scene
	RTexture* 		m_pTexBloomSource;			// Bloom effect source texture
	RTexture* 		m_pTexStarSource;			// Star effect source texture

	RTexture*		m_apTexBloom[NUM_BLOOM_TEXTURES];		// Blooming effect working textures
	RTexture*		m_apTexStar[NUM_STAR_TEXTURES];			// Star effect working textures

	bool        	m_bAdaptationInvalid;		// True when adaptation level needs refreshing
	int				m_nCropWidth;
	int				m_nCropHeight;

	CGlareDef		m_GlareDef;					// Glare defintion
	EGLARELIBTYPE	m_eGlareType;				// Enumerated glare type

	// for occlusion query
	int				m_nCurrentFrame;				// in 16 frames
	int				m_nDarkBucketFrame;
	int				m_nBrightBucketFrame;
	RBucket			m_arrBucket[NUM_MAX_BUCKETS];	// Constructs the Histogram for	
	float			m_fTonemapMin;
	float			m_fTonemapMax;

	// Contol Widget
	bool			m_bHDR;
	bool			m_bDebugRT;
	bool			m_bToneMap;					// True when scene is to be tone mapped            
//	bool        	m_bBlueShift;				// True when blue shift is to be factored in
	bool			m_bBloom;
	float			m_fBloomScale;
	bool			m_bGlare;
	float			m_fGlareScale;
	bool			m_bFullFrame;				// 

	float			m_fDarkBarrierRatio;		// HDR Histogram
	float			m_fBrightBarrierRatio;
	float			m_fLimitMin;				// 최저 밝기
	float			m_fLimitMax;				// 최대 밝기

	float			m_fAdaptationSpeed;	


	RListener*		m_pEventListener;			///< OnRestoreDevice / OnLostDevice 이벤트 수신객체
	string			m_strConfigPath;

public:
	RCameraSceneNode*		m_pCameraSceneNode;

	virtual void	UpdateCameraController(RMatrix& _rMatApplyToCamera) {}
	virtual void	OnClear();		// Clear
	virtual void	OnPostRender();	// Flip
	virtual void	OnRemoveFromCamera();
	virtual void	OnPreRender( RInferredRenderingStrategy* pInferredRenderingStrategy);	// 랜더 타겟을 관리

	bool 			Create();
	void 			Destroy();
	void			Restore();

	bool 			GetSampleOffsets_DownScale2x2( int nWidth, int nHeight, RVector2 vSampleOffsets[] );
	bool 			GetSampleOffsets_DownScale4x4( int nWidth, int nHeight, RVector2 vSampleOffsets[] );
	float			GaussianDistribution( float x, float y, float rho );
	bool 			GetSampleOffsets_GaussBlur5x5( int nTexWidth, int nTexHeight, RVector2* vTexCoordOffset, RVector4* vSampleWeight, FLOAT fMultiplier = 1.0f );
	bool 			GetSampleOffsets_Bloom(int nTexWidth, float afTexCoordOffset[15], RVector4* vColorWeight, float fDeviation, FLOAT fMultiplier=1.0f );
	bool 			GetSampleOffsets_Star( int nTexWidth, float afTexCoordOffset[15], RVector4* vColorWeight, float fDeviation );
	void 			DrawFullScreenQuad(RVector4 vCoord) { DrawFullScreenQuad(vCoord.x, vCoord.y, vCoord.z, vCoord.w); }
	void 			DrawFullScreenQuad(float fLeftU, float fTopV, float fRightU, float fBottomV, bool bSmall = true);
	bool 			GetTextureCoords( RTexture* pTexSrc, RECT* pRectSrc, RTexture* pTexDest, RECT* pRectDest, RVector4* pCoords );

	// Post-processing source textures creation
	bool 			Scene_To_SceneScaled();
	bool			OcclusionQueryTonemap();
	bool			CalculateTonemap();
	bool 			SceneScaled_To_BrightPass();
	bool			BrightPass_To_BloomSource();
	bool 			BrightPass_To_StarSource();
	bool 			StarSource_To_BloomSource();
	bool 			RenderBloom();
	bool 			RenderStar();
	void 			Flip();

	// 구현
	virtual bool 	CreateFromXML(MXmlElement &element, bool bBackgroundCreation = false){ return true; }
	virtual void 	OnRender();
	virtual bool 	FrustumTest(const RViewFrustum& Frustum, RFRUSTUMTESTMETHOD method /* = RFTM_FAST */) { return true; }

	RTexture*		GetHDRBuffer() { return m_pHDRBuffer; }

	inline bool 	GetHDREnable() { return m_bHDR; }
	inline bool 	GetDebugViewEnable() { return m_bDebugRT; }
//	inline bool 	GetBlueShiftEnable() { return m_bBlueShift; }
	inline bool 	GetBloomEnable() { return m_bBloom; }
	inline float 	GetBloomScale() { return m_fBloomScale;}
	inline bool 	GetGlareEnable() { return m_bGlare; }
	inline float 	GetGlareScale() { return m_fGlareScale;}
	inline int		GetGlareType() { return m_eGlareType; }
	inline bool 	GetToneMapEnable() { return m_bToneMap; }
//	inline float 	GetDarkBarrierRatio() { return m_fDarkBarrierRatio; }
	inline float 	GetBrightBarrierRatio() { return m_fBrightBarrierRatio; }
	inline float 	GetLimitMin() { return m_fLimitMin / MAX_LUMINANCE; }
	inline float 	GetLimitMax() { return m_fLimitMax / MAX_LUMINANCE; }
	inline float	GetAdaptationSpeed() { return m_fAdaptationSpeed; }

	inline void 	SetHDREnable(bool bEnable) { 
		m_bHDR = bEnable; 
	}
	inline void 	SetDebugViewEnable(bool bEnable) { m_bDebugRT = bEnable; }
//	inline void 	SetBlueShiftEnable(bool bEnable) { m_bBlueShift = bEnable; }
	inline void 	SetBloomEnable(bool bEnable) { m_bBloom = bEnable; }
	inline void 	SetBloomScale(float fValue) { m_fBloomScale = fValue;}
	inline void 	SetGlareEnable(bool bEnable) { 
		m_bGlare = bEnable; 
	}
	inline void 	SetGlareScale(float fValue) { m_fGlareScale = fValue;}
	inline void 	SetGlareType(int nType)	{

		m_eGlareType = ( EGLARELIBTYPE )nType;
		m_GlareDef.Initialize( m_eGlareType );
	}
	inline void 	SetToneMapEnable(bool bEnable) { m_bToneMap = bEnable; }
//	inline void		SetDarkBarrierRatio(float fValue) { m_fDarkBarrierRatio = fValue; }
	inline void		SetBrightBarrierRatio(float fValue) { m_fBrightBarrierRatio = fValue; }
	inline void		SetLimitMin(float fValue) { m_fLimitMin = MAX_LUMINANCE * fValue; }
	inline void		SetLimitMax(float fValue) { m_fLimitMax = MAX_LUMINANCE * fValue; }
	inline void		SetFullFrameEnable(bool bEnable) { m_bFullFrame = bEnable; }
	inline void		SetAdaptationSpeed(float fValue) { m_fAdaptationSpeed = fValue; }

	void			SaveToXML(MXmlElement &element);
	void			LoadToXML(MXmlElement &element);

	void			SetConfigPath(string strPath);
	void			LoadConfig();
};

}

#endif
