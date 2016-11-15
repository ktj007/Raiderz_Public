#include "StdAfx.h"
#include "ZLoginServerInfoManager.h"


void ZLoginServerInfoManager::Clear()
{
	for (ZLoginServerInfoMap::iterator itor = m_InfoMap.begin(); itor != m_InfoMap.end(); ++itor)
	{
		ZLoginServerInfo* pInfo = (*itor).second;
		delete pInfo;
	}
	m_InfoMap.clear();
}

bool ZLoginServerInfoManager::Insert(ZLoginServerInfo* pInfo)
{
	int nID = pInfo->m_nID;

	pair<ZLoginServerInfoMap::iterator, bool> pairRet;
	pairRet= m_InfoMap.insert(ZLoginServerInfoMap::value_type(nID, pInfo));
	return pairRet.second;
}

ZLoginServerInfo* ZLoginServerInfoManager::GetInfo(int nID)
{
	ZLoginServerInfoMap::iterator itor = m_InfoMap.find(nID);
	if (itor == m_InfoMap.end()) return NULL;

	return (*itor).second;
}
