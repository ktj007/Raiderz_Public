#pragma once

#include "RPostEffectInterface.h"
#include "RShaderConstant.h"

namespace rs3 {

class RActorNode;
class RTechnique;

class RS_API RPostEffectDistortion : public RPostEffectInterface
{
public:
	RPostEffectDistortion();
	virtual ~RPostEffectDistortion();

	virtual	bool	Init( int nW, int nH) override;
	virtual	void	Destroy() override;

	virtual void	Render( RCameraSceneNode* pCamera, RTexture* pColorBuffer, RTexture* pNormalBuffer, RTexture* pDepthBuffer);

	void			RenderEffectDistortion(RCameraSceneNode* pCamera, RDistortionParameter& param, bool bTriangleStrip);
	void			RenderActorNodeDistortion(RCameraSceneNode* pCamera, RActorNode* pActorNode, int nMtrIndex);
	void			RenderFullScreenDistortion(float fDistortionScale);
	
	bool			IsValid();
	bool			IsCulledVisible(RSceneNode* pNode, const RVector3& vCameraWorldPosition);

	void			BeginMasking(int nRT);
	void			EndMasking();

	bool			CheckRenderTargetFormat();

protected:
	virtual void	CreateBuffers()  override;
	virtual void	DestroyBuffers()  override;

private:	
	void			SetShaderConstant(const RMatrix& matViewProj, const RDistortionParameter& param);
	void			RenderTextureToFullScreen(RTexture* pTexture);
	void			SetActorNodeState(RActorNode* pActorNode, const RMatrix& matViewProj);

	RShaderFX*		m_pDistortionShader;	
	
	RTexture* 		m_pDistortedRT;		//¿Ö°îµÈ ÀÌ¹ÌÁö¸¦ ÀúÀåÇÏ´Â ·»´õÅ¸°Ù
	RTexture* 		m_pNormalMap;			//¹ý¼±¸Ê
	RTexture*		m_pMaskingMapOrg;
	int				m_nMaskRT;

	bool			m_bValid;
	float			m_fCullLength;
	float			m_fCullLengthSq;

	RShaderConstant	m_cDirection;
	RShaderConstant	m_cInfo;
	RShaderConstant	m_cTime;
	RShaderConstant	m_cWVP;
	RShaderConstant	m_cResolution;
	RShaderConstant	m_cBlendMatrices;
	RShaderConstant	m_cPhysique;
	RShaderConstant	m_cRenderTarget;
	RShaderConstant	m_cNormalMap;
	RShaderConstant	m_cMaskingMap;
};

}