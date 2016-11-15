#pragma once

#include "RPostEffectInterface.h"
#include "RPostEffectDOF.h"
#include "RPostEffectHDR.h"
#include "RPostEffectScreenMotionBlur.h"
#include "RPostEffectAccumMotionBlur.h"
#include "RPostEffectPixelMotionBlur.h"

namespace rs3 {

	typedef std::map< POST_EFFECT_TYPE, RPostEffectInterface* > POST_EFFECT_MAP;

	class RPostEffectEnumerator
	{
	public:
		RPostEffectEnumerator( const POST_EFFECT_MAP& mapPostEffect ) : m_mapPostEffect(mapPostEffect), m_itor(mapPostEffect.begin())
		{
		}

		~RPostEffectEnumerator()
		{
		}

		bool HasElement()
		{
			return ( m_itor != m_mapPostEffect.end() );
		}

		void Next()
		{
			++m_itor;
		}

		RPostEffectInterface* Get() const
		{
			RPostEffectInterface* pPostEffect = m_itor->second;
			return pPostEffect;
		}

	private:
		const POST_EFFECT_MAP& m_mapPostEffect;
		POST_EFFECT_MAP::const_iterator m_itor;
	};

	class RS_API RPostEffectManager
	{
	public:
		RPostEffectManager(void);
		virtual ~RPostEffectManager(void);

	protected:
		POST_EFFECT_MAP		m_mapPostEffect;		// 후처리들 맵

		DWORD m_dwW, m_dwH;

		// 입력용 출력용 서로 인덱스를 바꿔가며 갈아타기
		RTexture*	m_pBuffer[4];
		bool		m_nBufferIdx;
		RTexture*	m_pBufferForHDR;
		RTexture*	m_pBufferOrg;		// 원래의 랜더 타겟(보통은 백버퍼) 백업용 포인터
		RTexture*	m_pCurrentTarget;	// 현재 셋팅되는 랜더 타겟 포인터

	public:
		virtual bool Init(  int nW, int nH);
		virtual void Destroy();

		virtual bool Resize( int nW, int nH );
		virtual void Invalidate();
		virtual void Restore();

		void						PreRender( RCameraSceneNode* pCamera, bool bDepthClear);
		void						RenderPost( RCameraSceneNode* pCamera, RTexture* pNormalBuffer, RTexture* pDepthBuffer);

		bool						IsBeingEnableInstance()	{ return (!m_mapPostEffect.empty());		}

		RPostEffectInterface*		Get(POST_EFFECT_TYPE eType);

	protected:
		void						Add(POST_EFFECT_TYPE eType);
		void						Remove(POST_EFFECT_TYPE eType);
		RPostEffectInterface*		CreateInstance(POST_EFFECT_TYPE eType);

		bool						CreateBuffers();
		void						DestroyBuffers();

		RPostEffectInterface*		GetFirstActivePostEffect();
		RPostEffectInterface*		GetLastActivePostEffect();

	public:
		RPostEffectEnumerator		GetPostEffectEnumerator();

		void						SetEnable(POST_EFFECT_TYPE eType, bool bEnable);
		bool						IsEnable(POST_EFFECT_TYPE eType);

		// 랜더링 전략에서 이들을 호출한다.
		// HDR
		void						SetHDREnable( bool bEnable)					{ SetEnable( PE_HDR, bEnable);	}
		bool						IsEnableHDR()								{ return IsEnable(PE_HDR);		}
		bool						GetHDRBrightPassThreshold( RVector& vOutValue );
		bool						SetHDRBrightPassThreshold( RVector vValue );
		void						SetHDRViewRenderTarget( bool bEnable)		{ if( Get(PE_HDR)) ((RPostEffectHDR*)Get(PE_HDR))->SetDebugViewEnable(bEnable); }
		bool						IsHDRViewRenderTarget()						{ return Get(PE_HDR) ? ((RPostEffectHDR*)Get(PE_HDR))->GetDebugViewEnable() : false; }

