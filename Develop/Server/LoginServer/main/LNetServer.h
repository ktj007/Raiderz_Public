#pragma once

#include "MNetServer.h"
#include "MCommandHandler.h"

using namespace minet;

class LCmdHandlerGroup;


/// 네트웍 관리자. 주로 커맨드를 처리한다.
class LNetServer : public MNetServer
{
private:
	int						m_nRecvTotalCmdCount, m_nSendTotalCmdCount, m_nLocalTotalCmdCount;
	int						m_nRecvCPS, m_nSendCPS, m_nLocalCPS;
	int						m_nRecvCmdCnt, m_nSendCmdCnt, m_nLocalCmdCnt;
	unsigned int			m_nLastCheckTime;

	virtual void OnPrepareCommand(MCommand* pCommand);
	void UpdateCmdCount(MCommand* pCommand);

protected:
	LCmdHandlerGroup*			m_pCmdHandlerGroup;

	virtual void SendCommand(minet::MCommand* pCommand);
	void LogCommand(minet::MCommand* pCommand);
public:
	LNetServer(const MNetServerDesc& desc);
	virtual ~LNetServer();

	void ChangeCmdHandlerForServerStart();

	int GetRecvCPS() { return m_nRecvCPS; }
	int GetSendCPS() { return m_nSendCPS; }
	int GetLocalCPS() { return m_nLocalCPS; }
};
