#include "StdAfx.h"
#include "GGuildSystem.h"
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
#include "GGuildSystemForDBTask.h"
#include "GGuildSystemForMasterServer.h"
#include "GDBTaskDataGuild.h"
#include "GDB_CODE.h"
#include "GServerGuildRouter.h"
#include "GConst.h"
#include "CSNameStringChecker.h"
#include "GItemHolder.h"
#include "GPlayerInteraction.h"


GGuildSystem::GGuildSystem()
{
	m_pForDBTask = new GGuildSystemForDBTask;
	m_pMasterServer = new GGuildSystemForMasterServer;
}

GGuildSystem::~GGuildSystem()
{
	SAFE_DELETE(m_pForDBTask);
	SAFE_DELETE(m_pMasterServer);
}

void GGuildSystem::Serialize( GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return;
	if (0 == pPlayer->GetGID()) return;

	if (false == gmgr.pGuildMgr->IsExist(pPlayer->GetGID()))
	{
		gsys.pDBManager->GuildSerialize(pPlayer->GetUID(), pPlayer->GetGID());
	}
	else
	{
		gsys.pGuildSystem->RouteGuildInfo(pPlayer->GetUID(), pPlayer->GetGID());
	}
}

void GGuildSystem::RouteToMember(GGuild* pGuild, MCommand* pCmd, int nExceptMemberCID)
{
	if (NULL == pGuild) return;
	if (NULL == pCmd) return;

	MAP_GUILD_MEMBER& mapGuildMember = pGuild->GetContainer();

	for each (const MAP_GUILD_MEMBER::value_type& data in mapGuildMember)
	{
		int nCID = data.first;
		GGuildMember* pMember = data.second;

		if (nExceptMemberCID == nCID) continue;

		GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pMember->GetCID());
		if (NULL == pPlayer) continue;

		pPlayer->RouteToMe(pCmd->Clone());
	}

	SAFE_DELETE(pCmd);
}

void GGuildSystem::RouteToStorageInteractionMember(GGuild* pGuild, MCommand* pCmd, int nExceptMemberCID)
{
	if (NULL == pGuild) return;
	if (NULL == pCmd) return;

	MAP_GUILD_MEMBER& mapGuildMember = pGuild->GetContainer();

	for each (const MAP_GUILD_MEMBER::value_type& data in mapGuildMember)
	{
		int nCID = data.first;
		GGuildMember* pMember = data.second;

		if (nExceptMemberCID == nCID) continue;

		GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pMember->GetCID());
		if (NULL == pPlayer) continue;

		if (pPlayer->GetInteraction().IsGuildStorageInteraction())
		{
			pPlayer->RouteToMe(pCmd->Clone());
		}		
	}

	SAFE_DELETE(pCmd);
}

bool GGuildSystem::Create(GEntityPlayer* pPlayer, const wchar_t* szGuildName)
{
	if (NULL == pPlayer) return false;
	if (NULL == szGuildName) return false;

	int nCID = pPlayer->GetCID();
	int nGID = pPlayer->GetGID();

	if (true == gmgr.pGuildMgr->IsExist(nGID)) return pPlayer->FailAndRouteAdviceMsg(CR_FAIL_GUILD_ALREADY_JOIN);
	if (true == gmgr.pGuildMgr->IsExist(szGuildName)) return pPlayer->FailAndRouteAdviceMsg(CR_FAIL_GUILD_EXIST_NAME);
	if (pPlayer->GetMoney() < GConst::GUILD_CREATE_COST) return pPlayer->FailAndRouteAdviceMsg(CR_FAIL_GUILD_CREATE_NOT_ENOUGH_MONEY);
	CSNameStringChecker nameChecker(szGuildName, CSNameStringChecker::TYPE_GUILD);
	CSNameStringChecker::ERROR_CODE nErrorCode = nameChecker.Check();
	if (CSNameStringChecker::OK != nErrorCode)
	{
		return pPlayer->FailAndRouteAdviceMsg(static_cast<CCommandResultTable>(nErrorCode));
	}

	GDBT_GUILD data(pPlayer->GetAID()
		, pPlayer->GetUID()
		, pPlayer->GetPlayerInfo()->nCID
		, pPlayer->GetPlayerInfo()->nPlayTimeSec
		, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
		, pPlayer->GetMoney() - GConst::GUILD_CREATE_COST
		, GConst::GUILD_CREATE_COST
		, pPlayer->GetLevel()
		, GDB_CODE::CD_L_GUILD_INSERT
		, 0
		, szGuildName);

	if (!gsys.pDBManager->GuildInsert(data))
		return false;

	return true;
}

