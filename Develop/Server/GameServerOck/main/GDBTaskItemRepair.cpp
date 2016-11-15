#include "stdafx.h"
#include "GDBTaskItemRepair.h"
#include "GDBTaskGlobal.h"
#include "GMoneySystem.h"
#include "GItemDurabilityUpdater.h"
#include "GItemHolder.h"
#include "GNPCShopSystem.h"
#include "GNPCShopRepairer.h"
#include "GCharacterDBCache.h"
#include "GDBManager.h"


GDBTaskItemRepair::GDBTaskItemRepair( const MUID& uidPlayer ) 
: GDBAsyncTask(uidPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_ITEMREPAIR), m_SyncingList(uidPlayer)
{
	
}

GDBTaskItemRepair::~GDBTaskItemRepair()
{

}

bool GDBTaskItemRepair::Input( GDBT_REPAIR_ITEM& data )
{
	if (!BeginCharCacheSync(data.m_uidPlayer))
		return false;

	if (!BeginItemCacheSync(data.m_uidPlayer, data.m_nSlotType, data.m_nSlotID, data.m_nIUID))
	{
		m_SyncingList.EndCacheSync(false);
		return false;
	}
	
	m_Data = data;

	return true;
}

void GDBTaskItemRepair::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(ITEM_REPAIR));
}

mdb::MDB_THRTASK_RESULT GDBTaskItemRepair::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

bool GDBTaskItemRepair::BeginCharCacheSync( const MUID& uidPlayer )
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

bool GDBTaskItemRepair::BeginItemCacheSync( const MUID& uidPlayer, const uint8 nSlotType, const int16 nSlotID, const IUID nIUID )
{
	GItemDBCache* ic = gsys.pDBCacheSystem->FindItemCache(uidPlayer, (SH_ITEM_SLOT_TYPE)nSlotType, nSlotID, nIUID);
	if (NULL == ic)
		return false;

	if (!ic->BeginCacheSync())
		return false;

	if (NULL == m_SyncingList.PushItem(nIUID))
	{
		ic->EndCacheSync(false);
		return false;
	}

	return true;
}

void GDBTaskItemRepair::_OnPreCompleted()
{
	__super::_OnPreCompleted();
	m_SyncingList.EndCacheSync(IsSuccess());
}

void GDBTaskItemRepair::Completer::Do()
{
	gsys.pDBManager->NPCShop_RepairLog(m_Data.m_Char);

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_uidPlayer);
	if (NULL == pPlayer)
		return;

	SetMoney(pPlayer);
	SetItemDura(pPlayer);
	Route(pPlayer);	
}

void GDBTaskItemRepair::Completer::SetMoney( GEntityPlayer* pPlayer )
{
	gsys.pMoneySystem->DecreaseForDBTask(pPlayer, m_Data.m_Char.m_nModMoney);	
}

void GDBTaskItemRepair::Completer::SetItemDura( GEntityPlayer* pPlayer )
{
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	GItem* pItem = pItemHolder->GetItem((SH_ITEM_SLOT_TYPE)m_Data.m_nSlotType, m_Data.m_nSlotID);
	
	vector<GItem*>	vecItem;
	vector<int>		vecDura;

	vecItem.push_back(pItem);
	vecDura.push_back(pItem->m_pItemData->m_nMaxDurability); // 이게 추후에 복구 가능 최대 내구도로 수정되어야 한다.

	gsys.pItemSystem->GetDurabilityUpdater().ApplyServerAndRoute(pPlayer, vecItem, vecDura);
}

void GDBTaskItemRepair::Completer::Route(GEntityPlayer* pPlayer)
{
	vector<pair<int,int>>	vecSlotInfo;
	vecSlotInfo.push_back(pair<int,int>(m_Data.m_nSlotType, m_Data.m_nSlotID));

	gsys.pNPCShopSystem->GetRepairer().Route(pPlayer, vecSlotInfo);
}