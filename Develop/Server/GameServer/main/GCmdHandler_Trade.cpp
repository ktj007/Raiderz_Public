#include "StdAfx.h"
#include "GCmdHandler_Trade.h"
#include "CCommandTable.h"
#include "GTradeSystem.h"
#include "GEntityPlayer.h"
#include "GPlayerObjectManager.h"
#include "GGlobal.h"
#include "GTradeRequester.h"
#include "GTradeAcceptor.h"
#include "GTradeItemPutUper.h"
#include "GTradeItemPutDowner.h"
#include "GTradeSilverPutUper.h"
#include "GTradeCanceler.h"
#include "GTradeConfirmer.h"
#include "GTradeExchanger.h"
#include "GPlayerTrade.h"

GCmdHandler_Trade::GCmdHandler_Trade(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_TRADE_START_REQ,			OnRequestTradeStart);
	SetCmdHandler(MC_TRADE_START_ACCEPT_REQ,	OnRequestTradeStartAccept);
	SetCmdHandler(MC_TRADE_PUTUP_ITEM_REQ,		OnRequestTradePutUpItem);
	SetCmdHandler(MC_TRADE_PUTDOWN_ITEM_REQ,	OnRequestTradePutDownItem);
	SetCmdHandler(MC_TRADE_PUTUP_MONEY_REQ,		OnRequestTradePutUpMoney);
	SetCmdHandler(MC_TRADE_CANCEL_REQ,			OnRequestTradeCancel);
	SetCmdHandler(MC_TRADE_CONFIRM_REQ,			OnRequestTradeConfirm);
}

MCommandResult GCmdHandler_Trade::OnRequestTradeStart(MCommand* pCmd, MCommandHandler* pHandler)
{
	MUID uidPlayer = pCmd->GetSenderUID();
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);
	if (NULL == pEntityPlayer) return CR_FALSE;

	MUID uidTarPlayer;
	if (false == pCmd->GetParameter(&uidTarPlayer,	0, MPT_UID)) return CR_FALSE;

	GEntityPlayer* pTarPlayer = gmgr.pPlayerObjectManager->GetEntity(uidTarPlayer);
	if (NULL == pTarPlayer) return CR_FALSE;

	gsys.pTradeSystem->GetRequester().Request(pEntityPlayer, pTarPlayer);
	pEntityPlayer->OnDoSomething();

	return CR_TRUE;
}

MCommandResult GCmdHandler_Trade::OnRequestTradeStartAccept(MCommand* pCmd, MCommandHandler* pHandler)
{
	MUID uidPlayer = pCmd->GetSenderUID();
	GEntityPlayer* pAcceptPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);
	if (NULL == pAcceptPlayer) return CR_FALSE;

	bool bAccept;
	if (false == pCmd->GetParameter(&bAccept,		0, MPT_BOOL)) return CR_FALSE;

	MUID nTargetUID = pAcceptPlayer->GetPlayerTrade().GetTradeRequesterUID();
	GEntityPlayer* pTarPlayer = pAcceptPlayer->FindPlayer(nTargetUID);
	if (NULL == pTarPlayer) return CR_FALSE;

	gsys.pTradeSystem->GetAcceptor().Accept(pAcceptPlayer, pTarPlayer, bAccept);
	pAcceptPlayer->OnDoSomething();

	return CR_TRUE;
}

MCommandResult GCmdHandler_Trade::OnRequestTradePutUpItem(MCommand* pCmd, MCommandHandler* pHandler)
{
	MUID uidPlayer = pCmd->GetSenderUID();
	GEntityPlayer* pReqPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);
	if (NULL == pReqPlayer) return CR_FALSE;

	TD_TRADE_ITEM_PUTUP_REQ tdItemPutUpReq;
	if (false == pCmd->GetSingleBlob(tdItemPutUpReq, 0)) return CR_FALSE;

	GEntityPlayer* pTarPlayer = pReqPlayer->FindPlayer(pReqPlayer->GetPlayerTrade().GetTrader());
	if (NULL == pTarPlayer) return CR_FALSE;

	gsys.pTradeSystem->GetItemPutUper().PutUp(pReqPlayer, pTarPlayer, tdItemPutUpReq.m_nSlotID, tdItemPutUpReq.m_nAmount);
	pReqPlayer->OnDoSomething();

	return CR_TRUE;
}

MCommandResult GCmdHandler_Trade::OnRequestTradePutDownItem(MCommand* pCmd, MCommandHandler* pHandler)
{
	MUID uidPlayer = pCmd->GetSenderUID();
	GEntityPlayer* pReqPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);
	if (NULL == pReqPlayer) return CR_FALSE;

	TD_TRADE_ITEM_PUTDOWN tdItemPutDown;
	if (false == pCmd->GetSingleBlob(tdItemPutDown, 0)) return CR_FALSE;	

	GEntityPlayer* pTarPlayer = pReqPlayer->FindPlayer(pReqPlayer->GetPlayerTrade().GetTrader());
	if (NULL == pTarPlayer) return CR_FALSE;

	gsys.pTradeSystem->GetItemPutDowner().PutDown(pReqPlayer, pTarPlayer, tdItemPutDown.m_nSlotID, tdItemPutDown.m_nAmount);
	pReqPlayer->OnDoSomething();

	return CR_TRUE;
}

MCommandResult GCmdHandler_Trade::OnRequestTradePutUpMoney(MCommand* pCmd, MCommandHandler* pHandler)
{
	MUID uidPlayer = pCmd->GetSenderUID();
	GEntityPlayer* pReqPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);
	if (NULL == pReqPlayer) return CR_FALSE;

	int nMoney = 0;
	if (pCmd->GetParameter(&nMoney, 0, MPT_INT)==false) return CR_FALSE;

	GEntityPlayer* pTargetPlayer = pReqPlayer->FindPlayer(pReqPlayer->GetPlayerTrade().GetTrader());
	if (NULL == pTargetPlayer) return CR_FALSE;

	gsys.pTradeSystem->GetMoneyPutUper().PutUp(pReqPlayer, pTargetPlayer, nMoney);
	pReqPlayer->OnDoSomething();

	return CR_TRUE;
}

MCommandResult GCmdHandler_Trade::OnRequestTradeCancel(MCommand* pCmd, MCommandHandler* pHandler)
{
	MUID uidPlayer = pCmd->GetSenderUID();
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);
	if (NULL == pEntityPlayer) return CR_FALSE;

	gsys.pTradeSystem->GetCanceler().Cancel(pEntityPlayer);
	pEntityPlayer->OnDoSomething();

	return CR_TRUE;
}

MCommandResult GCmdHandler_Trade::OnRequestTradeConfirm(MCommand* pCmd, MCommandHandler* pHandler)
{
	MUID uidPlayer = pCmd->GetSenderUID();
	GEntityPlayer* pReqPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);
	if (NULL == pReqPlayer) return CR_FALSE;

	GEntityPlayer* pTarPlayer = pReqPlayer->FindPlayer(pReqPlayer->GetPlayerTrade().GetTrader());
	if (NULL == pTarPlayer) return CR_FALSE;

	if (false == gsys.pTradeSystem->GetConfirmer().Confirm(pReqPlayer, pTarPlayer))
	{
		return CR_FALSE;
	}

	if (true == pReqPlayer->GetPlayerTrade().IsConfirm() &&
		true == pTarPlayer->GetPlayerTrade().IsConfirm())
	{
		gsys.pTradeSystem->GetExchanger().Exchange(pReqPlayer, pTarPlayer);
	}	

	pReqPlayer->OnDoSomething();
	
	return CR_TRUE;
}