bool GGuildSystem::Invite_Req(GEntityPlayer* pInvitor, GEntityPlayer* pGuest)
{
	if (NULL == pInvitor) return false;
	if (NULL == pGuest) return false;

	// TODO: 정책 결정되면 거리 등 체크
	int nGID = pInvitor->GetPlayerInfo()->nGID;
	GGuild* pGuild = gmgr.pGuildMgr->Get(nGID);
	if (NULL == pGuild) return pInvitor->FailAndRouteAdviceMsg(CR_FAIL_GUILD_NO_JOIN);
	if (false == pGuild->IsGuildMaster(pInvitor->GetCID())) return pInvitor->FailAndRouteAdviceMsg(CR_FAIL_GUILD_NOT_MASTER);
	if (true == gmgr.pGuildMgr->IsExist(pGuest->GetGID())) return pInvitor->FailAndRouteAdviceMsg(CR_FAIL_GUILD_ALREADY_JOIN);
	if (MAX_GUILD_MEMBER_SIZE <= pGuild->GetMemberCount()) return pInvitor->FailAndRouteAdviceMsg(CR_FAIL_GUILD_MAX_MEMBER_COUNT);
	if (true == pGuest->HasDuel())								return pInvitor->FailAndRouteAdviceMsg(CR_FAIL_GUILD_OPPONENT_IS_DUEL);
	if (true == pGuest->GetPlayerPVPArea().IsLocatedPvPArea())	return pInvitor->FailAndRouteAdviceMsg(CR_FAIL_GUILD_OPPONENT_IN_PVPAREA);
	if (true == pGuest->GetPlayerBattleArena().IsEntered())		return pInvitor->FailAndRouteAdviceMsg(CR_FAIL_GUILD_OPPONENT_IN_BATTLEARENA);

	gmgr.pGuildMgr->Invite(pGuest->GetCID(), pInvitor->GetCID());

	MCommand* pNewCmd = MakeNewCommand(MC_GUILD_INVITE_NOTIFY, 
		2,
		NEW_UID(pInvitor->GetUID()),
		NEW_WSTR(pGuild->GetName()));

	pGuest->RouteToMe(pNewCmd);

	return true;
}

