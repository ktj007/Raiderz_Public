#include "StdAfx.h"
#include "RMaterialResource.h"
#include "RResourceID.h"
#include "RMaterial.h"
#include "RMesh.h"
#include "RMeshMgr.h"
#include "RProfileInfo.h"
#include "RResourceLoader.h"

namespace rs3 {

RMaterialResource::RMaterialResource(const char* _pSzResourceName, RSceneManager* pSceneManager)
	: RResource( _pSzResourceName, RRESOURCETYPE::RR_MATERIAL ),
	m_pMaterial(NULL),
	m_nUsingSystemMemorySize(0),
	m_nUsingVideoMemorySize(0)
{
	m_pSceneManager = pSceneManager;
}

RMaterialResource::~RMaterialResource()
{
	Destroy();
}

void RMaterialResource::Destroy()
{
	// by pok. 씬매니저 변경중
	_ASSERT( REngine::GetDevicePtr() );

	//if(!RGetDevice()) return;

	if (m_pMaterial)
	{
		for(int iLayer=0; iLayer<m_pMaterial->GetMaxLayer(); ++iLayer)
		{
			for(UINT iTex=0; iTex<m_pMaterial->GetTextureCountOfLayer(iLayer); ++iTex)
			{
				RMtrlTexInfo* pTexInfo = m_pMaterial->GetTextureInfo(iLayer, iTex);
				if(pTexInfo->GetTexture()) 
				{
					REngine::GetDevice().DeleteTexture(pTexInfo->GetTexture());
					pTexInfo->SetTexture(NULL);
				}
			}
			// 로딩중인것들도 삭제
			for(size_t j=0;j<m_textures[iLayer].size(); j++)
				REngine::GetDevice().DeleteTexture(m_textures[iLayer][j]);
			m_textures[iLayer].clear();
		}

		delete m_pMaterial;
		m_pMaterial = NULL;
	}
}

bool RMaterialResource::Open(MXmlElement *pElement, bool bBackground)
{
	m_pMaterial = new RMaterial;
	m_pMaterial->Open(pElement);

	_ASSERT(m_pMaterial->GetMaxLayer()<=MAX_TEXTURE_LAYER);

	for( int i=0;i<m_pMaterial->GetMaxLayer();i++) 
	{
		_ASSERT(m_textures[i].empty());
		for( UINT iTex=0; iTex<m_pMaterial->GetTextureCountOfLayer(i); ++iTex) 
		{
			RMtrlTexInfo* pTexInfo = m_pMaterial->GetTextureInfo(i, iTex);

			if(!pTexInfo->GetTextureName().empty()) 
			{
				if (pTexInfo->GetTextureName()[0] != '.') 
				{
					RTexture* pTexture = REngine::GetDevice().CreateTextureForBuildLater(pTexInfo->GetTextureName().c_str(),RTF_LINEAR,pTexInfo->GetCreationFlag(), RFMT_NONE, bBackground);
					m_textures[i].push_back(pTexture);
				}
			}
		}
	} // end of for

	return true;
}

bool RMaterialResource::CreateAsDefaultMaterial()
{
	m_pMaterial = new RMaterial;

	REngine::GetDevice().GetDefaultTexture()->AddRef();
	m_pMaterial->GetTextureInfo(0,0)->SetTexture(REngine::GetDevice().GetDefaultTexture());

	ForceSetState( RRS_LOADED );
	return true;
}

bool RMaterialResource::Fill()
{
	return true;
}

RResource::LOAD_RESULT RMaterialResource::Load()
{
	_ASSERT( REngine::GetDevicePtr() );

	m_nUsingSystemMemorySize = 0;
	m_nUsingVideoMemorySize = 0;

	bool bBackgroundLoading = IsBackgroundCreated();

	if(bBackgroundLoading)
	{
		for( int i=0;i<m_pMaterial->GetMaxLayer();++i) 
		{
			for( size_t j=0; j<m_textures[i].size(); ++j)
			{
				if( !m_textures[i][j]->IsLoadingCompleted() )
					return WAITING;
			}
		}
	}

	bool bSuccess = true;
	for( int i=0;i<m_pMaterial->GetMaxLayer();++i) 
	{
		if(m_textures[i].empty()) continue;

		for( UINT iTex=0; iTex<m_pMaterial->GetTextureCountOfLayer(i); ++iTex) 
		{
			RMtrlTexInfo* pTexInfo = m_pMaterial->GetTextureInfo(i, iTex);

			pTexInfo->SetTexture(m_textures[i][iTex]);
			if(pTexInfo->GetTexture() && pTexInfo->GetTexture()->GetState()==RRS_WRONG)
			{
				mlog_tool( "%s 텍스쳐 없음\n",pTexInfo->GetTexture()->GetName().c_str());
				bSuccess = false;
			}
		}
		m_textures[i].clear();
	}

	return bSuccess ? LOADED : WRONG;
}

}	//namespace rs3
