#include "StdAfx.h"
#include "RLightingShader.h"
#include "REngine.h"
#include "RShadowStrategy.h"
#include "RPSSMShadowStrategy.h"
#include "RRenderHelper.h"
#include "RShaderCompositeManager.h"
#include "RDirectionalLightSceneNode.h"
#include "RShaderFX.h"
#include "REnvironmentSet.h"


namespace rs3 {

const RLightingShader::FLAG_TABLE RLightingShader::flagTable[] =
{
	{ DLF_DIR_LIGHT, "DIR_LIGHT" },
	{ DLF_LIGHTING, "LIGHTING" },
	{ DLF_SPECULAR, "SPECULAR" },
	{ DLF_RIM, "RIM" },
	{ DLF_CLOUD, "CLOUD" },
	{ DLF_CLOUD2, "CLOUD2" },
	{ DLF_CALC_SHADOW, "CALC_SHADOW" },
	{ DLF_SHADOW_PSSM, "SHADOW_PSSM" },
	{ DLF_SHADOW_LERP_SPLIT, "SHADOW_LERP_SPLIT" },
	{ DLF_SHADOW_FILTER_POISSON, "SHADOW_FILTER_POISSON" },
	{ DLF_EXPORT_SHADOW_ONLY, "EXPORT_SHADOW_ONLY" },
	{ DLF_IMPORT_SHADOW, "IMPORT_SHADOW" },
	{ DLF_IMPORT_GI, "IMPORT_GI" },
	{ DLF_POINT_LIGHT, "POINT_LIGHT" },
	{ DLF_AREA_LIGHT, "AREA_LIGHT" },
	{ DLF_SPOT_LIGHT, "SPOT_LIGHT" },
	{ DLF_SPOT_INSTANCING, "SPOT_INSTANCING" },
	{ DLF_SPOT_MAP, "SPOT_MAP" },
	{ DLF_PROJECTION_SHADOW, "PROJECTION_SHADOW" },
	{ DLF_SUB_LIGHT_COUNT_1, "SUB_LIGHT_COUNT_1" },
	{ DLF_SUB_LIGHT_COUNT_2, "SUB_LIGHT_COUNT_2" },
	{ DLF_SUB_LIGHT_COUNT_3, "SUB_LIGHT_COUNT_3" },
	{ DLF_SUB_LIGHT_COUNT_4, "SUB_LIGHT_COUNT_4" },
	{ DLF_SUB_LIGHT_ONLY, "SUB_LIGHT_ONLY" },
	{ DLF_SKY_SPECULAR, "SKY_SPECULAR" },
	{ DLF_GAMMA_CORRECT, "USE_GAMMA_CORRECT" },
};

RLightingShader::RLightingShader() :
	m_pShaderFXPool(NULL)
{
	Destroy();
}

RLightingShader::~RLightingShader()
{
}

void RLightingShader::Init()
{
	if( !m_pShaderFXPool)
		m_pShaderFXPool = new RShaderFXPool();
	_ASSERT( m_pShaderFXPool );

	DWORD dwFlag = DLF_DIR_LIGHT | DLF_LIGHTING | DLF_SPECULAR | DLF_RIM | DLF_CALC_SHADOW | DLF_SHADOW_PSSM;
	RShaderFX* pShader = CreateShader( dwFlag );
	_ASSERT( pShader );
	if( pShader )
	{
		m_scWorldViewProj = pShader->GetConstant( "g_matWorldViewProj" );
		m_scWorldView = pShader->GetConstant( "g_matWorldView" );
		m_scFarClipCornerInView = pShader->GetConstant( "g_vFarClipCornerInView" );
		m_scResolution = pShader->GetConstant( "g_vResolution" );
		m_scLightTransform = pShader->GetConstant( "g_lightTransform" );
		m_scLightViewTransform = pShader->GetConstant( "g_lightViewTransform" );
		m_scLightTransformArray = pShader->GetConstant( "g_lightTransformArray" );
		m_scLightViewTransformArray = pShader->GetConstant( "g_lightViewTransformArray" );
		m_scDepthTexelSize = pShader->GetConstant( "g_fDepthTexelSize" );
		m_scCSMSplitInfo = pShader->GetConstant( "g_vCSMSplitInfo" );
		m_scSplitCount = pShader->GetConstant( "g_nSplitCount" );
		m_scDepthBias = pShader->GetConstant( "g_vDepthBias" );
		m_scCloudFactor = pShader->GetConstant( "g_fCloudFactor" );
		m_scCloudTransform = pShader->GetConstant( "g_cloudTransform" );
		m_scCloudFactor2 = pShader->GetConstant( "g_fCloudFactor2" );
		m_scCloudTransform2 = pShader->GetConstant( "g_cloudTransform2" );
		m_scLightVec = pShader->GetConstant("g_vLightVec");
		m_scLightColor = pShader->GetConstant("g_vLightColor");
		m_scLightAmbient = pShader->GetConstant("g_vLightAmbient");
		m_scShadowLuminosity = pShader->GetConstant("g_shadowLuminosity");
		m_scSubLightColor = pShader->GetConstant("g_vSubLightColor");
		m_scSubLightDir = pShader->GetConstant("g_vSubLightDir");
		m_scLightPosition = pShader->GetConstant("g_vLightPosition");
		m_scAttenuationInfo = pShader->GetConstant("g_vAttenuationInfo");
		m_scVisibility = pShader->GetConstant("g_fVisibility");
		m_scLightDirectionFOV = pShader->GetConstant("g_vLightDirectionFOV");
		m_scLightPositionArray = pShader->GetConstant("g_vLightPositionArray");
		m_scLightColorArray = pShader->GetConstant("g_vLightColorArray");
		m_scLightInfoArray = pShader->GetConstant("g_vLightInfoArray");
		m_scLightDirectionFOVArray = pShader->GetConstant("g_vLightDirectionFOVArray");
		m_scSkySpecularTerm = pShader->GetConstant("g_vSkySpecularTerm");
		m_scSubLightIntensity = pShader->GetConstant("g_fSubLightIntensity");
	}

	CreateShaderForPointLight( true);
	CreateShaderForAreaLight( true);
	CreateShaderForSpotLightInstancing( true);
	CreateShaderForSpotLight( true, true, true );
	CreateShaderForSpotLight( true, false, true );
	CreateShaderForSpotLight( false, true, true );
	CreateShaderForSpotLight( false, false, true );
	CreateShaderForShadowReceiver();

}

void RLightingShader::Destroy()
{
	for( SHADERLIST::iterator it = m_ShaderList.begin(); it != m_ShaderList.end(); ++it )
	{
		RShaderFX* pShader = it->second;
		_ASSERT( pShader );
		REngine::GetDevice().DeleteShaderFX(pShader);
	}
	m_ShaderList.clear();

	SAFE_DELETE( m_pShaderFXPool );

	m_scWorldViewProj.Destroy();
	m_scWorldView.Destroy();
	m_scFarClipCornerInView.Destroy();
	m_scResolution.Destroy();
	m_scLightSizeUV.Destroy();
	m_scLightFrustumDepth.Destroy();
	m_scLightTransform.Destroy();
	m_scLightViewTransform.Destroy();
	m_scLightTransformArray.Destroy();
	m_scLightViewTransformArray.Destroy();
	m_scDepthTexelSize.Destroy();
	m_scCSMSplitInfo.Destroy();
	m_scSplitCount.Destroy();
	m_scDepthBias.Destroy();
	m_scCloudFactor.Destroy();
	m_scCloudTransform.Destroy();
	m_scCloudFactor2.Destroy();
	m_scCloudTransform2.Destroy();
	m_scLightVec.Destroy();
	m_scLightColor.Destroy();
	m_scLightAmbient.Destroy();
	m_scShadowLuminosity.Destroy();
	m_scSubLightColor.Destroy();
	m_scSubLightDir.Destroy();
	m_scLightPosition.Destroy();
	m_scAttenuationInfo.Destroy();
	m_scVisibility.Destroy();
	m_scLightDirectionFOV.Destroy();
	m_scLightPositionArray.Destroy();
	m_scLightColorArray.Destroy();
	m_scLightInfoArray.Destroy();
	m_scLightDirectionFOVArray.Destroy();
	m_scSkySpecularTerm.Destroy();
	m_scSubLightIntensity.Destroy();
}

RShaderFX* RLightingShader::CreateDirectionalLightShader( RDirectionalLightSceneNode* pDirLight, bool bSpecular, bool bRim, bool bExportShadow, bool bImportShadow,
														 bool bImportGI, bool bFilter, bool bLerp, int nSubLightCount)
{
	DWORD dwFlag = DLF_DIR_LIGHT;

	// 광원의 길이가 0이면 Ambient만 적용
	if(!pDirLight || pDirLight->GetDirection().Length() <= 0)
		return CreateShader(dwFlag);

	dwFlag |= DLF_LIGHTING;

	if( REngine::GetSceneManager().GetRenderingStrategy()->GetGammaCorrect() )
		dwFlag |= DLF_GAMMA_CORRECT;

	// 셰도우만 출력 하는 것이 아닐 경우만 스페큘러와 림 처리. 굳이 이 체크는 안해도 fx에서 디파인 분기로 막히긴 함.
	if( bExportShadow)
	{
		dwFlag |= DLF_EXPORT_SHADOW_ONLY;
	}
	else
	{
		if( bSpecular)
			dwFlag |= DLF_SPECULAR;
		if( bRim)
			dwFlag |= DLF_RIM;
	}
	
	if( bImportShadow)
	{
		dwFlag |= DLF_IMPORT_SHADOW;
	}
	else
	{
		RSHADOWMETHOD shadowMethod = REngine::GetSceneManager().GetRealtimeShadowMethod();
		if( shadowMethod != SHADOW_NONE )
		{
			dwFlag |= DLF_CALC_SHADOW;
			if( bLerp)
				dwFlag |= DLF_SHADOW_LERP_SPLIT;
			if( bFilter)
				dwFlag |= DLF_SHADOW_FILTER_POISSON;

			if( shadowMethod == SHADOW_PSSM )
				dwFlag |= DLF_SHADOW_PSSM;
			else if( shadowMethod == SHADOW_PROJECTION )
				dwFlag |= DLF_PROJECTION_SHADOW;

			// 그림자를 그려야 할 시만 구름 그림자 역시 그린다. 이 조건 역시 fx에서 디파인 분기로 막히긴 함.
			REnvironmentSet* pEnvironmentSet = REngine::GetSceneManager().GetCurrentRootSceneNode(false)->GetEnvironmentSet();
			if( pEnvironmentSet )
			{
				size_t n = pEnvironmentSet->m_vecTilling.size();

				if( n )
				{
					dwFlag |= DLF_CLOUD;

					if( n == 2 )
						dwFlag |= DLF_CLOUD2;
				}
			}
		}
	}

	if( bImportGI)
	{
		dwFlag |= DLF_IMPORT_GI;
	}

	if(nSubLightCount>0)
	{
		if(nSubLightCount==1)
			dwFlag |= DLF_SUB_LIGHT_COUNT_1;
		else if(nSubLightCount==2)
			dwFlag |= DLF_SUB_LIGHT_COUNT_2;
		else if(nSubLightCount==3)
			dwFlag |= DLF_SUB_LIGHT_COUNT_3;
		else if(nSubLightCount==4)
			dwFlag |= DLF_SUB_LIGHT_COUNT_4;
	}

	if( pDirLight->IsUseSkySpecular() )
		dwFlag |= DLF_SKY_SPECULAR;

	return CreateShader(dwFlag);
}


void RLightingShader::RenderQuad( RTechnique* pTech, DWORD dwW, DWORD dwH)
{
	RPFC_THISFUNC;
	if(!pTech)
		return;

	pTech->Begin(NULL, TECH_PRESERVE_NOT);
	pTech->BeginPass();
	RRenderHelper::RenderQuad(0, 0, (float)dwW, (float)dwH);
	pTech->EndPass();
	pTech->End();
}

RShaderFX* RLightingShader::CreateShader( DWORD dwFlag )
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
	RShaderFX* pShader = REngine::GetDevice().CreateShaderFX( "Lighting.fx", strDefines, m_pShaderFXPool );

