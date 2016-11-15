#include "stdafx.h"
#include "GMsgCommandFacade.h"
#include "GSayMsgHandler.h"
#include "GAnnounceMsgHandler.h"
#include "GWhisperMsgHandler.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "CSMsgCommandHelper.h"
#include "GCommandCenter.h"
#include "GGlobalMsgHandler.h"
#include "GShoutMsgHandler.h"
#include "GShoutFakeMsgHandler.h"
#include "GAuctionMsgHandler.h"
#include "GPartyMsgHandler.h"
#include "GPartySystem.h"
#include "GGuildMsgHandler.h"
#include "GNarrationMsgHandler.h"
#include "GChannelMsgHandler.h"
#include "GFieldMsgHandler.h"
#include "GSystemMsgHandler.h"
#include "GNoticeMsgHandler.h"
#include "GAdviceMsgHandler.h"
#include "GWhisperBackMsgHandler.h"

GMsgCommandFacade::GMsgCommandFacade()
{
	SetHandler(MT_SAY, new GSayMsgHandler());
	SetHandler(MT_NARRATION, new GNarrationMsgHandler());	
	SetHandler(MT_SHOUT, new GShoutMsgHandler());
	SetHandler(MT_SHOUT_FAKE, new GShoutFakeMsgHandler());
	SetHandler(MT_AUCTION, new GAuctionMsgHandler());
	SetHandler(MT_ANNOUNCE, new GAnnounceMsgHandler());	
	SetHandler(MT_WHISPER, new GWhisperMsgHandler());	
	SetHandler(MT_WHISPER_BACK, new GWhisperBackMsgHandler());
	SetHandler(MT_PARTY, new GPartyMsgHandler());
	SetHandler(MT_GUILD, new GGuildMsgHandler());
	SetHandler(MT_CHANNEL, new GChannelMsgHandler());
	SetHandler(MT_FIELD, new GFieldMsgHandler());
	SetHandler(MT_GLOBAL, new GGlobalMsgHandler());
	SetHandler(MT_SYSTEM, new GSystemMsgHandler());
	SetHandler(MT_NOTICE, new GNoticeMsgHandler());	
	SetHandler(MT_ADVICE, new GAdviceMsgHandler());
}

GMsgCommandFacade::~GMsgCommandFacade()
{
	// do nothing
}

bool GMsgCommandFacade::RouteSystemMsg(MUID uidPlayer, CCommandResultTable nResult)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
	if (pPlayer == NULL)	return false;

	return RouteSystemMsg(pPlayer, nResult);
}

bool GMsgCommandFacade::RouteSystemMsg(GEntityPlayer* pPlayer, CCommandResultTable nResult)
{
	VALID_RET(NULL != pPlayer, false);

	CSMsgCommandHelper msgHelper;
	MCommand* pNewCmd = msgHelper.MakeNewClientCommandSystemRes(nResult);
	pPlayer->RouteToMe(pNewCmd);
	return true;
}

bool GMsgCommandFacade::RouteNoticeMsgToAll(wstring strMsg)
{
	CSMsgCommandHelper msgHelper;
	MCommand* pNewCmd = msgHelper.MakeNewClientCommandNoticeRes(strMsg);
	gsys.pCommandCenter->RouteToAll(pNewCmd);	
	
	return true;
}

bool GMsgCommandFacade::RouteNoticeMsgToField(GField* pField, wstring strMsg )
{
	VALID_RET(pField, false);

	CSMsgCommandHelper msgHelper;
	MCommand* pNewCmd = msgHelper.MakeNewClientCommandNoticeRes(strMsg);
	GFieldGridRouter router;
	router.RouteToField(pField, pNewCmd);

	return true;
}

bool GMsgCommandFacade::RouteNoticeMsgToMe(GEntityPlayer* pPlayer, wstring strMsg)
{
	CSMsgCommandHelper msgHelper;
	MCommand* pNewCmd = msgHelper.MakeNewClientCommandNoticeRes(strMsg);
	pPlayer->RouteToMe(pNewCmd);	

	return true;
}

bool GMsgCommandFacade::RouteAdviceMsg(MUID uidPlayer, CCommandResultTable nResult)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
	if (pPlayer == NULL)	return false;

	return RouteAdviceMsg(pPlayer, nResult);
}

bool GMsgCommandFacade::RouteAdviceMsg(GEntityPlayer* pPlayer, CCommandResultTable nResult)
{
	VALID_RET(NULL != pPlayer, false);

	CSMsgCommandHelper msgHelper;
	MCommand* pNewCmd = msgHelper.MakeNewClientCommandAdviceRes(nResult);
	pPlayer->RouteToMe(pNewCmd);
	return true;
}