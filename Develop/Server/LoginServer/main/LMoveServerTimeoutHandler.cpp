#include "stdafx.h"
#include "LMoveServerTimeoutHandler.h"
#include "LGlobal.h"
#include "LServer.h"
#include "LPlayerObjectManager.h"
#include "LPlayerObject.h"
#include "LCmdRouter_Player.h"


void LMoveServerTimeoutHandler::OnTimeout(MUID uidPlayer)
{
	LPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pPlayerObject)
	{
		LCmdRouter_Player::PostDeletePlayer(pPlayerObject->GetAccountInfo().nAID, uidPlayer);
	}

	gsys.pServer->DisconnectPlayer(uidPlayer);
}
