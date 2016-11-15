#pragma once

#include "MiNetLib.h"
#include "MSocket.h"
#include "MActor.h"

namespace minet {

class MAcceptor;
class MDisconnector;
class MSender;
class MReceiver;

class MINET_API MServerTcpSocket : public MSocket
{
private:
	void	InitBuf();
	MUID	m_uidLink;
	int		m_nSendPendingCounter;
	int		m_nSendPendingLimitCount;
	int		m_addressFamily;

public:
	char			m_AcceptBuffer[SOCK_BUFSIZE];
	char			m_RecvBuffer[SOCK_BUFSIZE];
public:
	MAcceptor*		m_pAcceptor;
	MDisconnector*	m_pDisconnector;
	MSender*		m_pSender;
	MReceiver*		m_pReceiver;
public:
	MServerTcpSocket(MAcceptor* pAcceptor, MDisconnector* pDisconnector, MSender* pSender, MReceiver* pReceiver);
	virtual ~MServerTcpSocket();

	bool Init(int addressFamily);
	bool ReInit();
	void Fini();

	void Recv();
	void Send(BYTE* buf, int buflen);
	bool Reuse();
	bool Disconnect();
	void HardDisconnect(MUID uidLink);

	void Shutdown(int how = SD_BOTH);

	SOCKET GetSocket() const { return m_hSocket; }
	MUID GetLink() const { return m_uidLink; }
	void SetLink(const MUID uidLink) { m_uidLink = uidLink; }
	void ReleaseLink();

	// 보내는 패킷을 일정 횟수 수신하지 않으면, 수신측에 문제가 생겼다고 판단한다.
	void ResetSendPendingCounter();
	bool IncreaseSendPendingCounter();
	void DecreaseSendPendingCounter();
	void SetSendPendingLimitCount(int nLimitCount);
};

}