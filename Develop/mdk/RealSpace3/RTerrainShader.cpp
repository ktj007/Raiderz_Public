#include "StdAfx.h"
#include "RTerrainShader.h"
#include "REngine.h"
#include "RRenderHelper.h"
#include "RShaderCompositeManager.h"
#include "RShaderFX.h"


namespace rs3 {

const RTerrainShader::FLAG_TABLE RTerrainShader::flagTable[] =
{
	{ TRF_SPECULAR, "SPECULAR" },
	{ TRF_RT1, "RT1" },
	{ TRF_RT2, "RT2" },
	{ TRF_RT3, "RT3" },
	{ TRF_BAKED, "BAKED" },
	{ TRF_LIGHTBUFFER, "LIGHTBUFFER" },
	{ TRF_FOG, "FOG" },
	{ TRF_SPLATTING_ONEPASS, "SPLATTING_ONEPASS" },
	{ TRF_LAYER_COUNT_2, "LAYER_COUNT_2" },
	{ TRF_LAYER_COUNT_3, "LAYER_COUNT_3" },
	{ TRF_LAYER_COUNT_4, "LAYER_COUNT_4" },
	{ TRF_LAYER_COUNT_5, "LAYER_COUNT_5" },
	{ TRF_LAYER_COUNT_6, "LAYER_COUNT_6" },
	{ TRF_LAYER_COUNT_7, "LAYER_COUNT_7" },
	{ TRF_LAYER_0_SPECULAR, "LAYER_0_SPECULAR" },
	{ TRF_LAYER_1_SPECULAR, "LAYER_1_SPECULAR" },
	{ TRF_LAYER_2_SPECULAR, "LAYER_2_SPECULAR" },
	{ TRF_LAYER_3_SPECULAR, "LAYER_3_SPECULAR" },
	{ TRF_LAYER_4_SPECULAR, "LAYER_4_SPECULAR" },
	{ TRF_LAYER_5_SPECULAR, "LAYER_5_SPECULAR" },
	{ TRF_LAYER_6_SPECULAR, "LAYER_6_SPECULAR" },
	{ TRF_LAYER_0_NORMALMAP, "LAYER_0_NORMALMAP" },
	{ TRF_LAYER_1_NORMALMAP, "LAYER_1_NORMALMAP" },
	{ TRF_LAYER_2_NORMALMAP, "LAYER_2_NORMALMAP" },
	{ TRF_LAYER_3_NORMALMAP, "LAYER_3_NORMALMAP" },
	{ TRF_LAYER_4_NORMALMAP, "LAYER_4_NORMALMAP" },
	{ TRF_LAYER_5_NORMALMAP, "LAYER_5_NORMALMAP" },
	{ TRF_LAYER_6_NORMALMAP, "LAYER_6_NORMALMAP" },
	{ TRF_USE_NORMALMAP, "USE_NORMALMAP" },
	{ TRF_GRID, "GRID" },
	{ TRF_MARKINGLYER, "MARKINGLYER" },
};

RTerrainShader::RTerrainShader() :
	m_pShaderFXPool(NULL),
	m_pFxMakeMask(NULL)
{
	Destroy();
}

RTerrainShader::~RTerrainShader()
{
}

void RTerrainShader::Init()
{
	if( !m_pShaderFXPool)
		m_pShaderFXPool = new RShaderFXPool();
	_ASSERT( m_pShaderFXPool );

	DWORD dwFlag = TRF_BAKED | TRF_RT3;
	RShaderFX* pShader = CreateShader( dwFlag );
	_ASSERT( pShader );
	if( pShader )
	{
		m_scFogFactor = pShader->GetConstant( "g_vFogFactor");
		m_scFogColor = pShader->GetConstant( "g_vFogColor");
		m_scWorld = pShader->GetConstant( "g_matWorld");
		m_scWorldViewProj = pShader->GetConstant( "g_matWorldViewProj");
		m_scWorldView = pShader->GetConstant( "g_matWorldView");
		m_scTextureTransform = pShader->GetConstant( "g_textureTransform");
		m_scTileTextureScaleAndTranslate = pShader->GetConstant( "g_tileTextureScaleAndTranslate");
		m_scAlphaTextureScaleAndTranslate = pShader->GetConstant( "g_alphaTextureScaleAndTranslate");
		m_scFarDist = pShader->GetConstant( "g_fFarDist");
		m_scResolution = pShader->GetConstant( "g_Resolution" );
		/// One-pass 사용
		m_scTileTextureScaleAndTranslateArray = pShader->GetConstant( "g_tileTextureScaleAndTranslateArray");
		/// marking에서 사용
		m_csMarkinglayer = pShader->GetConstant( "g_fMarkingLayer");
	}

	if( m_pFxMakeMask = REngine::GetDevice().CreateShaderFX( "TerrainMask.fx"))
	{
		m_scLayerCount = m_pFxMakeMask->GetConstant( "g_LayerCount");
	}
	_ASSERT( m_pFxMakeMask );

	//CreateShaderForSpotLight( true, true );
	//CreateShaderForSpotLight( true, false );
	//CreateShaderForSpotLight( false, true );
	//CreateShaderForSpotLight( false, false );
}

void RTerrainShader::Destroy()
{
	for( SHADERLIST::iterator it = m_ShaderList.begin(); it != m_ShaderList.end(); ++it )
	{
		RShaderFX* pShader = it->second;
		_ASSERT( pShader );
		REngine::GetDevice().DeleteShaderFX(pShader);
	}
	m_ShaderList.clear();
	SAFE_DELETE( m_pShaderFXPool );
	if( m_pFxMakeMask)
		REngine::GetDevice().DeleteShaderFX( m_pFxMakeMask);
	m_pFxMakeMask = NULL;
}


RShaderFX* RTerrainShader::GetShaderForMakeMask()
{
	return m_pFxMakeMask;
}


DWORD RTerrainShader::GetShaderFlag( RDirectionalLightSceneNode* pDirLight, bool bExportShadow, bool bImportShadow, bool bImportGI, bool bFilter, bool bLerp)
{
	DWORD dwFlag = 0;

	return dwFlag;
}

RShaderFX* RTerrainShader::CreateShader( DWORD dwFlag )
{
	SHADERLIST::const_iterator it = m_ShaderList.find( dwFlag );
	if( it != m_ShaderList.end() )
		return it->second;

	vector<string> strDefines;
	for( int i = 0; i < _countof(flagTable); ++i )
	{
		// 디파인을 무조건 밀어 넣고 1,0 값을 택일 하여 준다. 셰이더에서는 ifdef를 사용 하면 안되고 if만 사용 하여야 한다.
		strDefines.push_back( string(flagTable[i].strDefine) );
		if( ( flagTable[i].dwFlag & dwFlag ) != 0 )
			strDefines.push_back( "1" );
		else
			strDefines.push_back( "0" );
	}

	_ASSERT( m_pShaderFXPool );
	RShaderFX* pShader = REngine::GetDevice().CreateShaderFX( "Terrain.fx", strDefines, m_pShaderFXPool );

	_ASSERT( pShader );
	m_ShaderList.insert( SHADERLIST::value_type( dwFlag, pShader ) );
	return pShader;
}

DWORD RTerrainShader::GetRenderTargetCountFlag( int nMRT)
{
	DWORD dwFlag = 0;
	switch( nMRT)
	{
	case 1:
		dwFlag = TRF_RT1;	/// 라이트 프리 패스 Mstage
		break;
	case 2:
		dwFlag = TRF_RT2;	/// 라이트 프리 패스 Gstage
		break;
	case 3:
		dwFlag = TRF_RT3;	/// 디퍼드 Gstage
		break;
	}

	return dwFlag;
}


DWORD RTerrainShader::GetLayerCountFlag( int nLayerCount)
{
	DWORD dwFlag = 0;

	switch( nLayerCount)
	{
	case 7:	dwFlag = TRF_LAYER_COUNT_7; break;
	case 6:	dwFlag = TRF_LAYER_COUNT_6; break;
	case 5:	dwFlag = TRF_LAYER_COUNT_5; break;
	case 4:	dwFlag = TRF_LAYER_COUNT_4; break;
	case 3:	dwFlag = TRF_LAYER_COUNT_3; break;
	case 2:	dwFlag = TRF_LAYER_COUNT_2; break;
	}

	return dwFlag;
}


RShaderFX* RTerrainShader::GetShaderForBakedMapMStage( bool bLighting)
{
	DWORD dwFlag = TRF_BAKED | TRF_RT1 | TRF_FOG;
	if( bLighting)
		dwFlag |= TRF_LIGHTBUFFER;

	return CreateShader(dwFlag); 
}

RShaderFX* RTerrainShader::GetShaderForBakedMapGStage( int nMRT, bool bSpecular)
{
	DWORD dwFlag = TRF_BAKED;
	if( bSpecular)
		dwFlag |= TRF_SPECULAR;
	// MRT 갯수에 따라 용도가 다르다.
	dwFlag |= GetRenderTargetCountFlag(nMRT);
	
	return CreateShader(dwFlag); 
}


RShaderFX* RTerrainShader::GetShaderForGrid( int nMRT, bool bSpecular)
{
	DWORD dwFlag = TRF_GRID;
	if( bSpecular)
		dwFlag |= TRF_SPECULAR;

	// MRT 갯수에 따라 용도가 다르다.
	dwFlag |= GetRenderTargetCountFlag(nMRT);

	return CreateShader(dwFlag); 
}


RShaderFX* RTerrainShader::GetShaderForSplattingLayerOnePass( bool bLighting, int nLayerCount, int nRenderTargetCount, int* pSpecularFlagArray, int* pNormalmapFlagArray, bool bMarkingLayer)
{
	DWORD dwFlag = TRF_SPLATTING_ONEPASS;

	if( bMarkingLayer)
		dwFlag |= TRF_MARKINGLYER;

	/// bLighting이면 M stage용
	if( bLighting)
		dwFlag |= TRF_LIGHTBUFFER | TRF_FOG | TRF_RT1;
	else
	{
		dwFlag |= TRF_SPECULAR;
		// MRT 갯수에 따라 용도가 다르다.
		dwFlag |= GetRenderTargetCountFlag( nRenderTargetCount);
		_ASSERT( GetRenderTargetCountFlag( nRenderTargetCount));
	}

	dwFlag |= GetLayerCountFlag( nLayerCount);
	//_ASSERT( !GetLayerCountFlag( nLayerCount));

	bool buseNormalMap = false;
	if( nLayerCount > 0 && nLayerCount <= 9)
	{
		for( int i=0; i<nLayerCount; ++i)
		{
			if( pSpecularFlagArray && pSpecularFlagArray[i])
				dwFlag |= TRF_LAYER_0_SPECULAR << i;
			if( pNormalmapFlagArray && pNormalmapFlagArray[i])
			{
				dwFlag |= TRF_LAYER_0_NORMALMAP << i;
				buseNormalMap = true;
			}
		}
	}

	if( buseNormalMap && REngine::GetSceneManager().GetShaderCompositeManager()->GetUseNormalMap() )
		dwFlag |= TRF_USE_NORMALMAP;

	return CreateShader(dwFlag); 
}



}