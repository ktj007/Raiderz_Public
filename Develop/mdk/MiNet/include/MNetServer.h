#ifndef _MINET_SERVER_H
#define _MINET_SERVER_H

#include "MiNetLib.h"
#include "MCommandCommunicator.h"
#include "MTcpServer.h"
#include "MLink.h"
#include "MLinkMap.h"
#include "MTrafficMonitor.h"
#include "MDelayTester.h"
#include "MPacketBuilder.h"

namespace minet {

class MCommand;
class MCmdHandler_NetServer;

class MLink;
class MCommandServerAcceptListener;
class MCommandServerDisconnectListner;
class MCommandServerSendListener;
class MCommandServerRecvListener;

struct MINET_API MNetServerDesc
{
	/// 일정 시간마다 클라이언트에 패킷을 보내 ping, 커넥션 체크를 할 지 여부
	bool	bHeartBeat;

	/// HeartBeat 주기 타임(msec)
	uint32	nHeartBeatTickTime;

	/// HeartBeat 타임 아웃(msec)
	uint32	nHeartBeatTimeoutTime;

	// 테스트 관련 -------

	/// 지연 테스트할 지 여부
	bool	bDelayTest;

	/// 지연 테스트시 딜레이값(msec)
	uint32	nTestDelayTime;

	MNetServerDesc() 
		: bHeartBeat(false), 
		nHeartBeatTickTime(60000), 
		nHeartBeatTimeoutTime(300000),
		bDelayTest(false),
		nTestDelayTime(0)
	{

	}
};

class MINET_API MNetServer : public MCommandCommunicator
{
	friend class MCommandServerAcceptListener;
	friend class MCommandServerDisconnectListner;
	friend class MCommandServerSendListener;
	friend class MCommandServerRecvListener;
protected:
	MNetServerDesc						m_Desc;
	MTcpServer*							m_pTCPServer;
	MLinkMap							m_LinkMap;
	MServerPacketBuilder				m_PacketBuilder;
	MTrafficMonitor						m_TrafficMonitor;
	MDelayTester						m_DelayTester;
	MCommandServerAcceptListener*		m_pAcceptListener;
	MCommandServerDisconnectListner*	m_pDisconnectListner;
	MCommandServerSendListener*			m_pSendListener;
	MCommandServerRecvListener*			m_pRecvListener;
	unsigned int						m_nHeartBeatLastTime;

	mutable long 						m_nDisconnCountByHeartBeat;
	mutable long 						m_nDisconnCountByWrongPacket;

	virtual bool SendCommandToLink(MUID uidReceiver, MCommand* pCommand, MCommandMsg* pMsg, int nPacketSize);
protected:
	friend class minet::MCmdHandler_NetServer;
	MCmdHandler_NetServer*	m_pCmdHandler;

	virtual void SendCommand(minet::MCommand* pCommand);
	virtual void OnPrepareRun(void);

	void UpdateKeepingAlive();
public:
	MNetServer(const MNetServerDesc& desc);
	virtual ~MNetServer();
	/// 초기화
	virtual bool Create(int nPort, const bool bReuse=false, MUID uidSeed=MUID::ZERO, int nSocketPoolSize=2000, int nSendPendingLimitCount = INT_MAX, const char* szMyNetworkCardIP="");
	/// 해제
	virtual void Destroy();

	virtual void Disconnect(MUID uidLink);
	void HardDisconnect(MUID uidLink);

	/// 커맨드 입력 - 쓰레드에 안전함.
	virtual bool Post(MCommand* pCommand);

	bool ReAllocLinkUID(MUID uidOrigin, MUID uidReAlloc);


	void GetTotalTraffic(int64& noutRecvSize, int64& noutSendSize);
	void GetSecondTraffic(uint32& noutRecvSize, uint32& noutSendSize);

	MLinkMap* GetLinks() { return &m_LinkMap; }
	int GetLinkCount();

	void IncreaseDisconnCountByHearBeat();
	void IncreaseDisconnCountByWrongPacket();
	void ResetDisconnCount();
	long GetDisconnCountByHearBeat()	{ return m_nDisconnCountByHeartBeat; }
	long GetDisconnCountByWrongPacket()	{ return m_nDisconnCountByWrongPacket; }
};


} // namespace minet

#endif