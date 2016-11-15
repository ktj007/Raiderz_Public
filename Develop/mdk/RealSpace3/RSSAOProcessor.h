#pragma once


#include "RGIProcessor.h"


namespace rs3 {



class RS_API RSSAOProcessor : public RGIProcessor
{

protected:
	RShaderFX*		m_pShaderSSAO;
	RShaderFX*		m_pShaderEdgeDetect;

	RTexture*		m_ptexNoise;
	RTexture*		m_pTarget[2];
	RTexture*		m_pEdge;
	RTexture*		m_pDepthExpand;
	RTexture*		m_ptexBlurX;
	RTexture*		m_ptexBlurY;

	int				m_nPatternPixel;
	int				m_nGaussPixel;

	DWORD			m_dwWidthFull;
	DWORD			m_dwHeightFull;

	bool			m_bPointFilterBufferOnly;
	bool			m_bPointFilterDepthOnly;

public:
	RSSAOProcessor(int n);
	virtual ~RSSAOProcessor(void);

	virtual void Render( RCameraSceneNode* pCamera, int nWidth, int nHeight, bool bRenderDirect) override;
	virtual void Init( int nW, int nH);
	virtual void Destroy();

	void DrawFullScreenQuad(float fLeftU, float fTopV, float fRightU, float fBottomV, int bSmallSize);

	int 	SetSampleOffsets_GaussBlur( int nPixel, int nTexWidth, int nTexHeight, FLOAT fMultiplier = 1.0f );
	float	GaussianDistribution( float x, float y, float rho );
};



}