bool GGuildSystem::Invite_Res(GEntityPlayer* pGuest, bool bAccept)
{
	if (NULL == pGuest) return false;

	// TODO: 정책 결정되면 거리 등 체크
	CID nInvitorCID = gmgr.pGuildMgr->GetInvitorCID(pGuest->GetCID());
	GEntityPlayer* pInvitor = gmgr.pPlayerObjectManager->GetEntity(nInvitorCID);
	if (NULL == pInvitor) return false;
	GGuild* pGuild = gmgr.pGuildMgr->Get(pInvitor->GetGID());
	if (NULL == pGuild) return pGuest->FailAndRouteAdviceMsg(CR_FAIL_GUILD_NOT_INVITED_GUEST);
	GField* pField = pGuest->GetField();
	if (NULL == pField) return false;
	int nFieldID = pField->GetID();
	
	if (true == gmgr.pGuildMgr->IsExist(pGuest->GetGID())) return pGuest->FailAndRouteAdviceMsg(CR_FAIL_GUILD_ALREADY_JOIN);
	if (MAX_GUILD_MEMBER_SIZE <= pGuild->GetMemberCount()) return pGuest->FailAndRouteAdviceMsg(CR_FAIL_GUILD_MAX_MEMBER_COUNT);
	if (true == pGuest->HasDuel())								return pGuest->FailAndRouteAdviceMsg(CR_FAIL_GUILD_SELF_IS_DUEL);
	if (true == pGuest->GetPlayerPVPArea().IsLocatedPvPArea())	return pGuest->FailAndRouteAdviceMsg(CR_FAIL_GUILD_SELF_IN_PVPAREA);
	if (true == pGuest->GetPlayerBattleArena().IsEntered())		return pGuest->FailAndRouteAdviceMsg(CR_FAIL_GUILD_SELF_IN_BATTLEARENA);

	if (false == bAccept)
	{
		RouteReject(pInvitor, pGuest->GetUID());

		return true;
	}

		GDBT_GUILD data(pGuest->GetAID()
		, pGuest->GetUID()
		, pGuest->GetPlayerInfo()->nCID
		, 0
		, 0
		, 0
		, 0
		, 0
		, GDB_CODE::CD_L_GUILD_MEMBER_INSERT
		, pGuild->GetID());
	
	return gsys.pDBManager->GuildMemberInsert(data);	
}

bool GGuildSystem::Leave(GEntityPlayer* pPlayer)
{
	if (NULL == pPlayer) return false;

	GGuild* pGuild = gmgr.pGuildMgr->Get(pPlayer->GetGID());
	if (NULL == pGuild) return pPlayer->FailAndRouteAdviceMsg(CR_FAIL_GUILD_NO_JOIN);	
	if (true == pGuild->IsGuildMaster(pPlayer->GetCID())) return pPlayer->FailAndRouteAdviceMsg(CR_FAIL_GUILD_MASTER_CANNOT_LEAVE);

	
	GDBT_GUILD data(pPlayer->GetAID()
		, pPlayer->GetUID()
		, pPlayer->GetPlayerInfo()->nCID
		, 0
		, 0
		, 0
		, 0
		, 0
		, GDB_CODE::CD_L_GUILD_MEMBER_DELETE
		, pGuild->GetID());

	return gsys.pDBManager->GuildDeleteMember(data);
}

bool GGuildSystem::Kick(GEntityPlayer* pPlayer, const wchar_t* szTargetName)
{
	if (NULL == pPlayer) return false;
	if (NULL == szTargetName) return false;
	GGuild* pGuild = gmgr.pGuildMgr->Get(pPlayer->GetGID());
	if (NULL == pGuild) return pPlayer->FailAndRouteAdviceMsg(CR_FAIL_GUILD_NO_JOIN);
	GGuildMember* pTargetMember = pGuild->GetMember(szTargetName);
	if (NULL == pTargetMember) return false;

	if (false == pGuild->IsGuildMaster(pPlayer->GetCID())) return pPlayer->FailAndRouteAdviceMsg(CR_FAIL_GUILD_NOT_MASTER);
	if (true == pGuild->IsGuildMaster(pTargetMember->GetCID())) return pPlayer->FailAndRouteAdviceMsg(CR_FAIL_GUILD_MASTER_CANNOT_LEAVE);		

	GDBT_GUILD data(pTargetMember->GetAID()
		, pPlayer->GetUID()
		, pTargetMember->GetCID()
		, 0
		, 0
		, 0
		, 0
		, 0
		, GDB_CODE::CD_L_GUILD_MEMBER_KICK_OFF
		, pGuild->GetID());

	gsys.pDBManager->GuildDeleteMember(data);	

	return true;
}

