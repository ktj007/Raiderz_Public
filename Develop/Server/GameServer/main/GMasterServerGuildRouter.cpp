#include "StdAfx.h"
#include "GMasterServerGuildRouter.h"
#include "GGuild.h"
#include "GGlobal.h"
#include "GGuildMgr.h"
#include "GMasterServerFacade.h"
#include "CCommandTable_Master.h"
#include "CTransData.h"

void GMasterServerGuildRouter::GuilInfo( MUID uidPlayer, int nGID )
{
	GGuild* pGuild = gmgr.pGuildMgr->Get(nGID);
	if (NULL == pGuild) return;

	vector<CID> vecMemberCID = pGuild->CollectMemberCID();

	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_GUILD_MEMBER_ONLINEINFO_REQ,
		2,
		NEW_UID(uidPlayer),
		NEW_BLOB(vecMemberCID)
		);

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerGuildRouter::Destroy(int nGID)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_GUILD_DESTROY,
		1,
		NEW_INT(nGID));

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerGuildRouter::Join(CID nCID, AID nAID, int nGID, const wstring& strName, int nLevel, int nFieldID, int nChannelID)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_GUILD_JOIN,
		7,
		NEW_INT64(nCID),
		NEW_INT64(nAID),
		NEW_INT(nGID),
		NEW_WSTR(strName.c_str()),
		NEW_INT(nLevel),
		NEW_INT(nFieldID),
		NEW_INT(nChannelID));

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerGuildRouter::Leave( CID nCID, int nGID )
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_GUILD_LEAVE,
		2,
		NEW_INT64(nCID),
		NEW_INT(nGID));

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerGuildRouter::Kick( CID nCID, int nGID )
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_GUILD_KICK,
		2,
		NEW_INT64(nCID),
		NEW_INT(nGID));

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerGuildRouter::OnLine( CID nCID, int nGID, int nFieldID, int nChannelID )
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_GUILD_ONLINE,
		4,
		NEW_INT64(nCID),
		NEW_INT(nGID),
		NEW_INT(nFieldID),
		NEW_INT(nChannelID));

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerGuildRouter::OffLine( CID nCID, int nGID )
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_GUILD_OFFLINE,
		2,
		NEW_INT64(nCID),
		NEW_INT(nGID));

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerGuildRouter::MoveField( CID nCID, int nGID, int nFieldID, int nChannelID )
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_GUILD_MOVEFIELD,
		4,
		NEW_INT64(nCID),
		NEW_INT(nGID),
		NEW_INT(nFieldID),
		NEW_INT(nChannelID));

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerGuildRouter::ChangeMaster( CID nOldCID, CID nNewCID, int nGID )
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_GUILD_CHANGE_MASTER,
		3,
		NEW_INT64(nOldCID),
		NEW_INT64(nNewCID),
		NEW_INT(nGID));

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerGuildRouter::DepositStorageMoney(CID nCID, int nGID, int nDepositMoney, int nStorageMoney)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_GUILD_DEPOSIT_STORAGEMONEY,
		4,
		NEW_INT64(nCID),
		NEW_INT(nGID),
		NEW_INT(nDepositMoney),
		NEW_INT(nStorageMoney));

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerGuildRouter::WithdrawStorageMoney(CID nCID, int nGID, int nWithdrawMoney, int nStorageMoney)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_GUILD_WITHDRAW_STORAGEMONEY,
		4,
		NEW_INT64(nCID),
		NEW_INT(nGID),
		NEW_INT(nWithdrawMoney),
		NEW_INT(nStorageMoney));

	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GMasterServerGuildRouter::MoveStorageItem(CID nCID, int nGID, vector<TD_PLAYER_GAME_DATA_ITEM_INSTANCE>& vecTDItem)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_GUILD_MOVE_STORAGEITEM,
		3,
		NEW_INT64(nCID),
		NEW_INT(nGID),
		NEW_BLOB(vecTDItem));		

	gsys.pMasterServerFacade->Route(pNewCmd);
}
