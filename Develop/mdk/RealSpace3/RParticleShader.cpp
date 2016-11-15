#include "StdAfx.h"
#include "RParticleShader.h"
#include "RShaderFX.h"
#include "RUtil.h"

namespace rs3 {

const RParticleShader::FLAG_TABLE RParticleShader::flagTable[] =
{
	{ PSF_ALPHA_TEST, "ALPHA_TEST" },
	{ PSF_SELFILLUMINATION, "SELF_ILLUMINATION" },
	{ PSF_DISTORTION_MASKING, "DISTORTION_MASKING" },

	{ PSF_SOFT_PARTICLE, "SOFT_PARTICLE" },
	{ PSF_SOFT_PARTICLE_BLEND_ALPHA, "SOFT_PARTICLE_BLEND_ALPHA" },
	{ PSF_SOFT_PARTICLE_BLEND_COLOR, "SOFT_PARTICLE_BLEND_COLOR" },

	{ PSF_MULTI_LAYER_BLENDING, "MULTI_LAYER_BLENDING" },
};

RParticleShader::RParticleShader() :
	m_pShaderFXPool(NULL)
{
}

RParticleShader::~RParticleShader()
{
	Destroy();
}

void RParticleShader::Init()
{
	if( !m_pShaderFXPool)
		m_pShaderFXPool = new RShaderFXPool();
	_ASSERT( m_pShaderFXPool );

	DWORD dwFlag = 0;
	RShaderFX* pShader = CreateShader( dwFlag );
	_ASSERT( pShader );
	if( pShader )
	{
		m_scWorld = pShader->GetConstant( "g_matWorld" );
		m_scWorldViewProj = pShader->GetConstant( "g_matWorldViewProj" );
		m_scResolution = pShader->GetConstant("g_vResolution");
		m_scFarZ = pShader->GetConstant("g_fFarZ");
		m_scAlphaRef = pShader->GetConstant( "g_fAlphaRef" );
		m_scIlluminationScale = pShader->GetConstant( "g_vIlluminationScale" );
		m_scDepthBiasBlendDist = pShader->GetConstant( "g_fDepthBiasBlendDist" );
		m_scAttenuationBegin = pShader->GetConstant( "g_fAttenuationBegin" );
		m_scFogFactor = pShader->GetConstant( "g_vFogFactor" );
		m_scFogColor = pShader->GetConstant( "g_vFogColor" );
	}
}

void RParticleShader::Destroy()
{
	for( SHADERLIST::iterator it = m_ShaderList.begin(); it != m_ShaderList.end(); ++it )
	{
		RShaderFX* pShader = it->second;
		_ASSERT( pShader );
		REngine::GetDevice().DeleteShaderFX(pShader);
	}
	m_ShaderList.clear();

	SAFE_DELETE( m_pShaderFXPool );

	m_scWorld.Destroy();
	m_scWorldViewProj.Destroy();
	m_scAlphaRef.Destroy();
	m_scIlluminationScale.Destroy();
	m_scDepthBiasBlendDist.Destroy();
	m_scResolution.Destroy();
	m_scFarZ.Destroy();
	m_scAttenuationBegin.Destroy();
	m_scFogFactor.Destroy();
	m_scFogColor.Destroy();
}

RShaderFX* RParticleShader::CreateShader( DWORD dwFlag )
{
	SHADERLIST::const_iterator it = m_ShaderList.find( dwFlag );
	if( it != m_ShaderList.end() )
		return it->second;

	vector<string> strDefines;
	for( int i = 0; i < _countof(flagTable); ++i )
	{
		if( ( flagTable[i].dwFlag & dwFlag ) != 0 )
		{
			strDefines.push_back( string(flagTable[i].strDefine) );
			strDefines.push_back( "1" );
		}
	}

	_ASSERT( m_pShaderFXPool );
	RShaderFX* pShader = REngine::GetDevice().CreateShaderFX( "Particle.fx", strDefines, m_pShaderFXPool );

	_ASSERT( pShader );
	m_ShaderList.insert( SHADERLIST::value_type( dwFlag, pShader ) );
	return pShader;
}

void RParticleShader::SetTransformConstant( const RMatrix& matWorld, const RMatrix& matView, const RMatrix& matViewProj,
	const RVector& vFarClipCornerInView, DWORD dwScreenWidth, DWORD dwScreenHeight, float fFarZ)
{
	m_scWorld.SetMatrix( matWorld, false );
	m_scWorldViewProj.SetMatrix( matWorld * matViewProj, false );

	// VPOS ½Ã¸àÆ½ »ç¿ë
	m_scResolution.SetVector4( RVector4( (float)dwScreenWidth, (float)dwScreenHeight, 0.5f/(float)dwScreenWidth, 0.5f/(float)dwScreenHeight));
	m_scFarZ.SetFloat( fFarZ);
}

void RParticleShader::SetFogConstant( bool bEnable, float fNear, float fFar, DWORD dwFogColor )
{
	if( bEnable )
	{
		float fDist = fFar - fNear;
		fDist = ( fDist == 0.0f ? 0 : 1.0f / fDist );
		m_scFogFactor.SetVector4( fNear, fFar, fDist, 0.0f );

		SetFogColorConstant( dwFogColor );
	}
	else
	{
		m_scFogFactor.SetVector4( 0.f, 0.f, 0.f, 0.f );
	}
}

void RParticleShader::SetFogColorConstant( DWORD dwFogColor )
{
	RVector vFogColor;
	DWColorToFloat3( &vFogColor, dwFogColor );
	m_scFogColor.SetVector3( vFogColor );
}

void RParticleShader::SetAlphaRefConstant( float fAlphaRef )
{
	m_scAlphaRef.SetFloat( fAlphaRef );
}

void RParticleShader::SetSelfIlluminationScaleConstant( float fSelfIlluminationScale )
{
	m_scIlluminationScale.SetFloat( fSelfIlluminationScale );
}

void RParticleShader::SetDepthBiasBlendDistConstant( float fDist )
{
	m_scDepthBiasBlendDist.SetFloat( fDist );
}

void RParticleShader::SetDistortionAttenuationBeginConstant( float fAttenuationBegin )
{
	float fAttenuationBeginCorrection = fAttenuationBegin + 0.001f;
	_ASSERT( fAttenuationBeginCorrection > 0.f );
	m_scAttenuationBegin.SetFloat( fAttenuationBeginCorrection );
}

RShaderFX* RParticleShader::GetShader( const RParticleShaderCreateParam& param )
{
	DWORD dwFlag = 0;

	if( param.bAlphaTest )
		dwFlag |= PSF_ALPHA_TEST;

	if( param.bSelfIllumination )
		dwFlag |= PSF_SELFILLUMINATION;

	if( param.bDistortion )
		dwFlag |= PSF_DISTORTION_MASKING;

	if( param.bSoftParticleBlendAlpha )
		dwFlag |= PSF_SOFT_PARTICLE | PSF_SOFT_PARTICLE_BLEND_ALPHA;

	if( param.bSoftParticleBlendColor )
		dwFlag |= PSF_SOFT_PARTICLE | PSF_SOFT_PARTICLE_BLEND_COLOR;

	if( param.bMultiLayerBlend )
		dwFlag |= PSF_MULTI_LAYER_BLENDING;

	return CreateShader( dwFlag );
}

}
