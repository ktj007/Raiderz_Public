#include "StdAfx.h"
#include "GCmdHandler_QuestPVP.h"
#include "CCommandTable.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GPlayerPVPArea.h"
#include "GCommand.h"

GCmdHandler_QuestPVP::GCmdHandler_QuestPVP(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_QUESTPVP_ENTER_REQ,	OnEnterReq);
}

MCommandResult GCmdHandler_QuestPVP::OnEnterReq(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (false == pPlayer->GetPlayerPVPArea().IsLocatedPvPArea()) return CR_FALSE;

	QUESTPVP_TEAM nTeam = pPlayer->GetPlayerPVPArea().GetTeam();

	MCommand* pNewCmd = MakeNewCommand(MC_QUESTPVP_ENTER,
		2,
		NEW_USHORT(pPlayer->GetUIID()),
		NEW_INT(static_cast<int>(nTeam)));

	pPlayer->RouteToThisCell(pNewCmd);

	return CR_TRUE;
}