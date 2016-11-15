#include "StdAfx.h"
#include "PChattingLogic.h"
#include "PChatChannelFactory.h"
#include "PGlobal.h"
#include "PChatChannelManager.h"
#include "PChatChannel.h"
#include "PGameServerObject.h"
#include "PGameServerObjectManager.h"
#include "SProxyPlayerManager.h"
#include "SProxyPlayer.h"
#include "PServer.h"
#include "PChatPlayer.h"
#include "PCommandCenter.h"
#include "CCommandTable_App.h"
#include "PChatModule_EnterChannel.h"
#include "PChatModule_LeaveChannel.h"
#include "PChatModule_EnterPrivateChannel.h"
#include "PChatModule_LeavePartyChannel.h"
#include "PChatModules_PrivateChannel.h"
#include "PFieldInfoManager.h"
#include "PMsgCommandFacade.h"



bool PChattingLogic::CreateDefaultChannels(void)
{
	VALID_RET(0 == gmgr.pChatChannelManager->GetChannelCount(), false);

	PChatChannelFactory factory;
	PChatChannel* pChannel;
	
	// 거래채널 만들기	
	pChannel = factory.NewChatChannel_Auction();
	VALID_RET(pChannel != NULL, false);

	if (!gmgr.pChatChannelManager->AddChannel(pChannel))
	{
		delete pChannel;
		return false;
	}

	// 필드채널 만들기
	CSFieldList& fieldList = gmgr.pFieldInfoManager->GetFieldList();
	for(CSFieldList::iterator it = fieldList.begin(); it != fieldList.end(); it++)
	{
		int nFieldID = it->first;
		pChannel = factory.NewChatChannel_Field(nFieldID);
		VALID_RET(pChannel != NULL, false);

		if (!gmgr.pChatChannelManager->AddChannel(pChannel))
		{
			delete pChannel;
			return false;
		}
	}

	return true;
}

void PChattingLogic::CreateChannel_Private(int nPlayerCID, const std::wstring& strChannelName)
{
	VALID(0 != nPlayerCID);
	VALID(0 != strChannelName.size());

	if (strChannelName.size() > MAX_CHATTING_CHANNEL_NAME_LEN)
	{
		gsys.pMsgCommandFacade->RouteSystemMsg(CR_FAIL_MSG_OVER_CHATTING_CHANNEL_NAME, nPlayerCID);
		return;
	}

	SProxyPlayer* pPlayer = gsys.pServer->GetPlayerManager()->FindPlayer(nPlayerCID);					RVALID(pPlayer);
	PGameServerObject* pGameServer = gmgr.pGameServerObjectManager->Find(pPlayer->GetGameServerID());	RVALID(pGameServer);
	
	/// 이미 생성돼있는 채널인지 체크
	if (NULL != gmgr.pChatChannelManager->FindChannel(strChannelName))
	{
		gsys.pMsgCommandFacade->RouteSystemMsg(CR_FAIL_MSG_ALREADY_EXIST_CHANNEL_NAME, nPlayerCID);
		return;
	}

	/// 채널 생성
	PChatChannelFactory factoryChatChannel;
	PChatChannel* pNewChatChannel = factoryChatChannel.NewChatChannel_Private(nPlayerCID, strChannelName);
	VALID(pNewChatChannel);
	if (false == gmgr.pChatChannelManager->AddChannel(pNewChatChannel))
	{
		SAFE_DELETE(pNewChatChannel);
		return;
	}

	/// 플레이어 채널 입장 처리
	MUID uidChannel = pNewChatChannel->GetUID();
	MUID uidGameServer = pGameServer->GetUID();
	
	PChatModule_EnterPrivateChannel* pModule = dynamic_cast<PChatModule_EnterPrivateChannel*>(pNewChatChannel->FindChatModules(CMT_ENTER_CHANNEL));
	VALID(pModule != NULL);
	if (false == pModule->Run(uidChannel, nPlayerCID, uidGameServer))
	{
		gmgr.pChatChannelManager->DeleteChannel(pNewChatChannel);
	}
}

void PChattingLogic::RunChatModule(minet::MCommand* pCmd, CHATTING_MODULE_TYPE eType, CID cidRequestPlayer, const std::wstring& strChannelName)
{
	VALID(0 != strChannelName.size());
	VALID(pCmd);

	if (strChannelName.size() > MAX_CHATTING_CHANNEL_NAME_LEN)
	{
		gsys.pMsgCommandFacade->RouteSystemMsg(CR_FAIL_MSG_OVER_CHATTING_CHANNEL_NAME, cidRequestPlayer);
		return;
	}

	PChatChannel* pChannel = gmgr.pChatChannelManager->FindChannel(strChannelName);
	if (NULL == pChannel || pChannel->IsReserveDelete())
	{
		gsys.pMsgCommandFacade->RouteSystemMsg(CR_FAIL_MSG_NOT_EXIST_CHANNEL, cidRequestPlayer);
		return;
	}

	RunChatModule(pCmd, eType, cidRequestPlayer, pChannel->GetUID());
}

void PChattingLogic::RunChatModule(minet::MCommand* pCmd, CHATTING_MODULE_TYPE eType, CID cidRequestPlayer, const MUID& uidChannel)
{
	VALID(uidChannel.IsValid());
	VALID(pCmd);

	PChatChannel* pChannel = gmgr.pChatChannelManager->FindChannel(uidChannel);
	if (NULL == pChannel || pChannel->IsReserveDelete())
	{
		gsys.pMsgCommandFacade->RouteSystemMsg(CR_FAIL_MSG_NOT_EXIST_CHANNEL, cidRequestPlayer);
		return;
	}

	pChannel->RunChatModule(eType, pCmd);
}

