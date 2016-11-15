#ifndef _RCAMERA_SCREEN_MOTION_BLUR_CONTROLLER_H
#define _RCAMERA_SCREEN_MOTION_BLUR_CONTROLLER_H

#include "RCameraSceneNodeController.h"
#include "RShaderFX.h"
#include "RListener.h"


enum SCREEN_MOTION_BLUR_TYPE {
	SMB_BY_CAM_MOVE = 0,
	SMB_BY_EPICENTER
};

enum SCAT_ANI_TYPE {
	SAT_NORMAL = 0,
	SAT_FAST_SCAT,
	SAT_SLOW_SCAT,
	SAT_GATTER,
};


// Motion Blur ( Screen) 를 처리하는 클래스
// 기본적인 구조는 RCameraHDRController를 C&P해서 편집...
namespace rs3 {


	class RS_API RCameraScreenMotionBlurController : public RCameraSceneNodeController, public RListener
	{
	public:
		RCameraScreenMotionBlurController( RCameraSceneNode* pCameraSceneNode );
		virtual ~RCameraScreenMotionBlurController();

	protected:

		RShaderFX*		m_pShader;	

		// Render Target Texture
		RTexture* 		m_pColorBufferOwn;			// MRT가 사용 되지 않았을 시 RT를 생성
		// 복원용
		RTexture* 		m_pColorBufferOwnOrg;		// MRT가 사용 되지 않았을 시 RT를 생성
		
		// what about surface???

		// RT width and height
		DWORD m_dwWidth;
		DWORD m_dwHeight;
		
		bool	m_bActive;
		bool	m_bLockFlag;
		bool	m_bDebugRT;

		RMatrix	m_matPrevVP;						// 이전 프레임의 뷰프로젝션. 월드도 포함 해야 하는가?
		RMatrix	m_matInvVP;							// 뷰프로젝션의 인버스. 월드도 포함 해야 하는가?

		RMatrix		m_matPrevV;						// 이전 뷰 매트릭스를 그대로 사용 하는게 아니라 정해진 속력으로 보간하자..
		RVector3	m_vPrePos;
		RVector3	m_vPreUp;
		RVector3	m_vPreDir;
		RVector3	m_vMoveDir;

		int		m_nSamplerCnt;						// 샘플러수. 픽셀 셰이더에서는 샘플링 횟수로 쓰임
		float	m_fSpeed;							// 스피드. 이동 스피드. 꼬리의 길이. 
		bool	m_invalidateRotate;					// 회전 정보를 블러링 방향 설정에서 저외 할 것인지 아닌지

		BYTE	m_nType;

		RVector4	m_vEpicenter;					// 텍스쳐 좌표계 기준 0~1의 진앙지
		DWORD	m_dwScatStartTime;					// GetLastElapsedTime()의 값 저장 
		DWORD	m_dwPlayTime;						// Scat 에니메이션 총 시간
		DWORD	m_dwScatTime;						// 에니메이션 총 시간 중 흩어지는 부분 시간. 나머지는 모아지는 시간
		float	m_fScatSpeed;						// 에니메이션 중 최고점의 스피드. 이 값을 기반으로 m_fSpeed를 셋팅
		BYTE	m_nScarFactorType;					// 에니메이션의 scat 팩터

		RListener*		m_pEventListener;			///< OnRestoreDevice / OnLostDevice 이벤트 수신객체

	public:
		RCameraSceneNode*		m_pCameraSceneNode;

		virtual void	UpdateCameraController(RMatrix& _rMatApplyToCamera) {}
		virtual void	OnClear();		
		virtual void	OnPostRender();	
		virtual void	OnRemoveFromCamera();
		virtual void	OnPreRender( RInferredRenderingStrategy* pInferredRenderingStrategy);

		bool 			Create();
		void 			Destroy();
		void			Restore();
		void			FrameMove();	// 카메라 컨트롤러는 프레임 무브가 따로 없다. 랜더에서 호출 되도록 하자.

		// 구현
		void 			DrawFullScreenQuad(float fLeftU, float fTopV, float fRightU, float fBottomV, int b = 0);
		void			Render();
		void			RenderOverlay();
		void			RenderColorFill();
		virtual void 	OnRender();

		void			SetEpicenterScreenCoord( float x, float y)		{ m_vEpicenter = RVector4( x/(float)REngine::GetDevice().GetScreenWidth(), y/(float)REngine::GetDevice().GetScreenHeight(), 0, 0);	}
		void			SetEpicenterProjectionCoord( float x, float y);
		void			SetEpicenterWorld( RCameraSceneNode* pCurrentCamera, RVector3& v);
		// By Epicenter일시 폭파 블러를 자동 에니메이션. m_fSpeed는 해당 시간동안 강제 설정이 안된다.
		void			PlayScat( float fPlayTime, float fScatTime, float fSpeed, BYTE nScarFactorType);

		inline bool 	GetBlurEnable() { return m_bActive; }
		inline bool 	GetDebugViewEnable() { return m_bDebugRT; }

		inline void		SetActive( bool b)		{ if(!m_bLockFlag) m_bActive = b;	}
		inline bool		IsActive()				{ return m_bActive;					}

		// 스피드는 현재 카메라 위치와 이전 프레임의 카메라 위치 차를 스피드만큼 사용. m_fSpeed가 0이면 이전 프레임의 위치 그대로 사용
		inline void		SetSpeed( float f)		{ m_fSpeed = f;				}
		inline float	GetSpeed()				{ return m_fSpeed;			}
		// 샘플러수는는 15으로 제한한다. 굳이 15 이상으로 써도 작동은 하지만 셰이더에서 %15연산으로 사용 한다.
		// 많으면 많을수록 꼬리를 여러번 만들어 낸다.
		inline void		SetSamplerCount( int n)	{ m_nSamplerCnt = n;			}
		inline int		GetSamplerCount()		{ return m_nSamplerCnt;			}
		// 회전 정보를 블러링 방향 설정에서 저외 할 것인지 아닌지
		inline void		SetInvalidateRotate( bool b)	{ m_invalidateRotate = b;		}
		inline bool		GetInvalidateRotate()			{ return m_invalidateRotate;	}

		inline void		SetBasisByCamMove()				{ if(!m_bLockFlag) m_nType = SMB_BY_CAM_MOVE;	m_dwScatStartTime = 0;}
		inline void		SetBasisByEpicenter()			{ if(!m_bLockFlag) m_nType = SMB_BY_EPICENTER;	}

		
	};

}

#endif