bool GGuildSystem::Destroy(GEntityPlayer* pPlayer)
{
	if (NULL == pPlayer) return false;

	GGuild* pGuild = gmgr.pGuildMgr->Get(pPlayer->GetGID());
	if (NULL == pGuild) return pPlayer->FailAndRouteAdviceMsg(CR_FAIL_GUILD_NO_JOIN);	
	if (false == pGuild->IsGuildMaster(pPlayer->GetCID())) return pPlayer->FailAndRouteAdviceMsg(CR_FAIL_GUILD_NOT_MASTER);
	if (1 < pGuild->GetMemberCount()) return pPlayer->FailAndRouteAdviceMsg(CR_FAIL_GUILD_NEED_EMPTY_MEMBER); 

	GPlayerObject* pPlayerObj = gmgr.pPlayerObjectManager->GetPlayer(pPlayer->GetUID());
	if (NULL == pPlayerObj)
		return false;

	GDBT_GUILD data(pPlayer->GetAID()
		, pPlayer->GetUID()
		, pPlayer->GetPlayerInfo()->nCID
		, 0
		, 0
		, 0
		, 0
		, 0
		, GDB_CODE::CD_L_GUILD_DELETE
		, pGuild->GetID()
		, pGuild->GetName());

	return gsys.pDBManager->GuildDelete(data);	
}

void GGuildSystem::OnLine(GEntityPlayer* pPlayer)
{
	if (NULL == pPlayer) return;
	GGuild* pGuild = gmgr.pGuildMgr->Get(pPlayer->GetGID());
	if (NULL == pGuild) return;	

	int nFieldID = INVALID_ID;
	int nChanneldID= INVALID_ID;
	if (NULL != pPlayer->GetField())
	{
		nFieldID = pPlayer->GetFieldID();
		nChanneldID = GetChannelID(pPlayer->GetField());
	}

	gsys.pMasterServerFacade->GetGuildRouter()->OnLine(pPlayer->GetCID(), pGuild->GetID(), nFieldID, nChanneldID);
}

void GGuildSystem::OffLine(GEntityPlayer* pPlayer)
{
	if (NULL == pPlayer) return;
	GGuild* pGuild = gmgr.pGuildMgr->Get(pPlayer->GetGID());
	if (NULL == pGuild) return;

	gsys.pMasterServerFacade->GetGuildRouter()->OffLine(pPlayer->GetCID(), pGuild->GetID());
}

void GGuildSystem::MoveField(GEntityPlayer* pPlayer)
{
	if (NULL == pPlayer) return;
	GGuild* pGuild = gmgr.pGuildMgr->Get(pPlayer->GetGID());
	if (NULL == pGuild) return;
	GField* pField = pPlayer->GetField();
	if (NULL == pField) return;

	gsys.pMasterServerFacade->GetGuildRouter()->MoveField(pPlayer->GetCID(), pGuild->GetID(), pField->GetID(), GetChannelID(pField));
}

bool GGuildSystem::ChangeMaster(GEntityPlayer* pOldMaster, const wchar_t* szNewMasterName)
{
	if (NULL == pOldMaster) return false;
	if (NULL == szNewMasterName) return false;

	GGuild* pGuild = gmgr.pGuildMgr->Get(pOldMaster->GetGID());
	if (NULL == pGuild) return pOldMaster->FailAndRouteAdviceMsg(CR_FAIL_GUILD_NO_JOIN);
	if (false == pGuild->IsGuildMaster(pOldMaster->GetCID())) return pOldMaster->FailAndRouteAdviceMsg(CR_FAIL_GUILD_NOT_MASTER);
	GGuildMember* pNewMasterMember = pGuild->GetMember(szNewMasterName);
	if (NULL == pNewMasterMember) return false;
	if (pOldMaster->GetCID() == pNewMasterMember->GetCID()) return false;

	GDBT_GUILD_CHANGE_MASTER data(pOldMaster->GetUID(), pOldMaster->GetCID());
	data.SetNewMaster(pNewMasterMember->GetAID()
		, pNewMasterMember->GetCID()
		, GDB_CODE::CD_L_GUILD_MASTER_CHANGE
		, pGuild->GetID());

	return gsys.pDBManager->GuildChangeMaster(data);
}

