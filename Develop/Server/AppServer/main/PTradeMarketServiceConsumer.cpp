#include "stdafx.h"
#include "PTradeMarketServiceConsumer.h"
#include "PGlobal.h"
#include "PTradeMarketService.h"
#include "SProxyPlayer.h"
#include "PServer.h"
#include "SProxyPlayerManager.h"
#include "PGameServerObject.h"
#include "PGameServerObjectManager.h"
#include "PCommandCenter.h"

PTradeMarketServiceConsumer::PTradeMarketServiceConsumer()
{
}

PTradeMarketServiceConsumer::~PTradeMarketServiceConsumer()
{
}

void PTradeMarketServiceConsumer::NotifyAddItem(MCommand* cmd)
{
	gsys.pTradeMarketService->NotifyAddItem(cmd);
}

void PTradeMarketServiceConsumer::NotifyDelItem(MCommand* cmd)
{
	gsys.pTradeMarketService->NotifyDelItem(cmd);
}

void PTradeMarketServiceConsumer::RequestSearch(MCommand* cmd)
{
	gsys.pTradeMarketService->RequestSearch(cmd);
}

void PTradeMarketServiceConsumer::RequestSearchNext(MCommand* cmd)
{
	gsys.pTradeMarketService->RequestSearchNext(cmd);
}

void PTradeMarketServiceConsumer::OnRecvCmd(minet::MCommand* const cmd)
{
	if (NULL == cmd)
	{
		mlog("PTradeMarketServiceConsumer::OnRecvCmd() - error null cmd\n");
		return;
	}

	CID playerCID = 0;
	if (false == cmd->GetParameter(&playerCID, 0, MPT_INT))
	{
		mlog("PTradeMarketServiceConsumer::OnRecvCmd() - invalid playerCID, CID == 0\n");
		return;
	}

	SProxyPlayer* const pPlayer = gsys.pServer->GetPlayerManager()->FindPlayer(playerCID);

	if (pPlayer == NULL)
	{
		// 해당 플레이어는 이미 접속을 종료했다. 통보 않는다.
		return;
	}

	// 플레이어가 있는 게임서버 찾기
	int nGameServerID = pPlayer->GetGameServerID();
	PGameServerObject* const pGameServer = gmgr.pGameServerObjectManager->Find(nGameServerID);

	if (pGameServer == NULL)
	{
		// 게임서버를 찾을 수 없다면, 통보하지 않는다. 
		mlog("PTradeMarketServiceConsumer::OnRecvCmd() - no game server, server id : \n", nGameServerID);
		return;
	}

	gsys.pCommandCenter->SendCommand(pGameServer->GetUID(), cmd);
}
