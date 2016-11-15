#include "stdafx.h"
#include "MRealSession.h"


//MRealSession::MRealSession() 
//{ 
//	m_sdSocket = INVALID_SOCKET;
//	ZeroMemory(&m_SockAddr, sizeof(SOCKADDR_IN));
//	SetSessionState(SESSIONSTATE_IDLE); 
//	SetUserContext(NULL);
//
//	ZeroMemory(m_RecvBuffer, sizeof(CHAR)*MAX_BUFF_SIZE);
//}
//
//void MRealSession::SetSockAddr(SOCKADDR_IN* pAddr, int nAddrLen) 
//{ 
//	CopyMemory(&m_SockAddr, pAddr, min(sizeof(SOCKADDR_IN), nAddrLen)); 
//}
//
//////////////////////////////////////////////////////////////////////////////////
//void MSessionMap::Add(MRealSession* pSession) 
//{
//	Lock();
//		_ASSERT(pSession->GetSocket() != INVALID_SOCKET);
//		insert(MSessionMap::value_type(pSession->GetSocket(), pSession));
//	Unlock();
//}
//
//bool MSessionMap::Remove(SOCKET sd, MSessionMap::iterator* pNextItor) 
//{
//	bool bResult = false;
//	Lock();
//		MSessionMap::iterator i = find(sd);
//		if (i!=end()) {
//			MRealSession* pSession = (*i).second;
//			delete pSession;
//			MSessionMap::iterator itorTmp = erase(i);
//			if (pNextItor)
//				*pNextItor = itorTmp;
//			bResult = true;
//		}
//	Unlock();
//	return bResult;
//}
//
//void MSessionMap::RemoveAll() 
//{
//	OutputDebugString("MSessionMap::RemoveAll() Proceeding \n");
//	Lock();
//		MSessionMap::iterator itor = begin();
//		while( itor != end()) {
//			MRealSession* pSession = (*itor).second;
//			delete pSession;
//			itor = erase(itor);
//		}
//	Unlock();
//	OutputDebugString("MSessionMap::RemoveAll() Finished \n");
//}
//
//bool MSessionMap::IsExist(SOCKET sd) 
//{
//	bool bResult = false;
//	Lock();
//		MSessionMap::iterator i = find(sd);
//		if(i!=end()) bResult = true;				
//	Unlock();
//	return bResult;
//}
//
//MRealSession* MSessionMap::GetSession(SOCKET sd) 
//{
//	MRealSession* pSession = NULL;
//	Lock();
//		MSessionMap::iterator i = find(sd);
//		if(i!=end()) 
//			pSession = (*i).second;
//	Unlock();
//	return pSession;
//}
//
//
//MRealSession* MSessionMap::GetSessionUnsafe(SOCKET sd)
//{
//	MSessionMap::iterator i = find(sd);
//	if(i==end()) 
//		return NULL;
//	else
//		return (*i).second;
//}
//
//bool MSessionMap::IsExistUnsafe(MRealSession* pSession) 
//{
//	for (MSessionMap::iterator i=begin(); i!=end(); i++) {
//		MRealSession* pItorSession = (*i).second;
//		if (pItorSession == pSession)
//			return true;
//	}
//	return false;
//}
//bool MSessionMap::RemoveUnsafe(SOCKET sd, MSessionMap::iterator* pNextItor) 
//{
//	bool bResult = false;
//	MSessionMap::iterator i = find(sd);
//	if (i!=end()) {
//		MRealSession* pSession = (*i).second;
//		delete pSession;
//		MSessionMap::iterator itorTmp = erase(i);
//		if (pNextItor)
//			*pNextItor = itorTmp;
//		bResult = true;
//	}
//	return bResult;
//}
