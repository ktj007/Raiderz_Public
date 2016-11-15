#include "StdAfx.h"
#include "GGuildSystemForMasterServer.h"
#include "GPlayerObjectManager.h"
#include "GGlobal.h"
#include "GEntityPlayer.h"
#include "GGuild.h"
#include "GGuildMgr.h"
#include "GGuildSystem.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "SProxyPlayerManager.h"
#include "GMasterServerFacade.h"
#include "SProxyPlayer.h"
#include "GCommandCenter.h"
#include "GGuildStorage.h"
#include "GItemManager.h"


void GGuildSystemForMasterServer::MemberOnlineInfo( MUID uidPlayer, const vector<TD_GUILD_ONLINE_MEMBER_INFO>& vecOnlineMemberInfo )
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer); 
	if (NULL == pPlayer) return;

	GGuild* pGuild = gmgr.pGuildMgr->Get(pPlayer->GetGID());
	if (NULL == pGuild) return;

	for each (TD_GUILD_ONLINE_MEMBER_INFO onlineInfo in vecOnlineMemberInfo)
	{
		GGuildMember* pOnlineMember = pGuild->GetMember(onlineInfo.nCID);
		if (NULL == pOnlineMember) continue;

		pOnlineMember->OnLine(onlineInfo.nFieldID, onlineInfo.nChannelID);
	}

	gsys.pGuildSystem->RouteGuildInfo(uidPlayer, pPlayer->GetGID());
}

void GGuildSystemForMasterServer::DestroySync( int nGID )
{
	GGuild* pGuild = gmgr.pGuildMgr->Get(nGID);
	if (NULL == pGuild) return;

	MCommand* pCmd = MakeNewCommand(MC_GUILD_DESTROY, 0, NULL);

	vector<int> vecMemberCID = pGuild->CollectMemberCID();
	for each (int nMemberCID in vecMemberCID)
	{
		GEntityPlayer* pMember = gmgr.pPlayerObjectManager->GetEntity(nMemberCID);
		if (NULL == pMember) continue;

		pMember->RouteToMe(pCmd->Clone());
		gsys.pGuildSystem->RouteLeaveSector(pMember);
	}
	SAFE_DELETE(pCmd);	

	gmgr.pGuildMgr->Delete(nGID);
}

void GGuildSystemForMasterServer::JoinSync(int nCID, AID nAID, int nGID, const wstring& strName, int nLevel, int nFieldID, int nChannelID)
{
	GGuild* pGuild = gmgr.pGuildMgr->Get(nGID);
	if (NULL == pGuild)	return;

	pGuild->InsertMember(nAID
		, nCID
		, strName.c_str()
		, nLevel
		, GMG_NORMAL);

	GEntityPlayer* pJoinMember = gmgr.pPlayerObjectManager->GetEntity(nCID);
	if (NULL != pJoinMember)
	{
		vector<TD_GUILD> vecTDGuild;
		vector<TD_GUILD_MEMBER> vecTDGuildMember;
		gsys.pGuildSystem->MakeTD_GUILDAndTD_GUILD_MEMBER(nGID, vecTDGuild, vecTDGuildMember);

		MCommand* pNewCmd = MakeNewCommand(MC_GUILD_JOIN_ME, 
			2,
			NEW_BLOB(vecTDGuild),
			NEW_BLOB(vecTDGuildMember));

		pJoinMember->RouteToMe(pNewCmd);

		gsys.pGuildSystem->RouteJoinSector(pJoinMember, pGuild->GetName());
	}	

	MCommand* pNewCmd = MakeNewCommand(MC_GUILD_JOIN_OTHER,
		4,
		NEW_WSTR(strName.c_str()),
		NEW_INT(nLevel),		
		NEW_INT(nFieldID),
		NEW_INT(nChannelID));	

	gsys.pGuildSystem->RouteToMember(pGuild, pNewCmd, nCID);
}

