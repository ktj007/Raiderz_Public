#include "stdafx.h"
#include "GGameServerMgr.h"
#include "GGameServer.h"

GGameServerMgr::GGameServerMgr()
{

}

GGameServerMgr::~GGameServerMgr()
{
	Clear();
}

GGameServer* GGameServerMgr::Get(const MUID& uidClient)
{
	GGameServer* pGameServer = NULL;
	
	Lock();
	MAP_GAMESERVER::iterator itor = m_mapGameServer.find(uidClient);
	if (itor != m_mapGameServer.end())
	{
		pGameServer = (*itor).second;
	}
	Unlock();

	return pGameServer;
}

GGameServer* GGameServerMgr::New(MUID& uid)
{
	GGameServer* pNewGameServer = new GGameServer(uid);	
	return pNewGameServer;
}

void GGameServerMgr::Insert(GGameServer* pGameServer)
{
	Lock();
	m_mapGameServer.insert(MAP_GAMESERVER::value_type(pGameServer->GetUID(), pGameServer));
	Unlock();
}

void GGameServerMgr::Delete(MUID uidClient)
{
	Lock();
	MAP_GAMESERVER::iterator itor = m_mapGameServer.find(uidClient);
	if (itor != m_mapGameServer.end())
	{
		GGameServer* pGameServer = (*itor).second;
		m_mapGameServer.erase(itor);
		delete pGameServer; 
	}
	Unlock();
}

void GGameServerMgr::Clear()
{
	Lock();
	for (MAP_GAMESERVER::iterator itor = m_mapGameServer.begin(); m_mapGameServer.end() != itor; )
	{
		GGameServer* pGameServer = (*itor).second;
		itor = m_mapGameServer.erase(itor);
		delete pGameServer; 
	}
	m_mapGameServer.clear();
	Unlock();
}

void GGameServerMgr::Lock()
{
	m_Lock.Lock();
}

void GGameServerMgr::Unlock()
{
	m_Lock.Unlock();
}

void GGameServerMgr::PickUpLog(int nQuantity, vector<GLog*>& outvecLog)
{
	GGameServer* pGameServer = PickUpServer();
	if (NULL == pGameServer) return;

	pGameServer->PickUpLog(nQuantity, outvecLog);
}

GGameServer* GGameServerMgr::PickUpServer()
{
	int nTopLogPoint = 0;
	GGameServer* pTopLogPointGameServer = NULL;

	for each (MAP_GAMESERVER::value_type each in m_mapGameServer)
	{
		GGameServer* pGameServer = each.second;
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