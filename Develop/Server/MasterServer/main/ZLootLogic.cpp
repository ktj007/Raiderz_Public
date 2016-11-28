#include "StdAfx.h"
#include "ZLootLogic.h"
#include "ZPlayer.h"
#include "ZPlayerManager.h"
#include "MCommand.h"
#include "ZGlobal.h"
#include "ZCommandCenter.h"
#include "CCommandTable_Master.h"
#include "ZGameServerObject.h"
#include "ZGameServerObjectManager.h"
#include "CTransData.h"
#include "ZReceiveGameServerUIDSelector.h"

ZLootLogic::ZLootLogic(void)
{
}

ZLootLogic::~ZLootLogic(void)
{
}

void ZLootLogic::MasterLootNotifyReq(const vector<CID>& vecBeneficiaryCID, const vector<TD_LOOT_MASTERLOOT_NOTIFY>& vecTDMasterLootNotify)
{
	MAP_RECEIEVE_GAMESERVER_UID mapReceiveGameServerUID;
	ZReceiveGameServerUIDSelector::Select(vecBeneficiaryCID, mapReceiveGameServerUID);

	for each (const MAP_RECEIEVE_GAMESERVER_UID::value_type& val in mapReceiveGameServerUID)
	{
		const MUID& nReceiveGameServerUID = val.first;
		const vector<CID>& vecSelectedBeneficiaryCID = val.second;

		MCommand*pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_LOOT_MASTERLOOT_NOTIFY,
			nReceiveGameServerUID,
			2,
			NEW_BLOB(vecSelectedBeneficiaryCID),
			NEW_BLOB(vecTDMasterLootNotify));

		gsys.pCommandCenter->PostCommand(pNewCmd);
	}
}

void ZLootLogic::OtherGainitemReq(const vector<CID>& vecBeneficiaryCID, MUID nGainnerUID, const vector<TD_LOOT_OTHERGAIN_ITEM>& vecTDOtherGainItem)
{
	MAP_RECEIEVE_GAMESERVER_UID mapReceiveGameServerUID;
	ZReceiveGameServerUIDSelector::Select(vecBeneficiaryCID, mapReceiveGameServerUID);

	for each (const MAP_RECEIEVE_GAMESERVER_UID::value_type& val in mapReceiveGameServerUID)
	{
		const MUID& nReceiveGameServerUID = val.first;
		const vector<CID>& vecSelectedBeneficiaryCID = val.second;

		MCommand*pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_LOOT_OTHER_GAIN_ITEM,
			nReceiveGameServerUID,
			3,
			NEW_BLOB(vecSelectedBeneficiaryCID),
			NEW_UID(nGainnerUID),
			NEW_BLOB(vecTDOtherGainItem));

		gsys.pCommandCenter->PostCommand(pNewCmd);
	}	
}

void ZLootLogic::GettableItemAddReq(CID nBeneficiaryCID, const vector<TD_LOOT_GETTABLE_ITEM_ADD>& vecTDGettableItemAdd)
{
	MAP_RECEIEVE_GAMESERVER_UID mapReceiveGameServerUID;
	ZReceiveGameServerUIDSelector::Select(nBeneficiaryCID, mapReceiveGameServerUID);

	for each (const MAP_RECEIEVE_GAMESERVER_UID::value_type& val in mapReceiveGameServerUID)
	{
		const MUID& nReceiveGameServerUID = val.first;
		const vector<CID>& vecSelectedBeneficiaryCID = val.second;

		MCommand*pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_LOOT_GETTABLE_ITEM_ADD,
			nReceiveGameServerUID,
			2,
			NEW_INT64(nBeneficiaryCID),
			NEW_BLOB(vecTDGettableItemAdd));

		gsys.pCommandCenter->PostCommand(pNewCmd);
	}	
}

void ZLootLogic::RollResultReq(const vector<CID>& vecBeneficiaryCID, const vector<TD_LOOT_ROLL_ITEM>& vecTDRollItem, const vector<TD_LOOT_ROLL_RESULT>& vecTDRollResult)
{
	MAP_RECEIEVE_GAMESERVER_UID mapReceiveGameServerUID;
	ZReceiveGameServerUIDSelector::Select(vecBeneficiaryCID, mapReceiveGameServerUID);

	for each (const MAP_RECEIEVE_GAMESERVER_UID::value_type& val in mapReceiveGameServerUID)
	{
		const MUID& nReceiveGameServerUID = val.first;
		const vector<CID>& vecSelectedBeneficiaryCID = val.second;

		MCommand*pNewCmd = gsys.pCommandCenter->MakeNewCommand(MMC_LOOT_ROLLRESULT,
			nReceiveGameServerUID,
			3,
			NEW_BLOB(vecSelectedBeneficiaryCID),
			NEW_BLOB(vecTDRollItem),
			NEW_BLOB(vecTDRollResult));

		gsys.pCommandCenter->PostCommand(pNewCmd);
	}	
}