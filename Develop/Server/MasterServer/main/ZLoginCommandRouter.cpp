#include "stdafx.h"
#include "ZLoginCommandRouter.h"
#include "ZCommandCenter.h"
#include "ZCommandTable.h"
#include "STransData.h"
#include "CTransData.h"
#include "ZServer.h"

void ZLoginCommandRouter::RouteRequestLoginGameServer(MUID uidTarget, MUID uidRequestServer, MUID uidRequestPlayer, MUID uidConnectionKey, const TD_LOGIN_ACCOUNT_INFO* pLoginAccountInfo)
{
	_ASSERT(pLoginAccountInfo != NULL);

	MCommand* pCmd = gsys.pCommandCenter->MakeNewCommand(MMC_COMM_REQUEST_LOGIN_GAME_SERVER
						, uidTarget
						, 4
						, NEW_UID(uidRequestServer)
						, NEW_UID(uidRequestPlayer)
						, NEW_UID(uidConnectionKey)
						, NEW_SINGLE_BLOB(pLoginAccountInfo, sizeof(TD_LOGIN_ACCOUNT_INFO)));

	gsys.pCommandCenter->PostCommand(pCmd);
}

void ZLoginCommandRouter::RouteResponseLoginGameServer(MUID uidTarget, MUID uidRequestPlayer, int nResult, const TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo)
{
	int nCount = 1;
	if (nResult != CR_SUCCESS || pLoginGameServerInfo == NULL)
	{		
		nCount = 0;
	}

	MCommand* pCmd = gsys.pCommandCenter->MakeNewCommand(MLC_COMM_RESPONSE_LOGIN_GAME_SERVER,
						uidTarget,
						3,
						NEW_UID(uidRequestPlayer),
						NEW_INT(nResult),
						NEW_BLOB(pLoginGameServerInfo, sizeof(TD_LOGIN_GAME_SERVER_INFO), nCount));

	gsys.pCommandCenter->PostCommand(pCmd);
}
