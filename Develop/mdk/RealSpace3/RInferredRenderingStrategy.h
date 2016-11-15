#pragma once

#include "RRenderingStrategy.h"
#include "RVType.h"

namespace rs3 {

class RGIProcessor;
class RLightingShader;


class RS_API RInferredRenderingStrategy : public RRenderingStrategy
{
public:
	RInferredRenderingStrategy();
	virtual ~RInferredRenderingStrategy();

	virtual void RenderScene( RCameraSceneNode *pCUrrentCamera, RWorldSceneNode *RWorldScene, RShadowStrategy* pShadowStrategy, RRenderingCallbackInterface* pRenderingCallbackInterface);
	virtual void Init( int nW, int nH);
	virtual void Destroy();

	RTexture* GetLightBuffer()		{ return m_pLBuffer; }
	RTexture* GetNormalBuffer()		{ return m_pGBufferNormal; }
	virtual RTexture* GetDepthBuffer() override		{ return m_pGBufferDepth; }
	RTexture* GetSpecRimAOBuffer()	{ return m_pGBufferSpecRimAO; }
	RTexture* GetDSFBuffer()		{ return m_pGBufferDSF; }

	virtual int GetMRTCount();

protected:
	virtual void				SetStateGStagePass();

	virtual void				SetLStageTexture();
	virtual void				RestoreLStageState();

	virtual void				SetStateMStagePass( RCameraSceneNode* pCurrentCamera, RTexture* pViewNormalMap, RTexture* pViewDepthMap, RTexture* pViewLightMap, RTexture* pViewDSFMap);
	virtual void				ResetStateMStagePass(void);
	

	bool		m_bUseDSF;

	DWORD m_dwWidthSmall;
	DWORD m_dwHeightSmall;

	RTexture*	m_pDepthBufferGPass;	// depth stencil of Geometry, Light pass
	RTexture*	m_pDepthBufferMPass;	// depth stencil of Material pass
	RTexture*	m_pGBufferDepth;		// Depth of Cam view
	RTexture*	m_pGBufferNormal;		// Normal of Cam view
	RTexture*	m_pGBufferSpecRimAO;	// Specular, RIm mask, AO mask
	RTexture*	m_pGBufferDSF;			// DSF « ≈Õ
	RTexture*	m_pLBuffer;				// Light buffer


private:
	float					m_fQualityFactor;

};

}