void GGuildSystem::MakeTD_GUILDAndTD_GUILD_MEMBER(int nGID, vector<TD_GUILD>& outvecTDGuild, vector<TD_GUILD_MEMBER>& outvecTDGuildMember )
{
	// 길드 가입을 하지 않음
	if (0 == nGID) return;

	GGuild* pGuild = gmgr.pGuildMgr->Get(nGID);
	// DB로부터 정보를 아직 가지고 오지 않음
	if (NULL == pGuild)
	{
		return;		
	}

	TD_GUILD tdGuild;
	wcsncpy_s(tdGuild.szName, pGuild->GetName(), _TRUNCATE);
	wcsncpy_s(tdGuild.szMasterName, pGuild->GetMasterName(), _TRUNCATE);
	outvecTDGuild.push_back(tdGuild);

	MAP_GUILD_MEMBER mapMember = pGuild->GetContainer();
	for each (const MAP_GUILD_MEMBER::value_type& data in mapMember)
	{
		int nCID = data.first;
		GGuildMember* pMember = data.second;

		TD_GUILD_MEMBER tdGuildMember;		
		wcsncpy_s(tdGuildMember.szName, pMember->GetName(), _TRUNCATE);
		tdGuildMember.nGrade = pMember->GetGrade();
		tdGuildMember.nLevel = pMember->GetLevel();
		tdGuildMember.bOnline = pMember->IsOnline();
		tdGuildMember.nFieldID = pMember->GetFieldID();
		tdGuildMember.nChannelID = pMember->GetChannelID();

		outvecTDGuildMember.push_back(tdGuildMember);		
	}
}

void GGuildSystem::RouteGuildInfo( const MUID& uidPlayer, const int nGID )
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
	if (NULL == pPlayer)
		return;

	vector<TD_GUILD> vecGuild;
	vector<TD_GUILD_MEMBER> vecGMember;

	gsys.pGuildSystem->MakeTD_GUILDAndTD_GUILD_MEMBER(nGID, vecGuild, vecGMember);

	MCommand* pCmd = MakeNewCommand(MC_GUILD_INFO,
		2,
		NEW_BLOB(vecGuild),
		NEW_BLOB(vecGMember));

	pPlayer->RouteToMe(pCmd);
}

void GGuildSystem::RouteInsertGuildFailed( const MUID& uidCreator, const int nErrCode )
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidCreator);
	if (NULL == pPlayer)
		return;
	if (GUILD_ERR_DUPLICATED_NAME == nErrCode)
	{
		pPlayer->FailAndRouteAdviceMsg(CR_FAIL_GUILD_NOT_VALID_NAME);	
	}	
}

GGuildSystemForDBTask& GGuildSystem::ForDBTask()
{
	return *m_pForDBTask;
}

GGuildSystemForMasterServer& GGuildSystem::ForMasterServer()
{
	return *m_pMasterServer;
}

void GGuildSystem::RouteReject( GEntityPlayer* pPlayer, const MUID& uidRejector )
{
	if (NULL == pPlayer) return;

	MCommand* pNewCmd = MakeNewCommand(MC_GUILD_INVITE_REJECT,
		1,
		NEW_UID(uidRejector));	

	pPlayer->RouteToMe(pNewCmd);
}

void GGuildSystem::RouteJoinSector( GEntityPlayer* pPlayer, const wstring& strGuildName )
{
	if (NULL == pPlayer) return;

	MCommand* pNewCmd = MakeNewCommand(MC_GUILD_JOIN_SECTOR,
		2,
		NEW_USHORT(pPlayer->GetUIID()),
		NEW_WSTR(strGuildName.c_str()));	

	pPlayer->RouteToThisCell(pNewCmd, pPlayer->GetUID());
}

void GGuildSystem::RouteLeaveSector( GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return;

	MCommand* pNewCmd = MakeNewCommand(MC_GUILD_LEAVE_SECTOR,
		1,
		NEW_USHORT(pPlayer->GetUIID()));	

	pPlayer->RouteToThisCell(pNewCmd, pPlayer->GetUID());
}

