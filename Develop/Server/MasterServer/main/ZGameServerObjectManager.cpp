#include "stdafx.h"
#include "ZGameServerObjectManager.h"
#include "ZGameServerObject.h"
#include "ZGlobal.h"
#include "STransData.h"
#include "ZSmartFieldAllocator.h"

ZGameServerObjectManager::ZGameServerObjectManager() : SCommObjectManager()
{

}

ZGameServerObjectManager::~ZGameServerObjectManager()
{
	DeleteAllObject();
}

ZGameServerObject* ZGameServerObjectManager::NewGameServerObject( MUID uid, ZGameServerInfo* pGameServerInfo )
{
	ZGameServerObject* pNewPlayerObject = new ZGameServerObject(uid, pGameServerInfo);	
	pNewPlayerObject->Create();
	return pNewPlayerObject;
}

SCommObject* ZGameServerObjectManager::NewObject( MUID& uid )
{
	_ASSERT(0);
	return NULL;
}

ZGameServerObject* ZGameServerObjectManager::FindGameServer( MUID uid )
{
	SCommObject* pCommObject = GetClient(uid);
	if (pCommObject)
	{
		return static_cast<ZGameServerObject*>(pCommObject);
	}
	return NULL;
}

ZGameServerObject* ZGameServerObjectManager::FindGameServer( int nGameServerID )
{
	if (nGameServerID <= 0)	return NULL;

	ZServerObjectIDMap::iterator itor = m_GameServerObjectIDMap.find(nGameServerID);
	if (itor != m_GameServerObjectIDMap.end())
	{
		return (*itor).second;
	}

	return NULL;
}

ZGameServerObject* ZGameServerObjectManager::GetFirstGameServer(void)
{	
	SCommObjectMap::iterator it = m_ClientObjectMap.begin();

	if (it == m_ClientObjectMap.end())
	{
		return NULL;
	}

	return dynamic_cast<ZGameServerObject*>(it->second);
}

ZGameServerObject* ZGameServerObjectManager::FindAnotherGameServer(int nIgnoreServerID)
{
	SCommObjectMap::iterator itor = m_ClientObjectMap.begin();
	for (; itor != m_ClientObjectMap.end(); ++itor)
	{
		ZGameServerObject* pServerObject = static_cast<ZGameServerObject*>((*itor).second);
		if (pServerObject->GetID() != nIgnoreServerID)
			return pServerObject;
	}
	return NULL;
}

void ZGameServerObjectManager::AddObject( SCommObject* pClient )
{
	SCommObjectManager::AddObject(pClient);

	ZGameServerObject* pGameServerObject = static_cast<ZGameServerObject*>(pClient);
	m_GameServerObjectIDMap.insert(ZServerObjectIDMap::value_type(pGameServerObject->GetID(), pGameServerObject));
}

void ZGameServerObjectManager::DeleteObject( MUID uidClient )
{
	ZGameServerObject* pGameServerObject = FindGameServer(uidClient);
	if (pGameServerObject)
	{
		ZServerObjectIDMap::iterator itor = m_GameServerObjectIDMap.find(pGameServerObject->GetID());
		if (itor != m_GameServerObjectIDMap.end())
		{
			m_GameServerObjectIDMap.erase(itor);
		}
	}

	SCommObjectManager::DeleteObject(uidClient);
}

void ZGameServerObjectManager::DeleteAllObject()
{
	m_GameServerObjectIDMap.clear();

	SCommObjectManager::DeleteAllObject();
}

void ZGameServerObjectManager::Clear()
{
	m_GameServerObjectIDMap.clear();

	SCommObjectManager::Clear();
}

MUID ZGameServerObjectManager::IDToUID( int nID )
{
	ZGameServerObject* pServer = FindGameServer(nID);
	if (NULL == pServer) return MUID::ZERO;

	return pServer->GetUID();
}

int ZGameServerObjectManager::UIDToID( MUID nUID )
{
	ZGameServerObject* pServer = FindGameServer(nUID);
	if (NULL == pServer) return 0;

	return pServer->GetID();
}
