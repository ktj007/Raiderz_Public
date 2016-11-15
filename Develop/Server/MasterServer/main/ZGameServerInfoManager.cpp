#include "stdafx.h"
#include "ZGameServerInfoManager.h"

void ZGameServerInfoManager::Clear()
{
	for (ZGameServerInfoMap::iterator itor = m_InfoMap.begin(); itor != m_InfoMap.end(); ++itor)
	{
		ZGameServerInfo* pInfo = (*itor).second;
		delete pInfo;
	}
	m_InfoMap.clear();
}

bool ZGameServerInfoManager::Insert( ZGameServerInfo* pInfo )
{
	int nID = pInfo->m_nID;

	pair<ZGameServerInfoMap::iterator, bool> pairRet;
	pairRet = m_InfoMap.insert(ZGameServerInfoMap::value_type(nID, pInfo));
	return pairRet.second;
}

ZGameServerInfo* ZGameServerInfoManager::GetInfo( int nID )
{
	ZGameServerInfoMap::iterator itor = m_InfoMap.find(nID);
	if (itor == m_InfoMap.end()) return NULL;

	return (*itor).second;
}