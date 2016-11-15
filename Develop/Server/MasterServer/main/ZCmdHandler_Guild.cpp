#include "StdAfx.h"
#include "ZCmdHandler_Guild.h"
#include "CCommandTable_Master.h"
#include "ZGuildLogic.h"
#include "ZPlayerLogic.h"
#include "STransData_M2G.h"


ZCmdHandler_Guild::ZCmdHandler_Guild(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MMC_GUILD_MEMBER_ONLINEINFO_REQ,	OnMemberOnlineInfoReq);
	SetCmdHandler(MMC_GUILD_DESTROY,				OnDestroy);
	SetCmdHandler(MMC_GUILD_JOIN,					OnJoin);
	SetCmdHandler(MMC_GUILD_LEAVE,					OnLeave);
	SetCmdHandler(MMC_GUILD_KICK,					OnKick);
	SetCmdHandler(MMC_GUILD_ONLINE,					OnOnLine);
	SetCmdHandler(MMC_GUILD_OFFLINE,				OnOffLine);
	SetCmdHandler(MMC_GUILD_MOVEFIELD,				OnMoveField);
	SetCmdHandler(MMC_GUILD_CHANGE_MASTER,			OnChaneMaster);
	SetCmdHandler(MMC_GUILD_DEPOSIT_STORAGEMONEY,	OnDepositStorageMoney);
	SetCmdHandler(MMC_GUILD_WITHDRAW_STORAGEMONEY,	OnWithdrawStorageMoney);
	SetCmdHandler(MMC_GUILD_MOVE_STORAGEITEM,		OnMoveStorageItem);
}

MCommandResult ZCmdHandler_Guild::OnMemberOnlineInfoReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer;
	vector<int> vecMemberCID;
	if (!pCommand->GetParameter(&uidPlayer, 0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetBlob(vecMemberCID, 1))				return CR_ERROR;

	gsys.pGuildLogic->MemberOnlineInfo(pCommand->GetSenderUID(), uidPlayer, vecMemberCID);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Guild::OnDestroy(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nGID;
	if (!pCommand->GetParameter(&nGID, 0, MPT_INT))	return CR_ERROR;

	gsys.pGuildLogic->Destroy(nGID);
	
	return CR_TRUE;
}

MCommandResult ZCmdHandler_Guild::OnJoin(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCID;
	AID nAID;
	int nGID;
	wstring strName;
	int nLevel;
	int nFieldID;
	int nChanneldID;

	if (!pCommand->GetParameter(&nCID, 0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nAID, 1, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nGID, 2, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(strName, 3, MPT_WSTR))	return CR_ERROR;
	if (!pCommand->GetParameter(&nLevel, 4, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nFieldID, 5, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nChanneldID, 6, MPT_INT))	return CR_ERROR;

	gsys.pGuildLogic->Join(nCID, nAID, nGID, strName, nLevel,  nFieldID, nChanneldID);

	ZPlayerLogic playerLogic;
	playerLogic.JoinGuild(nCID, nGID);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Guild::OnLeave(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCID;
	int nGID;

	if (!pCommand->GetParameter(&nCID, 0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nGID, 1, MPT_INT))	return CR_ERROR;

	gsys.pGuildLogic->Leave(nCID, nGID);

	ZPlayerLogic playerLogic;
	playerLogic.LeaveGuild(nCID);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Guild::OnKick(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCID;
	int nGID;

	if (!pCommand->GetParameter(&nCID, 0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nGID, 1, MPT_INT))	return CR_ERROR;

	gsys.pGuildLogic->Kick(nCID, nGID);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Guild::OnOnLine(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCID;
	int nGID;
	int nFieldID;
	int nChannelID;

	if (!pCommand->GetParameter(&nCID, 0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nGID, 1, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nFieldID, 2, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nChannelID, 3, MPT_INT))	return CR_ERROR;

	gsys.pGuildLogic->OnLine(nCID, nGID, nFieldID, nChannelID);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Guild::OnOffLine(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCID;
	int nGID;

	if (!pCommand->GetParameter(&nCID, 0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nGID, 1, MPT_INT))	return CR_ERROR;

	gsys.pGuildLogic->OffLine(nCID, nGID);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Guild::OnMoveField(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCID;
	int nGID;
	int nFieldID;
	int nChannelID;

	if (!pCommand->GetParameter(&nCID, 0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nGID, 1, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nFieldID, 2, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nChannelID, 3, MPT_INT))	return CR_ERROR;

	gsys.pGuildLogic->MoveField(nCID, nGID, nFieldID, nChannelID);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Guild::OnChaneMaster(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nOldCID;
	int nNewCID;
	int nGID;

	if (!pCommand->GetParameter(&nOldCID, 0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nNewCID, 1, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nGID, 2, MPT_INT))	return CR_ERROR;

	gsys.pGuildLogic->ChangeMaster(nOldCID, nNewCID, nGID);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Guild::OnDepositStorageMoney(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCID;
	int nGID;
	int nDepoistMoney;
	int nStorageMoney;

	if (!pCommand->GetParameter(&nCID, 0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nGID, 1, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nDepoistMoney, 2, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nStorageMoney, 3, MPT_INT))	return CR_ERROR;

	gsys.pGuildLogic->DepositStorageMoney(pCommand->GetSenderUID(), nCID, nGID, nDepoistMoney, nStorageMoney);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Guild::OnWithdrawStorageMoney(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCID;
	int nGID;
	int nWithdrawMoney;
	int nStorageMoney;

	if (!pCommand->GetParameter(&nCID, 0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nGID, 1, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nWithdrawMoney, 2, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nStorageMoney, 3, MPT_INT))	return CR_ERROR;

	gsys.pGuildLogic->WithdrawStorageMoney(pCommand->GetSenderUID(), nCID, nGID, nWithdrawMoney, nStorageMoney);

	return CR_TRUE;
}

MCommandResult ZCmdHandler_Guild::OnMoveStorageItem(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCID;
	int nGID;
	vector<TD_PLAYER_GAME_DATA_ITEM_INSTANCE> vecTDItem;

	if (!pCommand->GetParameter(&nCID,			0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nGID,			1, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetBlob(vecTDItem,			2))	return CR_ERROR;

	gsys.pGuildLogic->MoveStorageItem(pCommand->GetSenderUID(), nCID, nGID, vecTDItem);

	return CR_TRUE;
}