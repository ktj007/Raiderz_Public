#ifndef MINET_CLIENT_H
#define MINET_CLIENT_H

#include "MiNetLib.h"
#include "MCommandCommunicator.h"
#include "MPacketCrypter.h"
#include "MTCPSocket.h"
#include "MiNetCommon.h"
#include "MTrafficMonitor.h"
#include "MPacketBuilder.h"

namespace minet {

class MCommand;
class MCmdHandler_NetClient;
class MCommandStream;

struct MINET_API MNetClientDesc
{
	bool	bKeepAlive;
	uint32	nKeepAliveTickTime;
	MNetClientDesc() 
		: bKeepAlive(false),
		  nKeepAliveTickTime(60000)
	{}
};

class MINET_API MClientConnectionKeeper
{
protected:
	uint32	m_nSendTime;
public:
	MClientConnectionKeeper() : m_nSendTime(0) {}
	void SetSendTime(uint32 nTime) { m_nSendTime = nTime; }
	bool CheckToSendCommand(uint32 nNowTime, uint32 nKeepAliveTickTime)
	{
		if (m_nSendTime == 0 || nNowTime - m_nSendTime < nKeepAliveTickTime)
		{
			return false;
		}
		m_nSendTime = nNowTime;
		return true;
	}
};

/// 클라이언트 커뮤니케이터
class MINET_API MNetClient : public MCommandCommunicator
{
private:
	friend class MCmdHandler_NetClient;

	struct _COMMAND_HANDLER
	{
		MCommandHandler*	m_pHandler;		///< 커맨드 핸들러
		MCommandHanderFunc* m_pFunc;		///< 커맨드 핸들러 함수 포인터
	};
	typedef std::map<int, _COMMAND_HANDLER> MCommandHandlerMap;
protected:
	MNetClientDesc			m_Desc;
	MUID					m_Server;				///< 연결된 커뮤니케이터
	MTCPSocket				m_TCPSocket;			///< 클라이언트 소켓용 클래스
	MCommandStream*			m_pCommandStream;
	MPacketCrypter			m_PacketCrypter;	///< MatchServer와의 암호화 클래스
	MCmdHandler_NetClient*	m_pCmdHandler;
	MPingData				m_PingData;
	MTrafficMonitor			m_TrafficMonitor;
	MClientPacketBuilder	m_PacketBuilder;
	MClientConnectionKeeper	m_ConnectionKeeper;
	MCommandHandlerMap		m_mapCommandHandler;
protected:
	virtual MCommandResult	OnCommand(MCommand* pCommand) override;
	virtual void			OnPrepareRun() override;


	virtual void			SendCommand(MCommand* pCommand);
	int						MakeCmdPacket(char* pOutPacket, int iMaxPacketSize, MCommand* pCommand);
	void					InitCrypt( MUID& uidHost, MUID& uidAlloc );
	
	// Socket Event - 왠만하면 상속받아서 처리하지 말고 대응하는 LOCAL 커맨드를 이용하도록 하자.
	virtual bool			OnSockConnect(SOCKET sock);
	virtual bool			OnSockDisconnect();
	virtual bool			OnSockRecv(SOCKET sock, void* pPacket, DWORD dwSize);
	virtual void			OnSockError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &nErrorCode);

	static bool				SocketRecvEvent(void* pCallbackContext, SOCKET sock, void* pPacket, DWORD dwSize);
	static bool				SocketConnectEvent(void* pCallbackContext, SOCKET sock);
	static bool				SocketDisconnectEvent(void* pCallbackContext);
	static void				SocketErrorEvent(void* pCallbackContext, SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode);

	virtual void			OnReplyConnect(MUID& uidHost, MUID& uidAlloc, unsigned int nTimeStamp);
	virtual void			OnReallocUID(MUID uidAlloc);

	void					CheckConnectionKeeper();
public:
							/// 생성자
							MNetClient(const MNetClientDesc& desc);
							/// 소멸자
	virtual					~MNetClient();
							/// 서버에 접속한다.
	virtual int				Connect(const char* szRemoteIP, int nPort, const char* szMyNetworkCardIP = "");
							/// 서버에서 접속을 끊는다.
	virtual void			Disconnect();
							/// 서버의 UID 반환
	MUID					GetServerUID(void)					{ return m_Server; }
							/// 소켓번호로 서버의 UID 반환
	MUID					GetSenderUIDBySocket(SOCKET socket);
							/// TCP 소켓 클래스 반환
	MTCPSocket*				GetTCPSocket()						{ return &m_TCPSocket; }
							/// 서버에 접속해있는지 여부를 반환
	virtual bool			IsConnected()						{ return m_TCPSocket.IsActive(); }
							/// 전송량 반환
	void					GetTraffic(int* nSendTraffic, int* nRecvTraffic);

							/// 서버로 보낼 커맨드 생성
	MCommand*				NewSendCommand( int nCmdID );

	void					Ping();
	uint32					GetPing() { return m_PingData.GetPing(); }
	MPingData&				GetPingData() { return m_PingData; }

	void					GetTotalTraffic(int64& noutRecvSize, int64& noutSendSize);
	void					GetSecondTraffic(uint32& noutRecvSize, uint32& noutSendSize);
	
	void					SetCommandHandler(int nCmdID, MCommandHandler* pHandler, MCommandHanderFunc* fnFunc) override;

};


} // namespace minet
#endif
