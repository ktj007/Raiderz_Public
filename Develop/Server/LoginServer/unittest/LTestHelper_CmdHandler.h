#pragma once

namespace minet
{
	class MTestNetAgent;
}
struct TD_LOGIN_GAME_SERVER_INFO;

class LTestHelper_CmdHandler
{
public:
	static MCommandResult OnRecv_MLC_COMM_RESPONSE_LOGIN_GAME_SERVER(minet::MTestNetAgent* pNetAgent, MUID uidPlayer, int nResult, TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo);
};