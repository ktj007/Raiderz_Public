#ifndef _GNET_SERVER_H
#define _GNET_SERVER_H


#include "MNetServer.h"

using namespace minet;

class GCmdHandlerGroup;

/// 네트웍 관리자. 주로 커맨드를 처리한다.
class GNetServer : public MNetServer
{
private:
	int						m_nRecvTotalCmdCount, m_nSendTotalCmdCount, m_nLocalTotalCmdCount;
	int						m_nRecvCPS, m_nSendCPS, m_nLocalCPS;
	int						m_nRecvCmdCnt, m_nSendCmdCnt, m_nLocalCmdCnt;
	unsigned int			m_nLastCheckTime;

	virtual void OnPrepareRun(void);
	virtual void OnPrepareCommand(MCommand* pCommand);
	void UpdateCmdCount(MCommand* pCommand);

	GCmdHandlerGroup*		m_pCmdHandlerGroup;
protected:
	virtual void OnCheckCrowded();
	virtual void SendCommand(minet::MCommand* pCommand);
	void LogCommand(minet::MCommand* pCommand);	

public:
	GNetServer(const MNetServerDesc& desc);
	virtual ~GNetServer();

	int GetRecvCPS() { return m_nRecvCPS; }
	int GetSendCPS() { return m_nSendCPS; }
	int GetLocalCPS() { return m_nLocalCPS; }

};



#endif
