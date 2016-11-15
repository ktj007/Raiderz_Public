#include "stdafx.h"
#include "SCommObjectManager.h"
#include "SCommObject.h"

SCommObjectManager::SCommObjectManager()
{

}

SCommObjectManager::~SCommObjectManager()
{
	DeleteAllObject();
}

SCommObject* SCommObjectManager::GetClient(const MUID& uidClient)
{
	SCommObject* pRetClient = NULL;
	
	Lock();
	SCommObjectMap::iterator itor = m_ClientObjectMap.find(uidClient);
	if (itor != m_ClientObjectMap.end())
	{
		pRetClient = (*itor).second;
	}
	Unlock();

	return pRetClient;
}

SCommObject* SCommObjectManager::NewObject(MUID& uid)
{
	SCommObject* pNewPlayerObject = new SCommObject(uid);	
	pNewPlayerObject->Create();
	return pNewPlayerObject;
}

void SCommObjectManager::AddObject(SCommObject* pPlayer)
{
	Lock();
	m_ClientObjectMap.insert(SCommObjectMap::value_type(pPlayer->GetUID(), pPlayer));
	Unlock();
}

void SCommObjectManager::DeleteObject(MUID uidClient)
{
	Lock();
	SCommObjectMap::iterator itor = m_ClientObjectMap.find(uidClient);
	if (itor != m_ClientObjectMap.end())
	{
		SCommObject* pPlayerObject = (*itor).second;
		m_ClientObjectMap.erase(itor);
		pPlayerObject->Destroy();
		delete pPlayerObject; 
	}
	Unlock();
}

void SCommObjectManager::DeleteAllObject()
{
	Lock();
	for (SCommObjectMap::iterator itor = m_ClientObjectMap.begin(); m_ClientObjectMap.end() != itor; )
	{
		SCommObject* pPlayerObject = (*itor).second;
		itor = m_ClientObjectMap.erase(itor);
		pPlayerObject->Destroy();
		delete pPlayerObject; 
	}
	Unlock();
}

void SCommObjectManager::Clear()
{
	Lock();
	m_ClientObjectMap.clear();
	Unlock();
}

void SCommObjectManager::Lock()
{
	m_Lock.Lock();
}

void SCommObjectManager::Unlock()
{
	m_Lock.Unlock();
}

