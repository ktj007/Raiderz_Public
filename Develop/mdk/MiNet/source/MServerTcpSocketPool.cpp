#include "stdafx.h"
#include "MServerTcpSocketPool.h"
#include "MServerTcpSocket.h"
#include "MAcceptor.h"

namespace minet {

MServerTcpSocketPool::MServerTcpSocketPool( MAcceptor* pAcceptor, MDisconnector* pDisconnector, MSender* pSender, MReceiver* pReceiver )
: m_pAcceptor(pAcceptor), m_pDisconnector(pDisconnector), m_pSender(pSender), m_pReceiver(pReceiver)
{

}

MServerTcpSocket* MServerTcpSocketPool::AddSocket(int addressFamily)
{
	MServerTcpSocket* pSocket = new MServerTcpSocket(m_pAcceptor, m_pDisconnector, m_pSender, m_pReceiver);
	assert(NULL!=pSocket);
	m_vecAcceptSockets.push_back(pSocket);

	pSocket->Init(addressFamily);

	return pSocket;
}

void MServerTcpSocketPool::Destroy()
{
	DeleteAllAcceptSockets();
}

void MServerTcpSocketPool::DeleteAllAcceptSockets()
{
	for (size_t i = 0; i < m_vecAcceptSockets.size(); i++)
	{
		m_vecAcceptSockets[i]->CloseSocket();
		SAFE_DELETE(m_vecAcceptSockets[i]);
	}
	m_vecAcceptSockets.clear();
}

void MServerTcpSocketPool::RestoreAll()
{
	cml2::MCriticalSection::Guard guard(m_csLock);

	if (m_RestorationTcpSockets.empty()) return;

	for (list<MServerTcpSocket*>::iterator itor = m_RestorationTcpSockets.begin(); itor != m_RestorationTcpSockets.end(); )
	{
		MServerTcpSocket* pTcpSocket = (*itor);

		if (RestoreSocket(pTcpSocket))
		{
			itor = m_RestorationTcpSockets.erase(itor);
		}
		else
		{
			++itor;
		}
	}
}

void MServerTcpSocketPool::AddToRestorationQueue( MServerTcpSocket* pTcpSocket )
{
	cml2::MCriticalSection::Guard guard(m_csLock);

	m_RestorationTcpSockets.push_back(pTcpSocket);
}

bool MServerTcpSocketPool::RestoreSocket( MServerTcpSocket* pTcpSocket )
{
	if (pTcpSocket->ReInit() == false)
	{
		return false;
	}

	// Accept
	if (m_pAcceptor->Register( pTcpSocket ) == false)
	{
		return false;
	}

	return true;
}

bool MServerTcpSocketPool::Restore( MServerTcpSocket* pTcpSocket )
{
	if (RestoreSocket(pTcpSocket) == false)
	{
		AddToRestorationQueue(pTcpSocket);

		RestoreAll();

		return false;
	}

	return true;
}

} // namespace minet