void GGuildSystemForMasterServer::LeaveSync( int nCID, int nGID )
{
	GGuild* pGuild = gmgr.pGuildMgr->Get(nGID);
	if (NULL == pGuild) return;
	GGuildMember* pLeaveMember = pGuild->GetMember(nCID);
	if (NULL == pLeaveMember) return;

	MCommand* pNewCmd = MakeNewCommand(MC_GUILD_LEAVE,
		1,
		NEW_WSTR(pLeaveMember->GetName()));
	gsys.pGuildSystem->RouteToMember(pGuild, pNewCmd);

	GEntityPlayer* pLeavePlayer = gmgr.pPlayerObjectManager->GetEntity(nCID);
	if (NULL != pLeavePlayer)
	{
		gsys.pGuildSystem->RouteLeaveSector(pLeavePlayer);
	}
	
	pGuild->DeleteMember(nCID);
}

void GGuildSystemForMasterServer::KickSync( int nCID, int nGID )
{
	GGuild* pGuild = gmgr.pGuildMgr->Get(nGID);
	if (NULL == pGuild) return;
	GGuildMember* pLeaveMember = pGuild->GetMember(nCID);
	if (NULL == pLeaveMember) return;

	MCommand* pNewCmd = MakeNewCommand(MC_GUILD_KICK,
		1,
		NEW_WSTR(pLeaveMember->GetName()));
	gsys.pGuildSystem->RouteToMember(pGuild, pNewCmd);

	GEntityPlayer* pLeavePlayer = gmgr.pPlayerObjectManager->GetEntity(nCID);
	if (NULL != pLeavePlayer)
	{
		gsys.pGuildSystem->RouteLeaveSector(pLeavePlayer);
	}

	pGuild->DeleteMember(nCID);
}

void GGuildSystemForMasterServer::OnLineSync( int nCID, int nGID, int nFieldID, int nChannelID)
{
	GGuild* pGuild = gmgr.pGuildMgr->Get(nGID);
	if (NULL == pGuild) return;
	GGuildMember* pGuildMember = pGuild->GetMember(nCID);
	if (NULL == pGuildMember) return;

	pGuildMember->OnLine(nFieldID, nChannelID);

	MCommand* pNewCmd = MakeNewCommand(MC_GUILD_ONLINE, 
		3,
		NEW_WSTR(pGuildMember->GetName()),
		NEW_INT(nFieldID),
		NEW_INT(nChannelID));

	gsys.pGuildSystem->RouteToMember(pGuild, pNewCmd);
}

void GGuildSystemForMasterServer::OffLineSync( int nCID, int nGID )
{
	GGuild* pGuild = gmgr.pGuildMgr->Get(nGID);
	if (NULL == pGuild) return;
	GGuildMember* pGuildMember = pGuild->GetMember(nCID);
	if (NULL == pGuildMember) return;

	pGuildMember->OffLine();

	MCommand* pNewCmd = MakeNewCommand(MC_GUILD_OFFLINE, 
		1, 
		NEW_WSTR(pGuildMember->GetName()));

	gsys.pGuildSystem->RouteToMember(pGuild, pNewCmd);
}

void GGuildSystemForMasterServer::MoveFieldSync( int nCID, int nGID, int nFieldID, int nChannelID )
{
	GGuild* pGuild = gmgr.pGuildMgr->Get(nGID);
	if (NULL == pGuild) return;
	GGuildMember* pGuildMember = pGuild->GetMember(nCID);
	if (NULL == pGuildMember) return;

	pGuildMember->MoveField(nFieldID, nChannelID);

	MCommand* pNewCmd = MakeNewCommand(MC_GUILD_MOVEFIELD, 
		3,
		NEW_WSTR(pGuildMember->GetName()),
		NEW_INT(nFieldID),
		NEW_INT(nChannelID));

	gsys.pGuildSystem->RouteToMember(pGuild, pNewCmd);
}

void GGuildSystemForMasterServer::ChangeMasterSync( int nOldCID, int nNewCID, int nGID )
{
	GGuild* pGuild = gmgr.pGuildMgr->Get(nGID);
	if (NULL == pGuild)	return;
	GGuildMember* pOldMaster = pGuild->GetMember(nOldCID);
	if (NULL == pOldMaster) return;
	GGuildMember* pNewMaster = pGuild->GetMember(nNewCID);
	if (NULL == pNewMaster) return;

	pGuild->ChangeGuildMaster(nOldCID, nNewCID);

	MCommand* pNewCmd = MakeNewCommand(MC_GUILD_CHANGE_MASTER,
		2,
		NEW_WSTR(pOldMaster->GetName()),
		NEW_WSTR(pNewMaster->GetName()));

	gsys.pGuildSystem->RouteToMember(pGuild, pNewCmd);
}

