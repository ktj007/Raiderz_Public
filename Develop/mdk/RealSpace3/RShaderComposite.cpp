#include "StdAfx.h"
#include "RShaderComposite.h"
#include "RShaderCompositeManager.h"
#include "RDeviceD3D.h"
#include "RShaderFX.h"
#include "RResourceLoader.h"

// 조합된 쉐이더를 디버깅 하고 싶으면 다음 디파인 주석을 풀어주세요
// 자꾸 풀고 체크인하네~ 자나깨나 주석조심 체킨주석 다시보자!!  -COMMENT OZ
//#define _DEBUG_SHADER_COMPOSITOR

namespace rs3 {

RShaderComposite::RShaderComposite(RSCID rscComponents) 
	: m_pShaderFX(NULL), m_rscComponents(rscComponents)
{
}

RShaderComposite::~RShaderComposite(void)
{
}

bool RShaderComposite::Composite( RShaderCompositeManager* pCompositeManager )
{
	_ASSERT(m_shaderComposer.GetComponentCount() == 0);

	RSCID rscCurrent( 0, 1 );
	RSCID rscComponents;
	int count = 0;
	int offset = RSCID::ELEMENTS_NUM - 1;
	for(size_t i=0; i<pCompositeManager->GetShaderComponentCount(); i++)
	{
		if( (m_rscComponents & rscCurrent).IsNonZero() )
		{
			RShaderComponent* pComponent = pCompositeManager->GetShaderComponent(i);
			if(!pComponent)
			{
				mlog("shader composite error : shader component %s not found \n", componentNames[i] );
				return false;
			}

			if(!m_shaderComposer.AddComponent(pComponent))
			{
				mlog("shader composite error at %s : %s\n", pComponent->GetName().c_str(), m_shaderComposer.GetErrorString().c_str());
				return false;
			}
			rscComponents |= rscCurrent;
		}

		count++;
		if( count >= 64 )
		{
			count = 0;

			for( int j = offset; j < RSCID::ELEMENTS_NUM; ++j )
			{
				rscCurrent.v[j] = 0;
			}
			offset--;
			rscCurrent.v[offset] = 1;
		}
		else
		{
			rscCurrent.v[offset] = rscCurrent.v[offset]<<1;
		}
	}
	if( rscComponents != m_rscComponents )
	{
		mlog("shader composite error : some components are missing, requested %s != composited %s \n",
			m_rscComponents.ToString(10).c_str(), rscComponents.ToString(10).c_str() );
		return false;
	}

	if( !m_shaderComposer.Make() )
	{
		mlog("shader composite error : %s\n", m_shaderComposer.GetErrorString().c_str() );

#ifdef _DEBUG
		FILE *fp = fopen("composite_error.fx","wt");
		if ( fp )
		{
			fprintf(fp,"%s\n", m_shaderComposer.GetShaderString().c_str());
			fclose(fp);
		}
#endif
		return false;
	}

	return true;
}

bool RShaderComposite::CompositeAndBuildShader( RShaderCompositeManager* pCompositeManager, bool bBackgroundLoading )
{
	if( !Composite( pCompositeManager ) )
	{
		_ASSERT( false );
		return false;
	}

	INIT_TCHECK("/check_shadercomposite_createtime");
	BEGIN_TCHECK;

	_ASSERT( NULL == m_pShaderFX );
	m_pShaderFX = REngine::GetDevicePtr()->CreateShaderFX();

	vector<string> vecMacro;

	char szFileName[256];
	sprintf_s(szFileName,"actor%s.fx",m_rscComponents.ToString(10).c_str());


	// COMMENTOZ
#ifdef _DEBUG_SHADER_COMPOSITOR


	FILE *file;
	file = fopen(szFileName,"w+");
	fprintf(file,m_shaderComposer.GetShaderString().c_str());
	fclose(file);

	mlog( " shader make : %s \n", szFileName);

	m_pShaderFX->SetFromFile(szFileName, vecMacro, pCompositeManager->GetShaderEffectPool());
#else
	m_pShaderFX->SetFromString(m_shaderComposer.GetShaderString(), vecMacro, pCompositeManager->GetShaderEffectPool(), szFileName);
#endif

	m_pShaderFX->SetLoadingPriority( RBGWK_HIGHEST );
	m_pShaderFX->BuildAll(bBackgroundLoading, NULL);

	END_TCHECK("RShaderComposite::Create");

	return true;
}

bool RShaderComposite::IsLoadingSucceeded()
{
	return ( m_pShaderFX && m_pShaderFX->IsLoadingSucceeded() );
}

void RShaderComposite::Destroy()
{
	if(m_pShaderFX)
	{
		REngine::GetDevice().DeleteShaderFX(m_pShaderFX);
		m_pShaderFX = NULL;
	}
}


}
