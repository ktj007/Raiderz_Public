#include "StdAfx.h"
#include "ZDuplicationLoginManager.h"
#include "AStlUtil.h"


ZDuplicationLoginManager::ZDuplicationLoginManager()
{
}

ZDuplicationLoginManager::~ZDuplicationLoginManager()
{
	SAFE_DELETE_MAP(m_mapDuplicationLogin);
}

bool ZDuplicationLoginManager::IsExist(AID nAID)
{
	ZDuplicationLoginMap::iterator itr = m_mapDuplicationLogin.find(nAID);
	return (itr != m_mapDuplicationLogin.end());
}

bool ZDuplicationLoginManager::Add(AID nAID, const wstring& strUserID, MUID uidPlayer, PmUserData* pmUserData)
{
	if (IsExist(nAID))
		return false;

	ZDUPLICATION_LOGIN_INFO* pDuplicationLoginInfo = new ZDUPLICATION_LOGIN_INFO();
	pDuplicationLoginInfo->strUserID = strUserID;
	pDuplicationLoginInfo->uidPlayer = uidPlayer;
	if (pmUserData != NULL)
	{
		pDuplicationLoginInfo->bExistPmUserData = true;
		pDuplicationLoginInfo->pmUserData = (*pmUserData);
	}
	m_mapDuplicationLogin.insert(ZDuplicationLoginMap::value_type(nAID, pDuplicationLoginInfo));
	return true;
}

void ZDuplicationLoginManager::Del(AID nAID)
{
	ZDuplicationLoginMap::iterator itr = m_mapDuplicationLogin.find(nAID);
	if (itr == m_mapDuplicationLogin.end()) return;

	SAFE_DELETE((*itr).second);
	m_mapDuplicationLogin.erase(itr);
}

ZDUPLICATION_LOGIN_INFO* ZDuplicationLoginManager::Get(AID nAID)
{
	ZDuplicationLoginMap::iterator itr = m_mapDuplicationLogin.find(nAID);
	if (itr == m_mapDuplicationLogin.end()) return NULL;

	return (*itr).second;
}