void GGuildSystemForMasterServer::DepositStorageMoneySync( int nCID, int nGID, int nDepositMoney, int nStorageMoney )
{
	GGuild* pGuild = gmgr.pGuildMgr->Get(nGID);
	if (NULL == pGuild) return;
	GGuildMember* pGuildMember = pGuild->GetMember(nCID);
	if (NULL == pGuildMember) return;

	pGuild->SetStorageMoney(nStorageMoney);

	MCommand* pNewCmd = MakeNewCommand(MC_GUILD_DEPOSIT_STORAGEMONEY,
		3,
		NEW_WSTR(pGuildMember->GetName()), 
		NEW_INT(nDepositMoney),
		NEW_INT(nStorageMoney));

	gsys.pGuildSystem->RouteToStorageInteractionMember(pGuild, pNewCmd, nCID);
}

void GGuildSystemForMasterServer::WithdrawStorageMoneySync( int nCID, int nGID, int nWithdrawMoney, int nStorageMoney )
{
	GGuild* pGuild = gmgr.pGuildMgr->Get(nGID);
	if (NULL == pGuild) return;
	GGuildMember* pGuildMember = pGuild->GetMember(nCID);
	if (NULL == pGuildMember) return;

	pGuild->SetStorageMoney(nStorageMoney);

	MCommand* pNewCmd = MakeNewCommand(MC_GUILD_WITHDRAW_STORAGEMONEY,
		3,
		NEW_WSTR(pGuildMember->GetName()), 
		NEW_INT(nWithdrawMoney),
		NEW_INT(nStorageMoney));

	gsys.pGuildSystem->RouteToStorageInteractionMember(pGuild, pNewCmd, nCID);
}

void GGuildSystemForMasterServer::MoveStorageItemSync(int nCID, int nGID, vector<TD_PLAYER_GAME_DATA_ITEM_INSTANCE>& vecTDItem)
{
	GGuild* pGuild = gmgr.pGuildMgr->Get(nGID);
	if (NULL == pGuild) return;
	GGuildMember* pGuildMember = pGuild->GetMember(nCID);
	if (NULL == pGuildMember) return;

	vector<TD_ITEM> vecTDItemForClient;
	for each (const TD_PLAYER_GAME_DATA_ITEM_INSTANCE& tdItem in vecTDItem)
	{
		GItem* pOldItem = pGuild->GetStorage()->PopItem(tdItem.nSlotID);
		SAFE_DELETE(pOldItem);

		GItem* pNewItem = gmgr.pItemManager->MakeNewItem(tdItem);
		pGuild->GetStorage()->PushItem(tdItem.nSlotID, pNewItem);

		TD_ITEM tdItemForClient;
		tdItemForClient.m_nItemID = tdItem.nItemID;
		tdItemForClient.m_nSlotID = tdItem.nSlotID;
		tdItemForClient.m_nQuantity = tdItem.nQuantity;
		tdItemForClient.m_nDurability = tdItem.nDurability;
		tdItemForClient.m_bBind = tdItem.bBind;
		tdItemForClient.m_nSoulQuantity = tdItem.nSoulQuantity;
		vecTDItemForClient.push_back(tdItemForClient);
	}

	MCommand* pNewCmd = MakeNewCommand(MC_GUILD_MOVE_STORAGEITEM,
		2,
		NEW_WSTR(pGuildMember->GetName()), 
		NEW_BLOB(vecTDItemForClient));

	gsys.pGuildSystem->RouteToStorageInteractionMember(pGuild, pNewCmd, nCID);
}

void GGuildSystemForMasterServer::MoveStorageItem(GGuild* pGuild, GItem* pFromItem, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID)
{
	if (SLOTTYPE_GUILDSTORAGE == nFromSlotType && SLOTTYPE_GUILDSTORAGE != nToSlotType)
	{
		pGuild->GetStorage()->PopItem(nFromSlotID);
		SAFE_DELETE(pFromItem);
	}
	else if (SLOTTYPE_GUILDSTORAGE != nFromSlotType && SLOTTYPE_GUILDSTORAGE == nToSlotType)
	{
		pGuild->GetStorage()->PushItem(nToSlotID, pFromItem);
	}
	else if (SLOTTYPE_GUILDSTORAGE == nFromSlotType && SLOTTYPE_GUILDSTORAGE == nToSlotType)
	{
		pGuild->GetStorage()->PopItem(nFromSlotID);
		pGuild->GetStorage()->PushItem(nToSlotID, pFromItem);
	}
}

