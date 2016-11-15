#pragma once

#include "RShaderComposer.h"
#include "RShaderComponentID.h"
#include "RTechnique.h"

namespace rs3 {

class RShaderCompositeManager;
class RShaderFX;

class RS_API RShaderComposite
{
public:
	RShaderComposite(RSCID rscComponents);
	virtual ~RShaderComposite();

	RShaderFX*	GetShaderFX()			{ return m_pShaderFX; }
	RSCID		GetComponent()			{ return m_rscComponents;}

	bool				CompositeAndBuildShader( RShaderCompositeManager* pCompositeManager, bool bBackgroundLoading );
	void				Destroy();

	bool				IsLoadingSucceeded();

protected:
	bool				Composite( RShaderCompositeManager* pCompositeManager );

	RShaderFX*			m_pShaderFX;
	RSCID				m_rscComponents;
	RShaderComposer		m_shaderComposer;
};

}