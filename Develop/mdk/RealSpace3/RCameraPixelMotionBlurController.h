#ifndef _RCAMERA_PIXEL_MOTION_BLUR_CONTROLLER_H
#define _RCAMERA_PIXEL_MOTION_BLUR_CONTROLLER_H

#include "RCameraSceneNodeController.h"
#include "RShaderFX.h"
#include "RListener.h"


// Motion Blur ( Pixel) 를 처리하는 클래스
// 기본적인 구조는 RCameraScreenMotionBlurController를 C&P해서 편집...


namespace rs3 {

	class RS_API RCameraPixelMotionBlurController : public RCameraSceneNodeController, public RListener
	{
	public:
		RCameraPixelMotionBlurController( RCameraSceneNode* pCameraSceneNode );
		virtual ~RCameraPixelMotionBlurController();

	protected:

		RShaderFX*		m_pShader;	

		// Render Target Texture. 퀄리티를 높이기 위해 두장 번갈아 가면서 서로를 참조하여 샘플링 반복.
		RTexture* 		m_pColorBuffer[2];
		// Render Target Texture
		RTexture* 		m_pVelocityBuffer;				
		// 복원용
		RTexture* 		m_pColorBufferOrg;
		// 그려지고 있는 액터 비교 용
		RSceneNode*		m_pRenderNode;

		// RT width and height
		DWORD m_dwWidth;
		DWORD m_dwHeight;

		DWORD m_dwWidthSmall;
		DWORD m_dwHeightSmall;

		BOOL	m_bActive;
		BOOL	m_bLockFlag;
		float	m_fBias;		// 속도 버퍼에 메쉬를 그릴 시 컬러 버퍼에 씌여진 메쉬와 z-fight를 피하기 위해 바이어스를 건다.

		int		m_nSamplerCnt;						// 샘플러수. 픽셀 셰이더에서는 샘플링 횟수로 쓰임
		int		m_nImgSamCnt;						// 이미지를 통채로 또 그리는 횟수.

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
		virtual void 	OnRender();

		inline void		SetActive( BOOL b)			{ if(!m_bLockFlag) m_bActive = b;	}
		inline BOOL		IsActive()					{ return m_bActive;					}

		// 진행 앞쪽은 블러 처리를 안하게 하면, 이미지 반복 샘플 시 꼬리가 줄어들어 이상하게 된다. 컴파일 오류를 없애기 위해 함수 껍데기만 둔다.
		inline void		SetImgSamCnt( int n)		{ /*m_nImgSamCnt = n;*/				}
		inline int		GetImgSamCnt()				{ return 1;/*m_nImgSamCnt*/;		}

		// 많으면 많을수록 꼬리를 여러번 만들어 낸다.
		inline void		SetSamplerCount( int n)	{ m_nSamplerCnt = n;			}
		inline int		GetSamplerCount()		{ return m_nSamplerCnt;			}

		bool			BeginRender( RSceneNode* pActorNode);
		bool			EndRender( RSceneNode* pActorNode);
	};

}

#endif
