#pragma once

#include "RShaderConstant.h"

namespace rs3 {

struct RParticleShaderCreateParam
{
	bool	bAlphaTest;
	bool	bSelfIllumination;
	bool	bDistortion;
	bool	bSoftParticleBlendAlpha;
	bool	bSoftParticleBlendColor;
	bool	bMultiLayerBlend;
};

// 파티클, 트레일 셰이더를 생성하고, 셰이더 상수 설정을 하는 클래스
class RParticleShader
{
public:
	RParticleShader();
	virtual ~RParticleShader();

	void		Init();
	void		Destroy();

	void		SetTransformConstant( const RMatrix& matWorld, const RMatrix& matView, const RMatrix& matViewProj,
					const RVector& vFarClipCornerInView, DWORD dwScreenWidth, DWORD dwScreenHeight, float fFarZ);
	void		SetFogConstant( bool bEnable, float fNear, float fFar, DWORD dwFogColor );
	void		SetFogColorConstant( DWORD dwFogColor );
	void		SetAlphaRefConstant( float fAlphaRef );
	void		SetSelfIlluminationScaleConstant( float fSelfIlluminationScale );
	void		SetDepthBiasBlendDistConstant( float fDist );
	void		SetDistortionAttenuationBeginConstant( float fAttenuationBegin );

	RShaderFX*	GetShader( const RParticleShaderCreateParam& param );


private:
	RShaderFX*	CreateShader( DWORD dwFlag );

	// Particle Shader Flag => PSF
	static const DWORD PSF_ALPHA_TEST					= 1;
	static const DWORD PSF_SELFILLUMINATION				= 1<<1;
	static const DWORD PSF_DISTORTION_MASKING			= 1<<2;
	// Soft Particle
	static const DWORD PSF_SOFT_PARTICLE				= 1<<3;
	static const DWORD PSF_SOFT_PARTICLE_BLEND_ALPHA	= 1<<4;
	static const DWORD PSF_SOFT_PARTICLE_BLEND_COLOR	= 1<<5;
	// Multi Layer Blending
	static const DWORD PSF_MULTI_LAYER_BLENDING			= 1<<6;

	RShaderConstant	m_scWorld;
	RShaderConstant	m_scWorldViewProj;
	RShaderConstant	m_scAlphaRef;
	RShaderConstant	m_scIlluminationScale;
	RShaderConstant	m_scDepthBiasBlendDist;
	RShaderConstant	m_scResolution;
	RShaderConstant m_scFarZ;
	RShaderConstant	m_scAttenuationBegin;
	RShaderConstant	m_scFogFactor;
	RShaderConstant	m_scFogColor;

	struct FLAG_TABLE
	{
		const DWORD dwFlag;
		char* strDefine;
	};
	static const FLAG_TABLE flagTable[];

	RShaderFXPool*	m_pShaderFXPool;

	typedef map<DWORD, RShaderFX*> SHADERLIST;
	SHADERLIST		m_ShaderList;
};

}
