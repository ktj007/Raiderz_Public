#include "StdAfx.h"
#include "GCmdHandler_Inn.h"
#include "CCommandTable.h"
#include "GInnSystem.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GInteractionSystem.h"
#include "GNPCInteractor.h"


GCmdHandler_Inn::GCmdHandler_Inn(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_STORAGE_DEPOSIT_MONEY_REQ,		OnBeginSleepReq);
}

MCommandResult GCmdHandler_Inn::OnBeginSleepReq(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (!gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pPlayer, IT_INN_SLEEP))
		return CR_FALSE;

	gsys.pInnsystem->BeginSleep(pPlayer);

	return CR_TRUE;
}