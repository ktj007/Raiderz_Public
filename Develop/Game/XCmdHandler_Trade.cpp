#include "StdAfx.h"
#include "XCmdHandler_Trade.h"
#include "XPost_Interaction.h"
#include "XStrings.h"
#include "XItemManager.h"
#include "XMyPlayer.h"
#include "XPost_Trade.h"
#include "XInteractionInfo.h"

XCmdHandler_Trade::XCmdHandler_Trade(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_TRADE_START_REQ_RELAY,		OnTradeStartReqRelay);	
	SetCmdHandler(MC_TRADE_START,				OnTradeStart);
	SetCmdHandler(MC_TRADE_START_REJECT,		OnTradeStartReject);
	SetCmdHandler(MC_TRADE_PUTUP_ITEM,			OnTradePutUpItem);
	SetCmdHandler(MC_TRADE_PUTDOWN_ITEM,		OnTradePutDownItem);
	SetCmdHandler(MC_TRADE_PUTUP_MONEY,			OnTradePutUpMoney);
	SetCmdHandler(MC_TRADE_CANCEL,				OnTradeCancel);
	SetCmdHandler(MC_TRADE_CONFIRM,				OnTradeConfirm);
	SetCmdHandler(MC_TRADE_COMPLETE,			OnTradeComplete);
}

