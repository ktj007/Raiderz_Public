#pragma once

#include "MCommandCommunicator.h"
#include "MTcpServer.h"
#include "MLink.h"

namespace minet {

class MCommand;
class MCmdHandler_NetServer;

class MLink;
class MCommandServerAcceptListener;
class MCommandServerDisconnectListner;
class MCommandServerSendListener;
class MCommandServerRecvListener;

class MNetServer : public minet::MCommandCommunicator
{
	friend class MCommandServerAcceptListener;
	friend class MCommandServerDisconnectListner;
	friend class MCommandServerSendListener;
	friend class MCommandServerRecvListener;
private:
	MTcpServer			m_TCPServer;
	MLinkMap			m_LinkMap;
	MCommandServerAcceptListener*		m_pAcceptListener;
	MCommandServerDisconnectListner*	m_pDisconnectListner;
	MCommandServerSendListener*			m_pSendListener;
	MCommandServerRecvListener*			m_pRecvListener;

	void SendCommandToLink(MUID uidReceiver, char* pData, int nDataSize);
protected:
	friend class minet::MCmdHandler_NetServer;

	MUID					m_uidGenerator;
	MCriticalSection		m_csNewUID;
	minet::MCmdHandler_NetServer*	m_pCmdHandler;

	virtual void SendCommand(minet::MCommand* pCommand);
public:
	MNetServer();
	virtual ~MNetServer();
	/// 초기화
	bool Create(int nPort, const bool bReuse = false );
	/// 해제
	void Destroy();

	virtual void Disconnect(MUID uidLink);
	virtual bool Post(minet::MCommand* pCommand);
	MUID NewUID();
};

} // namespace minet