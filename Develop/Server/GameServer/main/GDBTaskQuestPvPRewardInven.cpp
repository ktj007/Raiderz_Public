#include "stdafx.h"
#include "GDBTaskQuestPvPRewardInven.h"
#include "GEntityPlayer.h"
#include "GMoneySystem.h"
#include "GExpSystem.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GItemAdder.h"
#include "GCharacterDBCache.h"
#include "GItemDBCache.h"
#include "GDBCacheSystem.h"
#include "GItemHolder.h"
#include "GDBManager.h"
#include "GPlayerLevelUpdater.h"



GDBTaskQuestPvPRewardInven::GDBTaskQuestPvPRewardInven( const MUID& uidPlayer ) 
: GDBAsyncTask(uidPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_QUEST_PVP_REWARD_INVEN)
, m_SyncingList(uidPlayer)
{

}

bool GDBTaskQuestPvPRewardInven::BeginCharCacheSync(GDBT_QPER_TOINVEN& data)
{
	GCharacterDBCache* pcc = gsys.pDBCacheSystem->FindCharCache(data.m_uidPlayer);
	if (NULL == pcc)
	{
		m_SyncingList.EndCacheSync(false);
		return false;
	}

	if (!pcc->BeginCacheSync())
	{
		m_SyncingList.EndCacheSync(false);
		return false;
	}

	if (!m_SyncingList.PushChar())
	{
		m_SyncingList.EndCacheSync(false);
		return false;
	}

	return true;
}


bool GDBTaskQuestPvPRewardInven::BeginItemCacheSync( GDBT_QPER_TOINVEN& data )
{
	for each (const GDBT_QPER_ITEM& qi in data.m_vecItem)
	{
		if (0 == qi.m_nIUID)
			continue;

		GItemDBCache* pic = gsys.pDBCacheSystem->FindItemCache(data.m_uidPlayer, SLOTTYPE_INVENTORY, qi.m_nSlotID, qi.m_nIUID);
		if (NULL == pic)
		{
			m_SyncingList.EndCacheSync(false);
			return false;
		}

		if (pic->BeginCacheSync())
		{
			m_SyncingList.EndCacheSync(false);
			return false;
		}

		if (!m_SyncingList.PushItem(qi.m_nIUID))
		{
			m_SyncingList.EndCacheSync(false);
			return false;
		}	
	}	

	return true;
}

bool GDBTaskQuestPvPRewardInven::Input( GDBT_QPER_TOINVEN& data )
{
	if (!BeginCharCacheSync(data))
		return false;

	if (!BeginItemCacheSync(data))
		return false;

	m_Data.Set(data);

	return true;
}

void GDBTaskQuestPvPRewardInven::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);

	if (!ExecuteW(rs, GetSQLW(QPVP_REWARD_INVEN)))
		return;

	
	int16 SLOT_ID;
	int64 IUID;
	for (; !rs.IsEOF(); rs.MoveNext())
	{
		SLOT_ID = rs.FieldW(L"SLOT_ID").AsShort();
		IUID	= rs.FieldW(L"NEW_IUID").AsInt64();

		m_Data.m_vecRewardItemIUID.push_back(pair<int16, int64>(SLOT_ID, IUID));	
	}
}

bool GDBTaskQuestPvPRewardInven::IsSameSlotID(GDBT_QPER_ITEM& qi, pair<int16, int64> iuid)
{
	if (qi.m_nSlotID != iuid.first)
		return false;
		
	return true;
}

bool GDBTaskQuestPvPRewardInven::IsNewRewardItem(GDBT_QPER_ITEM& qi)
{
	if(0 != qi.m_nIUID)
		return false;

	return true;
}

bool GDBTaskQuestPvPRewardInven::SetRwardItemUID(GDBT_QPER_ITEM& qi, pair<int16, int64> iuid)
{
	if (IsSameSlotID(qi, iuid))
	{
		if (!IsNewRewardItem(qi))
		{
			if (qi.m_nIUID != iuid.second)
				return false;			
		}
		else
			qi.m_nIUID = iuid.second;		
	}

	return true;
}

void GDBTaskQuestPvPRewardInven::AddQPvPRewardItem(GEntityPlayer* pPlayer)
{
	vector<ADD_ITEM> vAddItem;
	for each (const pair<int16, int64>& iuid in m_Data.m_vecRewardItemIUID)
	{
		size_t i = 0;
		for (; i < m_Data.m_vecItem.size(); ++i)
		{
			GDBT_QPER_ITEM& qi = m_Data.m_vecItem[i];

			if (!SetRwardItemUID(qi, iuid))
			{
				MLog3("QPvP Reward to Inven. [CSN:%I64d, EventID:%d FROM_IUID:%I64d, TO_IUID:%I64d].\n"
					, m_Data.m_nCID, m_Data.m_nEventID, qi.m_nIUID, iuid.second);
				break;
			}

			vAddItem.push_back(
				ADD_ITEM(SLOTTYPE_INVENTORY
					, qi.m_nSlotID
					, qi.m_nStackAmt
					, qi.m_nModStackAmt
					, qi.m_nIUID
					, qi.m_nItemID
					, -1
					, 0
					, qi.m_nMaxDura
					, qi.m_nColor));
		}
	}

	gsys.pItemSystem->GetAdder().AddForDBTask(pPlayer->GetUID(), vAddItem);
}

mdb::MDB_THRTASK_RESULT GDBTaskQuestPvPRewardInven::_OnCompleted()
{
	gsys.pDBManager->QuestPVPERewardInvenLog(m_Data);

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_uidPlayer);
	if (NULL == pPlayer)
		return mdb::MDBTR_SUCESS;

	if (0 < m_Data.m_nDeltaLevel)
	{
		GPlayerLevelUpdater levelUpdater(pPlayer);
		levelUpdater.LevelUpForDBTask(m_Data.m_nLevel, m_Data.m_nXP, m_Data.m_nDeltaXP);
	}
	else
	{
		gsys.pExpSystem->AddExpForDB(pPlayer, m_Data.m_nDeltaXP);
	}
	
	gsys.pMoneySystem->IncreaseForDBTask(pPlayer, m_Data.m_nDeltaMoney);

	AddQPvPRewardItem(pPlayer);

	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT GDBTaskQuestPvPRewardInven::_OnFailed()
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_uidPlayer);
	if (NULL == pPlayer) return mdb::MDBTR_SUCESS;

	for each (const GDBT_QPER_ITEM& qi in m_Data.m_vecItem)
	{
		if (qi.m_nStackAmt == qi.m_nModStackAmt)
		{
			pPlayer->GetItemHolder()->CancelReserveSlot(SLOTTYPE_INVENTORY, qi.m_nSlotID);
		}
	}

	return mdb::MDBTR_SUCESS;
}

void GDBTaskQuestPvPRewardInven::_OnPreCompleted()
{
	__super::_OnPreCompleted();

	m_SyncingList.EndCacheSync(IsSuccess());
}
