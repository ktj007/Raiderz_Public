#include "stdafx.h"
#include "XResourceRefMgr.h"
#include "RResourceID.h"
#include "RDevice.h"
#include "RTexture.h"
#include "MLocale.h"

XResourceRefMgr::XResourceRefMgr()
{

}

XResourceRefMgr::~XResourceRefMgr()
{

}

void XResourceRefMgr::Create()
{
	m_bCreated = true;
}

void XResourceRefMgr::Destroy(RDevice* pDevice)
{
	if (!m_bCreated) return;

	DropAll(pDevice);

	m_bCreated = false;
}

void XResourceRefMgr::ReserveAddRef( const TCHAR* szResourceName )
{
	m_ManagedResourceNameSet.insert(tstring(szResourceName));
}

void XResourceRefMgr::ReleaseAddRef( const TCHAR* szResourceName )
{
	set<tstring>::iterator itor = m_ManagedResourceNameSet.find(tstring(szResourceName));
	if (itor != m_ManagedResourceNameSet.end())
	{
		m_ManagedResourceNameSet.erase(itor);
	}
}

bool XResourceRefMgr::IsReservedAddRef( const tstring& strResourceName )
{
	set<tstring>::iterator itor = m_ManagedResourceNameSet.find(strResourceName);
	if (itor != m_ManagedResourceNameSet.end())
	{
		return true;
	}
	return false;
}

void XResourceRefMgr::Grab( RResource* pResource )
{
	string strRes = pResource->GetName();

	RRESOURCEHASHMAP::iterator itor = m_ManagedResourceHashMap.find(strRes);
	if (itor != m_ManagedResourceHashMap.end()) return;

	m_ManagedResourceHashMap.insert(RRESOURCEHASHMAP::value_type(strRes, pResource));
	pResource->AddRef();
}

void XResourceRefMgr::Drop( RDevice* pDevice, RResource* pResource )
{
	string strRes = pResource->GetName();

	RRESOURCEHASHMAP::iterator itor = m_ManagedResourceHashMap.find(strRes);
	if (itor != m_ManagedResourceHashMap.end())
	{
		m_ManagedResourceHashMap.erase(itor);

		if (pDevice && pResource->GetResID() == RRESOURCETYPE::RR_TEXTURE)
		{
			// 텍스쳐의 레퍼런스 카운터는 RTextureManager에서 관리한다.
			RTexture* pTexture = dynamic_cast<RTexture*>(pResource);
			pDevice->DeleteTexture(pTexture);
		}
		else
		{
			pResource->Release();
		}
	}
}

void XResourceRefMgr::OnCreateResource( RResource* pResource )
{
	tstring strRes = MLocale::ConvAnsiToTCHAR(pResource->GetName().c_str());

	if (IsReservedAddRef(strRes))
	{
		Grab(pResource);
	}
}

void XResourceRefMgr::DropAll(RDevice* pDevice)
{
	RRESOURCEHASHMAP bufferMap = m_ManagedResourceHashMap;

	for (RRESOURCEHASHMAP::iterator itor = bufferMap.begin(); itor != bufferMap.end(); ++itor)
	{
		RResource* pResource = (*itor).second;
		Drop(pDevice, pResource);
	}
	m_ManagedResourceHashMap.clear();
}