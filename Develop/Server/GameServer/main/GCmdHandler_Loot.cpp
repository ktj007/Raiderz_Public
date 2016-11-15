#include "StdAfx.h"
#include "GCmdHandler_Loot.h"
#include "CCommandTable.h"
#include "MiNetCommon.h"
#include "GLootSystem.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GCommand.h"
#include "CTransData.h"
#include "GPlayerLoot.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GNPCLoot.h"
#include "GDropList.h"
#include "GDropItems.h"
#include "GLootBeginner.h"
#include "GLootShower.h"
#include "GLootLooter.h"
#include "GLootEnder.h"
#include "GLootMasterLooter.h"
#include "GLootBPart.h"

GCmdHandler_Loot::GCmdHandler_Loot(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_LOOT_BEGIN_REQ,			OnBegin);	
	SetCmdHandler(MC_LOOT_SEE_ITEM_REQ,			OnSeeItem);
	SetCmdHandler(MC_LOOT_LOOT_ITEM_REQ,		OnLootItem);
	SetCmdHandler(MC_LOOT_MASTERLOOT_ITEM_REQ,	OnMasterLootItem);
	SetCmdHandler(MC_LOOT_END_REQ,				OnRequestEnd);	
}

MCommandResult GCmdHandler_Loot::OnBegin(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	MUID uidNPC;
	if (pCmd->GetParameter(&uidNPC,	0, MPT_UID)==false) return CR_FALSE;
	
	if (false == gsys.pLootSystem->GetBeginner().KillNPC(pPlayer, uidNPC))
	{
		gsys.pLootSystem->GetEnder().End(pPlayer);
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_Loot::OnSeeItem(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	MUID uidNPC;
	if (pCmd->GetParameter(&uidNPC,	0, MPT_UID)==false) return CR_FALSE;

	if (gsys.pLootSystem->GetBPart().IsBPartLoot(uidNPC))
	{
		gsys.pLootSystem->GetBPart().TakeAll(pPlayer, uidNPC);
		gsys.pLootSystem->GetEnder().End(pPlayer);

		return CR_TRUE;
	}

	if (false == gsys.pLootSystem->GetShower().Show(pPlayer, uidNPC))
	{		
		gsys.pLootSystem->GetEnder().End(pPlayer);
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_Loot::OnLootItem(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	MUID uidNPC;
	GVectorMUID vecDropItemUID;

	if (pCmd->GetParameter(&uidNPC,	0, MPT_UID)==false) return CR_FALSE;
	if (pCmd->GetBlob(vecDropItemUID.Vector(), 1)==false) return CR_FALSE;

	if (false == gsys.pLootSystem->GetLooter().LootItem(pPlayer, uidNPC, vecDropItemUID.Vector()))
	{
		gsys.pLootSystem->GetEnder().End(pPlayer);
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_Loot::OnMasterLootItem(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	MUID uidNPC;
	MUID uidDropItem;
	uint8 nSelectedIndex;
	bool bRandom;
	if (pCmd->GetParameter(&uidNPC,				0, MPT_UID)==false) return CR_FALSE;
	if (pCmd->GetParameter(&uidDropItem,		1, MPT_UID)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nSelectedIndex,		2, MPT_UCHAR)==false) return CR_FALSE;
	if (pCmd->GetParameter(&bRandom,			3, MPT_BOOL)==false) return CR_FALSE;	

	if (false == gsys.pLootSystem->GetMasterLooter().MasterLootItem(pPlayer, uidNPC, uidDropItem, nSelectedIndex, bRandom))
	{
		gsys.pLootSystem->GetEnder().End(pPlayer);
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_Loot::OnRequestEnd(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	gsys.pLootSystem->GetEnder().End(pPlayer);

	return CR_TRUE;
}
