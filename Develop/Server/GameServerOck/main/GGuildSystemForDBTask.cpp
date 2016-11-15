#include "stdafx.h"
#include "GGuildSystemForDBTask.h"
#include "GGuildMgr.h"
#include "GGuild.h"
#include "GEntityPlayer.h"
#include "GGuildMember.h"
#include "CTransData.h"
#include "CCommandTable.h"
#include "GGuildSystem.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GDBManager.h"
#include "GServer.h"
#include "GSTLUtil.h"
#include "GCommand.h"
#include "GSharedField.h"
#include "GPlayerPVPArea.h"
#include "GPlayerBattleArena.h"
#include "GServerPlayerRouter.h"
#include "GMasterServerFacade.h"
#include "CCommandResultTable.h"
#include "GDBTaskGuildSerialize.h"
#include "GServerGuildRouter.h"
#include "GDBTaskDataGuild.h"
#include "GMoneySystem.h"


void GGuildSystemForDBTask::GuildSerialize( GDBTaskGuildSerializeData& Data )
{
	if (NULL != gmgr.pGuildMgr->Get((int)Data.nGID)) return;	
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(Data.uidPlayer);
	if (NULL == pPlayer) return;

	GGuild* pGuild = gmgr.pGuildMgr->Insert((int)Data.nGID, Data.strGuildName.c_str(), Data.nStorageMoney);
	if (NULL == pGuild)
		return;

	for (size_t i = 0; i < Data.vecGuildMember.size(); ++i)
	{
		pGuild->InsertMember(Data.vecGuildMember[i].nAID
			, (int)Data.vecGuildMember[i].nCID
			, Data.vecGuildMember[i].strCharName.c_str()
			, Data.vecGuildMember[i].nLevel
			, (GUILD_MEMBER_GRADE)Data.vecGuildMember[i].nGuildGrade);
	}

	Data.qItemInst.SetToServer(pPlayer);
	gsys.pMasterServerFacade->GetGuildRouter()->GuilInfo(Data.uidPlayer, (int)Data.nGID);
}

void GGuildSystemForDBTask::InsertGuild(GDBT_GUILD& data)
{
	GGuild* pNewGuild = gmgr.pGuildMgr->Insert((int)data.m_nGID, data.m_strName.c_str());
	if (NULL == pNewGuild) return;
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(data.m_uidPlayer);
	if (NULL == pPlayer) return;

	gsys.pMoneySystem->DecreaseForDBTask(pPlayer, data.m_nDeltaMoney);

	pNewGuild->InsertMember(pPlayer->GetAID()
		, pPlayer->GetCID()
		, pPlayer->GetPlayerInfo()->szName
		, pPlayer->GetPlayerInfo()->nLevel
		, GMG_MASTER);	

	MCommand* pNewCmd = MakeNewCommand(MC_GUILD_CREATE,
		2,
		NEW_USHORT(pPlayer->GetUIID()),
		NEW_WSTR(data.m_strName.c_str()));
	pPlayer->RouteToThisCell(pNewCmd);

	gsys.pGuildSystem->RouteJoinSector(pPlayer, data.m_strName.c_str());
}

void GGuildSystemForDBTask::DeleteGuild( const int nGID )
{
	gsys.pMasterServerFacade->GetGuildRouter()->Destroy(nGID);	
}

void GGuildSystemForDBTask::Join( const MUID& uidJoinMember, const int nGID )
{
	GEntityPlayer* pJoinMember = gmgr.pPlayerObjectManager->GetEntity(uidJoinMember);
	if (NULL == pJoinMember) return;

	gsys.pMasterServerFacade->GetGuildRouter()->Join(pJoinMember->GetCID(), (AID)pJoinMember->GetAID(), nGID, pJoinMember->GetName(), pJoinMember->GetLevel(), pJoinMember->GetFieldID(), GetChannelID(pJoinMember->GetField()));
}

void GGuildSystemForDBTask::Leave( const MUID& uidMember, const int nGID )
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidMember);
	if (NULL == pPlayer) return;

	gsys.pMasterServerFacade->GetGuildRouter()->Leave(pPlayer->GetCID(), nGID);
}

void GGuildSystemForDBTask::Kick(int nCID, const int nGID )
{
	gsys.pMasterServerFacade->GetGuildRouter()->Kick(nCID, nGID);
}


void GGuildSystemForDBTask::ChangeMaster(const int nGID, const MUID& uidCurMaster, int nNewMasterCID)
{
	GEntityPlayer* pCurMaster = gmgr.pPlayerObjectManager->GetEntity(uidCurMaster);
	if (NULL == pCurMaster)	return;

	gsys.pMasterServerFacade->GetGuildRouter()->ChangeMaster(pCurMaster->GetCID(), nNewMasterCID, nGID);
}

void GGuildSystemForDBTask::DepositStorageMoney( GDBT_GUILD_UPDATE_STORAGEMONEY& data )
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(data.m_uidPlayer);
	if (NULL == pPlayer) return;
	GGuild* pGuild = gmgr.pGuildMgr->Get((int)data.m_nGID);
	if (NULL == pGuild) return;

	int nDepositMoney = data.m_nStorageMoney - pGuild->GetStorageMoney();

	pGuild->SetStorageMoney(data.m_nStorageMoney);
	pPlayer->GetPlayerInfo()->SetMoney(data.m_nMoney);	

	MCommand* pNewCmd = MakeNewCommand(MC_GUILD_DEPOSIT_STORAGEMONEY,
		3,
		NEW_WSTR(pPlayer->GetName()), 
		NEW_INT(nDepositMoney),
		NEW_INT(data.m_nStorageMoney));

	gsys.pGuildSystem->RouteToStorageInteractionMember(pGuild, pNewCmd);
	gsys.pMasterServerFacade->GetGuildRouter()->DepositStorageMoney(pPlayer->GetCID(), pGuild->GetID(), nDepositMoney, data.m_nStorageMoney);
}

void GGuildSystemForDBTask::WithdrawStorageMoney( GDBT_GUILD_UPDATE_STORAGEMONEY& data )
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(data.m_uidPlayer);
	if (NULL == pPlayer) return;
	GGuild* pGuild = gmgr.pGuildMgr->Get((int)data.m_nGID);
	if (NULL == pGuild) return;

	int nWithdrawMoney = pGuild->GetStorageMoney() - data.m_nStorageMoney;

	pGuild->SetStorageMoney(data.m_nStorageMoney);
	pPlayer->GetPlayerInfo()->SetMoney(data.m_nMoney);	

	MCommand* pNewCmd = MakeNewCommand(MC_GUILD_WITHDRAW_STORAGEMONEY,
		3,
		NEW_WSTR(pPlayer->GetName()), 
		NEW_INT(nWithdrawMoney),
		NEW_INT(data.m_nStorageMoney));

	gsys.pGuildSystem->RouteToStorageInteractionMember(pGuild, pNewCmd);
	gsys.pMasterServerFacade->GetGuildRouter()->WithdrawStorageMoney(pPlayer->GetCID(), pGuild->GetID(), nWithdrawMoney, data.m_nStorageMoney);
}
