#pragma once

#include "RPostEffectInterface.h"

namespace rs3 {

class RS_API RPostEffectEdge : public RPostEffectInterface
{
public:
	RPostEffectEdge();
	virtual ~RPostEffectEdge();

	virtual	bool	Init( int nW, int nH) override;
	virtual	void	Destroy() override;

	virtual void	Render( RCameraSceneNode* pCamera, RTexture* pColorBuffer, RTexture* pNormalBuffer, RTexture* pDepthBuffer);

	void			EnablePEEdge( bool _bProjection = false );
	void			DisablePEEdge();
	void			PEEdgeQuality(int _iBlurCount = 2, int _iScreenSize = 1);
	void			SetFactorTickNess( const float _fTickness, const float _fBlurFactor, const float _fColorPower );
	void			AddCount();
	void			MinusCount();

protected:
	virtual void	CreateBuffers()  override;
	virtual void	DestroyBuffers()  override;

private:

	RShaderFX*		m_pEdgeShader;
	RTexture* 		m_pTexEdgeH;
	RTexture* 		m_pTexEdgeV;

	float			m_fEdgePower;
	float			m_fTickNess;
	float			m_fBulrFactor;
	bool			m_bProjection;

	int				m_iBlurCount;
	int				m_iScreenDiv;
	int				m_iPEEdgeCount;

private:
	void	EdgeBlurV();
	void	EdgeBlurH();
};

}