		// DOF
		void						SetDOFEnable( bool bEnable)					{ SetEnable( PE_DOF, bEnable);	}
		bool						IsEnableDOF()								{ return IsEnable(PE_DOF);		}
		void						SetDOFActive( bool bActive)					{ if( Get(PE_DOF)) ((RPostEffectDOF*)Get(PE_DOF))->SetActive(bActive);		}
		bool						AddDOFFocalDist(float f)					{ return ( Get(PE_DOF) && ((RPostEffectDOF*)Get(PE_DOF))->AddFocalRange(f));		}
		bool						AddDOFFocalRange(float f)					{ return ( Get(PE_DOF) && ((RPostEffectDOF*)Get(PE_DOF))->AddFocalRange(f));		}
		bool						AddDOFCOCSize(float f)						{ return ( Get(PE_DOF) && ((RPostEffectDOF*)Get(PE_DOF))->AddCOCSize(f));			}
		float						GetDOFFocalDist()							{ return Get(PE_DOF) ? ((RPostEffectDOF*)Get(PE_DOF))->GetFocalDist() : -1;		}
		float						GetDOFFocalRange()							{ return Get(PE_DOF) ? ((RPostEffectDOF*)Get(PE_DOF))->GetFocalRange() : -1;		}
		float						GetDOFCOCSize()								{ return Get(PE_DOF) ? ((RPostEffectDOF*)Get(PE_DOF))->GetCOCSize() : -1;			}
		float						GetDOFNearBlurDist()						{ return Get(PE_DOF) ? ((RPostEffectDOF*)Get(PE_DOF))->GetNearBlurDist() : -1;	}
		float						GetDOFFarBlurDist()							{ return Get(PE_DOF) ? ((RPostEffectDOF*)Get(PE_DOF))->GetFarBlurDist() : -1;		}
		float						GetDOFMaxBlurOfBehind()						{ return Get(PE_DOF) ? ((RPostEffectDOF*)Get(PE_DOF))->GetMaxBlurOfBehind() : -1;	}
		int							GetDOFTapsCount()							{ return Get(PE_DOF) ? ((RPostEffectDOF*)Get(PE_DOF))->GetTapsCount() : -1;	}
		bool						IsDOFEdgeBlur()								{ return Get(PE_DOF) ? ((RPostEffectDOF*)Get(PE_DOF))->IsEdgeBlur() : false;			}
		bool						IsDOFUseNoise()								{ return Get(PE_DOF) ? ((RPostEffectDOF*)Get(PE_DOF))->IsUseNoise() : false;			}
		bool						SetDOFFocalDist(float f)					{ return ( Get(PE_DOF) && ((RPostEffectDOF*)Get(PE_DOF))->SetFocalDist(f));		}
		bool						SetDOFFocalRange(float f)					{ return ( Get(PE_DOF) && ((RPostEffectDOF*)Get(PE_DOF))->SetFocalRange(f));		}
		bool						SetDOFCOCSize(float f)						{ return ( Get(PE_DOF) && ((RPostEffectDOF*)Get(PE_DOF))->SetCOCSize(f));			}
		bool						SetDOFNearBlurDist(float f)					{ return ( Get(PE_DOF) && ((RPostEffectDOF*)Get(PE_DOF))->SetNearBlurDist(f));	}
		bool						SetDOFFarBlurDist(float f)					{ return ( Get(PE_DOF) && ((RPostEffectDOF*)Get(PE_DOF))->SetFarBlurDist(f));		}
		bool						SetDOFMaxBlurOfBehind(float f)				{ return ( Get(PE_DOF) && ((RPostEffectDOF*)Get(PE_DOF))->SetMaxBlurOfBehind(f));	}
		bool						SetDOFEdgeBlur( bool b= true)				{ return ( Get(PE_DOF) && ((RPostEffectDOF*)Get(PE_DOF))->SetEdgeBlur(b));		}
		bool						SetDOFUseNoise( bool b= true)				{ return ( Get(PE_DOF) && ((RPostEffectDOF*)Get(PE_DOF))->SetUseNoise(b));		}
		bool						SetDOFTapsCount(int f)						{ return ( Get(PE_DOF) && ((RPostEffectDOF*)Get(PE_DOF))->SetTapsCount(f));	}

