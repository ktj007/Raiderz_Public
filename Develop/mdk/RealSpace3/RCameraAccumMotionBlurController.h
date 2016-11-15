#ifndef _RCAMERA_ACCUM_MOTION_BLUR_CONTROLLER_H
#define _RCAMERA_ACCUM_MOTION_BLUR_CONTROLLER_H

#include "RCameraSceneNodeController.h"
#include "RShaderFX.h"
#include "RListener.h"



// Motion Blur ( Accum buffer) 를 처리하는 클래스
// 기본적인 구조는 RCameraScreenMotionBlurController를 C&P해서 편집...


// 현재 프레임에서 활성화 된 버퍼들의 리스트를 매번 구축한다.
// 액터는 자신이 누적 블러가 설정 되면 카메라 컨트롤러에 통지한다.
// 카메라 컨트롤러는 통지를 받으면 액터와 버퍼의 짝을 만들고( 이미 있으면 말고~) 액터에 할당 된 버퍼에 현재 모습을 그린다.
// 후처리 마지막 단계에서는 버퍼들을 현재 화면에 덧댄다.
// 누적 버퍼를 액터별로 할 수 있게되지롱~
// 
// 액터는 소멸 시 카메라 컨트롤러에 이를 알리고 버퍼를 해지할 수 있게 하자~

namespace rs3 {

typedef std::map< RSceneNode*, RTexture*>	ACCUM_BUFFER_MAP;
typedef std::list< RTexture*>				DRAW_BUFFER_LIST;

class RS_API RCameraAccumMotionBlurController : public RCameraSceneNodeController, public RListener
{
public:
	RCameraAccumMotionBlurController( RCameraSceneNode* pCameraSceneNode );
	virtual ~RCameraAccumMotionBlurController();

protected:

	RShaderFX*		m_pShader;	

	// Render Target Texture
	RTexture* 		m_pColorBuffer;
	// Render Target Texture
	RTexture* 		m_pAccumBuffer[2];				
	// 복원용
	RTexture* 		m_pColorBufferOrg;
	// 현재 프레임의 액터들이 임시로 그려지게 될 버퍼
	RTexture* 		m_pTmpBuffer;
	// 그려지고 있는 액터 비교 용
	RSceneNode*		m_pRenderNode;

	// RT width and height
	DWORD m_dwWidth;
	DWORD m_dwHeight;

	DWORD m_dwWidthSmall;
	DWORD m_dwHeightSmall;

	bool	m_bActive;
	bool	m_bLockFlag;
	bool	m_id;

	float m_fAccumRate;
	DWORD m_dwAccumRate;
	float m_fDecreaseAlpha;
	DWORD m_dwTimeAccum;
	float m_fLastAlphaValue;

	RListener*		m_pEventListener;			///< OnRestoreDevice / OnLostDevice 이벤트 수신객체

public:
	RCameraSceneNode*		m_pCameraSceneNode;

	virtual void	UpdateCameraController(RMatrix& _rMatApplyToCamera) {}
	virtual void	OnClear();		
	virtual void	OnPostRender();	
	virtual void	OnRemoveFromCamera();
	virtual void	OnPreRender( RInferredRenderingStrategy* pInferredRenderingStrategy);	// 랜더 타겟을 관리

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

	inline void		SetActive( bool b)			{ if(!m_bLockFlag) m_bActive = b;	}
	inline bool		IsActive()					{ return m_bActive;					}

	inline void		SetDecreaseAlpha( float f)	{ m_fDecreaseAlpha = f;			}
	inline float	GetDecreaseAlpha()			{ return m_fDecreaseAlpha;		}

	inline void		SetAccumRate( float f)		{ m_fAccumRate = f;	m_dwAccumRate = (DWORD)(m_fAccumRate*1000.0f);	}
	inline float	GetAccumRate()				{ return m_fAccumRate;		}

	bool			DeleteActorNode( RSceneNode* pActorNode);
	bool			BeginRender( RSceneNode* pActorNode);
	bool			EndRender( RSceneNode* pActorNode);

	inline bool		IsBeingBlur()				{ return (bool)(m_fLastAlphaValue>0);	}

protected:
	RTexture*		GetRenderTarget( RSceneNode* pActorNode);
};

}

#endif
