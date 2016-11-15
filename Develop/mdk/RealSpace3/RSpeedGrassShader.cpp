#include "StdAfx.h"
#include "RSpeedGrassShader.h"
#include "REngine.h"
#include "RRenderHelper.h"
#include "RShaderCompositeManager.h"
#include "RShaderFX.h"


namespace rs3 {

const RSpeedGrassShader::FLAG_TABLE RSpeedGrassShader::flagTable[] =
{
	{ SGRF_LIGHTBUFFER, "LIGHTBUFFER" },
	{ SGRF_RT1, "RT1" },
	{ SGRF_RT2, "RT2" },
	{ SGRF_RT3, "RT3" },
};

RSpeedGrassShader::RSpeedGrassShader() :
m_pShaderFXPool(NULL)
{
	Destroy();
}

RSpeedGrassShader::~RSpeedGrassShader()
{
}

void RSpeedGrassShader::Init()
{
	if( !m_pShaderFXPool)
		m_pShaderFXPool = new RShaderFXPool();
	_ASSERT( m_pShaderFXPool );

	DWORD dwFlag = SGRF_LIGHTBUFFER | SGRF_RT3;
	RShaderFX* pShader = CreateShader( dwFlag );
	_ASSERT( pShader );
	if( pShader )
	{
		m_scWorldView				= pShader->GetConstant("g_matWorldView");
		m_scWorldViewProj			= pShader->GetConstant("g_matWorldViewProj");
		m_scTimes					= pShader->GetConstant("g_fTime");
		m_scWindStrength			= pShader->GetConstant("g_fWindStrength");
		m_scWindDirection			= pShader->GetConstant("g_vWindDirection");
		m_scBillboard				= pShader->GetConstant("g_avUnitizedGrassBillboard");
		m_scFarLod					= pShader->GetConstant("g_fGrassFarLod");
		m_scLodTransition			= pShader->GetConstant("g_fGrassShrinkTransitionLength");
		m_scGrassJigglingPeriod		= pShader->GetConstant("g_fGrassJigglingPeriod");
		m_scGrassJigglingStrength	= pShader->GetConstant("g_fGrassJigglingStrength");
		m_scGrassIncliningStrength	= pShader->GetConstant("g_fGrassIncliningStrength");
		m_scResolution				= pShader->GetConstant("g_vResolution");
		m_scFarZ					= pShader->GetConstant("g_fFarDist");
	}

}

void RSpeedGrassShader::Destroy()
{
	for( SHADERLIST::iterator it = m_ShaderList.begin(); it != m_ShaderList.end(); ++it )
	{
		RShaderFX* pShader = it->second;
		_ASSERT( pShader );
		REngine::GetDevice().DeleteShaderFX(pShader);
	}
	m_ShaderList.clear();
	SAFE_DELETE( m_pShaderFXPool );

	m_scWorldView.Destroy();
	m_scWorldViewProj.Destroy();
	m_scTimes.Destroy();
	m_scWindStrength.Destroy();
	m_scWindDirection.Destroy();
	m_scBillboard.Destroy();
	m_scFarLod.Destroy();
	m_scLodTransition.Destroy();
	m_scGrassJigglingPeriod.Destroy();
	m_scGrassJigglingStrength.Destroy();
	m_scGrassIncliningStrength.Destroy();
	m_scResolution.Destroy();
	m_scFarZ.Destroy();
}

void RSpeedGrassShader::SetTransformConstant(const RMatrix& matWorld, const RMatrix& matView, const RMatrix& matViewProj, const DWORD dwScreenWidth, const DWORD dwScreenHeight, const float fFarZ)
{
	RPFC_THISFUNC;
	//m_cWorld.SetMatrix(matWorld, false);
	m_scWorldView.SetMatrix( matWorld * matView, false);
	m_scWorldViewProj.SetMatrix( matWorld * matViewProj, false);	
	// VPOS 시멘틱 사용
	m_scResolution.SetVector4( RVector4( (float)dwScreenWidth, (float)dwScreenHeight, 0.5f/(float)dwScreenWidth, 0.5f/(float)dwScreenHeight));
	m_scFarZ.SetFloat( fFarZ);
}


void RSpeedGrassShader::SetWindConstant( const float fNormalizedTime, const RVector3& vWindDirection, const float fWindStrength, const float fGrassJigglingPeriod, const float fGrassJigglingStrength, const float fGrassIncliningStrength)
{
	m_scTimes.SetFloat( fNormalizedTime);
	m_scWindDirection.SetVector3( vWindDirection );
	m_scWindStrength.SetFloat( fWindStrength);
	m_scGrassJigglingPeriod.SetFloat( fGrassJigglingPeriod);	
	m_scGrassJigglingStrength.SetFloat( fGrassJigglingStrength);	
	m_scGrassIncliningStrength.SetFloat( fGrassIncliningStrength);	
}

void RSpeedGrassShader::SetLodConstant( const float fLodFarDistance, const float fLodTransitionLength)
{
	m_scFarLod.SetFloat( fLodFarDistance);
	m_scLodTransition.SetFloat( fLodTransitionLength);
}

void RSpeedGrassShader::SetBillboardConstant( const float* pUnitBillboard)
{
	m_scBillboard.SetVector4Array((RVector4*)pUnitBillboard, 4);
}

DWORD RSpeedGrassShader::GetShaderFlag( int nRenderTargetCount, bool bLightBuffer)
{
	DWORD dwFlag = 0;
	if( bLightBuffer)
		dwFlag |= SGRF_LIGHTBUFFER;
	// MRT 갯수에 따라 용도가 다르다.
	switch( nRenderTargetCount)
	{
	case 1:
		dwFlag |= SGRF_RT1;	/// 라이트 프리 패스 Mstage
		break;
	case 2:
		dwFlag |= SGRF_RT2;	/// 라이트 프리 패스 Gstage
		break;
	case 3:
		dwFlag |= SGRF_RT3;	/// 디퍼드 Gstage
		break;
	default:
		return 0;
	}

	return dwFlag;
}

RShaderFX* RSpeedGrassShader::CreateShader( DWORD dwFlag )
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
	RShaderFX* pShader = REngine::GetDevice().CreateShaderFX( "SpeedGrass.fx", strDefines, m_pShaderFXPool );

	_ASSERT( pShader );
	m_ShaderList.insert( SHADERLIST::value_type( dwFlag, pShader ) );
	return pShader;
}

RShaderFX* RSpeedGrassShader::GetShaderForGStageDeferred()
{
	DWORD dwFlag = GetShaderFlag( REngine::GetSceneManager().GetRenderingStrategy()->GetMRTCount(), false);
	return CreateShader(dwFlag); 
}

RShaderFX* RSpeedGrassShader::GetShaderForGStageInferred()
{
	DWORD dwFlag = GetShaderFlag( 2, false);
	return CreateShader(dwFlag); 
}

RShaderFX* RSpeedGrassShader::GetShaderForMStageInferred()
{
	DWORD dwFlag = GetShaderFlag( 1, true);
	return CreateShader(dwFlag); 
}



}
