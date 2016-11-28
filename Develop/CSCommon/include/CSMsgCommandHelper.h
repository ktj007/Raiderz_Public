#pragma once

#include "MCommand.h"
#include "MTstring.h"
#include "CSChrInfo.h"

enum CCommandResultTable;


class CSMsgCommandHelper
{
public:
	minet::MCommand* MakeNewClientCommandSayReq(tstring strMsg);
	minet::MCommand* MakeNewClientCommandSayRes(tstring strMsg, tstring strSenderName, GPlayerGrade nPlayerGrade);	
	minet::MCommand* MakeNewClientCommandNarrationReq(tstring strMsg);
	minet::MCommand* MakeNewClientCommandNarrationRes(tstring strMsg);
	minet::MCommand* MakeNewClientCommandShoutReq(tstring strMsg);
	minet::MCommand* MakeNewClientCommandShoutRes(tstring strMsg, tstring strSenderName);
	minet::MCommand* MakeNewClientCommandShoutFakeRes(tstring strMsg, tstring strSenderName);
	minet::MCommand* MakeNewClientCommandAuctionReq(tstring strMsg);
	minet::MCommand* MakeNewClientCommandAuctionRes(tstring strMsg, tstring strSenderName);
	minet::MCommand* MakeNewClientCommandAnnounceReq(tstring strMsg);
	minet::MCommand* MakeNewClientCommandAnnounceRes(tstring strMsg, tstring strSenderName);
	minet::MCommand* MakeNewClientCommandWhisperReq(tstring strMsg, tstring strReceiverName);
	minet::MCommand* MakeNewClientCommandWhisperRes(tstring strMsg, tstring strSenderName);
	minet::MCommand* MakeNewClientCommandWhisperBackRes(tstring strMsg, tstring strReceiverName);
	minet::MCommand* MakeNewClientCommandPartyReq(tstring strMsg);
	minet::MCommand* MakeNewClientCommandPartyRes(tstring strMsg, tstring strSenderName);
	minet::MCommand* MakeNewClientCommandGuildReq(tstring strMsg);
	minet::MCommand* MakeNewClientCommandGuildRes(tstring strMsg, tstring strSenderName);
	minet::MCommand* MakeNewClientCommandChannelReq(tstring strMsg, MUID uidChannel);
	minet::MCommand* MakeNewClientCommandChannelRes(tstring strMsg, tstring strSenderName, MUID uidChannel);
	minet::MCommand* MakeNewClientCommandGlobalReq(tstring strMsg);
	minet::MCommand* MakeNewClientCommandGlobalRes(tstring strMsg, tstring strSenderName);
	minet::MCommand* MakeNewClientCommandFieldReq(tstring strMsg);
	minet::MCommand* MakeNewClientCommandFieldRes(tstring strMsg, tstring strSenderName);	
	minet::MCommand* MakeNewClientCommandSystemRes(CCommandResultTable nResult);
	minet::MCommand* MakeNewClientCommandAdviceRes(CCommandResultTable nResult);
	minet::MCommand* MakeNewClientCommandNoticeReq(tstring strMsg);	
	minet::MCommand* MakeNewClientCommandNoticeRes(tstring strMsg);
	minet::MCommand* MakeNewClientCommandNoviceReq(tstring strMsg);
	minet::MCommand* MakeNewClientCommandNoviceRes(tstring strMsg, tstring strSenderName);
	minet::MCommand* MakeNewClientCommandLFPReq(tstring strMsg);
	minet::MCommand* MakeNewClientCommandLFPRes(tstring strMsg, tstring strSenderName);

	minet::MCommand* MakeNewServerCommandAnnounceReq(tstring strMsg, CID nSenderCID);
	minet::MCommand* MakeNewServerCommandAnnounceRes(tstring strMsg, tstring strSenderName);
	minet::MCommand* MakeNewServerCommandWhisperReq(tstring strMsg, CID nSenderCID, tstring strReceiverName);
	minet::MCommand* MakeNewServerCommandWhisperRes(tstring strMsg, tstring strSenderName, CID nReceiverCID);
	minet::MCommand* MakeNewServerCommandWhisperBackRes(tstring strMsg, CID nSenderCID, tstring strReceiverName);
	minet::MCommand* MakeNewServerCommandPartyReq(tstring strMsg, CID nSenderCID, MUID uidParty);
	minet::MCommand* MakeNewServerCommandPartyRes(tstring strMsg, tstring strSenderName, MUID uidParty);
	minet::MCommand* MakeNewServerCommandGuildReq(tstring strMsg, CID nSenderCID, int nGuildID);
	minet::MCommand* MakeNewServerCommandGuildRes(tstring strMsg, tstring strSenderName, int nGuildID);
	minet::MCommand* MakeNewServerCommandChannelReq(tstring strMsg, CID nSenderCID, MUID uidChannel);
	minet::MCommand* MakeNewServerCommandChannelRes(tstring strMsg, tstring strSenderName, MUID uidChannel);
	minet::MCommand* MakeNewServerCommandGlobalReq(tstring strMsg, CID nSenderCID);
	minet::MCommand* MakeNewServerCommandGlobalRes(tstring strMsg, tstring strSenderName);
	minet::MCommand* MakeNewServerCommandAuctionReq(tstring strMsg, CID nSenderCID);
	minet::MCommand* MakeNewServerCommandAuctionRes(tstring strMsg, tstring strSenderName);
	minet::MCommand* MakeNewServerCommandFieldReq(tstring strMsg, CID nSenderCID, int nFieldID);
	minet::MCommand* MakeNewServerCommandFieldRes(tstring strMsg, tstring strSenderName, int nFieldID);	
	minet::MCommand* MakeNewServerCommandSystemRes(CCommandResultTable nResult, CID nReceiverCID);
	minet::MCommand* MakeNewServerCommandNoticeReq(tstring strMsg);
	minet::MCommand* MakeNewServerCommandNoticeRes(tstring strMsg);
	minet::MCommand* MakeNewServerCommandAdviceRes(CCommandResultTable nResult, CID nReceiverCID);
	minet::MCommand* MakeNewServerCommandNoviceReq(tstring strMsg, CID nSenderCID);
	minet::MCommand* MakeNewServerCommandNoviceRes(tstring strMsg, tstring strSenderName);
	minet::MCommand* MakeNewServerCommandLFPReq(tstring strMsg, CID nSenderCID);
	minet::MCommand* MakeNewServerCommandLFPRes(tstring strMsg, tstring strSenderName);

	bool IsSenderEmpty(const minet::MCommand* pCmd);
	bool IsReceiverEmpty(const minet::MCommand* pCmd);
	tstring GetSenderName(const minet::MCommand* pCmd);
	tstring GetReceiverName(const minet::MCommand* pCmd);	
	int64 GetSenderID(const minet::MCommand* pCmd);
	int64 GetReceiverID(const minet::MCommand* pCmd);
	MUID GetSenderUID(const minet::MCommand* pCmd);
	MUID GetReceiverUID(const minet::MCommand* pCmd);
	

private:
	template <typename S, typename R>	
	minet::MCommand* MakeNewCommand(int nID, int nType, tstring strMsg, S sender, R receiver, int nAdditionalData);
	
	template <typename T>
	void AddParameter(minet::MCommand* pCmd, T blob);
	
	template <>
	void AddParameter(minet::MCommand* pCmd, string strBlob);

	template <>
	void AddParameter(minet::MCommand* pCmd, wstring strBlob);
};
