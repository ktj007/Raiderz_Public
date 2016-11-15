
#pragma once

#include "RShaderComponentID.h"
#include "RProjectionShadow.h"
#include "RCameraSceneNode.h"
#include "RListener.h"
#include "RPostEffectManager.h"

namespace rs3 {

class RCameraSceneNode;
class RShadowStrategy;
class RRenderingCallbackInterface;
class RLightingShader;
class RGIProcessor;



enum RRENDERINGMETHOD 
{
	RENDERING_FORWARD	= 0x00,
	RENDERING_INFERRED,
	RENDERING_DEFERRED,
	RENDERING_NULL,
	RENDERING_END
};

enum RRENDERING_MODE
{
	RRM_FULL_LIGHT =0,
	RRM_NO_SPECULAR_RIM_LIGHT,
	RRM_DIFFUSE_ONLY
};

class ROcclusionCullingMgrSet;
class RS_API RRenderingStrategy : 
	public RListener	// 스크린 크기 변경 등에 G,L,M버퍼등의 버퍼를 백버퍼 크기와 동일하게 유지 시켜 주기 위해서 
{
public:
	RRenderingStrategy();
	virtual ~RRenderingStrategy();

	virtual void Init( int nW, int nH );
	virtual void Destroy();

	virtual void Resize( int nW, int nH );
	virtual void Invalidate();
	virtual void Restore( DWORD dwWidth, DWORD dwHeight );
	virtual void SetBackgroundBufferDivideSize( DWORD dwHalf) { }

 	virtual void RenderScene( RCameraSceneNode *pViewCamera, RWorldSceneNode *pWorld, RShadowStrategy* pShadowStrategy, RRenderingCallbackInterface* pRenderingCallbackInterface) {}
 		
	virtual RTexture* GetDepthBuffer()								{ return NULL; }
	virtual RTexture* GetBackgroundBuffer()							{ return NULL; }


	RRENDERINGMETHOD getMethod()	{ return m_eMethod;}

	void UsetLighting( bool b)				{ m_bUseLighting = b;}
	bool IsUsetLighting()					{ return m_bUseLighting;}

	DWORD GetWidth()						{ return m_dwWidth; }
	DWORD GetHeight()						{ return m_dwHeight; }

	void CalculateLOD( RCameraSceneNode* pViewCamera, RCameraSceneNode* pCamera);
	void DoPreRender( RCameraSceneNode* pCamera );
	void DoPostRender( RCameraSceneNode* pCamera );

	void OcclusionCullAndSetOccludedSceneNodes(RWorldSceneNode* pWorldScene,ROcclusionCullingMgrSet* pOcclusionCullingMgrSet,RCameraSceneNode *pCamera,SCENE_SET* pOccluderSceneList);
	void ResetOcludedSceneNodes(ROcclusionCullingMgrSet* pOcclusionCullingMgrSet);


	// 후처리들... 카메라 씬 노드에 있던 것들 싸그리 씬매니져로 이사가고
	// 씬매니져에서 이것들을 호출해준다.
	RPostEffectInterface*		GetPostEffect(POST_EFFECT_TYPE eType)		{ return m_pPostEffectManager ? m_pPostEffectManager->Get(eType) : NULL;	}

	// HDR관현
	void						SetHDREnable( bool b)						{ if( m_pPostEffectManager) m_pPostEffectManager->SetHDREnable( b);			}
	bool						IsEnableHDR()								{ return ( m_pPostEffectManager && m_pPostEffectManager->IsEnableHDR());	}
	bool						GetHDRBrightPassThreshold( RVector& vOutValue ) { return m_pPostEffectManager ? m_pPostEffectManager->GetHDRBrightPassThreshold(vOutValue) : false; }
	bool						SetHDRBrightPassThreshold( RVector vValue )	{ return m_pPostEffectManager ? m_pPostEffectManager->SetHDRBrightPassThreshold(vValue) : false; }
	void						SetHDRViewRenderTarget( bool b)				{ if( m_pPostEffectManager) m_pPostEffectManager->SetHDRViewRenderTarget( b);		}
	bool						IsHDRViewRenderTarget()						{ return ( m_pPostEffectManager && m_pPostEffectManager->IsHDRViewRenderTarget());	}

	// DOF 관련
	void						SetDOFEnable( bool b)						{ if( m_pPostEffectManager) m_pPostEffectManager->SetDOFEnable( b);					}
	bool						IsEnableDOF()								{ return ( m_pPostEffectManager && m_pPostEffectManager->IsEnableDOF());			}
	void						SetDOFActive( bool bActive)					{ if( m_pPostEffectManager) m_pPostEffectManager->SetDOFActive(bActive);			}
	bool						AddDOFFocalDist(float f)					{ return ( m_pPostEffectManager && m_pPostEffectManager->AddDOFFocalDist(f));		}
	bool						AddDOFFocalRange(float f)					{ return ( m_pPostEffectManager && m_pPostEffectManager->AddDOFFocalRange(f));		}
	bool						AddDOFCOCSize(float f)						{ return ( m_pPostEffectManager && m_pPostEffectManager->AddDOFCOCSize(f));			}
	float						GetDOFFocalDist()							{ return m_pPostEffectManager ? m_pPostEffectManager->GetDOFFocalDist() : -1;		}
	float						GetDOFFocalRange()							{ return m_pPostEffectManager ? m_pPostEffectManager->GetDOFFocalRange() : -1;		}
	float						GetDOFCOCSize()								{ return m_pPostEffectManager ? m_pPostEffectManager->GetDOFCOCSize() : -1;			}
	float						GetDOFNearBlurDist()						{ return m_pPostEffectManager ? m_pPostEffectManager->GetDOFNearBlurDist() : -1;	}
	float						GetDOFFarBlurDist()							{ return m_pPostEffectManager ? m_pPostEffectManager->GetDOFFarBlurDist() : -1;		}
	float						GetDOFMaxBlurOfBehind()						{ return m_pPostEffectManager ? m_pPostEffectManager->GetDOFMaxBlurOfBehind() : -1;	}
	bool						IsDOFEdgeBlur()								{ return m_pPostEffectManager ? m_pPostEffectManager->IsDOFEdgeBlur() : false;		}
	bool						SetDOFFocalDist(float f)					{ return ( m_pPostEffectManager && m_pPostEffectManager->SetDOFFocalDist(f));		}
	bool						SetDOFFocalRange(float f)					{ return ( m_pPostEffectManager && m_pPostEffectManager->SetDOFFocalRange(f));		}
	bool						SetDOFCOCSize(float f)						{ return ( m_pPostEffectManager && m_pPostEffectManager->SetDOFCOCSize(f));			}
	bool						SetDOFNearBlurDist(float f)					{ return ( m_pPostEffectManager && m_pPostEffectManager->SetDOFNearBlurDist(f));	}
	bool						SetDOFFarBlurDist(float f)					{ return ( m_pPostEffectManager && m_pPostEffectManager->SetDOFFarBlurDist(f));		}
	bool						SetDOFMaxBlurOfBehind(float f)				{ return ( m_pPostEffectManager && m_pPostEffectManager->SetDOFMaxBlurOfBehind(f));	}
	bool						SetDOFEdgeBlur( bool b= true)				{ return ( m_pPostEffectManager && m_pPostEffectManager->SetDOFEdgeBlur(b));		}
	// ScreenMotionBlur 관련
	void						SetScreenMotionBlurEnable( bool b)			{ if( m_pPostEffectManager) m_pPostEffectManager->SetScreenMotionBlurEnable(b);				}
	bool						IsEnableScreenMotionBlur()					{ return ( m_pPostEffectManager && m_pPostEffectManager->IsEnableScreenMotionBlur());		}
	void						SetScreenMotionBlurActive( bool bActive)	{ if( m_pPostEffectManager) m_pPostEffectManager->SetScreenMotionBlurActive(bActive);		}
	void						SetScreenMotionBlurSpeed( float f)			{ if( m_pPostEffectManager) m_pPostEffectManager->SetScreenMotionBlurSpeed(f);		}
	void						SetScreenMotionBlurSamplerCount( int n)		{ if( m_pPostEffectManager) m_pPostEffectManager->SetScreenMotionBlurSamplerCount(n);		}
	void						SetScreenMotionBlurInvalidateRotate( bool b){ if( m_pPostEffectManager) m_pPostEffectManager->SetScreenMotionBlurInvalidateRotate(b);		}
	void						SetScreenMotionBlurBasisByCamMove()			{ if( m_pPostEffectManager) m_pPostEffectManager->SetScreenMotionBlurBasisByCamMove();		}
	void						SetScreenMotionBlurBasisByEpicenter()		{ if( m_pPostEffectManager) m_pPostEffectManager->SetScreenMotionBlurBasisByEpicenter();		}
	void						SetScreenMotionBlurEpicenterScreenCoord( float x, float y)	{ if( m_pPostEffectManager) m_pPostEffectManager->SetScreenMotionBlurEpicenterScreenCoord( x,y);		}
	void						SetScreenMotionBlurEpicenterProjectionCoord( float x, float y)	{ if( m_pPostEffectManager) m_pPostEffectManager->SetScreenMotionBlurEpicenterProjectionCoord(x,y);		}
	void						SetScreenMotionBlurEpicenterWorld( RCameraSceneNode* pCurrentCamera, RVector3& v)	{ if( m_pPostEffectManager) m_pPostEffectManager->SetScreenMotionBlurEpicenterWorld( pCurrentCamera, v);		}
	void						SetScreenMotionBlurPlayScat( float fPlayTime, float fScatTime, float fSpeed, BYTE nScarFactorType)	{ if( m_pPostEffectManager) m_pPostEffectManager->SetScreenMotionBlurPlayScat( fPlayTime, fScatTime, fSpeed, nScarFactorType);		}

	// AccumMotionBlur 관련
	void						SetAccumMotionBlurEnable( bool b)			{ if( m_pPostEffectManager) m_pPostEffectManager->SetAccumMotionBlurEnable(b);					}
	bool						IsEnableAccumMotionBlur()					{ return ( m_pPostEffectManager && m_pPostEffectManager->IsEnableAccumMotionBlur());			}
	void						SetAccumMotionBlurActive( bool b)			{ if( m_pPostEffectManager) m_pPostEffectManager->SetAccumMotionBlurActive(b);					}
	bool						IsAccumMotionBlurBeing()					{ return ( m_pPostEffectManager && m_pPostEffectManager->IsAccumMotionBlurBeing());				}
	bool						SetAccumMotionBlurDecreaseAlpha( float f)	{ return ( m_pPostEffectManager && m_pPostEffectManager->SetAccumMotionBlurDecreaseAlpha(f));	}
	bool						SetAccumMotionBlurAccumRate( float f)		{ return ( m_pPostEffectManager && m_pPostEffectManager->SetAccumMotionBlurAccumRate(f));	}

	// PixelMotionBlur 관련
	void						SetPixelMotionBlurEnable( bool b)			{ if( m_pPostEffectManager) m_pPostEffectManager->SetPixelMotionBlurEnable(b);					}
	bool						IsEnablePixelMotionBlur()					{ return ( m_pPostEffectManager && m_pPostEffectManager->IsEnablePixelMotionBlur());			}
	void						SetPixelMotionBlurActive( bool b)			{ if( m_pPostEffectManager) m_pPostEffectManager->SetPixelMotionBlurActive(b);					}
	void						SetPixelMotionBlurSamplerCount( int n)		{ if( m_pPostEffectManager) m_pPostEffectManager->SetPixelMotionBlurSamplerCount(n);			}

	//Distortion 관련
	void						SetDistortionEnable(bool b)					{ if( m_pPostEffectManager) m_pPostEffectManager->SetDistortionEnable(b);						}
	bool						IsEnableDistortion()						{ return (m_pPostEffectManager && m_pPostEffectManager->IsEnableDistortion());					}

	//PEEdge 관련
	void						SetPEQuality(int _iBlurCount, int _iScreenSize);
	void						SetPEEdgeEnable(bool b)						{ if( m_pPostEffectManager) m_pPostEffectManager->SetPEEdgeEnable(b);							}
	bool						IsEnablePEEdge()							{ return (m_pPostEffectManager && m_pPostEffectManager->IsEnablePEEdge());						}
	void						SetPEFactorTickNess(const float _fTickness, const float _fBlutFactor, const float _fColorPower);

	// 소프트 파티클 관련
	void						SetSoftParticleEnable(bool b)				{ m_bSoftParticle = b; }
	bool						IsEnableSoftParticle()						{ return m_bSoftParticle; }

	// 소프트 파티클 관련
	void						SetEarlyZCullEnable(bool b)					{ m_bEarlyZCull = b; }
	bool						IsEnableEarlyZCull()						{ return m_bEarlyZCull; }

	bool						GetGammaCorrect()							{ return m_bUseGammaCorrect; }
	virtual	void				SetGammaCorrect( bool _bGammaCorrect );

	int							GetFXAA_Level()							{ return m_iFXAA_Lv; }
	virtual	void				SetFXAA_Level( int _iFxaa_Lv );	


	//For SSAO
	void						UseSSAO( int b);
	int							IsUsingSSAO();


	void						DisablePEEdge();

	RPostEffectManager*			GetPostEffectManager()	{ return m_pPostEffectManager; }


	void						SetRenderingMode(RRENDERING_MODE mode)		{ m_RenderingMode =mode; }
	RRENDERING_MODE				GetRenderingMode()							{ return m_RenderingMode; }
	virtual int					GetMRTCount()								{ return 1; }

protected:
	void						RenderSpotLightShadowMap(RCameraSceneNode* pViewCamera, SCENE_SET* pSpotLightSet);

	virtual void				SetStateGStagePass() {}
	void						RenderGStagePass( RCameraSceneNode* pCurrentCamera, RRENDERPASS beginPass, RRENDERPASS endPass);
	void						RenderGStagePassForProjectionShadow( RCameraSceneNode* pCurrentCamera, RRENDERPASS beginPass, RRENDERPASS endPass);
	virtual void				ResetStateGStagePass() {}

	void						SetLStageShaderConstant(RCameraSceneNode* pCamera, DWORD dwBufferWidth, DWORD dwBufferHeight);
	virtual void				SetLStageTexture() {}
	void						RenderProjectionShadowBuffer(RCameraSceneNode* pCamera);
	void						RenderDirectionalLightingToLBuffer( RCameraSceneNode* pCamera, SCENE_SET* pLightSet, const RTextureInfo& LBufferInfo );
	void						RenderPointLightingToLBuffer( RCameraSceneNode* pCurrentCamera,SCENE_SET* pLightSet );
	void						RenderSpotLightingToLBuffer( RCameraSceneNode* pCurrentCamera,SCENE_SET* pLightSet );
	void						RenderLight(RCameraSceneNode* pCamera, RCullingMgr* pCullingMgr, const RTextureInfo& LBufferInfo);
	void						RenderGIToLBuffer( RCameraSceneNode* pCamera, const RTextureInfo& LBufferInfo);
	virtual void				RestoreLStageState() {}
	void						RenderLStagePass(RCameraSceneNode* pCurrentCamera, RGIProcessor* pGIProcessor, RCullingMgr* pCullingMgr, const RTextureInfo& LBufferInfo);

	void						RenderKStagePass( RCameraSceneNode* pCurrentCamera, RRENDERPASS pass, RTexture* pViewNormalMap, RTexture* pViewDepthMap, RTexture* pViewLightMap, RTexture* pViewDSFMap);

	virtual void				SetStateMStagePass( RCameraSceneNode* pCurrentCamera, RTexture* pViewNormalMap, RTexture* pViewDepthMap, RTexture* pViewLightMap, RTexture* pViewDSFMap);
	void						RenderMStagePass( RCameraSceneNode* pCurrentCamera, RRENDERPASS beginPass, RRENDERPASS endPass, RTexture* pViewNormalMap, RTexture* pViewDepthMap, RTexture* pViewLightMap, RTexture* pViewDiffuseMap);
	virtual void				ResetStateMStagePass(void);

	virtual void				SetStateZStagePass( RCameraSceneNode* pCurrentCamera);
	void						RenderZStagePass( RCameraSceneNode* pCurrentCamera, RRENDERPASS beginPass, RRENDERPASS endPass);
	virtual void				ResetStateZStagePass(void);

	DWORD m_dwWidth;
	DWORD m_dwHeight;
	RRENDERINGMETHOD	m_eMethod;
	bool	m_bUseLighting;

	RListener*				m_pLostEventListener;			///< OnLostDevice 이벤트 수신객체
	RListener*				m_pRestoreEventListener;		///< OnRestoreDevice 이벤트 수신객체
	RPostEffectManager*		m_pPostEffectManager;
	RLightingShader*		m_pLightingShader;


	bool					m_bSoftParticle;
	RGIProcessor*			m_pGIProcessor;
	bool					m_bEarlyZCull;

	RRENDERING_MODE			m_RenderingMode;
	bool					m_bUseGammaCorrect;
	int						m_iFXAA_Lv;
};

}