void GGuildSystemForMasterServer::SplitStorageItem(GGuild* pGuild, GItem* pFromItem, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount, int64 nSplitIUID)
{
	if (SLOTTYPE_GUILDSTORAGE == nFromSlotType && SLOTTYPE_GUILDSTORAGE != nToSlotType)
	{
		pFromItem->m_nAmount -= nAmount;
	}
	else if (SLOTTYPE_GUILDSTORAGE != nFromSlotType && SLOTTYPE_GUILDSTORAGE == nToSlotType)
	{
		GItem* pNewSplitItem = gmgr.pItemManager->MakeNewItem(pFromItem, nSplitIUID, nAmount);
		pGuild->GetStorage()->PushItem(nToSlotID, pNewSplitItem);
		SAFE_DELETE(pFromItem);				
	}
	else if (SLOTTYPE_GUILDSTORAGE == nFromSlotType && SLOTTYPE_GUILDSTORAGE == nToSlotType)
	{
		pFromItem->m_nAmount -= nAmount;
		GItem* pNewSplitItem = gmgr.pItemManager->MakeNewItem(pFromItem, nSplitIUID, nAmount);
		pGuild->GetStorage()->PushItem(nToSlotID, pNewSplitItem);
	}
}

void GGuildSystemForMasterServer::MergeStorageItem(GItem* pFromItem, SH_ITEM_SLOT_TYPE nFromSlotType, GItem* pToItem , SH_ITEM_SLOT_TYPE nToSlotType, int nAmount)
{
	if (SLOTTYPE_GUILDSTORAGE == nFromSlotType && SLOTTYPE_GUILDSTORAGE != nToSlotType)
	{
		pFromItem->m_nAmount -= nAmount;
		SAFE_DELETE(pToItem);
	}
	else if (SLOTTYPE_GUILDSTORAGE != nFromSlotType && SLOTTYPE_GUILDSTORAGE == nToSlotType)
	{
		pToItem->m_nAmount += nAmount;
		SAFE_DELETE(pFromItem);
	}
	else if (SLOTTYPE_GUILDSTORAGE == nFromSlotType && SLOTTYPE_GUILDSTORAGE == nToSlotType)
	{
		pFromItem->m_nAmount -= nAmount;
		pToItem->m_nAmount += nAmount;
	}
}

void GGuildSystemForMasterServer::SwpaStorageItem(GGuild* pGuild, GItem* pFromItem, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, GItem* pToItem, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID)
{
	if (SLOTTYPE_GUILDSTORAGE == nFromSlotType && SLOTTYPE_GUILDSTORAGE != nToSlotType)
	{
		pGuild->GetStorage()->PopItem(nFromSlotID);
		SAFE_DELETE(pFromItem);
		pGuild->GetStorage()->PushItem(nFromSlotID, pToItem);
	}
	else if (SLOTTYPE_GUILDSTORAGE != nFromSlotType && SLOTTYPE_GUILDSTORAGE == nToSlotType)
	{
		pGuild->GetStorage()->PopItem(nToSlotID);
		SAFE_DELETE(pToItem);
		pGuild->GetStorage()->PushItem(nToSlotID, pFromItem);
	}
	else if (SLOTTYPE_GUILDSTORAGE == nFromSlotType && SLOTTYPE_GUILDSTORAGE == nToSlotType)
	{
		pGuild->GetStorage()->PopItem(nFromSlotID);
		pGuild->GetStorage()->PopItem(nToSlotID);

		// PopItem 하면 Item의 nSlotType, nSlotID 가 변경되서 Item정보로 Push하지 않는다.
		pGuild->GetStorage()->PushItem(nToSlotID, pFromItem);						
		pGuild->GetStorage()->PushItem(nFromSlotID, pToItem);
	}
}