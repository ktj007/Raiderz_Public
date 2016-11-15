#include "stdafx.h"
#include "XEngine.h"
#include "RSceneManager.h"
#include "RWeatherEffectSet.h"
#include "XResourceManager.h"
#include "XResourceRefMgr.h"
#include "MLocale.h"

XEngine::XEngine() : m_pDevice(NULL), m_pResourceManager(NULL)
{
	m_pWeatherEffectSet = new rs3::RWeatherEffectSet();
}

XEngine::~XEngine()
{
	SAFE_DELETE(m_pWeatherEffectSet);
}

void XEngine::Init( RDevice* pDevice, XResourceManager* pResourceManager )
{
	m_pDevice = pDevice;
	m_pResourceManager = pResourceManager;
}

void XEngine::ReleaseMesh( RMesh* pMesh )
{
	REngine::GetSceneManager().ReleaseResource( pMesh );
}

RMesh* XEngine::CreateMesh( const TCHAR* szMeshName )
{
	RMesh* pMesh = REngine::GetSceneManager().CreateResource< RMesh >(MLocale::ConvTCHARToAnsi(szMeshName).c_str());

	if (pMesh)
	{
		m_pResourceManager->OnCreateResource(pMesh);
	}

	return pMesh;
}

bool XEngine::InitSharedResource(bool bPreloadPlayersMesh)
{
	if (m_pWeatherEffectSet->LoadXML() == false)
	{
		return false;
	}

	// 디버깅 시에는 플레이어 미리 로딩 안함
#ifdef _DEBUG
	return true;
#endif

	if (bPreloadPlayersMesh)
	{
		RMesh* pHumanFemaleMesh = CreateMesh(_T("data/model/player/hf/hf.elu"));
		if (pHumanFemaleMesh) 
		{
			pHumanFemaleMesh->BuildAll(false);
			m_vecSharedMeshes.push_back(pHumanFemaleMesh);
		}

		RMesh* pHumanMaleMesh = CreateMesh(_T("data/model/player/hm/hm.elu"));
		if (pHumanMaleMesh) 
		{
			pHumanMaleMesh->BuildAll(false);
			m_vecSharedMeshes.push_back(pHumanMaleMesh);
		}
	}

	return true;
}

void XEngine::DestroySharedResource()
{
	for (size_t i = 0; i < m_vecSharedMeshes.size(); i++)
	{
		RMesh* pMesh = m_vecSharedMeshes[i];
		ReleaseMesh(pMesh);
	}
	m_vecSharedMeshes.clear();

	SAFE_DELETE(m_pWeatherEffectSet);
}

RWeatherEffectSet* XEngine::GetWeatherEffectSet()
{
	return m_pWeatherEffectSet;
}

RTexture* XEngine::CreateTexture( const TCHAR*fileName, const RTEXTUREFILTERTYPE filter /*= RTF_LINEAR*/, DWORD flags /*= RCT_AUTOGENMIPMAP*/, RFORMAT pf /*= RFMT_NONE*/, bool bBackground /*= false*/, RResource::LoadingController* pController /*= NULL*/ )
{
	if (m_pDevice == NULL) return NULL;

	RTexture* pTexture = m_pDevice->CreateTexture(MLocale::ConvTCHARToAnsi(fileName).c_str(),
							 filter,
							 flags,
							 pf,
							 bBackground,
							 pController);
	if (pTexture)
	{
		m_pResourceManager->OnCreateResource(pTexture);
	}

	return pTexture;
}

RTexture* XEngine::CreateTexture( int nWidth, int nHeight, RFORMAT pf , const RTEXTUREFILTERTYPE filter /*= RTF_LINEAR*/, DWORD flags /*= 0*/, void *pixels /*= NULL*/, const TCHAR* szName /*= NULL*/ )
{
	if (m_pDevice == NULL) return NULL;

	RTexture* pTexture = m_pDevice->CreateTexture(nWidth,
									nHeight,
									pf,
									filter,
									flags,
									pixels,
									MLocale::ConvTCHARToAnsi(szName).c_str());

	if (pTexture)
	{
		m_pResourceManager->OnCreateResource(pTexture);
	}

	return pTexture;
}


void XEngine::DeleteTexture( RTexture* pTexture )
{
	if (m_pDevice == NULL) return;

	m_pDevice->DeleteTexture(pTexture);
}