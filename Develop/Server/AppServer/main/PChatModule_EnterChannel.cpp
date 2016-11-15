#include "StdAfx.h"
#include "PChatModule_EnterChannel.h"
#include "PChatChannel.h"
#include "CCommandTable_App.h"
#include "SDef.h"
#include "MCommandParameter.h"
#include "PGlobal.h"
#include "PChatChannelManager.h"
#include "PChatPlayer.h"
#include "PCmdRouter_Chat.h"
#include "PMsgCommandFacade.h"
#include "PGameServerObject.h"
#include "PGameServerObjectManager.h"


PChatModule_EnterChannel::PChatModule_EnterChannel(PChatChannel* pOwnerChannel)
: PChatModuleBase(pOwnerChannel)
{
}

PChatModule_EnterChannel::~PChatModule_EnterChannel(void)
{
}

void PChatModule_EnterChannel::OnRun(minet::MCommand* pCmd)
{
	VALID(MPC_MSG_ENTER_CHANNEL_REQ == pCmd->GetID());

	CID cidRequestPlayer;
	wstring strChannelName;
	VALID(pCmd->GetParameter(&cidRequestPlayer,	0, MPT_INT));
	VALID(pCmd->GetParameter(strChannelName,	1, MPT_WSTR));
	
	VALID(0 < cidRequestPlayer);
	RVALID(strChannelName == m_pOwnerChannel->GetName());
	
	MUID uidChannel = m_pOwnerChannel->GetUID();
	MUID uidGameServer = pCmd->GetSenderUID();
	Run(uidChannel, cidRequestPlayer, uidGameServer);
}

bool PChatModule_EnterChannel::Run(MUID uidChannel, CID cidPlayer, MUID uidGameServer)
{
	VALID_RET(uidChannel == m_pOwnerChannel->GetUID(), false);
	VALID_RET(0 < cidPlayer, false);

	PGameServerObject* pGameServerObject = gmgr.pGameServerObjectManager->Find(uidGameServer);
	if (NULL == pGameServerObject) return false;

	// 체크 - 플레이어가 이미 3개의 채널에 입장해 있는 상태인지
	PChatPlayer* pChatPlayer = gmgr.pChatChannelManager->FindPlayer(cidPlayer);
	if (NULL != pChatPlayer)
	{
		if (MAX_ENTERABLE_PRIVATE_CHAT_CHANNEL_COUNT <= pChatPlayer->GetEnteredPrivateChannelCount())
		{
			gsys.pMsgCommandFacade->RouteSystemMsg(CR_FAIL_MSG_LIMIT_ENTER_PRIVATE_CHANNEL, cidPlayer);
			return false;
		}
	}

	// 체크 - 이미 입장한 채널인지
	if (m_pOwnerChannel->IsExistPlayer(cidPlayer))
	{
		gsys.pMsgCommandFacade->RouteSystemMsg(CR_FAIL_MSG_ALREADY_ENTERED_CHANNEL, cidPlayer);
		return false;
	}

	// 입장
	gmgr.pChatChannelManager->EnterChannel(m_pOwnerChannel, cidPlayer, pGameServerObject->GetServerID());

	// 통보
	PCmdRouter_Chat router;	
	router.PostEnterChannel(uidGameServer, cidPlayer, m_pOwnerChannel);

	return true;
}
