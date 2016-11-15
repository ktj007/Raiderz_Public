#include "StdAfx.h"
#include "GCmdHandler_Master_Loot.h"
#include "CCommandTable_Master.h"
#include "CTransData.h"
#include "GGlobal.h"
#include "GLootSystem.h"
#include "GLootSystemForMasterServer.h"


GCmdHandler_Master_Loot::GCmdHandler_Master_Loot(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MMC_LOOT_MASTERLOOT_NOTIFY,		OnMasterLootNotify);
	SetCmdHandler(MMC_LOOT_OTHER_GAIN_ITEM,			OnOtherGainItem);
	SetCmdHandler(MMC_LOOT_GETTABLE_ITEM_ADD,		OnGettableItemAdd);
}

MCommandResult GCmdHandler_Master_Loot::OnMasterLootNotify(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	vector<int> vecBeneficiaryCID;
	vector<TD_LOOT_MASTERLOOT_NOTIFY> vecTDLootMasterLootNotify;

	if (!pCommand->GetBlob(vecBeneficiaryCID,			0))				return CR_ERROR;
	if (!pCommand->GetBlob(vecTDLootMasterLootNotify,	1))				return CR_ERROR;

	gsys.pLootSystem->ForMasterServer().MasterLootNotify(vecBeneficiaryCID, vecTDLootMasterLootNotify);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Loot::OnOtherGainItem(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	vector<int> vecBeneficiaryCID;
	MUID nGainerUID;
	vector<TD_LOOT_OTHERGAIN_ITEM> vecTDOtherGainItem;

	if (!pCommand->GetBlob(vecBeneficiaryCID,	0))				return CR_ERROR;
	if (!pCommand->GetParameter(&nGainerUID,	1, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetBlob(vecTDOtherGainItem,	2))				return CR_ERROR;

	gsys.pLootSystem->ForMasterServer().OtherGainItem(vecBeneficiaryCID, nGainerUID, vecTDOtherGainItem);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Loot::OnGettableItemAdd(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커맨드 확인
	int nBeneficiaryCID;	
	vector<TD_LOOT_GETTABLE_ITEM_ADD> vecTDGettableItemAdd;

	if (!pCommand->GetParameter(&nBeneficiaryCID,	0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetBlob(vecTDGettableItemAdd,	1))				return CR_ERROR;

	gsys.pLootSystem->ForMasterServer().GettableItemAdd(nBeneficiaryCID, vecTDGettableItemAdd);

	return CR_TRUE;
}