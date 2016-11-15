#include "StdAfx.h"
#include "GCmdHandler_Master_Guild.h"
#include "CCommandTable_Master.h"
#include "GGlobal.h"
#include "GGuildSystem.h"
#include "GGuildSystemForMasterServer.h"
#include "STransData_M2G.h"

GCmdHandler_Master_Guild::GCmdHandler_Master_Guild(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MMC_GUILD_MEMBER_ONLINEINFO,			OnMemberOnlineInfo);
	SetCmdHandler(MMC_GUILD_DESTROY_SYNC,				OnDestroySync);
	SetCmdHandler(MMC_GUILD_JOIN_SYNC,					OnJoinSync);
	SetCmdHandler(MMC_GUILD_LEAVE_SYNC,					OnLeaveSync);
	SetCmdHandler(MMC_GUILD_KICK_SYNC,					OnKickSync);
	SetCmdHandler(MMC_GUILD_ONLINE_SYNC,				OnOnLineSync);
	SetCmdHandler(MMC_GUILD_OFFLINE_SYNC,				OnOffLineSync);
	SetCmdHandler(MMC_GUILD_MOVEFIELD_SYNC,				OnMoveFieldSync);
	SetCmdHandler(MMC_GUILD_CHANGE_MASTER_SYNC,			OnChangeMasterSync);
	SetCmdHandler(MMC_GUILD_DEPOSIT_STORAGEMONEY_SYNC,	OnDepositStorageMoneySync);
	SetCmdHandler(MMC_GUILD_WITHDRAW_STORAGEMONEY_SYNC,	OnWithdrawStorageMoneySync);
	SetCmdHandler(MMC_GUILD_MOVE_STORAGEITEM_SYNC,		OnMoveStorageItemSync);
}

MCommandResult GCmdHandler_Master_Guild::OnMemberOnlineInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer;
	vector<TD_GUILD_ONLINE_MEMBER_INFO> vecOnlineMemberInfo;

	if (!pCommand->GetParameter(&uidPlayer,		0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetBlob(vecOnlineMemberInfo,	1))				return CR_ERROR;

	gsys.pGuildSystem->ForMasterServer().MemberOnlineInfo(uidPlayer, vecOnlineMemberInfo);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Guild::OnDestroySync(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nGID;
	if (!pCommand->GetParameter(&nGID,		0, MPT_INT))	return CR_ERROR;

	gsys.pGuildSystem->ForMasterServer().DestroySync(nGID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Guild::OnJoinSync(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCID;
	AID nAID;
	int nGID;
	wstring strName;
	int nLevel;
	int nFieldID;
	int nChannelID;

	if (!pCommand->GetParameter(&nCID,		0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nAID,		1, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nGID,		2, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(strName,	3, MPT_WSTR))	return CR_ERROR;
	if (!pCommand->GetParameter(&nLevel,	4, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nFieldID,	5, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nChannelID,6, MPT_INT))	return CR_ERROR;

	gsys.pGuildSystem->ForMasterServer().JoinSync(nCID, nAID, nGID, strName, nLevel, nFieldID, nChannelID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Guild::OnLeaveSync(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCID;
	int nGID;
	
	if (!pCommand->GetParameter(&nCID,		0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nGID,		1, MPT_INT))	return CR_ERROR;

	gsys.pGuildSystem->ForMasterServer().LeaveSync(nCID, nGID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Guild::OnKickSync(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCID;
	int nGID;

	if (!pCommand->GetParameter(&nCID,		0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nGID,		1, MPT_INT))	return CR_ERROR;

	gsys.pGuildSystem->ForMasterServer().KickSync(nCID, nGID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Guild::OnOnLineSync(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCID;
	int nGID;
	int nFieldID;
	int nChannelID;

	if (!pCommand->GetParameter(&nCID,		0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nGID,		1, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nFieldID,	2, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nChannelID,3, MPT_INT))	return CR_ERROR;

	gsys.pGuildSystem->ForMasterServer().OnLineSync(nCID, nGID,  nFieldID, nChannelID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Guild::OnOffLineSync(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCID;
	int nGID;

	if (!pCommand->GetParameter(&nCID,		0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nGID,		1, MPT_INT))	return CR_ERROR;

	gsys.pGuildSystem->ForMasterServer().OffLineSync(nCID, nGID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Guild::OnMoveFieldSync(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCID;
	int nGID;
	int nFieldID;
	int nChannelID;

	if (!pCommand->GetParameter(&nCID,		0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nGID,		1, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nFieldID,	2, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nChannelID,3, MPT_INT))	return CR_ERROR;

	gsys.pGuildSystem->ForMasterServer().MoveFieldSync(nCID, nGID,  nFieldID, nChannelID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Guild::OnChangeMasterSync(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nOldCID;
	int nNewCID;	
	int nGID;

	if (!pCommand->GetParameter(&nOldCID,	0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nNewCID,	1, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nGID,		2, MPT_INT))	return CR_ERROR;

	gsys.pGuildSystem->ForMasterServer().ChangeMasterSync(nOldCID, nNewCID, nGID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Guild::OnDepositStorageMoneySync(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCID;
	int nGID;
	int nDepositMoney;
	int nStorageMoney;

	if (!pCommand->GetParameter(&nCID,			0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nGID,			1, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nDepositMoney,	2, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nStorageMoney,	3, MPT_INT))	return CR_ERROR;

	gsys.pGuildSystem->ForMasterServer().DepositStorageMoneySync(nCID, nGID, nDepositMoney, nStorageMoney);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Guild::OnWithdrawStorageMoneySync(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCID;
	int nGID;
	int nWithdrawMoney;
	int nStorageMoney;

	if (!pCommand->GetParameter(&nCID,			0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nGID,			1, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nWithdrawMoney,2, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nStorageMoney,	3, MPT_INT))	return CR_ERROR;

	gsys.pGuildSystem->ForMasterServer().WithdrawStorageMoneySync(nCID, nGID, nWithdrawMoney, nStorageMoney);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Master_Guild::OnMoveStorageItemSync(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCID;
	int nGID;
	vector<TD_PLAYER_GAME_DATA_ITEM_INSTANCE> vecTDItem;

	if (!pCommand->GetParameter(&nCID,			0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nGID,			1, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetBlob(vecTDItem,			2))				return CR_ERROR;

	gsys.pGuildSystem->ForMasterServer().MoveStorageItemSync(nCID, nGID, vecTDItem);

	return CR_TRUE;
}