#pragma once

#include "PDef_Chatting.h"
#include "STypes.h"

class PChatChannel;

class PChattingLogic
{
public:
	bool	CreateDefaultChannels(void);
	void	CreateChannel_Private(int nPlayerCID, const std::wstring& strChannelName);

	void	RunChatModule(minet::MCommand* pCmd, CHATTING_MODULE_TYPE eType, CID cidRequestPlayer, const std::wstring& strChannelName);
	void	RunChatModule(minet::MCommand* pCmd, CHATTING_MODULE_TYPE eType, CID cidRequestPlayer, const MUID& uidChannel);

	void	ChangedGameServerID(CID cidPlayer, int nNewGameServerID);

	void	EnterFieldChannel(CID cidPlayer, int nGameServerID, int nFieldID);
	void	LeaveFieldChannel(CID cidPlayer, int nGameServerID, int nFieldID);
	void	EnterAuctionChannel(CID cidPlayer, int nGameServerID);
	void	LeaveAuctionChannel(CID cidPlayer, int nGameServerID);
	void	EnterPartyChannel(CID cidPlayer, int nGameServerID, MUID uidParty);
	void	LeavePartyChannel(CID cidPlayer, int nGameServerID, MUID uidParty);

	void	LeaveAllChannel(CID cidPlayer);

	void	RunEnterModule(PChatChannel* pChannel, CID cidPlayer, int nGameServerID);
	void	RunLeaveModule(PChatChannel* pChannel, CID cidPlayer, int nGameServerID);
};
