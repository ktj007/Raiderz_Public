#pragma once

#include "MCommand.h"
#include "GNetServer.h"
#include "GCommandLogger.h"

/// 커맨드 흐름을 관리하는 클래스
class GCommandCenter
{
protected:
	GNetServer&			m_NetServer;
	GCommandLogger		m_CommandLogger;
public:
	GCommandCenter(GNetServer& netserver);
	~GCommandCenter() {}

	MCommand*	NewCommand(int nCmdID, MUID TargetUID);
	MCommand*	NewCommand(int nCmdID);
	void		PostCommand(MCommand* pCommand);
	void		RouteToAll(MCommand* pCommand);

	void IgnoreDebugCommandLog();
	void LogCommand(MCommand* pCmd);
	minet::MCommandManager* GetCommandManager() 
	{
		return m_NetServer.GetCommandManager();
	}
	GNetServer& GetNetServer() { return m_NetServer; }
};



