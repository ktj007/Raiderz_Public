#pragma once

#include "MCommandCommunicator.h"
#include "MCommand.h"
#include "XNetClient.h"
#include "MPacketBuilder.h"

#define DUMMYCLIENT_COMMAND_CROWDED_QUEUE_SIZE				(2000)

class XBirdDummyAgent;

namespace minet
{
	class MServerTcpSocket;
}

class XBirdClient : public MCommandCommunicator
{
private:
	friend class MCmdHandler_NetClient;
protected:
	MUID					m_Server;				///< 연결된 커뮤니케이터
	minet::MServerTcpSocket*m_socket;
	MSendRecvTrafficLog		m_TrafficLog;
	uint32					m_totalSendBytes;
	uint32					m_totalRecvBytes;
	MCriticalSection		m_csRecvLock;			///< CommandQueue critical section
	MCommandStream*			m_pCommandStream;
	MPacketCrypter			m_PacketCrypter;	///< MatchServer와의 암호화 클래스
	XBirdDummyAgent*		m_pAgent;
	MPingData				m_PingData;

	MClientPacketBuilder	m_PacketBuilder;

	void					OnCheckCrowded();

protected:
	virtual	void			OnRegisterCommand(MCommandManager* pCommandManager);
	virtual void			SendCommand(MCommand* pCommand);
	virtual MCommand*		GetCommand();
	virtual MCommandResult	OnCommand(MCommand* pCommand);
	virtual void			OnPrepareCommand(MCommand* pCommand);
	void					LogCommand( MCommand* pCommand );

	int						MakeCmdPacket(char* pOutPacket, int iMaxPacketSize, MCommand* pCommand);
	void					OnReplyConnect(MUID& uidHost, MUID& uidAlloc, unsigned int nTimeStamp);
	void					OnReallocUID( MUID uidAlloc );

private:
	friend class XBirdDummyAgent;
	void					OnConnect(minet::MServerTcpSocket& socket);
	void					OnDisconnect();
	void					OnRecv(const BYTE* const data,const DWORD& dataLen);
	void					OnSend(DWORD bytes_transferred);

public:
	static bool				m_bPrintCommandLog;
	XBirdClient(XBirdDummyAgent* pAgent=NULL);
	virtual					~XBirdClient();
	virtual bool			Post(MCommand* pCommand);
	void					Disconnect();
	MUID					GetServerUID(void)					{ return m_Server; }
//	MUID					GetSenderUIDBySocket(SOCKET socket);
	void					GetTraffic(int& nSendTraffic, int& nRecvTraffic);
	uint32					GetTotalSendBytes();
	uint32					GetTotalRecvBytes();
	void					SetSocket(minet::MServerTcpSocket* const socket) { assert(NULL!=socket); m_socket = socket; }
	minet::MServerTcpSocket* const Socket() { return m_socket; }	//test

	//void SetActiveRecv(bool b)
	//{
	//	m_pTCPSocket->SetActiveRecv(b);
	//}
};

inline bool XBirdPostCommand(XBirdClient* pDummyClient, MCommand* pCmd)
{
	return pDummyClient->Post(pCmd); 
}

inline MCommand* XBirdNewCmd(int nCmdID, XBirdClient* pDummyClient)
{
	MUID uidTarget= pDummyClient->GetServerUID();
	return pDummyClient->NewCommand(nCmdID, uidTarget);
}

#define XBIRDPOSTCMD0(_CLIENT, _ID)										{ MCommand* pC=XBirdNewCmd(_ID,_CLIENT); XBirdPostCommand(_CLIENT, pC); }
#define XBIRDPOSTCMD1(_CLIENT, _ID, _P0)								{ MCommand* pC=XBirdNewCmd(_ID,_CLIENT); pC->AP(_P0); XBirdPostCommand(_CLIENT, pC); }
#define XBIRDPOSTCMD2(_CLIENT, _ID, _P0, _P1)							{ MCommand* pC=XBirdNewCmd(_ID,_CLIENT); pC->AP(_P0); pC->AP(_P1); XBirdPostCommand(_CLIENT, pC); }
#define XBIRDPOSTCMD3(_CLIENT, _ID, _P0, _P1, _P2)						{ MCommand* pC=XBirdNewCmd(_ID,_CLIENT); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); XBirdPostCommand(_CLIENT, pC); }
#define XBIRDPOSTCMD4(_CLIENT, _ID, _P0, _P1, _P2, _P3)					{ MCommand* pC=XBirdNewCmd(_ID,_CLIENT); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); XBirdPostCommand(_CLIENT, pC); }
#define XBIRDPOSTCMD5(_CLIENT, _ID, _P0, _P1, _P2, _P3, _P4)			{ MCommand* pC=XBirdNewCmd(_ID,_CLIENT); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); XBirdPostCommand(_CLIENT, pC); }
#define XBIRDPOSTCMD6(_CLIENT, _ID, _P0, _P1, _P2, _P3, _P4, _P5)		{ MCommand* pC=XBirdNewCmd(_ID,_CLIENT); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); pC->AP(_P5); XBirdPostCommand(_CLIENT, pC); }
#define XBIRDPOSTCMD7(_CLIENT, _ID, _P0, _P1, _P2, _P3, _P4, _P5, _P6)	{ MCommand* pC=XBirdNewCmd(_ID,_CLIENT); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); pC->AP(_P5); pC->AP(_P6); XBirdPostCommand(_CLIENT, pC); }

#define XBIRDNEWCMD0(_C, _CLIENT, _ID)										{ _C=XBirdNewCmd(_ID,_CLIENT);}
#define XBIRDNEWCMD1(_C, _CLIENT, _ID, _P0)									{ _C=XBirdNewCmd(_ID,_CLIENT); _C->AP(_P0); }
#define XBIRDNEWCMD2(_C, _CLIENT, _ID, _P0, _P1)							{ _C=XBirdNewCmd(_ID,_CLIENT); _C->AP(_P0); _C->AP(_P1); }
#define XBIRDNEWCMD3(_C, _CLIENT, _ID, _P0, _P1, _P2)						{ _C=XBirdNewCmd(_ID,_CLIENT); _C->AP(_P0); _C->AP(_P1); _C->AP(_P2); }
#define XBIRDNEWCMD4(_C, _CLIENT, _ID, _P0, _P1, _P2, _P3)					{ _C=XBirdNewCmd(_ID,_CLIENT); _C->AP(_P0); _C->AP(_P1); _C->AP(_P2); _C->AP(_P3); }
#define XBIRDNEWCMD5(_C, _CLIENT, _ID, _P0, _P1, _P2, _P3, _P4)				{ _C=XBirdNewCmd(_ID,_CLIENT); _C->AP(_P0); _C->AP(_P1); _C->AP(_P2); _C->AP(_P3); _C->AP(_P4); }
#define XBIRDNEWCMD6(_C, _CLIENT, _ID, _P0, _P1, _P2, _P3, _P4, _P5)		{ _C=XBirdNewCmd(_ID,_CLIENT); _C->AP(_P0); _C->AP(_P1); _C->AP(_P2); _C->AP(_P3); _C->AP(_P4); _C->AP(_P5); }
#define XBIRDNEWCMD7(_C, _CLIENT, _ID, _P0, _P1, _P2, _P3, _P4, _P5, _P6)	{ _C=XBirdNewCmd(_ID,_CLIENT); _C->AP(_P0); _C->AP(_P1); _C->AP(_P2); _C->AP(_P3); _C->AP(_P4); _C->AP(_P5); _C->AP(_P6); }

