#include "stdafx.h"
#include "MAcceptor.h"
#include "MProactor.h"
#include "MTcpSocket.h"
#include "MTCPListenSocket.h"
#include "MLink.h"
#include "MServerTcpSocketPool.h"

namespace minet {

MAcceptor::MAcceptor() : m_pListener(NULL), m_pSocketPool(NULL), m_pTcpListenSocket(NULL)
{

}

void MAcceptor::ProcEvent( MAct* act, DWORD bytes_transferred )
{
	DelGabage(act);

	MTcpAct* pTcpAct = static_cast<MTcpAct*>(act);

	assert(pTcpAct->m_pTcpSocket);
	assert(m_pProactor);

	MServerTcpSocket& tcpsocket = *(pTcpAct->m_pTcpSocket);

	// Get Address First //
	int locallen, remotelen;
	sockaddr_in *plocal = 0, *premote = 0;
	sockaddr_in LocalAddr, RemoteAddr;

	GetAcceptExSockaddrs((LPVOID)(tcpsocket.m_AcceptBuffer),
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		(sockaddr **)&plocal,
		&locallen,
		(sockaddr **)&premote,
		&remotelen);

	memcpy(&LocalAddr, plocal, sizeof(sockaddr_in));
	memcpy(&RemoteAddr, premote, sizeof(sockaddr_in));
	// Get Address End //

	HANDLE hSocketHandle = reinterpret_cast<HANDLE>(tcpsocket.GetSocket());
	m_pProactor->RegisterToCompletionPort(hSocketHandle);

	OnAccept(tcpsocket);

	tcpsocket.Recv();
}

void MAcceptor::ProcError( MAct* act, DWORD bytes_transferred, DWORD error )
{
	MTcpAct* pTcpAct = static_cast<MTcpAct*>(act);

	assert(pTcpAct);
	assert(pTcpAct->m_pTcpSocket);

	DelGabage(act);

	MServerTcpSocket* pTcpSocket = pTcpAct->m_pTcpSocket;

	//minet_logd("Acceptor ProcError s(%d) err(%d)\n", pTcpSocket->GetSocket(), error );

	pTcpSocket->Fini();
	m_pSocketPool->Restore(pTcpSocket);
}

void MAcceptor::Init( MTcpListenSocket* pTcpListenSocket, MProactor* pProactor, MServerTcpSocketPool* pTcpSocketPool )
{
	m_pTcpListenSocket = pTcpListenSocket;
	m_pProactor = pProactor;
	m_pSocketPool = pTcpSocketPool;
}

bool MAcceptor::Register( MServerTcpSocket* pAcceptSocket )
{
	DWORD dwRecvNumBytes;

	if (NULL == m_pTcpListenSocket)
	{
		return false;
	}

	SOCKET sdListenSocket = m_pTcpListenSocket->GetSocket();
	SOCKET sdAcceptSocket = pAcceptSocket->GetSocket();

	MTcpActAccept* pTcpActAccept = new MTcpActAccept();
	pTcpActAccept->Init(this, pAcceptSocket, pAcceptSocket->m_AcceptBuffer, SOCK_BUFSIZE, sdAcceptSocket);

	AddGabage(pTcpActAccept);

	BOOL ret =	AcceptEx(
		sdListenSocket, 
		sdAcceptSocket, 
		pAcceptSocket->m_AcceptBuffer, 
		0, 
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		&dwRecvNumBytes,
		pTcpActAccept 
		);

	if (ret == FALSE)
	{
		int error = WSAGetLastError();

		if( error != ERROR_IO_PENDING ) 
		{
			DelGabage(pTcpActAccept);

			SAFE_DELETE(pTcpActAccept);

			minet_logd( "AcceptEx Error!!! sock(%d), err(%d)\n", pAcceptSocket->GetSocket(), error );	

			return false;
		}
	}
	else
	{
		minet_log( "AcceptEx Return True sock(%d)\n", pAcceptSocket->GetSocket());	
	}

	return true;
}

void MAcceptor::OnAccept( MServerTcpSocket& tcp_socket )
{
	if (m_pListener) m_pListener->OnAccept(tcp_socket);
}

void MAcceptor::AddGabage( MAct* pAct )
{
	cml2::MCriticalSection::Guard guard(m_csGabageLock);
	int n = (int)pAct;

	m_Gabages.insert(map<int, MAct*>::value_type(n, pAct));
}

void MAcceptor::DelGabage( MAct* pAct )
{
	cml2::MCriticalSection::Guard guard(m_csGabageLock);

	int n = (int)pAct;

	map<int, MAct*>::iterator itor = m_Gabages.find(n);
	if (itor != m_Gabages.end())
	{
		m_Gabages.erase(itor);
	}
}

void MAcceptor::ClearGabages()
{
	cml2::MCriticalSection::Guard guard(m_csGabageLock);

	for (map<int, MAct*>::iterator itor = m_Gabages.begin(); itor != m_Gabages.end(); ++itor)
	{
		MAct* pAct = (*itor).second;
		SAFE_DELETE(pAct);
	}
	m_Gabages.clear();
}

MAcceptor::~MAcceptor()
{
	ClearGabages();
}

} // namespace minet {