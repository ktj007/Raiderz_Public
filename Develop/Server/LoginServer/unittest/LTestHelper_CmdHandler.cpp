#include "stdafx.h"
#include "LTestHelper_CmdHandler.h"
#include "MMockNetClient.h"
#include "CTransData.h"
#include "CCommandTable_Master.h"


MCommandResult LTestHelper_CmdHandler::OnRecv_MLC_COMM_RESPONSE_LOGIN_GAME_SERVER(minet::MTestNetAgent* pNetAgent, MUID uidPlayer, int nResult, TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo)
{
	int nBlobCount = 0;
	if (NULL != pLoginGameServerInfo)
	{
		nBlobCount = 1;
	}

	return pNetAgent->OnRecv(MLC_COMM_RESPONSE_LOGIN_GAME_SERVER
		, 3
		, NEW_UID(uidPlayer)
		, NEW_INT(nResult)
		, NEW_BLOB(pLoginGameServerInfo, sizeof(TD_LOGIN_GAME_SERVER_INFO), nBlobCount)
		);

}