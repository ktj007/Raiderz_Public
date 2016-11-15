#include "stdafx.h"
#include "MSession.h"

namespace minet {

MSession::MSession()
{
	m_sdSocket = INVALID_SOCKET;
	ZeroMemory(&m_SockAddr, sizeof(SOCKADDR_IN));
	ZeroMemory(m_RecvBuffer, sizeof(CHAR)*MAX_BUFF_SIZE);
}

void MSession::SetSockAddr(SOCKADDR_IN* pAddr, int nAddrLen)
{
	CopyMemory(&m_SockAddr, pAddr, min(sizeof(SOCKADDR_IN), nAddrLen)); 

}

void MSession::Initialize(SOCKET sdSocket)
{
	m_sdSocket = sdSocket;
}

void MSession::Finalize()
{
	m_sdSocket = INVALID_SOCKET;
}

//////////////////////////////////////////////////////////////////////////////////////
MSessionMap::MSessionMap()
{

}

MSessionMap::~MSessionMap()
{

}

void MSessionMap::Add(MSession* pSession)
{
	_ASSERT(pSession->GetSocket() != INVALID_SOCKET);
	if (pSession->GetSocket() == INVALID_SOCKET) return;

	Lock();
		m_SessionMap.insert(map<SOCKET, MSession*>::value_type(pSession->GetSocket(), pSession));
	Unlock();
}


void MSessionMap::RemoveAll()
{
	OutputDebugString("MSessionMap::RemoveAll() Proceeding \n");

	Lock();
		map<SOCKET, MSession*>::iterator itor = m_SessionMap.begin();
		while( itor != m_SessionMap.end()) 
		{
			MSession* pSession = (*itor).second;
			delete pSession;
			itor = m_SessionMap.erase(itor);
		}
	Unlock();

	OutputDebugString("MSessionMap::RemoveAll() Finished \n");
}


MSession* MSessionMap::GetSession(SOCKET sd)
{
	MSession* pSession = NULL;
	Lock();
		map<SOCKET, MSession*>::iterator itor = m_SessionMap.find(sd);
		if(itor != m_SessionMap.end()) 
			pSession = (*itor).second;
	Unlock();
	return pSession;
}

MSession* MSessionMap::GetSessionUnsafe(SOCKET sd)
{
	map<SOCKET, MSession*>::iterator itor = m_SessionMap.find(sd);
	if(itor == m_SessionMap.end()) 
		return NULL;
	else
		return (*itor).second;
}

bool MSessionMap::IsExistUnsafe(MSession* pSession)
{
	for (map<SOCKET, MSession*>::iterator itor = m_SessionMap.begin(); itor != m_SessionMap.end(); itor++) 
	{
		MSession* pItorSession = (*itor).second;
		if (pItorSession == pSession) return true;
	}
	return false;
}

bool MSessionMap::RemoveUnsafe(SOCKET sd)
{
	bool bResult = false;
	map<SOCKET, MSession*>::iterator itor = m_SessionMap.find(sd);
	if (itor != m_SessionMap.end()) 
	{
		MSession* pSession = (*itor).second;
		delete pSession;

		map<SOCKET, MSession*>::iterator itorTmp = m_SessionMap.erase(itor);
		bResult = true;
	}
	return bResult;
}

} // namespace minet


