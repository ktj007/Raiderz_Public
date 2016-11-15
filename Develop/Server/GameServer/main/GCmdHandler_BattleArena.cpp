#include "StdAfx.h"
#include "GCmdHandler_BattleArena.h"
#include "CCommandTable.h"
#include "GEntityPlayer.h"
#include "GPlayerObjectManager.h"
#include "GGlobal.h"
#include "GPlayerBattleArena.h"

GCmdHandler_BattleArena::GCmdHandler_BattleArena(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_BATTLEARENA_REGISTER_REQ,			OnRegister);
	SetCmdHandler(MC_BATTLEARENA_DEREGISTER_REQ,		OnDeregister);
}

MCommandResult GCmdHandler_BattleArena::OnRegister(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	pPlayer->GetPlayerBattleArena().RegisterEntry();

	return CR_TRUE;
}

MCommandResult GCmdHandler_BattleArena::OnDeregister(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	pPlayer->GetPlayerBattleArena().DeregisterEntry();

	return CR_TRUE;
}
