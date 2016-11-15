#pragma once

#include "MCommand.h"
#include "MNetServer.h"
#include "ServerCommonLib.h"

class SCommandLogger;

using namespace minet;

/// 커맨드 흐름을 관리하는 클래스
class SCOMMON_API SCommandCenter
{
protected:
	MNetServer*		m_pNetServer;
	SCommandLogger*	m_pCommandLogger;

public:
	SCommandCenter(MNetServer* netserver);
	virtual ~SCommandCenter();

	MCommand*	NewCommand(int nCmdID, MUID TargetUID);
	MCommand*	NewCommand(int nCmdID);
	void		PostCommand(MCommand* pCommand);
	void		SendCommand(MUID uidTarget, MCommand* pCommand);

	MCommand* MakeNewCommand(int nCmdID, MUID TargetUID, int nParamCount, MCommandParameter* pCmdParam, ... );
	MCommand* MakeNewCommand(int nCmdID, int nParamCount, MCommandParameter* pCmdParam, ... );

	void IgnoreDebugCommandLog();
	void LogCommand(MCommand* pCmd);
	MNetServer* GetNetServer() { return m_pNetServer; }
};
