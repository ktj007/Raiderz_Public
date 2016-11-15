#include "StdAfx.h"
#include "PChatModule_EnterPrivateChannel.h"
#include "PChatChannel.h"
#include "CCommandTable_App.h"
#include "PGlobal.h"
#include "PChatChannelManager.h"
#include "PCmdRouter_Chat.h"
#include "PMsgCommandFacade.h"
#include "PServer.h"
#include "SProxyPlayerManager.h"
#include "SProxyPlayer.h"


void PChatModule_EnterPrivateChannel::OnRun(minet::MCommand* pCmd)
{
	VALID(CCT_PRIVATE == m_pOwnerChannel->GetType());

	// 커멘드 확인
	VALID(MPC_MSG_ENTER_CHANNEL_REQ == pCmd->GetID());

	CID cidRequestPlayer;
	wstring strChannelName;
	VALID(pCmd->GetParameter(&cidRequestPlayer,		0, MPT_INT));
	VALID(pCmd->GetParameter(strChannelName,		1, MPT_WSTR));

	SProxyPlayer* pRequestPlayer = gsys.pServer->GetPlayerManager()->FindPlayer(cidRequestPlayer);
	VALID(pRequestPlayer);	
	RVALID(strChannelName == m_pOwnerChannel->GetName());


	// 체크 - 밴 당한 채널인지
	if (m_pOwnerChannel->IsBanedPlayer(pRequestPlayer->GetName()))
	{
		gsys.pMsgCommandFacade->RouteSystemMsg(CR_FAIL_MSG_BANED_CHANNEL, cidRequestPlayer);
		return;
	}

	// 일반 입장 처리
	MUID uidChannel = m_pOwnerChannel->GetUID();
	MUID uidGameServer = pCmd->GetSenderUID();
	if (false == PChatModule_EnterChannel::Run(uidChannel, cidRequestPlayer, uidGameServer)) return;

	// 통보
	PCmdRouter_Chat router;
	router.RouteEnterChannelPlayerInfo(m_pOwnerChannel, pRequestPlayer->GetName(), cidRequestPlayer);
}