		// ScreenMotionBlur 관련
		void						SetScreenMotionBlurEnable( bool bEnable)	{ SetEnable( PE_SCREEN_MOTION_BLUR, bEnable);	}
		bool						IsEnableScreenMotionBlur()					{ return IsEnable(PE_SCREEN_MOTION_BLUR);		}
		void						SetScreenMotionBlurActive( bool bActive)	{ if( Get(PE_SCREEN_MOTION_BLUR)) ((RPostEffectScreenMotionBlur*)Get(PE_SCREEN_MOTION_BLUR))->SetActive(bActive);		}
		void						SetScreenMotionBlurSpeed( float f)			{ if( Get(PE_SCREEN_MOTION_BLUR)) ((RPostEffectScreenMotionBlur*)Get(PE_SCREEN_MOTION_BLUR))->SetSpeed(f);		}
		void						SetScreenMotionBlurSamplerCount( int n)		{ if( Get(PE_SCREEN_MOTION_BLUR)) ((RPostEffectScreenMotionBlur*)Get(PE_SCREEN_MOTION_BLUR))->SetSamplerCount(n);		}
		void						SetScreenMotionBlurInvalidateRotate( bool b){ if( Get(PE_SCREEN_MOTION_BLUR)) ((RPostEffectScreenMotionBlur*)Get(PE_SCREEN_MOTION_BLUR))->SetInvalidateRotate(b);		}
		void						SetScreenMotionBlurBasisByCamMove()			{ if( Get(PE_SCREEN_MOTION_BLUR)) ((RPostEffectScreenMotionBlur*)Get(PE_SCREEN_MOTION_BLUR))->SetBasisByCamMove();		}
		void						SetScreenMotionBlurBasisByEpicenter()		{ if( Get(PE_SCREEN_MOTION_BLUR)) ((RPostEffectScreenMotionBlur*)Get(PE_SCREEN_MOTION_BLUR))->SetBasisByEpicenter();		}
		void						SetScreenMotionBlurEpicenterScreenCoord( float x, float y)	{ if( Get(PE_SCREEN_MOTION_BLUR)) ((RPostEffectScreenMotionBlur*)Get(PE_SCREEN_MOTION_BLUR))->SetEpicenterScreenCoord( x,y);		}
		void						SetScreenMotionBlurEpicenterProjectionCoord( float x, float y)	{ if( Get(PE_SCREEN_MOTION_BLUR)) ((RPostEffectScreenMotionBlur*)Get(PE_SCREEN_MOTION_BLUR))->SetEpicenterProjectionCoord(x,y);		}
		void						SetScreenMotionBlurEpicenterWorld( RCameraSceneNode* pCurrentCamera, RVector3& v)	{ if( Get(PE_SCREEN_MOTION_BLUR)) ((RPostEffectScreenMotionBlur*)Get(PE_SCREEN_MOTION_BLUR))->SetEpicenterWorld( pCurrentCamera, v);		}
		void						SetScreenMotionBlurPlayScat( float fPlayTime, float fScatTime, float fSpeed, BYTE nScarFactorType)	{ if( Get(PE_SCREEN_MOTION_BLUR)) ((RPostEffectScreenMotionBlur*)Get(PE_SCREEN_MOTION_BLUR))->PlayScat( fPlayTime, fScatTime, fSpeed, nScarFactorType);		}

		// AccumMotionBlur 관련
		void						SetAccumMotionBlurEnable( bool bEnable)		{ SetEnable( PE_ACCUM_MOTION_BLUR, bEnable);																				}
		bool						IsEnableAccumMotionBlur()					{ return IsEnable(PE_ACCUM_MOTION_BLUR);																					}
		void						SetAccumMotionBlurActive( bool bActive)		{ if( Get(PE_ACCUM_MOTION_BLUR)) ((RPostEffectAccumMotionBlur*)Get(PE_ACCUM_MOTION_BLUR))->SetActive(bActive);				}
		bool						IsAccumMotionBlurBeing()					{ return ( Get(PE_ACCUM_MOTION_BLUR) && ((RPostEffectAccumMotionBlur*)Get(PE_ACCUM_MOTION_BLUR))->IsBeingBlur());			}
		bool						SetAccumMotionBlurDecreaseAlpha( float f)	{ return ( Get(PE_ACCUM_MOTION_BLUR) && ((RPostEffectAccumMotionBlur*)Get(PE_ACCUM_MOTION_BLUR))->SetDecreaseAlpha(f));		}
		bool						SetAccumMotionBlurAccumRate( float f)		{ return ( Get(PE_ACCUM_MOTION_BLUR) && ((RPostEffectAccumMotionBlur*)Get(PE_ACCUM_MOTION_BLUR))->SetAccumRate(f));			}

		// PixelMotionBlur 관련
		void						SetPixelMotionBlurEnable( bool bEnable)		{ SetEnable( PE_PIXEL_MOTION_BLUR, bEnable);																	}
		bool						IsEnablePixelMotionBlur()					{ return IsEnable(PE_PIXEL_MOTION_BLUR);																		}
		void						SetPixelMotionBlurActive( bool b)			{ if( Get(PE_PIXEL_MOTION_BLUR)) ((RPostEffectPixelMotionBlur*)Get(PE_PIXEL_MOTION_BLUR))->SetActive(b);		}
		void						SetPixelMotionBlurSamplerCount( int n)		{ if( Get(PE_PIXEL_MOTION_BLUR)) ((RPostEffectPixelMotionBlur*)Get(PE_PIXEL_MOTION_BLUR))->SetSamplerCount(n);	}

		//Distortion 관련
		void						SetDistortionEnable( bool bEnable)			{ SetEnable(PE_DISTORTION, bEnable); }
		bool						IsEnableDistortion()						{ return IsEnable(PE_DISTORTION); }

		//PEEdge 관련
		void						SetPEEdgeEnable( bool bEnable)				{ SetEnable(PE_EDGE, bEnable); }
		bool						IsEnablePEEdge()							{ return IsEnable(PE_EDGE); }
		void						PEEdgeSet( bool _bProjection = false );
		void						PEEdgeEnable( bool _bEdge );
		void						PEEdgeQuality(int _iBlurCount, int _iScreenSize);
		void						PEFactorTickNess(const float _fTickness, const float _fBlutFactor, const float _fColorPower);
		void						AddPEEdgeCount();
		void						MinusPEEdgeCount();
	};

}