#include "stdafx.h"
#include "PProxyPlayerListener.h"
#include "SProxyPlayer.h"
#include "PChattingLogic.h"
#include "STransData.h"

void PProxyPlayerListener::OnAddComplate(const SProxyPlayer* pPlayer)
{
	PChattingLogic logic;

	CID cid = pPlayer->GetCID();
	int nFieldID = pPlayer->GetFieldID();
	MUID uidParty = pPlayer->GetPartyUID();
	int nGameServerID = pPlayer->GetGameServerID();

	// 거래채널 입장
	logic.EnterAuctionChannel(cid, nGameServerID);
		
	// 필드채널 입장
	logic.EnterFieldChannel(cid, nGameServerID, nFieldID);

	// 파티채널 입장
	if (uidParty.IsValid())
	{
		logic.EnterPartyChannel(cid, nGameServerID, uidParty);
	}
}

void PProxyPlayerListener::OnPreRemove(const SProxyPlayer* pPlayer)
{	
	PChattingLogic logic;
	
	// 전채널 퇴장
	CID cid = pPlayer->GetCID();
	logic.LeaveAllChannel(cid);
}

void PProxyPlayerListener::OnPreUpdate(const SProxyPlayer* pPlayer, const TD_PROXY_PLAYER_INFO& info)
{
	PChattingLogic logic;

	CID cid = pPlayer->GetCID();
	int nFieldID = pPlayer->GetFieldID();
	MUID uidParty = pPlayer->GetPartyUID();
	int nGameServerID = pPlayer->GetGameServerID();

	// 게임서버 변경
	if (nGameServerID != info.nGameServerID)
	{
		logic.ChangedGameServerID(cid, info.nGameServerID);
	}

	// 필드채널 변경
	if (nFieldID != info.nFieldID)
	{
		logic.LeaveFieldChannel(cid, nGameServerID, nFieldID);
		logic.EnterFieldChannel(cid, info.nGameServerID, info.nFieldID);
	}

	// 파티채널 변경
	if (uidParty != info.uidParty)
	{
		if (uidParty.IsValid())
		{
			logic.LeavePartyChannel(cid, nGameServerID, uidParty);
		}

		if (info.uidParty.IsValid())
		{
			logic.EnterPartyChannel(cid, info.nGameServerID, info.uidParty);
		}
	}
}
