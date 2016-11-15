#include "stdafx.h"
#include "MCommObject.h"
#include "MCommandCommunicator.h"
#include "MCommandStream.h"

namespace minet {

MCommObject::MCommObject(MCommandCommunicator* pCommunicator)
{
	m_uid				= 0;
	m_dwUserContext		= 0;
	m_szIP[0]			= NULL;
	m_nPort				= 0;
	m_bAllowed			= true;
	m_pCommandStream	= new MCommandStream(0, pCommunicator->GetUID(), pCommunicator->GetCommandManager());
}

MCommObject::~MCommObject()
{
	if (m_pCommandStream) 
	{
		delete m_pCommandStream;
		m_pCommandStream = NULL;
	}
}

void MCommObject::Init(MUID uidServer, MUID uidComm, DWORD dwUserContext, const char* pszIP, int nPort)
{
	m_uid			= uidComm;
	m_dwUserContext = dwUserContext;

	m_pCommandStream->SetUID(uidServer, uidComm);

	strcpy_s(m_szIP, pszIP);
	m_dwIP = inet_addr(m_szIP);
	m_nPort = nPort;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// MCommObjectManager /////////////////////////////////////////////////////////////////////////

MCommObjectManager::MCommObjectManager()
{

}

MCommObjectManager::~MCommObjectManager()
{

}

void MCommObjectManager::Create()
{

}

void MCommObjectManager::Destroy()
{
	DeleteAll();
}

void MCommObjectManager::Add(MCommObject* pCommObj)
{
	cml2::MCriticalSection::Guard guard(m_csLock);
	
	m_CommObjectMap.insert(MCommObjectMap::value_type(pCommObj->GetUID(), pCommObj));
	m_CommObjectKeyMap.insert(MCommObjectKeyMap::value_type(pCommObj->GetUserContext(), pCommObj));
}

void MCommObjectManager::Delete(const MUID& uid)
{
	cml2::MCriticalSection::Guard guard(m_csLock);

	MCommObjectMap::iterator itor = m_CommObjectMap.find(uid);
	if (itor != m_CommObjectMap.end())
	{
		MCommObject* pCommObj = (*itor).second;


		MCommObjectKeyMap::iterator itor2 = m_CommObjectKeyMap.find(pCommObj->GetUserContext());
		if (itor2 != m_CommObjectKeyMap.end())
		{
			m_CommObjectKeyMap.erase(itor2);
		}

		delete pCommObj;
		m_CommObjectMap.erase(itor);
	}
}

void MCommObjectManager::Delete(DWORD nKey)
{
	cml2::MCriticalSection::Guard guard(m_csLock);

	MCommObjectKeyMap::iterator itor = m_CommObjectKeyMap.find(nKey);
	if (itor != m_CommObjectKeyMap.end())
	{
		MCommObject* pCommObj = (*itor).second;


		MCommObjectMap::iterator itor2 = m_CommObjectMap.find(pCommObj->GetUID());
		if (itor2 != m_CommObjectMap.end())
		{
			m_CommObjectMap.erase(itor2);
		}

		delete pCommObj;
		m_CommObjectKeyMap.erase(itor);
	}
}

void MCommObjectManager::DeleteAll()
{
	cml2::MCriticalSection::Guard guard(m_csLock);

	for(MCommObjectMap::iterator itor = m_CommObjectMap.begin(); itor != m_CommObjectMap.end(); itor++)
	{
		delete (*itor).second;
	}

	m_CommObjectMap.clear();
	m_CommObjectKeyMap.clear();
}

int MCommObjectManager::GetCount()
{
	cml2::MCriticalSection::Guard guard(m_csLock);
	int nCount = (int)m_CommObjectMap.size();
	return nCount;
}

DWORD MCommObjectManager::GetCommObjectKey(const MUID& uid)
{
	cml2::MCriticalSection::Guard guard(m_csLock);

	DWORD nKey = 0;

	MCommObjectMap::iterator itor = m_CommObjectMap.find(uid);
	if (itor != m_CommObjectMap.end())
	{
		MCommObject* pCommObj = (*itor).second;
		nKey = pCommObj->GetUserContext();
	}
	
	return nKey;
}

MCommObject* MCommObjectManager::GetCommObjectByKey(DWORD nClientKey)
{
	MCommObjectKeyMap::iterator itor = m_CommObjectKeyMap.find(nClientKey);
	if (itor != m_CommObjectKeyMap.end())
	{
		MCommObject* pCommObj = (*itor).second;
		return pCommObj;
	}
	
	return NULL;
}

MUID MCommObjectManager::GetCommObjectUID(const DWORD nClientKey)
{
	cml2::MCriticalSection::Guard guard(m_csLock);

	MUID uid = 0;

	MCommObjectKeyMap::iterator itor = m_CommObjectKeyMap.find(nClientKey);
	if (itor != m_CommObjectKeyMap.end())
	{
		MCommObject* pCommObj = (*itor).second;
		uid = pCommObj->GetUID();
	}
	
	return uid;
}


MCommObject* MCommObjectManager::GetCommObjectByUID(const MUID& uidComm)
{
	cml2::MCriticalSection::Guard guard(m_csLock);

	DWORD nKey = 0;

	MCommObjectMap::iterator itor = m_CommObjectMap.find(uidComm);
	if (itor != m_CommObjectMap.end())
	{
		MCommObject* pCommObj = (*itor).second;
		nKey = pCommObj->GetUserContext();
	}

	return GetCommObjectByKey( nKey );
}


void MCommObjectManager::Lock()
{
	m_csLock.Lock();
}


void MCommObjectManager::Unlock()
{
	m_csLock.Unlock();
}

} // namespace minet