bool GGuildSystem::RouteShowCreateUI( GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return false;
	if (true == gmgr.pGuildMgr->IsExist(pPlayer->GetGID())) return pPlayer->FailAndRouteAdviceMsg(CR_FAIL_GUILD_ALREADY_JOIN);

	MCommand* pNewCmd = MakeNewCommand(MC_GUILD_SHOW_CREATE_UI, 0, NULL);	

	pPlayer->RouteToMe(pNewCmd);

	return true;
}

bool GGuildSystem::RouteShowDestroyUI( GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return false;
	if (false == gmgr.pGuildMgr->IsExist(pPlayer->GetGID())) return pPlayer->FailAndRouteAdviceMsg(CR_FAIL_GUILD_NO_JOIN);

	MCommand* pNewCmd = MakeNewCommand(MC_GUILD_SHOW_DESTROY_UI, 0, NULL);	

	pPlayer->RouteToMe(pNewCmd);

	return true;
}

bool GGuildSystem::ShowStorage( GEntityPlayer* pPlayer )
{
	if (NULL == pPlayer) return false;

	GGuild* pGuild = gmgr.pGuildMgr->Get(pPlayer->GetGID());
	if (NULL == pGuild) return false;

	vector<TD_ITEM> vecTDItem;
	pPlayer->GetItemHolder()->MakeTD(SLOTTYPE_GUILDSTORAGE, vecTDItem);

	MCommand* pNewCmd = MakeNewCommand(MC_GUILD_SHOW_STORAGE_UI, 2, NEW_INT(pGuild->GetStorageMoney()), NEW_BLOB(vecTDItem));
	pPlayer->RouteToMe(pNewCmd);

	return true;
}

bool GGuildSystem::DepositStorageMoney( GEntityPlayer* pPlayer, int nMoney )
{
	if (!pPlayer) return false;
	GGuild* pGuild = gmgr.pGuildMgr->Get(pPlayer->GetGID());
	if (NULL == pGuild) return false;

	if (pPlayer->GetMoney() < nMoney) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_NOT_ENOUGH_MONEY);
	if (MAX_MONEY < pGuild->GetStorageMoney() + nMoney) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_OVER_DEPOSIT_MONEY);

	if (!gsys.pDBManager->GuildUpdateStorageMoney(GDBT_GUILD_UPDATE_STORAGEMONEY(GDBT_GUILD_UPDATE_STORAGEMONEY::DEPOSIT, pPlayer->GetAID(), pPlayer->GetUID(), pPlayer->GetCID(), pPlayer->GetMoney()-nMoney, pGuild->GetID(), pGuild->GetStorageMoney()+nMoney, nMoney)))
		return false;

	return true;
}

bool GGuildSystem::WithdrawStorageMoney( GEntityPlayer* pPlayer, int nMoney )
{
	if (!pPlayer) return false;
	GGuild* pGuild = gmgr.pGuildMgr->Get(pPlayer->GetGID());
	if (NULL == pGuild) return false;

	if (pGuild->GetStorageMoney() < nMoney) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_NOT_ENOUGH_STORAGEMONEY);
	if (MAX_MONEY < pPlayer->GetMoney() + nMoney) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_OVER_WITHDRAW_MONEY);

	if (!gsys.pDBManager->GuildUpdateStorageMoney(GDBT_GUILD_UPDATE_STORAGEMONEY(GDBT_GUILD_UPDATE_STORAGEMONEY::WITHDRAW, pPlayer->GetAID(), pPlayer->GetUID(), pPlayer->GetCID(), pPlayer->GetMoney()+nMoney, pGuild->GetID(), pGuild->GetStorageMoney()-nMoney, nMoney)))
		return false;

	return true;
}

void GGuildSystem::RouteSyncFail(MUID uidPlayer)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
	if (NULL == pPlayer) return;

	pPlayer->RouteSystemMsg(CR_FAIL_GUILD_MEMBER_MODIFY_STORAGE);
}