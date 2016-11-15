#include "StdAfx.h"
#include "PChatModule_LeaveChannel.h"
#include "SDef.h"
#include "PGlobal.h"
#include "CCommandTable_App.h"
#include "PCmdRouter_Chat.h"
#include "PChatChannelManager.h"
#include "PChatChannel.h"
#include "PMsgCommandFacade.h"

PChatModule_LeaveChannel::PChatModule_LeaveChannel(PChatChannel* pOwnerChannel)
: PChatModuleBase(pOwnerChannel)
{
}

PChatModule_LeaveChannel::~PChatModule_LeaveChannel(void)
{
}

void PChatModule_LeaveChannel::OnRun(minet::MCommand* pCmd)
{
	// 커멘드 확인
	VALID(MPC_MSG_LEAVE_CHANNEL_REQ == pCmd->GetID());

	CID cidRequestPlayer;
	MUID uidChannel;
	VALID(pCmd->GetParameter(&cidRequestPlayer,		0, MPT_INT));
	VALID(pCmd->GetParameter(&uidChannel,			1, MPT_UID));

	MUID uidGameServer = pCmd->GetSenderUID();
	Run(uidChannel, cidRequestPlayer, uidGameServer);
}

bool PChatModule_LeaveChannel::Run(MUID uidChannel, CID cidPlayer, MUID uidGameServer)
{
	VALID_RET(uidChannel == m_pOwnerChannel->GetUID(), false);
	VALID_RET(0 < cidPlayer, false);

	// 체크 : 입장하지 않은 채널인지
	if (false == m_pOwnerChannel->IsExistPlayer(cidPlayer))
	{
		gsys.pMsgCommandFacade->RouteSystemMsg(CR_FAIL_MSG_NOT_ENTERED_CHANNEL, cidPlayer);
		return false;
	}
	
	// 퇴장
	gmgr.pChatChannelManager->LeaveChannel(m_pOwnerChannel, cidPlayer);

	// 통보	
	if (uidGameServer.IsValid())
	{
		PCmdRouter_Chat router;	
		router.PostLeaveChannel(uidGameServer, cidPlayer, m_pOwnerChannel);
	}

	return true;
}
