#include "StdAfx.h"
#include "ZCmdHandler_Char.h"
#include "CCommandTable_Master.h"
#include "ZCharLogic.h"

ZCmdHandler_Char::ZCmdHandler_Char(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MMC_CHAR_NPCDIE_REWARD_REQ,	OnNPCDieRewardReq);
}

MCommandResult ZCmdHandler_Char::OnNPCDieRewardReq(MCommand* pCommand, MCommandHandler* pHandler)
{	
	vector<int> vecBeneficiaryCID;
	int nAllBeneficiaryCount;
	int nNPCID;

	if (!pCommand->GetBlob(vecBeneficiaryCID, 0)) return CR_ERROR;
	if (!pCommand->GetParameter(&nAllBeneficiaryCount, 1, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nNPCID, 2, MPT_INT)) return CR_ERROR;

	gsys.pCharLogic->NPCDieReward(vecBeneficiaryCID, nAllBeneficiaryCount, nNPCID);

	return CR_TRUE;
}
