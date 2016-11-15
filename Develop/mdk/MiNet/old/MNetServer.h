#ifndef _MINET_SERVER_H
#define _MINET_SERVER_H

#include "MCommandCommunicator.h"
#include "MIocpNet.h"

namespace minet {

class MCommand;
class MCommObject;
class MCommObjectManager;
class MCmdHandler_NetServer;

class MServerTrafficChecker
{
private:
	int m_nRecvTraffic;
	int m_nSendTraffic;
	int m_nRecvTrafficCnt;
	int m_nSendTrafficCnt;
	unsigned int m_nLastTrafficCheckTime;
	MCriticalSection		m_csLock;
public:
	MServerTrafficChecker() : m_nRecvTraffic(0), m_nSendTraffic(0), m_nRecvTrafficCnt(0), m_nSendTrafficCnt(0),
								m_nLastTrafficCheckTime(0) { }
	~MServerTrafficChecker() { }
	void CheckTraffic(DWORD* nRecvPacketLen, DWORD* nSendPacketLen)
	{
		m_csLock.Lock();

		if (nRecvPacketLen) m_nRecvTrafficCnt += (int)(*nRecvPacketLen);
		if (nSendPacketLen) m_nSendTrafficCnt += (int)(*nSendPacketLen);

		unsigned int nNowTime = timeGetTime();
		if (nNowTime - m_nLastTrafficCheckTime >= 1000)
		{
			m_nLastTrafficCheckTime = nNowTime;
			m_nRecvTraffic = m_nRecvTrafficCnt; m_nRecvTrafficCnt = 0;
			m_nSendTraffic = m_nSendTrafficCnt; m_nSendTrafficCnt = 0;
		}
		m_csLock.Unlock();
	}
	void GetTraffic(int* noutRecvTraffic, int* noutSendTraffic)
	{
		m_csLock.Lock();
			if (noutRecvTraffic) *noutRecvTraffic = m_nRecvTraffic;
			if (noutSendTraffic) *noutSendTraffic = m_nSendTraffic;
		m_csLock.Unlock();
	}
};

class MNetServer : public MCommandCommunicator
{
private:
	MServerTrafficChecker	m_Traffic;
	void SendCommandToCommObject(MUID uidReceiver, char* pData, int nDataSize);
protected:
	friend class MCmdHandler_NetServer;

	MIocpNet				m_IocpNet;
	MCommObjectManager*		m_pCommObjectManager;
	MUID					m_uidGenerator;
	MCriticalSection		m_csNewUID;
	MCmdHandler_NetServer*	m_pCmdHandler;

	////////// IOCP Worker Thread에서 넘어온 함수 /////////////////////
	static void _IocpCallback(void* pCallbackContext, MIOCP_OPERATION nIO, DWORD nKey, MPacketHeader* pPacket, DWORD dwPacketLen);
	void IocpCallback(MIOCP_OPERATION nIO, DWORD nKey, MPacketHeader* pPacket, DWORD dwPacketLen);	// Thread not safe

	void OnAccept(DWORD nClientKey);
	void OnRecv(DWORD nClientKey, MPacketHeader* pPacket, DWORD dwPacketLen);
	void OnSend(DWORD nClientKey, DWORD dwPacketLen);
	void OnDisconnect(DWORD nClientKey);
	////////// IOCP Worker Thread에서 넘어온 함수 /////////////////////

	virtual void SendCommand(MCommand* pCommand);
	
	
public:
	MNetServer();
	virtual ~MNetServer();
	/// 초기화
	bool Create(int nPort, const bool bReuse = false );
	/// 해제
	void Destroy();

	virtual void Disconnect(MUID uidComm);
	virtual bool Post(MCommand* pCommand);
	MUID NewUID();

	void GetTraffic(int* noutRecvTraffic, int* noutSendTraffic);
};



} // namespace minet

#endif