void PChattingLogic::ChangedGameServerID(CID cidPlayer, int nNewGameServerID)
{
	gmgr.pChatChannelManager->ChangedGameServerID(cidPlayer, nNewGameServerID);
}

void PChattingLogic::EnterFieldChannel(CID cidPlayer, int nGameServerID, int nFieldID)
{	
	// 채널 찾기
	PChatChannel* pChannel = gmgr.pChatChannelManager->FindChannel_Field(nFieldID);
	if (pChannel == NULL)	return;

	// 입장
	RunEnterModule(pChannel, cidPlayer, nGameServerID);
}

void PChattingLogic::LeaveFieldChannel(CID cidPlayer, int nGameServerID, int nFieldID)
{
	// 채널 찾기
	PChatChannel* pChannel = gmgr.pChatChannelManager->FindChannel_Field(nFieldID);
	if (pChannel == NULL)	return;

	// 퇴장
	RunLeaveModule(pChannel, cidPlayer, nGameServerID);
}

void PChattingLogic::EnterAuctionChannel(CID cidPlayer, int nGameServerID)
{
	// 채널 찾기
	PChatChannel* pChannel = gmgr.pChatChannelManager->FindChannel_Auction();
	if (pChannel == NULL)	return;

	// 입장
	RunEnterModule(pChannel, cidPlayer, nGameServerID);
}

void PChattingLogic::LeaveAuctionChannel(CID cidPlayer, int nGameServerID)
{
	// 채널 찾기
	PChatChannel* pChannel = gmgr.pChatChannelManager->FindChannel_Auction();
	if (pChannel == NULL)	return;

	// 퇴장
	RunLeaveModule(pChannel, cidPlayer, nGameServerID);
}

void PChattingLogic::EnterPartyChannel(CID cidPlayer, int nGameServerID, MUID uidParty)
{
	// 채널 찾기
	PChatChannel* pChannel = gmgr.pChatChannelManager->FindChannel_Party(uidParty);

	// 없으면 생성
	if (pChannel == NULL)
	{
		PChatChannelFactory factoryChatChannel;
		pChannel = factoryChatChannel.NewChatChannel_Party(uidParty);
		VALID(pChannel != NULL);

		if (!gmgr.pChatChannelManager->AddChannel(pChannel))
		{
			SAFE_DELETE(pChannel);
			return;
		}
	}

	// 입장
	RunEnterModule(pChannel, cidPlayer, nGameServerID);
}

void PChattingLogic::LeavePartyChannel(CID cidPlayer, int nGameServerID, MUID uidParty)
{
	// 채널 찾기
	PChatChannel* pChannel = gmgr.pChatChannelManager->FindChannel_Party(uidParty);
	if (pChannel == NULL)	return;

	// 퇴장
	MUID uidChannel = pChannel->GetUID();
	MUID uidGameServer = MUID::Invalid();

	PGameServerObject* pGameServer = gmgr.pGameServerObjectManager->Find(nGameServerID);
	if (pGameServer != NULL)
	{
		uidGameServer = pGameServer->GetUID();
	}

	PChatModule_LeavePartyChannel* pModule = dynamic_cast<PChatModule_LeavePartyChannel*>(pChannel->FindChatModules(CMT_LEAVE_CHANNEL));
	VALID(pModule != NULL);
	pModule->Run(uidChannel, cidPlayer, uidGameServer);
}

void PChattingLogic::LeaveAllChannel(CID cidPlayer)
{
	gmgr.pChatChannelManager->LeaveAllChannel(cidPlayer);
}

void PChattingLogic::RunEnterModule(PChatChannel* pChannel, CID cidPlayer, int nGameServerID)
{
	VALID(pChannel != NULL);

	MUID uidChannel = pChannel->GetUID();
	MUID uidGameServer = MUID::Invalid();

	PGameServerObject* pGameServer = gmgr.pGameServerObjectManager->Find(nGameServerID);
	if (pGameServer != NULL)
	{
		uidGameServer = pGameServer->GetUID();
	}
	
	PChatModule_EnterChannel* pModule = dynamic_cast<PChatModule_EnterChannel*>(pChannel->FindChatModules(CMT_ENTER_CHANNEL));
	VALID(pModule != NULL);
	pModule->Run(uidChannel, cidPlayer, uidGameServer);
}

void PChattingLogic::RunLeaveModule(PChatChannel* pChannel, CID cidPlayer, int nGameServerID)
{
	VALID(pChannel != NULL);

	MUID uidChannel = pChannel->GetUID();
	MUID uidGameServer = MUID::Invalid();

	PGameServerObject* pGameServer = gmgr.pGameServerObjectManager->Find(nGameServerID);
	if (pGameServer != NULL)
	{
		uidGameServer = pGameServer->GetUID();
	}

	PChatModule_LeaveChannel* pModule = dynamic_cast<PChatModule_LeaveChannel*>(pChannel->FindChatModules(CMT_LEAVE_CHANNEL));
	VALID(pModule != NULL);
	pModule->Run(uidChannel, cidPlayer, uidGameServer);
}
