#pragma once

#include "RPostEffectInterface.h"


namespace rs3 {

class RS_API RPostEffectPixelMotionBlur : public RPostEffectInterface
{
public:
	RPostEffectPixelMotionBlur();
	virtual ~RPostEffectPixelMotionBlur();

protected:
	virtual void CreateBuffers();
	virtual void DestroyBuffers();

	RShaderFX*		m_pShader;	

	// 그려지고 있는 액터 비교 용
	RSceneNode*		m_pRenderNode;

	DWORD m_dwWidthSmall;
	DWORD m_dwHeightSmall;

	float	m_fBias;		// 속도 버퍼에 메쉬를 그릴 시 컬러 버퍼에 씌여진 메쉬와 z-fight를 피하기 위해 바이어스를 건다.

	int		m_nSamplerCnt;						// 샘플러수. 픽셀 셰이더에서는 샘플링 횟수로 쓰임
	int		m_nImgSamCnt;						// 이미지를 통채로 또 그리는 횟수.

public:
	RCameraSceneNode*		m_pCameraSceneNode;

	virtual	bool	Init( int nW, int nH) override;
	virtual	void	Destroy() override;

	virtual void	Render( RCameraSceneNode* pCamera, RTexture* pColorBuffer, RTexture* pNormalBuffer, RTexture* pDepthBuffer);

	// 구현
	void			RenderOverlay();

	// 많으면 많을수록 꼬리를 여러번 만들어 낸다.
	inline void		SetSamplerCount( int n)	{ m_nSamplerCnt = n;			}
	inline int		GetSamplerCount()		{ return m_nSamplerCnt;			}
};

}


