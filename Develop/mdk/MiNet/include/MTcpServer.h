#pragma once

#include "MiNetLib.h"
#include "MiNetPrerequisites.h"
#include "MTcpListenSocket.h"

namespace minet {

class MProactor;
class MAcceptor;
class MDisconnector;
class MReceiver;
class MSender;
class MServerTcpSocket;
class MAcceptListener;
class MConnectListener;
class MDisconnectListener;
class MSendListener;
class MRecvListener;
class MConnector;
class MServerTcpSocketPool;

class MINET_API MTcpServer
{
private:
	MProactor*			m_pProactor;
	MAcceptor*			m_pAcceptor;
	MConnector*			m_pConnector;
	MDisconnector*		m_pDisconnector;
	MReceiver*			m_pReceiver;
	MSender*			m_pSender;
	MTcpListenSocket	m_ListenSocket;
	MServerTcpSocketPool*	m_pAcceptSocketPool;
private:
	WORD					m_nListenPort;
	int						m_nThreadPoolSize;
	int						m_nSocketPoolSize;

public:
	MTcpServer();
	virtual ~MTcpServer();

	bool MakeWorkers(const int& threadCnt);
	bool Start(int addressFamily, int nListenPort, int nSocketPoolSize=2000, int nSendPendingLimitCount = INT_MAX, const char* szMyNetworkCardIP="");
	void Stop();
	MServerTcpSocket* const Connect(const char* const addr,const char* const port);

	void SetAcceptorListener(MAcceptListener* pListner);
	void SetConnectListener(MConnectListener* pListner);
	void SetDisconnectorListener(MDisconnectListener* pListener);
	void SetSenderListener(MSendListener* pListener);
	void SetReceiverListener(MRecvListener* pListener);
};







} // namespace minet {