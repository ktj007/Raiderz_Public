#include "StdAfx.h"
#include "PChatModule_LeavePrivateChannel.h"
#include "PChatChannel.h"
#include "CCommandTable_App.h"
#include "PGlobal.h"
#include "PChatChannelManager.h"
#include "PCmdRouter_Chat.h"
#include "PMsgCommandFacade.h"
#include "PServer.h"
#include "PChatModules_PrivateChannel.h"
#include "SProxyPlayerManager.h"
#include "SProxyPlayer.h"


void PChatModule_LeavePrivateChannel::OnRun(minet::MCommand* pCmd)
{
	VALID(CCT_PRIVATE == m_pOwnerChannel->GetType());

	// 커멘드 확인
	VALID(MPC_MSG_LEAVE_CHANNEL_REQ == pCmd->GetID());

	CID cidRequestPlayer;
	MUID uidChannel;
	VALID(pCmd->GetParameter(&cidRequestPlayer,		0, MPT_INT));
	VALID(pCmd->GetParameter(&uidChannel,			1, MPT_UID));

	SProxyPlayer* pLeavePlayer = gsys.pServer->GetPlayerManager()->FindPlayer(cidRequestPlayer);
	VALID(pLeavePlayer);
	VALID(uidChannel == m_pOwnerChannel->GetUID());

	// 일반 퇴장 처리
	MUID uidGameServer = pCmd->GetSenderUID();
	if (false == PChatModule_LeaveChannel::Run(uidChannel, cidRequestPlayer, uidGameServer)) return;

	// 채널에 플레이어가 남아있지 않을 경우, 지연 제거
	if (m_pOwnerChannel->IsEmptyPlayer())
	{
		gmgr.pChatChannelManager->LazilyDeleteChannel(m_pOwnerChannel);
		return;
	}

	// 통보
	PCmdRouter_Chat router;
	router.RouteLeaveChannelPlayerInfo(m_pOwnerChannel, pLeavePlayer->GetName());

	// 방장 변경 모듈 호출
	//	방장이 나갔을 경우 &&
	//	채널에 플레이어가 남아있을 경우
	if (cidRequestPlayer == m_pOwnerChannel->GetOwnerCID() &&
		1 <= m_pOwnerChannel->GetPlayerCount())
	{
		CID cidNextOwnerPlayer = m_pOwnerChannel->GetFirstPlayerCID();
		SProxyPlayer* pNewOwner = gsys.pServer->GetPlayerManager()->FindPlayer(cidNextOwnerPlayer);
		VALID(pNewOwner);

		PChatModule_ChangeHost* pModule = dynamic_cast<PChatModule_ChangeHost* >(m_pOwnerChannel->FindChatModules(CMT_CHANGE_HOST));
		VALID(pModule != NULL);
		pModule->Run(uidGameServer, uidChannel, cidRequestPlayer, pNewOwner->GetName());
	}
}
