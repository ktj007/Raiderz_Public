#include "stdafx.h"
#include "PGameServerObjectManager.h"
#include "PGameServerObject.h"
#include "PDef.h"


PGameServerObjectManager::PGameServerObjectManager() : SCommObjectManager()
{

}

PGameServerObjectManager::~PGameServerObjectManager()
{

}

SCommObject* PGameServerObjectManager::NewObject(MUID& uid, int nServerID)
{
	SCommObject* pNewPlayerObject = new PGameServerObject(uid, nServerID);
	pNewPlayerObject->Create();
	return pNewPlayerObject;
}


PGameServerObject* PGameServerObjectManager::PickUpServer()
{
	int nTopLogPoint = 0;
	PGameServerObject* pTopLogPointGameServer = NULL;

	for each (SCommObjectMap::value_type each in m_ClientObjectMap)
	{
		PGameServerObject* pGameServer = static_cast<PGameServerObject*>(each.second);
		if (NULL == pGameServer) continue;

		int nLogPoint = pGameServer->CalcLogPoint();
		if (nTopLogPoint < nLogPoint)
		{			
			pTopLogPointGameServer = pGameServer;
			nTopLogPoint = nLogPoint;
		}
	}

	return pTopLogPointGameServer;
}

void PGameServerObjectManager::PickUpLog(int nQuantity, vector<PLog*>& outvecLog)
{
	PGameServerObject* pGameServer = PickUpServer();
	if (NULL == pGameServer) return;

	pGameServer->PickUpLog(nQuantity, outvecLog);
}

PGameServerObject* PGameServerObjectManager::Find( MUID& uid )
{
	return static_cast<PGameServerObject*>(GetClient(uid));
}
PGameServerObject* PGameServerObjectManager::Find(int nGameServerID)
{
	PGameServerIDMap::iterator iter = m_mapGameServerByID.find(nGameServerID);
	if (iter == m_mapGameServerByID.end()) return NULL;
	return iter->second;
}

void PGameServerObjectManager::AddObject(SCommObject* pClient)
{
	if (NULL == pClient) return;

	SCommObjectManager::AddObject(pClient);

	PGameServerObject* pGameServerObject = static_cast<PGameServerObject*>(pClient);
	m_mapGameServerByID.insert(PGameServerIDMap::value_type(pGameServerObject->GetServerID(), pGameServerObject));
}
void PGameServerObjectManager::DeleteObject(MUID uidClient)
{
	PGameServerObject* pGameServerObject = Find(uidClient);
	if (NULL == pGameServerObject) return;

	m_mapGameServerByID.erase(pGameServerObject->GetServerID());

	SCommObjectManager::DeleteObject(uidClient);
}
void PGameServerObjectManager::DeleteAllObject()
{
	m_mapGameServerByID.clear();
	SCommObjectManager::DeleteAllObject();
}
void PGameServerObjectManager::Clear()
{
	m_mapGameServerByID.clear();
	SCommObjectManager::Clear();
}

MUID PGameServerObjectManager::GetGameServerUID(int nServerID)
{
	PGameServerObject* pGameServer = Find(nServerID);
	if (pGameServer == NULL)	return MUID::Invalid();

	return pGameServer->GetUID();
}

int PGameServerObjectManager::GetGameServerID(MUID uidServer)
{
	PGameServerObject* pGameServer = Find(uidServer);
	if (pGameServer == NULL)	return 0;

	return pGameServer->GetServerID();
}