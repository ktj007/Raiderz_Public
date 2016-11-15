#include "StdAfx.h"
#include "ZAppServerObjectManager.h"
#include "ZAppServerObject.h"
#include "ZAppServerInfo.h"

ZAppServerObjectManager::ZAppServerObjectManager() : SCommObjectManager()
{

}

ZAppServerObjectManager::~ZAppServerObjectManager()
{

}

ZAppServerObject* ZAppServerObjectManager::NewAppServerObject( MUID uid, ZAppServerInfo* pAppServerInfo )
{
	ZAppServerObject* pNewPlayerObject = new ZAppServerObject(uid, pAppServerInfo);
	pNewPlayerObject->Create();
	return pNewPlayerObject;
}

SCommObject* ZAppServerObjectManager::NewObject( MUID& uid )
{
	_ASSERT(0);
	return NULL;
}

ZAppServerObject* ZAppServerObjectManager::FindAppServer(MUID uid)
{
	SCommObject* pCommObject = GetClient(uid);
	if (pCommObject)
	{
		return static_cast<ZAppServerObject*>(pCommObject);
	}
	return NULL;
}

ZAppServerObject* ZAppServerObjectManager::FindAppServerFromServerID(int nAppServerID)
{
	ZServerObjectIDMap::iterator itor = m_AppServerObjectIDMap.find(nAppServerID);
	if (itor != m_AppServerObjectIDMap.end())
	{
		return (*itor).second;
	}
	return NULL;
}

void ZAppServerObjectManager::AddObject( SCommObject* pClient )
{
	SCommObjectManager::AddObject(pClient);

	ZAppServerObject* pAppServerObject = static_cast<ZAppServerObject*>(pClient);
	m_AppServerObjectIDMap.insert(ZServerObjectIDMap::value_type(pAppServerObject->GetID(), pAppServerObject));
}

void ZAppServerObjectManager::DeleteObject( MUID uidClient )
{
	ZAppServerObject* pAppServerObject = FindAppServer(uidClient);
	if (pAppServerObject)
	{
		ZServerObjectIDMap::iterator itor = m_AppServerObjectIDMap.find(pAppServerObject->GetID());
		if (itor != m_AppServerObjectIDMap.end())
		{
			m_AppServerObjectIDMap.erase(itor);
		}
	}

	SCommObjectManager::DeleteObject(uidClient);
}

void ZAppServerObjectManager::DeleteAllObject()
{
	m_AppServerObjectIDMap.clear();

	SCommObjectManager::DeleteAllObject();
}

void ZAppServerObjectManager::Clear()
{
	m_AppServerObjectIDMap.clear();

	SCommObjectManager::Clear();
}

ZAppServerObjectManager::client_iterator ZAppServerObjectManager::GetClientBegin(void)
{
	SCommObjectMap& mapClient = GetObjects();
	return mapClient.begin();
}

ZAppServerObjectManager::client_iterator ZAppServerObjectManager::GetClientEnd(void)
{
	SCommObjectMap& mapClient = GetObjects();
	return mapClient.end();
}
