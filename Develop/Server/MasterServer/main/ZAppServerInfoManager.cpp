#include "StdAfx.h"
#include "ZAppServerInfoManager.h"


void ZAppServerInfoManager::Clear()
{
	for (ZAppServerInfoMap::iterator itor = m_InfoMap.begin(); itor != m_InfoMap.end(); ++itor)
	{
		ZAppServerInfo* pInfo = (*itor).second;
		delete pInfo;
	}
	m_InfoMap.clear();
}

bool ZAppServerInfoManager::Insert(ZAppServerInfo* pInfo)
{
	int nID = pInfo->m_nID;

	pair<ZAppServerInfoMap::iterator, bool> pairRet;
	pairRet= m_InfoMap.insert(ZAppServerInfoMap::value_type(nID, pInfo));
	return pairRet.second;
}

ZAppServerInfo* ZAppServerInfoManager::GetInfo(int nID)
{
	ZAppServerInfoMap::iterator itor = m_InfoMap.find(nID);
	if (itor == m_InfoMap.end()) return NULL;

	return (*itor).second;
}
