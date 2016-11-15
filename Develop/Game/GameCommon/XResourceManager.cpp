#include "stdafx.h"
#include "RMeshMgr.h"
#include "XResourceManager.h"
#include "REffectInstance.h"
#include "RSceneManager.h"
#include "XResourceRefMgr.h"
#include "RResourceManager.h"
#include "RDevice.h"
#include "RTextureManager.h"
#include "XBaseResourceGrabber.h"
#include "XFieldResourceGrabber.h"

#include "REffectManager.h"

//////////////////////////////////////////////////////////////////////////
void XResourceManager::InitCaching(rs3::RSceneManager* pSceneManager, const TCHAR* szPreLoad)
{
	m_Cache.Init(pSceneManager, szPreLoad);
}

void XResourceManager::FiniCaching()
{
	m_Cache.Fini();

	if (IsUsingEffectCacheLog())
		LogEffectCache(_T(""));
}

void XResourceManager::UpdateCaching(float fDelta)
{
	m_Cache.Update(fDelta);
}

void XResourceManager::ClearCaching()
{
	m_Cache.Clear();
}

XResourceManager::XResourceManager() : m_pAlwaysResourceGrabber(NULL)
{
	m_pFieldResourceGrabber = new XFieldResourceGrabber();
}

XResourceManager::~XResourceManager()
{
	SAFE_DELETE(m_pFieldResourceGrabber);
	SAFE_DELETE(m_pAlwaysResourceGrabber);
}

void XResourceManager::Init(XBaseAlwaysResourceGrabber* pAlwaysResourceGrabber)
{
	m_pAlwaysResourceGrabber = pAlwaysResourceGrabber;
	m_pAlwaysResourceGrabber->Create();
}

void XResourceManager::Fini(RDevice* pDevice)
{
	FiniCaching();
	if (m_pAlwaysResourceGrabber)
		m_pAlwaysResourceGrabber->Destroy(pDevice);
}

void XResourceManager::GrabFieldResource( tstring& strZoneResFile )
{
	m_pFieldResourceGrabber->Grab(strZoneResFile);
}

void XResourceManager::DropFieldResource()
{
	m_pFieldResourceGrabber->Drop();
}

void XResourceManager::OnCreateResource( RResource* pResource )
{
	m_pAlwaysResourceGrabber->OnCreateResource(pResource);
}

void XResourceManager::GrabAlwaysResources()
{
	m_pAlwaysResourceGrabber->Grab();
}

std::vector< std::tstring >* XResourceManager::GetNpcEffectCacheList( int _nId )
{
	return m_Cache.GetNPCEffectPreLoadList(_nId);
}
