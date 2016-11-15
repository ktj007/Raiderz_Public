#pragma once

#include "RPostEffectInterface.h"


// Depth Of Field를 처리하는 클래스
// 기본적인 구조는 RCameraHDRController를 C&P해서 편집...
namespace rs3 {


class RS_API RPostEffectDOF : public RPostEffectInterface
{
public:
	enum DOF_FILTER_TYPE
	{
		DFT_COC,
		DFT_COC_N_GAUSSIAN,
		DFT_GAUSSIAN,
	};

	RPostEffectDOF();
	virtual ~RPostEffectDOF();

protected:
	virtual void	CreateBuffers()  override;
	virtual void	DestroyBuffers()  override;

	DOF_FILTER_TYPE	m_eFiterType;

	RShaderFX*		m_pDOFShader;	

	// Render Target Texture
	// 가우시안 블러용
	RTexture* 		m_pGaussianBuffer[2];
	// 최종 사용 될 정보 버퍼
	RTexture* 		m_pDepthBlurBuffer;
	RTexture* 		m_pDepthBlurBufferBlured;
	RTexture* 		m_pDepthBlurBufferBluredExpanded;
	RTexture*		m_ptexNoise;

	// RT of Gaussian width and height
	DWORD m_dwWidthGaussian;
	DWORD m_dwHeightGaussian;

	int	m_nNumTaps;

	bool m_bEdgeBlur;
	bool m_bUseNoise;

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


public:
	
	virtual	bool	Init( int nW, int nH) override;
	virtual	void	Destroy() override;

	void			SetupViewportScale();
	void			SetupFilterKernel();

	// 가우시안 적용 시
	float			GaussianDistribution( float x, float y, float rho );
	bool 			GetSampleOffsets_GaussBlur5x5( int nTexWidth, int nTexHeight, RVector2* vTexCoordOffset, RVector4* vSampleWeight, FLOAT fMultiplier = 1.0f );	// HDR 컨트롤러의 것을

	// 구현
	void			RenderDOF_CoC( RCameraSceneNode* pCamera, RTexture* pColorBuffer, RTexture* pNormalBuffer, RTexture* pDepthBuffer);
	void			RenderDOF_CoC_Gaussian( RCameraSceneNode* pCamera, RTexture* pColorBuffer, RTexture* pNormalBuffer, RTexture* pDepthBuffer);
	void			RenderOverlay();
	virtual void	Render( RCameraSceneNode* pCamera, RTexture* pColorBuffer, RTexture* pNormalBuffer, RTexture* pDepthBuffer);


	void	RefreshParamsBasisFocusRange(){
		m_fNearBlurDist = m_fFocusDist - m_fFocusRange/2;
		m_fFarBlurDist = m_fFocusDist + m_fFocusRange/2;
	}

	FLOAT GetCOCSize()				{ return m_fCoCSize;			}
	FLOAT GetFocalDist()			{ return m_fFocusDist;			}
	FLOAT GetFocalRange()			{ return m_fFocusRange;	}
	FLOAT GetFocalRangeValue()		{ return 2.0f / m_fFocusRange;	}

	bool SetFocalDist( float f)		{ m_fFocusDist = f;		return true;		}
	bool SetFocalRange( float f)	{ m_fFocusRange = f;		RefreshParamsBasisFocusRange();	return true;}
	bool SetCOCSize(FLOAT size)		{ m_fCoCSize = size;	return true;		}

	bool AddFocalDist( float f)		{ m_fFocusDist += f;	if( m_fFocusDist<0)		m_fFocusDist = 0;	return true;	}
	bool AddFocalRange( float f)	{ m_fFocusRange += f;	if( m_fFocusRange<0)	m_fFocusRange = 0;	RefreshParamsBasisFocusRange();	return true;}
	bool AddCOCSize(FLOAT size)		{ m_fCoCSize += size;	if( m_fCoCSize<0)		m_fCoCSize = 0;		return true;}

	bool SetEdgeBlur( bool b= true)	{ m_bEdgeBlur = b;			return true;	}
	bool IsEdgeBlur()				{ return m_bEdgeBlur;		}

	bool SetNearBlurDist( float f)		{ m_fNearBlurDist = f;			return true;	}
	bool SetFarBlurDist( float f)		{ m_fFarBlurDist = f;			return true;	}
	bool SetMaxBlurOfBehind( float f)	{ m_fMaxBlurOfBehind = f;		return true;	}	// 0~1

	FLOAT GetNearBlurDist()				{ return m_fNearBlurDist;			}
	FLOAT GetFarBlurDist()				{ return m_fFarBlurDist;			}
	FLOAT GetMaxBlurOfBehind()			{ return m_fMaxBlurOfBehind;		}

	bool SetTapsCount( int nCnt)		{ m_nNumTaps = min( nCnt, 13);	return true;}
	int	 GetTapsCount()					{ return m_nNumTaps;			}

	bool SetUseNoise( bool b= true)	{ m_bUseNoise = b;			return true;	}
	bool IsUseNoise()				{ return m_bUseNoise;						}
	
};

}

