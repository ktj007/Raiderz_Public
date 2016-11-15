#include "StdAfx.h"
#include "RVType.h"
#include "RShaderCompositeManager.h"
#include "RShaderFX.h"
#include "RSUtil.h"
#include "RPostEffectFinalScene.h"


namespace rs3
{

const RPostEffectFinalScene::FLAG_TABLE RPostEffectFinalScene::flagTable[] =
{
	{ PEF_GAMMA_CORRECT, "USE_GAMMA_CORRECT" },
	{ PEF_FXAA_PRESET0, "FXAA_PRESET0" },
	{ PEF_FXAA_PRESET1, "FXAA_PRESET1" },
	{ PEF_FXAA_PRESET2, "FXAA_PRESET2" },
	{ PEF_FXAA_PRESET3, "FXAA_PRESET3" },
	{ PEF_FXAA_PRESET4, "FXAA_PRESET4" },
	{ PEF_FXAA_PRESET5, "FXAA_PRESET5" },
	{ PEF_FXAA_PRESET6, "FXAA_PRESET6" },

};

RPostEffectFinalScene::RPostEffectFinalScene()
	: RPostEffectInterface( PE_FINAL_SCENE, RRP_FINAL_SCENE )
{
	SetActive( true );
}


RPostEffectFinalScene::~RPostEffectFinalScene()
{
	RPostEffectInterface::Destroy();

	for( SHADERLIST::iterator it = m_ShaderList.begin(); it != m_ShaderList.end();  )
	{
		RShaderFX* pShader = it->second;
		_ASSERT( pShader );
		REngine::GetDevice().DeleteShaderFX(pShader);
		it = m_ShaderList.erase( it );
	}
	m_ShaderList.clear();
	m_pPEFinalSceneShader = NULL;
}


bool RPostEffectFinalScene::Init( int nW, int nH )
{
	if(!RPostEffectInterface::Init(nW, nH))
		return false;

	if ( false == REngine::GetDevice().QueryFeature(RQF_PS20) )
		return false;

	if ( false == REngine::GetDevice().QueryFeature(RQF_PS30) )
		return false;

	CreateShader();

	Restore();

	return true;
}


void RPostEffectFinalScene::Destroy()
{
	RPostEffectInterface::Destroy();

	for( SHADERLIST::iterator it = m_ShaderList.begin(); it != m_ShaderList.end();  )
	{
		RShaderFX* pShader = it->second;
		_ASSERT( pShader );
		REngine::GetDevice().DeleteShaderFX(pShader);
		it = m_ShaderList.erase( it );
	}
	m_ShaderList.clear();


	m_pPEFinalSceneShader = NULL;

	Invalidate();
}

void RPostEffectFinalScene::CreateBuffers()
{

}

void RPostEffectFinalScene::DestroyBuffers()
{

}


void RPostEffectFinalScene::CreateShader()
{

	DWORD dwFlag( 0 );

	if( REngine::GetSceneManager().GetRenderingStrategy()->GetGammaCorrect() )
		dwFlag |= PEF_GAMMA_CORRECT;

	switch( REngine::GetSceneManager().GetRenderingStrategy()->GetFXAA_Level() )
	{
	case 0:
		dwFlag |= PEF_FXAA_PRESET0;
		break;
	case 1:
		dwFlag |= PEF_FXAA_PRESET1;
		break;
	case 2:
		dwFlag |= PEF_FXAA_PRESET2;
		break;
	case 3:
		dwFlag |= PEF_FXAA_PRESET3;
		break;
	case 4:
		dwFlag |= PEF_FXAA_PRESET4;
		break;
	case 5:
		dwFlag |= PEF_FXAA_PRESET5;
		break;
	case 6:
		dwFlag |= PEF_FXAA_PRESET6;
		break;
	}

	SHADERLIST::const_iterator it = m_ShaderList.find( dwFlag );
	if( it != m_ShaderList.end() )
	{
		m_pPEFinalSceneShader = it->second;
		return;
	}

	vector<string> strDefines;
	for( int i = 0; i < _countof(flagTable); ++i )
	{
		strDefines.push_back( string(flagTable[i].strDefine) );
		if( ( flagTable[i].dwFlag & dwFlag ) != 0 )
			strDefines.push_back( "1" );
		else
			strDefines.push_back( "0" );
	}

	m_pPEFinalSceneShader = REngine::GetDevice().CreateShaderFX("PostEffectFinal.fx", strDefines );
	m_ShaderList.insert( SHADERLIST::value_type( dwFlag, m_pPEFinalSceneShader ) );
}


void RPostEffectFinalScene::Render( RCameraSceneNode* pCamera, RTexture* pColorBuffer, RTexture* pNormalBuffer, RTexture* pDepthBuffer)
{
	RTechnique Technique;
	UINT uiPass(0);
	UINT uiPassCount(0);

	Technique = m_pPEFinalSceneShader->GetTechnique("FinalScene");

	m_pPEFinalSceneShader->GetConstant("g_txScene").SetTexture(pColorBuffer, REngine::GetDevicePtr());
	RVector2 vRcp( RVector2( 1.f / (float)REngine::GetDevice().GetScreenWidth(), 1.f / (float)REngine::GetDevice().GetScreenHeight()) );
	m_pPEFinalSceneShader->GetConstant("g_RcpFrame").SetVector2( vRcp  );

	Technique.Begin(&uiPassCount);
	Technique.BeginPass( uiPass );
	DrawFullScreenQuad( 0.0f, 0.0f, pColorBuffer->GetInfo().nTextureWidth, pColorBuffer->GetInfo().nTextureHeight);
	Technique.EndPass();
	Technique.End();	
}

}