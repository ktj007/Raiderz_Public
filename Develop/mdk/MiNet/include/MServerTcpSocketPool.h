#pragma once

#include "MiNetPrerequisites.h"
#include "MThread2.h"

namespace minet {

class MAcceptor;
class MDisconnector;
class MSender;
class MReceiver;
class MServerTcpSocket;

class MINET_API MServerTcpSocketPool
{
private:
	MAcceptor*			m_pAcceptor;
	MDisconnector*		m_pDisconnector;
	MSender*			m_pSender;
	MReceiver*			m_pReceiver;
	list<MServerTcpSocket*>	m_RestorationTcpSockets;

	cml2::MCriticalSection		m_csLock;
	void AddToRestorationQueue(MServerTcpSocket* pTcpSocket);
	bool RestoreSocket(MServerTcpSocket* pTcpSocket);
public:
	MServerTcpSocketPool(MAcceptor* pAcceptor, MDisconnector* pDisconnector, MSender* pSender, MReceiver* pReceiver);
	~MServerTcpSocketPool()
	{

	}

	vector<MServerTcpSocket*>		m_vecAcceptSockets;



	void DeleteAllAcceptSockets();

	MServerTcpSocket* AddSocket(int addressFamily);

	bool Restore(MServerTcpSocket* pTcpSocket);
	void RestoreAll();

	void Destroy();
};

} // namespace minet