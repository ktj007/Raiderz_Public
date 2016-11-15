#include "StdAfx.h"
#include "ZCharLogic.h"
#include "ZReceiveGameServerUIDSelector.h"
#include "MCommand.h"
#include "ZCommandCenter.h"
#include "CCommandTable_Master.h"

ZCharLogic::ZCharLogic(void)
{
}

ZCharLogic::~ZCharLogic(void)
{
}

void ZCharLogic::NPCDieReward(const vector<int>& vecBeneficiaryCID, int nAllBeneficiaryCount, int nNPCID)
{
	MAP_RECEIEVE_GAMESERVER_UID mapReceiveGameServerUID;
	ZReceiveGameServerUIDSelector::Select(vecBeneficiaryCID, mapReceiveGameServerUID);

	for each (const MAP_RECEIEVE_GAMESERVER_UID::value_type& val in mapReceiveGameServerUID)
	{
		const MUID& nReceiveGameServerUID = val.first;
		const vector<int>& vecSelectedBeneficiaryCID = val.second;

		MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_CHAR_NPCDIE_REWARD,
			nReceiveGameServerUID,
			3,
			NEW_BLOB(vecSelectedBeneficiaryCID),
			NEW_INT(nAllBeneficiaryCount),
			NEW_INT(nNPCID));

		gsys.pCommandCenter->PostCommand(pNewCmd);
	}
}