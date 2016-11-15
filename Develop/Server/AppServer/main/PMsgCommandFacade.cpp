#include "stdafx.h"
#include "PMsgCommandFacade.h"
#include "PAuctionMsgHandler.h"
#include "PAnnounceMsgHandler.h"
#include "PWhisperMsgHandler.h"
#include "PPartyMsgHandler.h"
#include "PGuildMsgHandler.h"
#include "PChannelMsgHandler.h"
#include "PFieldMsgHandler.h"
#include "PGlobalMsgHandler.h"
#include "SDef.h"
#include "PGlobal.h"
#include "PServer.h"
#include "PGameServerObjectManager.h"
#include "SProxyPlayerManager.h"
#include "SProxyPlayer.h"
#include "PCommandCenter.h"
#include "PNoticeMsgHandler.h"


PMsgCommandFacade::PMsgCommandFacade()
{
	SetHandler(MT_AUCTION, new PAuctionMsgHandler());
	SetHandler(MT_ANNOUNCE, new PAnnounceMsgHandler());	
	SetHandler(MT_WHISPER, new PWhisperMsgHandler());	
	SetHandler(MT_PARTY, new PPartyMsgHandler());
	SetHandler(MT_GUILD, new PGuildMsgHandler());
	SetHandler(MT_CHANNEL, new PChannelMsgHandler());
	SetHandler(MT_FIELD, new PFieldMsgHandler());
	SetHandler(MT_GLOBAL, new PGlobalMsgHandler());
	SetHandler(MT_NOTICE, new PNoticeMsgHandler());
}

PMsgCommandFacade::~PMsgCommandFacade()
{
	// do nothing
}

void PMsgCommandFacade::RouteNoticeMsg(wstring strMsg)
{
	CSMsgCommandHelper helper;
	MCommand* pNewCmd = helper.MakeNewServerCommandNoticeRes(strMsg);

	gsys.pCommandCenter->RouteToAll(pNewCmd);	
}

bool PMsgCommandFacade::RouteSystemMsg(CCommandResultTable nResult, CID cidTargetPlayer)
{
	CSMsgCommandHelper helper;
	MCommand* pNewCmd = helper.MakeNewServerCommandSystemRes(nResult, cidTargetPlayer);
	
	return RouteTargetPlayer(pNewCmd, cidTargetPlayer);	
}

bool PMsgCommandFacade::RouteTargetPlayer(MCommand* pCmd, CID cidTargetPlayer)
{
	VALID_RET(pCmd != NULL, false);

	// 목표 확인
	SProxyPlayerManager* pManager = gsys.pServer->GetPlayerManager();
	SProxyPlayer* pPlayer = pManager->FindPlayer(cidTargetPlayer);
	if (pPlayer != NULL)
	{
		// 목표 서버 확인
		int nGameServerID = pPlayer->GetGameServerID();
		MUID uidGameServer = gmgr.pGameServerObjectManager->GetGameServerUID(nGameServerID);
		if (uidGameServer.IsValid())
		{
			pCmd->AddReceiver(uidGameServer);
			gsys.pCommandCenter->PostCommand(pCmd);
			return true;
		}
	}

	// 커멘드 제거
	delete pCmd;
	return false;
}
