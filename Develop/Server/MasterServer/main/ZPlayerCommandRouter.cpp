#include "stdafx.h"
#include "ZPlayerCommandRouter.h"
#include "ZCommandCenter.h"
#include "ZCommandTable.h"
#include "ZGlobal.h"
#include "ZPlayerManager.h"
#include "SCommandTable.h"
#include "STransData.h"
#include "ZSharedField.h"
#include "ZFieldManager.h"

void ZPlayerCommandRouter::MakeProxyPlayerInfo(TD_PROXY_PLAYER_INFO* pInfo, const ZPlayer* pPlayer)
{
	pInfo->nAID = pPlayer->GetAID();
	pInfo->nCID = pPlayer->GetCID();
	pInfo->UID = pPlayer->GetUID();
	wcsncpy_s(pInfo->szName, const_cast<ZPlayer*>(pPlayer)->GetPlayerName().c_str(), _TRUNCATE);
	pInfo->nGameServerID = pPlayer->GetGameServerID();
	pInfo->uidField = pPlayer->GetFieldUID();
	pInfo->uidParty = pPlayer->GetPartyUID();
	pInfo->nGuildID = pPlayer->GetGuildID();
	pInfo->nFieldID = pPlayer->GetFieldID();
	
	ZSharedField* pSharedField = gmgr.pFieldManager->FindSharedField(pPlayer->GetFieldUID());
	if (pSharedField != NULL)
	{
		pInfo->nChannelID = pSharedField->GetChannelID();
	}

	pInfo->uidFieldGroup = pPlayer->GetFieldGroupUID();
}

void ZPlayerCommandRouter::SendAddPlayerResponse(const MUID& uidLoginServer, const MUID& uidPlayer, int nResult)
{
	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MLC_PLAYER_ADD_PLAYER_RES
		, uidLoginServer
		, 2
		, NEW_UID(uidPlayer)
		, NEW_INT(nResult));

	gsys.pCommandCenter->PostCommand(pNewCommand);
}

void ZPlayerCommandRouter::SendPmangAddPlayerResponse(const MUID& uidLoginServer, const MUID& uidPlayer, int nResult)
{
	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MLC_PLAYER_PMANG_ADD_PLAYER_RES
		, uidLoginServer
		, 2
		, NEW_UID(uidPlayer)
		, NEW_INT(nResult));

	gsys.pCommandCenter->PostCommand(pNewCommand);
}

void ZPlayerCommandRouter::SendDuplicatedPlayerLogin_ToLoginServer(const MUID& uidLoginServer, const MUID& uidPlayer, const MUID& uidConnectionKey/*=MUID::ZERO*/)
{
	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MLC_PLAYER_DUPLICATED_PLAYER_LOGIN
		, uidLoginServer
		, 2
		, NEW_UID(uidPlayer)
		, NEW_UID(uidConnectionKey)
		);

	gsys.pCommandCenter->PostCommand(pNewCommand);
}

void ZPlayerCommandRouter::SendDuplicatedPlayerLogin_ToGameServer(const MUID& uidGameServer, const MUID& uidPlayer, const MUID& uidConnectionKey/*=MUID::ZERO*/)
{
	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MMC_PLAYER_DUPLICATED_PLAYER_LOGIN
		, uidGameServer
		, 2
		, NEW_UID(uidPlayer)
		, NEW_UID(uidConnectionKey)
		);

	gsys.pCommandCenter->PostCommand(pNewCommand);
}

void ZPlayerCommandRouter::SendPlayerStateInWorldRes(const MUID& uidGameServer, CID nCID, int nResult)
{
	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MMC_PLAYER_STATE_IN_WORLD_RES
		, uidGameServer
		, 2
		, NEW_INT(nCID)
		, NEW_INT(nResult));

	gsys.pCommandCenter->PostCommand(pNewCommand);
}

void ZPlayerCommandRouter::RouteProxyPlayerAdd(const ZPlayer* pPlayer)
{
	TD_PROXY_PLAYER_INFO info;
	MakeProxyPlayerInfo(&info, pPlayer);

	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MSC_PROXY_PLAYER_ADD,
															1,
															NEW_SINGLE_BLOB(&info, sizeof(info))
															);

	gsys.pCommandCenter->RouteProxyInfo(pNewCmd);
}

void ZPlayerCommandRouter::RouteProxyPlayerRemove(CID cid)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MSC_PROXY_PLAYER_REMOVE,
															1,
															NEW_INT(cid)
															);

	gsys.pCommandCenter->RouteProxyInfo(pNewCmd);
}

void ZPlayerCommandRouter::RouteProxyPlayerUpdate(const ZPlayer* pPlayer)
{
	TD_PROXY_PLAYER_INFO info;
	MakeProxyPlayerInfo(&info, pPlayer);

	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MSC_PROXY_PLAYER_UPDATE,
															1,
															NEW_SINGLE_BLOB(&info, sizeof(info))
															);

	gsys.pCommandCenter->RouteProxyInfo(pNewCmd);
}

void ZPlayerCommandRouter::RouteAllProxyPlayer(const MUID& uidTarget)
{
	// 모든 플레이어 정보 모으기
	const int PLAYER_TRANS_LIMIT = 400;		// 30800 kb
	const ZPlayerAIDMap& mapPlayer = gmgr.pPlayerManager->GetPlayerMap();
	
	vector<TD_PROXY_PLAYER_INFO> vecInfo;

	ZPlayerAIDMap::const_iterator it = mapPlayer.begin();
	while(it != mapPlayer.end())	
	{
		// 전송정보 만들기
		ZPlayer* pPlayer = it->second;
		TD_PROXY_PLAYER_INFO tdProxyPlayerInfo;

		MakeProxyPlayerInfo(&tdProxyPlayerInfo, pPlayer);
		vecInfo.push_back(tdProxyPlayerInfo);

		it++;


		// 전송 제한 인원 분량이 되었거나, 마지막 정보면 전송한다.
		if (PLAYER_TRANS_LIMIT <= vecInfo.size() || it == mapPlayer.end())
		{
			// 전송
			MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MSC_PROXY_ALL_PLAYER_INFO, 1, NEW_BLOB(vecInfo));
			gsys.pCommandCenter->SendCommand(uidTarget, pNewCmd);

			// 다음 전송 정보를 위해 초기화
			vecInfo.clear();
		}
	}
}
