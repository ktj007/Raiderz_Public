#pragma once

#include "RResourceManager.h"
#include "MTstring.h"

class XResourceRefMgr
{
private:
	bool	m_bCreated;

	set<tstring>				m_ManagedResourceNameSet;
	RRESOURCEHASHMAP		m_ManagedResourceHashMap;

public:
	XResourceRefMgr();
	virtual ~XResourceRefMgr();
	void Create();
	void Destroy(RDevice* pDevice);

	void ReserveAddRef(const TCHAR* szResourceName);
	void ReleaseAddRef(const TCHAR* szResourceName);
	void OnCreateResource(RResource* pResource);

	void Grab(RResource* pResource);
	void Drop(RDevice* pDevice, RResource* pResource);
	void DropAll(RDevice* pDevice);

	bool IsCreated() { return m_bCreated; }
	bool IsReservedAddRef(const tstring& strResourceName);
};