	_ASSERT( pShader );
	m_ShaderList.insert( SHADERLIST::value_type( dwFlag, pShader ) );
	return pShader;
}

RShaderFX* RLightingShader::CreateShaderForPointLight( bool bSpecular)
{
	DWORD dwFlag = DLF_POINT_LIGHT;
	if( bSpecular)
		dwFlag |= DLF_SPECULAR;

	return CreateShader( dwFlag );
}

RShaderFX* RLightingShader::CreateShaderForAreaLight( bool bSpecular)
{
	RPFC_THISFUNC;
	DWORD dwFlag = DLF_POINT_LIGHT | DLF_AREA_LIGHT;
	if( bSpecular)
		dwFlag |= DLF_SPECULAR;

	return CreateShader( dwFlag );
}

RShaderFX* RLightingShader::CreateShaderForSpotLightInstancing( bool bSpecular)
{
	DWORD dwFlag = DLF_SPOT_LIGHT | DLF_SPOT_INSTANCING;
	if( bSpecular)
		dwFlag |= DLF_SPECULAR;

	return CreateShader( dwFlag );
}

RShaderFX* RLightingShader::CreateShaderForSpotLight( bool bShadow, bool bSpotmap, bool bSpecular)
{
	DWORD dwFlag = DLF_SPOT_LIGHT;
	if( bSpecular)
		dwFlag |= DLF_SPECULAR;
	if( bShadow )
		dwFlag |= DLF_CALC_SHADOW;
	if( bSpotmap )
		dwFlag |= DLF_SPOT_MAP;

	return CreateShader( dwFlag );
}

RShaderFX* RLightingShader::CreateShaderForShadowReceiver()
{
	return CreateShader( DLF_PROJECTION_SHADOW );
}

RShaderFX* RLightingShader::CreateShaderForSubLight(int nCount)
{
	RPFC_THISFUNC;
	DWORD dwFlag = DLF_SUB_LIGHT_ONLY;

	if(nCount==1)
		dwFlag |= DLF_SUB_LIGHT_COUNT_1;
	else if(nCount==2)
		dwFlag |= DLF_SUB_LIGHT_COUNT_2;
	else if(nCount==3)
		dwFlag |= DLF_SUB_LIGHT_COUNT_3;
	else if(nCount==4)
		dwFlag |= DLF_SUB_LIGHT_COUNT_4;

	return CreateShader( dwFlag );
}

}