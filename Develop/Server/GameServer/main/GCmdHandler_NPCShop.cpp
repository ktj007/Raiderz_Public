#include "StdAfx.h"
#include "GCmdHandler_NPCShop.h"
#include "CCommandTable.h"
#include "GNPCShopSystem.h"
#include "GInteractionSystem.h"
#include "GPlayerObjectManager.h"
#include "GGlobal.h"
#include "GItemHolder.h"
#include "GEntityPlayer.h"
#include "GNPCInteractor.h"
#include "GPlayerInteraction.h"
#include "GNPCShopBuyer.h"
#include "GNPCShopSeller.h"
#include "GNPCShopRepairer.h"

GCmdHandler_NPCShop::GCmdHandler_NPCShop(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_NPCSHOP_BUY_REQ, OnRequestBuy);
	SetCmdHandler(MC_NPCSHOP_SELL_REQ, OnRequestSell);
	SetCmdHandler(MC_NPCSHOP_SELL_VERYCOMMON_REQ, OnRequestSellVeryCommon);
	SetCmdHandler(MC_NPCSHOP_REPAIR_ONE_REQ, OnRequestRepairOne);
	SetCmdHandler(MC_NPCSHOP_REPAIR_ALL_REQ, OnRequestRepairAll);
}

MCommandResult GCmdHandler_NPCShop::OnRequestBuy(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pEntityPlayer) return CR_FALSE;

	int nItemID;
	USHORT nItemAmount;
	if (false == pCmd->GetParameter(&nItemID, 0, MPT_INT)) return CR_FALSE;
	if (false == pCmd->GetParameter(&nItemAmount, 1, MPT_USHORT)) return CR_FALSE;

	if (false == gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pEntityPlayer, IT_NPCSHOP)) return CR_FALSE;

	gsys.pNPCShopSystem->GetBuyer().Buy(pEntityPlayer, pEntityPlayer->GetInteraction().GetSelectedIElementInfo().GetSinlgeAct(), nItemID, static_cast<int>(nItemAmount));

	return CR_TRUE;
}

MCommandResult GCmdHandler_NPCShop::OnRequestSell(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pEntityPlayer) return CR_FALSE;

	int nInvenSlotID;
	USHORT nItemAmount;
	if (false == pCmd->GetParameter(&nInvenSlotID, 0, MPT_INT)) return CR_FALSE;
	if (false == pCmd->GetParameter(&nItemAmount, 1, MPT_USHORT)) return CR_FALSE;

	if (false == gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pEntityPlayer, IT_NPCSHOP)) return CR_FALSE;

	gsys.pNPCShopSystem->GetSeller().Sell(pEntityPlayer, pEntityPlayer->GetInteraction().GetSelectedIElementInfo().GetSinlgeAct(), nInvenSlotID, static_cast<int>(nItemAmount));

	return CR_TRUE;
}

MCommandResult GCmdHandler_NPCShop::OnRequestSellVeryCommon(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pEntityPlayer) return CR_FALSE;

	if (false == gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pEntityPlayer, IT_NPCSHOP)) return CR_FALSE;

	gsys.pNPCShopSystem->GetSeller().SellVeryCommon(pEntityPlayer, pEntityPlayer->GetInteraction().GetSelectedIElementInfo().GetSinlgeAct());

	return CR_TRUE;
}

MCommandResult GCmdHandler_NPCShop::OnRequestRepairOne(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pEntityPlayer) return CR_FALSE;

	int8 slotType;
	int nSlotID;
	if (false == pCmd->GetParameter(&slotType, 0, MPT_CHAR))	return CR_FALSE;
	if (false == pCmd->GetParameter(&nSlotID, 1, MPT_INT))		return CR_FALSE;

	if (false == gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pEntityPlayer, IT_NPCSHOP)) return CR_FALSE;	
	
	gsys.pNPCShopSystem->GetRepairer().RepairOne(pEntityPlayer, pEntityPlayer->GetInteraction().GetSelectedIElementInfo().GetSinlgeAct(), slotType, nSlotID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_NPCShop::OnRequestRepairAll(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pEntityPlayer) return CR_FALSE;

	if (false == gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pEntityPlayer, IT_NPCSHOP)) return CR_FALSE;

	gsys.pNPCShopSystem->GetRepairer().RepairAll(pEntityPlayer, pEntityPlayer->GetInteraction().GetSelectedIElementInfo().GetSinlgeAct());

	return CR_TRUE;
}
