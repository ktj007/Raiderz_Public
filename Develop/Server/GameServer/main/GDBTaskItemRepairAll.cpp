#include "stdafx.h"
#include "GDBTaskItemRepairAll.h"
#include "GDBTaskGlobal.h"
#include "GMoneySystem.h"
#include "GItemDurabilityUpdater.h"
#include "GItemHolder.h"
#include "GNPCShopSystem.h"
#include "GNPCShopRepairer.h"
#include "GCharacterDBCache.h"
#include "GDBManager.h"

GDBTaskItemRepairAll::GDBTaskItemRepairAll( const MUID& uidPlayer ) 
: GDBAsyncTask(uidPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_ITEMREPAIR_ALL), m_SyncingList(uidPlayer)
{
	
}

GDBTaskItemRepairAll::~GDBTaskItemRepairAll()
{

}

bool GDBTaskItemRepairAll::Input( GDBT_REPAIR_ALL_ITEM& data )
{
	if (!BeginCharCacheSync(data.m_uidPlayer))
		return false;

	if (!BeginItemsCacheSync(data))
	{
		m_SyncingList.EndCacheSync(false);
		return false;
	}
		
	m_Data.SetData(data);

	return true;
}

void GDBTaskItemRepairAll::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(ITEM_REPAIR_ALL));
}

mdb::MDB_THRTASK_RESULT GDBTaskItemRepairAll::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

bool GDBTaskItemRepairAll::BeginCharCacheSync( const MUID& uidPlayer )
{
	GCharacterDBCache* cc = gsys.pDBCacheSystem->FindCharCache(uidPlayer);
	if (NULL == cc)
		return true;

	if (!cc->BeginCacheSync())
		return false;

	if (NULL == m_SyncingList.PushChar())
	{
		cc->EndCacheSync(false);
		return false;
	}

	return true;
}

bool GDBTaskItemRepairAll::BeginItemsCacheSync( GDBT_REPAIR_ALL_ITEM& data )
{
	for each (const GDBT_ITEM_REPAIR_ALL& rep in data.m_vRepair)
	{
		GItemDBCache* ic = gsys.pDBCacheSystem->FindItemCache(data.m_uidPlayer, (SH_ITEM_SLOT_TYPE)rep.m_nSlotType, rep.m_nSlotID, rep.m_nIUID);
		if (NULL == ic)
			return false;

		if (!ic->BeginCacheSync())
			return false;
		
		if (NULL == m_SyncingList.PushItem(rep.m_nIUID))
		{
			ic->EndCacheSync(false);
			return false;
		}
	}

	return true;
}

void GDBTaskItemRepairAll::_OnPreCompleted()
{
	__super::_OnPreCompleted();
	m_SyncingList.EndCacheSync(IsSuccess());
}

void GDBTaskItemRepairAll::Completer::Do()
{
	gsys.pDBManager->NPCShop_RepairLog(m_Data.m_Char);

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_uidPlayer);
	if (NULL == pPlayer)
		return;

	SetMoney(pPlayer);
	SetItemDura(pPlayer);
	Route(pPlayer);
}

void GDBTaskItemRepairAll::Completer::SetMoney( GEntityPlayer* pPlayer )
{
	gsys.pMoneySystem->DecreaseForDBTask(pPlayer, m_Data.m_Char.m_nModMoney);
}

void GDBTaskItemRepairAll::Completer::SetItemDura( GEntityPlayer* pPlayer )
{
	GItem*			pItem;
	vector<GItem*>	vecItem;
	vector<int>		vecDura;
	for (size_t i = 0; i < m_Data.m_vRepair.size(); ++i)
	{
		GItemHolder* pItemHolder = pPlayer->GetItemHolder();
		pItem = pItemHolder->GetItem((SH_ITEM_SLOT_TYPE)m_Data.m_vRepair[i].m_nSlotType, m_Data.m_vRepair[i].m_nSlotID);
		if (pItem == NULL)
		{
			continue;
		}
		
		vecItem.push_back(pItem);
		vecDura.push_back(pItem->m_pItemData->m_nMaxDurability); // 이게 추후에 복구 가능 최대 내구도로 수정되어야 한다.
	}

	gsys.pItemSystem->GetDurabilityUpdater().ApplyServerAndRoute(pPlayer, vecItem, vecDura);
}

void GDBTaskItemRepairAll::Completer::Route( GEntityPlayer* pPlayer )
{
	vector<pair<int,int>> vecSlotInfo;

	for each (const GDBT_ITEM_REPAIR_VEC::value_type& data in m_Data.m_vRepair)
	{
		vecSlotInfo.push_back(pair<int,int>(data.m_nSlotType, data.m_nSlotID));
	}

	gsys.pNPCShopSystem->GetRepairer().Route(pPlayer, vecSlotInfo);
}