MCommandResult XCmdHandler_Trade::OnTradeStartReqRelay(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidReqPlayer;
	if (pCommand->GetParameter(&uidReqPlayer, 0, MPT_UID)==false)	return CR_ERROR;

	if ( XGETCFG_GAME_REJECT_TRADE)
	{
		XObject* pReqPlayer = gg.omgr->Find(uidReqPlayer);
		if(pReqPlayer == NULL) return CR_FALSE;

		global.ui->OnSystemMsg( XGetStr( L"TRADE_CANCELEDBYDENY"), FSParam(pReqPlayer->GetName()));

		XPostTrade_StartAccept(false);
		gvar.Game.bTradeReq = false;
	}
	else
	{
		const wchar_t* szMsg = XGetStr(L"SMSG_TRADE_START_REQ");
		global.ui->OnPresentationLower(szMsg, PRESENTATION_ICON_INFORMATION, true);

		gvar.Game.pMyPlayer->SafeStop();

		XGetInteractionInfo().SetRequestPlayerUID(uidReqPlayer);
		gvar.Game.bTradeReq	= true;

		if( global.script)
		{
			global.script->GetGlueGameEvent().OnGameEvent( "TRADE", "REQUESTED");
		}

		if( global.ui)
			global.ui->OnGuideMsg(GUIDE_MSG_LUA_RECV_TRADE_REQUESTED);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Trade::OnTradeStart(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidTrader;
	int nTraderLevel;
	if (pCommand->GetParameter(&uidTrader,		0, MPT_UID)==false)	return CR_ERROR;
	if (pCommand->GetParameter(&nTraderLevel,	1, MPT_INT)==false)	return CR_ERROR;

	XGetInteractionInfo().SetRequestPlayerUID(uidTrader);

	global.ui->OnSystemMsg( XGetStr( L"TRADE_STARTING"));

	_ASSERT(gvar.MyInfo.TradeContents.IsTrade() != true);

	gvar.MyInfo.TradeContents.SetTrade(true);

	XModuleMyControl* pModuleMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	if ( pModuleMyControl != NULL)		pModuleMyControl->DoActionHold();

	if ( global.script)		global.script->GetGlueGameEvent().OnGameEvent( "TRADE", "BEGIN");

	return CR_TRUE;
}

MCommandResult XCmdHandler_Trade::OnTradeStartReject(MCommand* pCommand, MCommandHandler* pHandler)
{
	const wchar_t* szMsg = XGetStr(L"SMSG_TRADE_REJECT");
	global.ui->OnSystemMsg(szMsg);

	return CR_TRUE;
}


MCommandResult XCmdHandler_Trade::OnTradePutUpItem(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidReqPlayer;
	TD_TRADE_ITEM_PUTUP tdItemPutUp;
	if (pCommand->GetParameter(&uidReqPlayer,	0,	MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetSingleBlob(tdItemPutUp,	1)==false)	return CR_ERROR;
	
	if (XGetMyUID() == uidReqPlayer)
	{
		gvar.MyInfo.TradeContents.m_MyTradeContainer.InsertItem(tdItemPutUp);
		if (true == gvar.MyInfo.TradeContents.m_bMyConfirm)
		{
			gvar.MyInfo.TradeContents.m_bMyConfirm = false;
			global.ui->OnSystemMsg( XGetStr( L"TRADE_CHECKOFF"));
		}
		if (true == gvar.MyInfo.TradeContents.m_bOtherConfirm)
		{
			gvar.MyInfo.TradeContents.m_bOtherConfirm = false;
			global.ui->OnSystemMsg( XGetStr( L"TRADE_CHECKOFFOTHER"));
		}
	}
	else
	{
		gvar.MyInfo.TradeContents.m_OtherTradeContainer.InsertItem(tdItemPutUp);
		if (true == gvar.MyInfo.TradeContents.m_bMyConfirm)
		{
			gvar.MyInfo.TradeContents.m_bMyConfirm = false;
			global.ui->OnSystemMsg( XGetStr( L"TRADE_CHECKOFF"));
		}
		if (true == gvar.MyInfo.TradeContents.m_bOtherConfirm)
		{
			gvar.MyInfo.TradeContents.m_bOtherConfirm = false;
			global.ui->OnSystemMsg( XGetStr( L"TRADE_CHECKOFFOTHER"));
		}
	}

	if(global.script)
	{
		global.script->GetGlueGameEvent().OnGameEvent( "TRADE", "REFRESH");
		//global.script->GetGlueGameEvent().OnNoticeDialogShow(szChat);
	}

	if (global.ui)	global.ui->RefreshTradeUI();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Trade::OnTradePutDownItem(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidReqPlayer;
	TD_TRADE_ITEM_PUTDOWN tdItemPutDown;
	if (pCommand->GetParameter(&uidReqPlayer,	0,	MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetSingleBlob(tdItemPutDown,	1)==false)	return CR_ERROR;

	if (XGetMyUID() == uidReqPlayer)
	{
		if (false == gvar.MyInfo.TradeContents.m_MyTradeContainer.DecItem(tdItemPutDown)) return CR_ERROR;

		if (true == gvar.MyInfo.TradeContents.m_bMyConfirm)
		{
			gvar.MyInfo.TradeContents.m_bMyConfirm = false;
			global.ui->OnSystemMsg( XGetStr( L"TRADE_CHECKOFF"));
		}
		if (true == gvar.MyInfo.TradeContents.m_bOtherConfirm)
		{
			gvar.MyInfo.TradeContents.m_bOtherConfirm = false;
			global.ui->OnSystemMsg( XGetStr( L"TRADE_CHECKOFFOTHER"));
		}
	}
	else
	{
		if (false == gvar.MyInfo.TradeContents.m_OtherTradeContainer.DecItem(tdItemPutDown)) return CR_ERROR;

		if (true == gvar.MyInfo.TradeContents.m_bMyConfirm)
		{
			gvar.MyInfo.TradeContents.m_bMyConfirm = false;
			global.ui->OnSystemMsg( XGetStr( L"TRADE_CHECKOFF"));
		}
		if (true == gvar.MyInfo.TradeContents.m_bOtherConfirm)
		{
			gvar.MyInfo.TradeContents.m_bOtherConfirm = false;
			global.ui->OnSystemMsg( XGetStr( L"TRADE_CHECKOFFOTHER"));
		}
	}

	if(global.script)
	{
		global.script->GetGlueGameEvent().OnGameEvent( "TRADE", "REFRESH");
		//global.script->GetGlueGameEvent().OnNoticeDialogShow(szChat);
	}

	if (global.ui)	global.ui->RefreshTradeUI();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Trade::OnTradePutUpMoney(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidReqPlayer;
	int	nPutUpMoney;
	if (pCommand->GetParameter(&uidReqPlayer,	0,	MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nPutUpMoney,		1,	MPT_INT)==false) return CR_ERROR;

	if (0 > nPutUpMoney) return CR_ERROR;

	if (XGetMyUID() == uidReqPlayer)
	{
		gvar.MyInfo.TradeContents.m_nMySilver = nPutUpMoney;
		if (true == gvar.MyInfo.TradeContents.m_bMyConfirm)
		{
			gvar.MyInfo.TradeContents.m_bMyConfirm = false;
			global.ui->OnSystemMsg( XGetStr( L"TRADE_CHECKOFF"));
		}
		if (true == gvar.MyInfo.TradeContents.m_bOtherConfirm)
		{
			gvar.MyInfo.TradeContents.m_bOtherConfirm = false;
			global.ui->OnSystemMsg( XGetStr( L"TRADE_CHECKOFFOTHER"));
		}
	}
	else
	{
		gvar.MyInfo.TradeContents.m_nOtherSilver = nPutUpMoney;
		if (true == gvar.MyInfo.TradeContents.m_bMyConfirm)
		{
			gvar.MyInfo.TradeContents.m_bMyConfirm = false;
			global.ui->OnSystemMsg( XGetStr( L"TRADE_CHECKOFF"));
		}
		if (true == gvar.MyInfo.TradeContents.m_bOtherConfirm)
		{
			gvar.MyInfo.TradeContents.m_bOtherConfirm = false;
			global.ui->OnSystemMsg( XGetStr( L"TRADE_CHECKOFFOTHER"));
		}
	}

	if (global.ui)	global.ui->RefreshTradeUI();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Trade::OnTradeCancel(MCommand* pCommand, MCommandHandler* pHandler)
{
	gvar.MyInfo.TradeContents.Reset();
	global.ui->OnSystemMsg( XGetStr( L"TRADE_CANCELLED"));

	if(global.script)
	{
		global.script->GetGlueGameEvent().OnGameEvent( "TRADE", "CANCEL");
		global.script->GetGlueGameEvent().OnGameEvent( "TRADE", "END");
	}

	XModuleMyControl* pModuleMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	if ( pModuleMyControl != NULL)		pModuleMyControl->DoActionIdle();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Trade::OnTradeConfirm(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidConfirmPlayer;
	if (pCommand->GetParameter(&uidConfirmPlayer,		0, MPT_UID)==false)	return CR_ERROR;

	if (XGetMyUID() == uidConfirmPlayer)
	{
		gvar.MyInfo.TradeContents.m_bMyConfirm = true;
	}
	else
	{
		gvar.MyInfo.TradeContents.m_bOtherConfirm = true;
	}

	if(global.script)
		global.script->GetGlueGameEvent().OnGameEvent( "TRADE", "REFRESH");

	return CR_TRUE;
}

MCommandResult XCmdHandler_Trade::OnTradeComplete(MCommand* pCommand, MCommandHandler* pHandler)
{
	gvar.MyInfo.TradeContents.Reset();
	global.ui->OnSystemMsg( XGetStr( L"TRADE_COMPLETED"));

	if(global.script)
	{
		global.script->GetGlueGameEvent().OnGameEvent( "TRADE", "SUCCESS");
		global.script->GetGlueGameEvent().OnGameEvent( "TRADE", "END");
		//global.script->GetGlueGameEvent().OnNoticeDialogShow(szChat);
	}

	XModuleMyControl* pModuleMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	if ( pModuleMyControl != NULL)		pModuleMyControl->DoActionIdle();

	return CR_TRUE;
}
