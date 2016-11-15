#pragma once

#include "RRenderingStrategy.h"
#include "RVType.h"
#include "RShaderConstant.h"

//#define STREOPIC_TEST

namespace rs3 {

class RGIProcessor;
class RLightingShader;


class RS_API RDeferredRenderingStrategy : public RRenderingStrategy
{
public:
	struct FLAG_TABLE
	{
		const DWORD dwFlag;
		char* strDefine;
	};

	static const DWORD DRF_GAMMA_CORRECT					= 1;
	static const FLAG_TABLE flagTable[];


	RDeferredRenderingStrategy();
	virtual ~RDeferredRenderingStrategy();

	virtual void RenderScene( RCameraSceneNode *pCUrrentCamera, RWorldSceneNode *RWorldScene, RShadowStrategy* pShadowStrategy, RRenderingCallbackInterface* pRenderingCallbackInterface);
	virtual void Init( int nW, int nH);
	virtual void Destroy();
	virtual void Resize( int nW, int nH );
	virtual void Invalidate();
	virtual void Restore( DWORD dwWidth, DWORD dwHeight );
	
	RTexture* GetLightBuffer()		{ return m_pLBuffer; }
	RTexture* GetNormalBuffer()		{ return m_pGBufferNormal; }
	virtual RTexture* GetDepthBuffer()	override	{ return m_pGBufferDepthSpecRim; }
	RTexture* GetDiffuseBuffer()	{ return m_pGBufferDiffuseAO; }

	virtual void SetBackgroundBufferDivideSize( DWORD dwHalf) { }

	virtual int GetMRTCount();

	void		SetGammaCorrect( bool _bGammaCorrect );
	void		SetFXAA_Level( int _iFxaa_Lv );
	
protected:
	virtual void				SetLStageTexture();
	virtual void				RestoreLStageState();

	void						RenderBackgroundPass( RCameraSceneNode* pCurrentCamera, RRENDERPASS beginPass, RRENDERPASS endPass);
	void						RenderMergeDiffuseLight( RCameraSceneNode* pCurrentCamera, RTexture* pGBufferNormal, RTexture* pGBufferDepth, RTexture* pLBuffer, RTexture* pGBufferDiffuse);


	// 3D test --------------------------------------------------------------
	void						RenderBlend();



	RTexture*	m_pGBufferDepthSpecRim;		// Depth of Cam view, Spec and rim
	RTexture*	m_pGBufferNormal;		// Normal of Cam view
	RTexture*	m_pGBufferDiffuseAO;		// Diffuse of Cam view, AO mask
	RTexture*	m_pLBuffer;				// Light buffer
	RTexture*	m_pDepthStencil;

#ifdef STREOPIC_TEST
	// 3D Å×½ºÆ®
	///////////////////////////////////////////////
	RTexture*	m_pBufferLeft;
	RTexture*	m_pBufferRight;
	RTechnique	m_TechniqueBlend;
	//////////////////////////////////////////////
#endif

	RTechnique		m_TechniqueMergeFog;
	RTechnique		m_TechniqueMergeNoFog;

	RShaderConstant	m_cFogFactor;
	RShaderConstant	m_cFogColor;
	RShaderConstant	m_scFarClipCornerInView;


private:
	typedef map<DWORD, RShaderFX*> SHADERLIST;
	SHADERLIST		m_ShaderList;

private:
	void	SetDefaultTechnique( RShaderFX*  _pMergeEffect );
	void	CreateShader( DWORD dwFlag );
	void RenderCamera( RCameraSceneNode *pCUrrentCamera, RWorldSceneNode *RWorldScene, RShadowStrategy* pShadowStrategy, RRenderingCallbackInterface* pRenderingCallbackInterface);
	void RenderDiffuseOnly( RCameraSceneNode *pCurrentCamera, RWorldSceneNode *pWorldScene, RRenderingCallbackInterface* pRenderingCallbackInterface);
	void DestroyConstant();
};

}