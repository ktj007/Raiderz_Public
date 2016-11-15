#include "StdAfx.h"
#include "GCmdHandler_Storage.h"
#include "CCommandTable.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GStorageSystem.h"
#include "GInteractionSystem.h"
#include "GNPCInteractor.h"
#include "CSDef_Interaction.h"

GCmdHandler_Storage::GCmdHandler_Storage(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_STORAGE_DEPOSIT_MONEY_REQ,		OnDepositMoney);
	SetCmdHandler(MC_STORAGE_WITHDRAW_MONEY_REQ,	OnWithdrawMoney);
}

MCommandResult GCmdHandler_Storage::OnDepositMoney(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	int nMoney;
	if (pCmd->GetParameter(&nMoney,	0, MPT_INT)==false) return CR_FALSE;

	if (!gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pPlayer, IT_STORAGE))
		return CR_FALSE;

	gsys.pStorageSystem->DepositMoney(pPlayer, nMoney);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Storage::OnWithdrawMoney(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	int nMoney;
	if (pCmd->GetParameter(&nMoney,	0, MPT_INT)==false) return CR_FALSE;

	if (!gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pPlayer, IT_STORAGE))
		return CR_FALSE;

	gsys.pStorageSystem->WithdrawMoney(pPlayer, nMoney);

	return CR_TRUE;
}