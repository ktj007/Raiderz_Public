#include "stdafx.h"
#include "CSMsgCommandHelper.h"
#include "CCommandTable.h"
#include "CCommandTable_App.h"
#include "CSDef.h"
#include "CCommandResultTable.h"
#include "MLocale.h"

using namespace minet;

MCommand* CSMsgCommandHelper::MakeNewClientCommandSayReq(tstring strMsg)
{
	return MakeNewCommand(MC_MSG_REQ, MT_SAY, strMsg, NULL, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandSayRes(tstring strMsg, tstring strSenderName, GPlayerGrade nPlayerGrade)
{
	return MakeNewCommand(MC_MSG_RES, MT_SAY, strMsg, strSenderName, NULL, nPlayerGrade);
}

minet::MCommand* CSMsgCommandHelper::MakeNewClientCommandNarrationReq(tstring strMsg)
{
	return MakeNewCommand(MC_MSG_REQ, MT_NARRATION, strMsg, NULL, NULL, 0);
}

minet::MCommand* CSMsgCommandHelper::MakeNewClientCommandNarrationRes(tstring strMsg)
{
	return MakeNewCommand(MC_MSG_RES, MT_NARRATION, strMsg, NULL, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandShoutReq(tstring strMsg)
{
	return MakeNewCommand(MC_MSG_REQ, MT_SHOUT, strMsg, NULL, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandShoutRes(tstring strMsg, tstring strSenderName)
{
	return MakeNewCommand(MC_MSG_RES, MT_SHOUT, strMsg, strSenderName, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandShoutFakeRes(tstring strMsg, tstring strSenderName)
{
	return MakeNewCommand(MC_MSG_RES, MT_SHOUT_FAKE, strMsg, strSenderName, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandAuctionReq(tstring strMsg)
{
	return MakeNewCommand(MC_MSG_REQ, MT_AUCTION, strMsg, NULL, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandAuctionRes(tstring strMsg, tstring strSenderName)
{
	return MakeNewCommand(MC_MSG_RES, MT_AUCTION, strMsg, strSenderName, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandAnnounceReq(tstring strMsg)
{
	return MakeNewCommand(MC_MSG_REQ, MT_ANNOUNCE, strMsg, NULL, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandAnnounceRes(tstring strMsg, tstring strSenderName)
{
	return MakeNewCommand(MC_MSG_RES, MT_ANNOUNCE, strMsg, strSenderName, NULL, 0);
}
	
MCommand* CSMsgCommandHelper::MakeNewClientCommandWhisperReq(tstring strMsg, tstring strReceiverName)
{
	return MakeNewCommand(MC_MSG_REQ, MT_WHISPER, strMsg, NULL, strReceiverName, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandWhisperRes(tstring strMsg, tstring strSenderName)
{
	return MakeNewCommand(MC_MSG_RES, MT_WHISPER, strMsg, strSenderName, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandWhisperBackRes(tstring strMsg, tstring strReceiverName)
{
	return MakeNewCommand(MC_MSG_RES, MT_WHISPER_BACK, strMsg, NULL, strReceiverName, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandPartyReq(tstring strMsg)
{
	return MakeNewCommand(MC_MSG_REQ, MT_PARTY, strMsg, NULL, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandPartyRes(tstring strMsg, tstring strSenderName)
{
	return MakeNewCommand(MC_MSG_RES, MT_PARTY, strMsg, strSenderName, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandGuildReq(tstring strMsg)
{
	return MakeNewCommand(MC_MSG_REQ, MT_GUILD, strMsg, NULL, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandGuildRes(tstring strMsg, tstring strSenderName)
{
	return MakeNewCommand(MC_MSG_RES, MT_GUILD, strMsg, strSenderName, NULL, 0); 
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandChannelReq(tstring strMsg, MUID uidChannel)
{
	return MakeNewCommand(MC_MSG_REQ, MT_CHANNEL, strMsg, NULL, uidChannel, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandChannelRes(tstring strMsg, tstring strSenderName, MUID uidChannel)
{
	return MakeNewCommand(MC_MSG_RES, MT_CHANNEL, strMsg, strSenderName, uidChannel, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandGlobalReq(tstring strMsg)
{
	return MakeNewCommand(MC_MSG_REQ, MT_GLOBAL, strMsg, NULL, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandGlobalRes(tstring strMsg, tstring strSenderName)
{
	return MakeNewCommand(MC_MSG_RES, MT_GLOBAL, strMsg, strSenderName, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandFieldReq(tstring strMsg)
{
	return MakeNewCommand(MC_MSG_REQ, MT_FIELD, strMsg, NULL, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandFieldRes(tstring strMsg, tstring strSenderName)
{
	return MakeNewCommand(MC_MSG_RES, MT_FIELD, strMsg, strSenderName, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandSystemRes(CCommandResultTable nResult)
{
	return MakeNewCommand(MC_MSG_RES, MT_SYSTEM, tstring(), NULL, NULL, nResult);
}

minet::MCommand* CSMsgCommandHelper::MakeNewClientCommandAdviceRes( CCommandResultTable nResult )
{
	return MakeNewCommand(MC_MSG_RES, MT_ADVICE, tstring(), NULL, NULL, nResult);
}

minet::MCommand* CSMsgCommandHelper::MakeNewClientCommandNoticeReq(tstring strMsg)
{
	return MakeNewCommand(MC_MSG_REQ, MT_NOTICE, strMsg, NULL, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandNoticeRes(tstring strMsg)
{
	return MakeNewCommand(MC_MSG_RES, MT_NOTICE, strMsg, NULL, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandNoviceReq(tstring strMsg)
{
	return MakeNewCommand(MC_MSG_REQ, MT_NOVICE, strMsg, NULL, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandNoviceRes(tstring strMsg, tstring strSenderName)
{
	return MakeNewCommand(MC_MSG_RES, MT_NOVICE, strMsg, strSenderName, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandLFPReq(tstring strMsg)
{
	return MakeNewCommand(MC_MSG_REQ, MT_LFP, strMsg, NULL, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewClientCommandLFPRes(tstring strMsg, tstring strSenderName)
{
	return MakeNewCommand(MC_MSG_RES, MT_LFP, strMsg, strSenderName, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandAnnounceReq(tstring strMsg, CID nSenderCID)
{
	return MakeNewCommand(MPC_MSG_REQ, MT_ANNOUNCE, strMsg, nSenderCID, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandAnnounceRes(tstring strMsg, tstring strSenderName)
{
	return MakeNewCommand(MPC_MSG_RES, MT_ANNOUNCE, strMsg, strSenderName, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandWhisperReq(tstring strMsg, CID nSenderCID, tstring strReceiverName)
{
	return MakeNewCommand(MPC_MSG_REQ, MT_WHISPER, strMsg, nSenderCID, strReceiverName, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandWhisperRes(tstring strMsg, tstring strSenderName, CID nReceiverCID)
{
	return MakeNewCommand(MPC_MSG_RES, MT_WHISPER, strMsg, strSenderName, nReceiverCID, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandWhisperBackRes(tstring strMsg, CID nSenderCID, tstring strReceiverName)
{
	return MakeNewCommand(MPC_MSG_RES, MT_WHISPER_BACK, strMsg, nSenderCID, strReceiverName, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandPartyReq(tstring strMsg, CID nSenderCID, MUID uidParty)
{
	return MakeNewCommand(MPC_MSG_REQ, MT_PARTY, strMsg, nSenderCID, uidParty, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandPartyRes(tstring strMsg, tstring strSenderName, MUID uidParty)
{
	return MakeNewCommand(MPC_MSG_RES, MT_PARTY, strMsg, strSenderName, uidParty, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandGuildReq(tstring strMsg, CID nSenderCID, int nGuildID)
{
	return MakeNewCommand(MPC_MSG_REQ, MT_GUILD, strMsg, nSenderCID, nGuildID, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandGuildRes(tstring strMsg, tstring strSenderName, int nGuildID)
{
	return MakeNewCommand(MPC_MSG_RES, MT_GUILD, strMsg, strSenderName, nGuildID, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandChannelReq(tstring strMsg, CID nSenderCID, MUID uidChannel)
{
	return MakeNewCommand(MPC_MSG_REQ, MT_CHANNEL, strMsg, nSenderCID, uidChannel, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandChannelRes(tstring strMsg, tstring strSenderName, MUID uidChannel)
{
	return MakeNewCommand(MPC_MSG_RES, MT_CHANNEL, strMsg, strSenderName, uidChannel, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandGlobalReq(tstring strMsg, CID nSenderCID)
{
	return MakeNewCommand(MPC_MSG_REQ, MT_GLOBAL, strMsg, nSenderCID, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandGlobalRes(tstring strMsg, tstring strSenderName)
{
	return MakeNewCommand(MPC_MSG_RES, MT_GLOBAL, strMsg, strSenderName, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandAuctionReq(tstring strMsg, CID nSenderCID)
{
	return MakeNewCommand(MPC_MSG_REQ, MT_AUCTION, strMsg, nSenderCID, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandAuctionRes(tstring strMsg, tstring strSenderName)
{
	return MakeNewCommand(MPC_MSG_RES, MT_AUCTION, strMsg, strSenderName, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandFieldReq(tstring strMsg, CID nSenderCID, int nFieldID)
{
	return MakeNewCommand(MPC_MSG_REQ, MT_FIELD, strMsg, nSenderCID, nFieldID, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandFieldRes(tstring strMsg, tstring strSenderName, int nFieldID)
{
	return MakeNewCommand(MPC_MSG_RES, MT_FIELD, strMsg, strSenderName, nFieldID, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandSystemRes(CCommandResultTable nResult, CID nReceiverCID)
{
	return MakeNewCommand(MPC_MSG_RES, MT_SYSTEM, tstring(), NULL, nReceiverCID, nResult);
}

minet::MCommand* CSMsgCommandHelper::MakeNewServerCommandNoticeReq(tstring strMsg)
{
	return MakeNewCommand(MPC_MSG_REQ, MT_NOTICE, strMsg, NULL, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandNoticeRes(tstring strMsg)
{
	return MakeNewCommand(MPC_MSG_RES, MT_NOTICE, strMsg, NULL, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandAdviceRes(CCommandResultTable nResult, CID nReceiverCID)
{
	return MakeNewCommand(MPC_MSG_RES, MT_ADVICE, tstring(), NULL, nReceiverCID, nResult);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandNoviceReq(tstring strMsg, CID nSenderCID)
{
	return MakeNewCommand(MPC_MSG_REQ, MT_NOVICE, strMsg, nSenderCID, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandNoviceRes(tstring strMsg, tstring strSenderName)
{
	return MakeNewCommand(MPC_MSG_RES, MT_NOVICE, strMsg, strSenderName, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandLFPReq(tstring strMsg, CID nSenderCID)
{
	return MakeNewCommand(MPC_MSG_REQ, MT_LFP, strMsg, nSenderCID, NULL, 0);
}

MCommand* CSMsgCommandHelper::MakeNewServerCommandLFPRes(tstring strMsg, tstring strSenderName)
{
	return MakeNewCommand(MPC_MSG_RES, MT_LFP, strMsg, strSenderName, NULL, 0);
}

bool CSMsgCommandHelper::IsSenderEmpty(const MCommand* pCmd)
{
	MCommandParameter* pParameter = pCmd->GetParameter(2);
	if (pParameter == NULL)	return true;

	return 0 == pParameter->GetBlobCount();
}

bool CSMsgCommandHelper::IsReceiverEmpty(const MCommand* pCmd)
{
	MCommandParameter* pParameter = pCmd->GetParameter(3);
	if (pParameter == NULL)	return true;

	return 0 == pParameter->GetBlobCount();
}

tstring CSMsgCommandHelper::GetSenderName(const MCommand* pCmd)
{
	wchar_t* pName;
	int nCount;
	if (!pCmd->GetBlob(pName, nCount, 2))	return tstring();
	if (nCount != 1)						return tstring();
	if (wcsnlen(pName, PLAYER_NAME_LEN + 1) > PLAYER_NAME_LEN) return tstring();
	
#ifdef _UNICODE
	return pName;	
#else
	return MLocale::ConvUTF16ToAnsi(pName);
#endif
}

tstring CSMsgCommandHelper::GetReceiverName(const MCommand* pCmd)
{
	wchar_t* pName;
	int nCount;
	if (!pCmd->GetBlob(pName, nCount, 3))	return tstring();
	if (nCount != 1)						return tstring();
	if (wcsnlen(pName, PLAYER_NAME_LEN + 1) > PLAYER_NAME_LEN) return tstring();

#ifdef _UNICODE
	return pName;	
#else
	return MLocale::ConvUTF16ToAnsi(pName);
#endif
}

int64 CSMsgCommandHelper::GetSenderID(const minet::MCommand* pCmd)
{
	int64* pID;
	int nCount;
	if (!pCmd->GetBlob(pID, nCount, 2))	return INVALID_ID;
	if (nCount != 1)						return INVALID_ID;

	return *pID;
}

int64 CSMsgCommandHelper::GetReceiverID(const minet::MCommand* pCmd)
{
	int64* pID;
	int nCount;
	if (!pCmd->GetBlob(pID, nCount, 3))	return INVALID_ID;
	if (nCount != 1)						return INVALID_ID;

	return *pID;
}

MUID CSMsgCommandHelper::GetSenderUID(const minet::MCommand* pCmd)
{
	MUID* pUID;
	int nCount;
	if (!pCmd->GetBlob(pUID, nCount, 2))		return MUID::Invalid();
	if (nCount != 1)							return MUID::Invalid();

	return *pUID;
}

MUID CSMsgCommandHelper::GetReceiverUID(const minet::MCommand* pCmd)
{
	MUID* pUID;
	int nCount;
	if (!pCmd->GetBlob(pUID, nCount, 3))		return MUID::Invalid();
	if (nCount != 1)							return MUID::Invalid();

	return *pUID;
}

template <typename S, typename R>
MCommand* CSMsgCommandHelper::MakeNewCommand(int nID, int nType, tstring strMsg, S sender, R receiver, int nAdditionalData)
{
	MCommand* pCmd = new MCommand();

	pCmd->SetID(nID);
	pCmd->AddParameter(NEW_INT(nType));

#ifdef _UNICODE
	pCmd->AddParameter(NEW_WSTR(strMsg.substr(0, CHATTING_LIMIT_SIZE).c_str()));
#else
	pCmd->AddParameter(NEW_WSTR(MLocale::ConvAnsiToUCS2(strMsg.substr(0, CHATTING_LIMIT_SIZE).c_str()).c_str()));
#endif

	AddParameter(pCmd, sender);
	AddParameter(pCmd, receiver);
	pCmd->AddParameter(NEW_INT(nAdditionalData));

	return pCmd;
}

template <typename T>
void CSMsgCommandHelper::AddParameter(MCommand* pCmd, T blob)
{
	if (blob == 0)	// Invalid
	{
		pCmd->AddParameter(NEW_BLOB());
	}
	else
	{
		pCmd->AddParameter(NEW_BLOB(&blob, sizeof(blob), 1));
	}
}

template <>
void CSMsgCommandHelper::AddParameter(MCommand* pCmd, string strBlob)
{
	wstring strWideBlob = MLocale::ConvAnsiToUCS2(strBlob.c_str());
	AddParameter(pCmd, strWideBlob);
}

template <>
void CSMsgCommandHelper::AddParameter(MCommand* pCmd, wstring strBlob)
{
	size_t len = (strBlob.length() + 1) * sizeof(wchar_t);
	pCmd->AddParameter(NEW_BLOB(strBlob.c_str(), static_cast<unsigned short>(len), 1));
}
