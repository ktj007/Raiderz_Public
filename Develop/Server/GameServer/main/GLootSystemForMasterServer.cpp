#include "StdAfx.h"
#include "GLootSystemForMasterServer.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GEntityPlayer.h"

GLootSystemForMasterServer::GLootSystemForMasterServer(void)
{
}

GLootSystemForMasterServer::~GLootSystemForMasterServer(void)
{
}

void GLootSystemForMasterServer::MasterLootNotify(const vector<int>& vecBeneficiaryCID, const vector<TD_LOOT_MASTERLOOT_NOTIFY>& vecTDMasterLootNotify)
{
	for each (int nBeneficiaryCID in vecBeneficiaryCID)
	{
		GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(nBeneficiaryCID);
		if (NULL == pPlayer) continue;

		MCommand* pNewCmd = MakeNewCommand(MC_LOOT_MASTERLOOT_NOTIFY,
			1, 
			NEW_BLOB(vecTDMasterLootNotify));

		pPlayer->RouteToMe(pNewCmd);
	}
}

void GLootSystemForMasterServer::OtherGainItem(const vector<int>& vecBeneficiaryCID, const MUID& nGainerUID, const vector<TD_LOOT_OTHERGAIN_ITEM>& vecTDOtherGainItem)
{
	for each (int nBeneficiaryCID in vecBeneficiaryCID)
	{
		GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(nBeneficiaryCID);
		if (NULL == pPlayer) continue;

		MCommand* pNewCmd = MakeNewCommand(MC_LOOT_OTHER_GAIN_ITEM,
			2,
			NEW_UID(nGainerUID),
			NEW_BLOB(vecTDOtherGainItem));

		pPlayer->RouteToMe(pNewCmd);
	}
}

void GLootSystemForMasterServer::GettableItemAdd(int nBeneficiaryCID, const vector<TD_LOOT_GETTABLE_ITEM_ADD>& vecTDGettableItemAdd)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(nBeneficiaryCID);
	if (NULL == pPlayer) return;

	MCommand* pNewCmd = MakeNewCommand(MC_LOOT_GETTABLE_ITEM_ADD,
		1,
		NEW_BLOB(vecTDGettableItemAdd));

	pPlayer->RouteToMe(pNewCmd);
}

void GLootSystemForMasterServer::RollResult(const vector<int>& vecBeneficiaryCID, const vector<TD_LOOT_ROLL_ITEM>& vecTDRollItem, const vector<TD_LOOT_ROLL_RESULT>& vecTDRollResult)
{
	for each (int nBeneficiaryCID in vecBeneficiaryCID)
	{
		GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(nBeneficiaryCID);
		if (NULL == pPlayer) continue;

		MCommand* pNewCmd = MakeNewCommand(MC_LOOT_ROLLRESULT,
			2, 
			NEW_BLOB(vecTDRollItem), 
			NEW_BLOB(vecTDRollResult));

		pPlayer->RouteToMe(pNewCmd);
	}
}