#pragma once

#include "RPostEffectInterface.h"

namespace rs3 {

class RS_API RPostEffectFinalScene : public RPostEffectInterface
{
public:
	struct FLAG_TABLE
	{
		const DWORD dwFlag;
		char* strDefine;
	};

	static const DWORD PEF_GAMMA_CORRECT					= 1;
	static const DWORD PEF_FXAA_PRESET0						= 1<<1;
	static const DWORD PEF_FXAA_PRESET1						= 1<<2;
	static const DWORD PEF_FXAA_PRESET2						= 1<<3;
	static const DWORD PEF_FXAA_PRESET3						= 1<<4;
	static const DWORD PEF_FXAA_PRESET4						= 1<<5;
	static const DWORD PEF_FXAA_PRESET5						= 1<<6;
	static const DWORD PEF_FXAA_PRESET6						= 1<<7;

	static const FLAG_TABLE flagTable[];

	RPostEffectFinalScene();
	virtual ~RPostEffectFinalScene();

	virtual	bool	Init( int nW, int nH) override;
	virtual	void	Destroy() override;

	virtual void	Render( RCameraSceneNode* pCamera, RTexture* pColorBuffer, RTexture* pNormalBuffer, RTexture* pDepthBuffer);

	void			CreateShader();

protected:
	virtual void	CreateBuffers()  override;
	virtual void	DestroyBuffers()  override;

private:

	RShaderFX*		m_pPEFinalSceneShader;

	typedef map<DWORD, RShaderFX*> SHADERLIST;
	SHADERLIST		m_ShaderList;

	

private:

};

}