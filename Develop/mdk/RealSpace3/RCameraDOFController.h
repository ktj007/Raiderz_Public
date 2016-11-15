#ifndef _RCAMERA_DOF_CONTROLLER_H
#define _RCAMERA_DOF_CONTROLLER_H

#include "RCameraSceneNodeController.h"
#include "RShaderFX.h"
#include "RListener.h"



// Depth Of Field를 처리하는 클래스
// 기본적인 구조는 RCameraHDRController를 C&P해서 편집...
namespace rs3 {


	class RS_API RCameraDOFController : public RCameraSceneNodeController, public RListener
	{
	public:
		enum DOF_FILTER_TYPE
		{
			DFT_COC,
			DFT_COC_N_GAUSSIAN,
			DFT_GAUSSIAN,
		};

		RCameraDOFController( RCameraSceneNode* pCameraSceneNode );
		virtual ~RCameraDOFController();

	protected:
		DOF_FILTER_TYPE	m_eFiterType;

		RShaderFX*		m_pDOFShader;	

		// Render Target Texture
		//RTexture* 		m_pColorBuffer;				// 참조 혹은 생성한 것으로 참조
		RTexture* 		m_pColorBufferOwn;				// MRT가 사용 되지 않았을 시 RT를 생성
		RTexture* 		m_pInfoBufferExpandBlurred;	
		
		RTexture* 		m_pColorBufferOwnOrg;			// MRT가 사용 되지 않았을 시 RT를 생성
		// 가우시안 블러용
		RTexture* 		m_pGaussianBuffer;
		// 최종 사용 될 정보 버퍼
		RTexture* 		m_pDepthBlurBuffer;
		RTexture* 		m_pDepthBlurBufferBlured;
		RTexture* 		m_pDepthBlurBufferBluredExpanded;

		// what about surface???

		// RT width and height
		DWORD m_dwWidth;
		DWORD m_dwHeight;
		// RT of Gaussian width and height
		DWORD m_dwWidthGaussian;
		DWORD m_dwHeightGaussian;

		int	m_nNumTaps;

		bool m_bEdgeBlur;

		// CoC size
		FLOAT m_fCoCSize;

		// 화면 중앙의 포커스 거리
		FLOAT m_fFocusDist;
		// Scheuemann의 방식을 사용 할 떄의 변수 추가
		FLOAT m_fNearBlurDist;
		FLOAT m_fFarBlurDist;
		FLOAT m_fMaxBlurOfBehind;

		// 기존 코드와의 호환을 위한 컨트롤용으로 놔둔다. 이 값이 셋팅 되면 Near와 Far를 자동 설정 되게..
		FLOAT m_fFocusRange;

		bool	m_bActive;
		bool	m_bDebugRT;

		RListener*		m_pEventListener;			///< OnRestoreDevice / OnLostDevice 이벤트 수신객체
		//string			m_strConfigPath;			// 게임 상에서 실시간으로 값들이 조절이 될 텐데 설정 파일이 필요한지..

	public:
		RCameraSceneNode*		m_pCameraSceneNode;

		virtual void	UpdateCameraController(RMatrix& _rMatApplyToCamera) {}
		virtual void	OnClear();		
		virtual void	OnPostRender();	
		virtual void	OnRemoveFromCamera();
		virtual void	OnPreRender( RInferredRenderingStrategy* pInferredRenderingStrategy);	// 랜더 타겟을 관리 - 090617, OZ

		bool 			Create();
		void 			Destroy();
		void			Restore();

		void			SetupViewportScale();
		void			SetupFilterKernel();

		// 가우시안 적용 시
		float			GaussianDistribution( float x, float y, float rho );
		bool 			GetSampleOffsets_GaussBlur5x5( int nTexWidth, int nTexHeight, RVector2* vTexCoordOffset, RVector4* vSampleWeight, FLOAT fMultiplier = 1.0f );	// HDR 컨트롤러의 것을

		// 구현
		void 			DrawFullScreenQuad(float fLeftU, float fTopV, float fRightU, float fBottomV, int b = 0);
		void			RenderDOF_CoC();
		void			RenderDOF_CoC_Gaussian();
		void			RenderOverlay();
		//virtual bool 	CreateFromXML(MXmlElement &element, bool bBackgroundCreation = false){ return true; } // do u need ?
		virtual void 	OnRender();
		//virtual bool 	FrustumTest(const RViewFrustum& Frustum, RFRUSTUMTESTMETHOD method /* = RFTM_FAST */) { return true; }	// need ?

		//RTexture*		GetDOFBuffer() { return m_pDOFBuffer; } need?

		inline bool 	GetDOFEnable() { return m_bActive; }
		inline bool 	GetDebugViewEnable() { return m_bDebugRT; }

		//inline void 	SetDebugViewEnable(bool bEnable) { m_bDebugRT = bEnable; } need?

		//void			SaveToXML(MXmlElement &element); need?
		//void			LoadToXML(MXmlElement &element); need?

		//void			SetConfigPath(string strPath); need?
		//void			LoadConfig(); need?

		void	RefreshParamsBasisFocusRange(){
			m_fNearBlurDist = m_fFocusDist - m_fFocusRange/2;
			m_fFarBlurDist = m_fFocusDist + m_fFocusRange/2;
		}

		void Active( bool b)			{ m_bActive = b;				}
		bool IsActive()					{ return m_bActive;				}

		FLOAT GetCoCSize()				{ return m_fCoCSize;			}
		FLOAT GetFocusDist()			{ return m_fFocusDist;			}
		FLOAT GetFocusRange()			{ return m_fFocusRange;	}
		FLOAT GetFocusRangeValue()		{ return 2.0f / m_fFocusRange;	}

		void SetFocusDist( float f)		{ m_fFocusDist = f;				}
		void SetFocusRange( float f)	{ m_fFocusRange = f;		RefreshParamsBasisFocusRange();	}
		void SetCoCSize(FLOAT size)		{ m_fCoCSize = size;			}

		void AddFocusDist( float f)		{ m_fFocusDist += f;	if( m_fFocusDist<0)		m_fFocusDist = 0;		}
		void AddFocusRange( float f)	{ m_fFocusRange += f;	if( m_fFocusRange<0)	m_fFocusRange = 0;	RefreshParamsBasisFocusRange();	}
		void AddCoCSize(FLOAT size)		{ m_fCoCSize += size;	if( m_fCoCSize<0)		m_fCoCSize = 0;		}

		void SetEdgeBlur( bool b= true)	{ m_bEdgeBlur = b;				}
		bool IsEdgeBlur()				{ return m_bEdgeBlur;			}

		void SetNearBlurDist( float f)		{ m_fNearBlurDist = f;				}
		void SetFarBlurDist( float f)		{ m_fFarBlurDist = f;				}
		void SetMaxBlurOfBehind( float f)	{ m_fMaxBlurOfBehind = f;			}	// 0~1

		FLOAT GetNearBlurDist()				{ return m_fNearBlurDist;			}
		FLOAT GetFarBlurDist()				{ return m_fFarBlurDist;			}
		FLOAT GetMaxBlurOfBehind()			{ return m_fMaxBlurOfBehind;		}

		
	};

}

#endif
