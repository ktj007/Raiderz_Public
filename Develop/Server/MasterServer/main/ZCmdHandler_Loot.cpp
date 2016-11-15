#include "StdAfx.h"
#include "ZCmdHandler_Loot.h"
#include "CCommandTable_Master.h"
#include "CTransData.h"
#include "ZLootLogic.h"

ZCmdHandler_Loot::ZCmdHandler_Loot(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MMC_LOOT_MASTERLOOT_NOTIFY_REQ,		OnMasterLootNotifyReq);
	SetCmdHandler(MMC_LOOT_OTHER_GAIN_ITEM_REQ,			OnOtherGainItemReq);
	SetCmdHandler(MMC_LOOT_GETTABLE_ITEM_ADD_REQ,		OnGettableItemAddReq);
}

MCommandResult ZCmdHandler_Loot::OnMasterLootNotifyReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<int> vecBeneficiaryCID;
	vector<TD_LOOT_MASTERLOOT_NOTIFY>	vecTDLootMasterLootNotify;

	if (!pCommand->GetBlob(vecBeneficiaryCID,			0))	return CR_ERROR;
	if (!pCommand->GetBlob(vecTDLootMasterLootNotify,	1))	return CR_ERROR;

	gsys.pLootLogic->MasterLootNotifyReq(vecBeneficiaryCID, vecTDLootMasterLootNotify);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Loot::OnOtherGainItemReq(MCommand* pCommand, MCommandHandler* pHandler)
{	
	vector<int> vecBeneficiaryCID;
	MUID nPlayerUID;
	vector<TD_LOOT_OTHERGAIN_ITEM> vecTDOtherGainItem;

	if (!pCommand->GetBlob(vecBeneficiaryCID,	0))	return CR_ERROR;
	if (!pCommand->GetParameter(&nPlayerUID,	1, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetBlob(vecTDOtherGainItem,	2))	return CR_ERROR;

	gsys.pLootLogic->OtherGainitemReq(vecBeneficiaryCID, nPlayerUID, vecTDOtherGainItem);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Loot::OnGettableItemAddReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nBeneficiaryCID;
	vector<TD_LOOT_GETTABLE_ITEM_ADD> vecTDGettableItemAdd;

	if (!pCommand->GetParameter(&nBeneficiaryCID,	1, MPT_INT))return CR_ERROR;
	if (!pCommand->GetBlob(vecTDGettableItemAdd,	0))			return CR_ERROR;
	
	gsys.pLootLogic->GettableItemAddReq(nBeneficiaryCID, vecTDGettableItemAdd);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Loot::OnRollResultReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<int> vecBeneficiaryCID;
	vector<TD_LOOT_ROLL_ITEM> vecTDRollItem;
	vector<TD_LOOT_ROLL_RESULT> vecTDRollResult;

	if (!pCommand->GetBlob(vecBeneficiaryCID,	0))	return CR_ERROR;
	if (!pCommand->GetBlob(vecTDRollItem,		1))	return CR_ERROR;
	if (!pCommand->GetBlob(vecTDRollResult,		2))	return CR_ERROR;

	gsys.pLootLogic->RollResultReq(vecBeneficiaryCID, vecTDRollItem, vecTDRollResult);

	return CR_TRUE;
}