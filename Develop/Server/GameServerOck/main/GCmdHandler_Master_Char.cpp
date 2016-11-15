#include "StdAfx.h"
#include "GCmdHandler_Master_Char.h"
#include "CCommandTable_Master.h"
#include "GNPCRewarder.h"
#include "GPlayerObjectManager.h"
#include "GGlobal.h"
#include "GReserveCmdManager.h"

GCmdHandler_Master_Char::GCmdHandler_Master_Char(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MMC_CHAR_NPCDIE_REWARD,		OnNPCDieReward);
}

MCommandResult GCmdHandler_Master_Char::OnNPCDieReward(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<int> vecBeneficiaryCID;
	int nAllBeneficiaryCount;
	int nNPCID;

	if (!pCommand->GetBlob(vecBeneficiaryCID, 0))					return CR_ERROR;
	if (!pCommand->GetParameter(&nAllBeneficiaryCount, 1, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nNPCID, 2, MPT_INT))				return CR_ERROR;

	GNPCRewarder npcRewarder;

	vector<GEntityPlayer*> vecExistBeneficiary;
	vector<int> vecNotExistBeneficiaryCID;

	for each (int nBeneficiaryCID in vecBeneficiaryCID)
	{
		GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(nBeneficiaryCID);
		if (NULL == pPlayer) continue;

		vecExistBeneficiary.push_back(pPlayer);
	}

	npcRewarder.RewardByDieForMasterServer(vecExistBeneficiary, nAllBeneficiaryCount, nNPCID);
	gmgr.pReserveCmdManager->Reserve(vecNotExistBeneficiaryCID, pCommand->Clone());

	return CR_TRUE;
}