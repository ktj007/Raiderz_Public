#include "stdafx.h"
#include "GDBTaskQuestDone.h"
#include "GDBTaskGlobal.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "GQuestRewarderForDBTask.h"
#include "GQuestSystem.h"
#include "GItemRemover.h"
#include "GExpSystem.h"
#include "GMoneySystem.h"
#include "GInteractionSystem.h"
#include "GNPCInteractor.h"
#include "GCharacterDBCache.h"
#include "GDBManager.h"
#include "GItemHolder.h"


GDBTaskQuestDone::GDBTaskQuestDone(const MUID& uidReqPlayer) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_QUESTDONE), m_SyncingList(uidReqPlayer)
{	
}

GDBTaskQuestDone::~GDBTaskQuestDone()
{

}

bool GDBTaskQuestDone::Input( GDBT_QEUST_DONE& data )
{
	if (!BeginCacheSync(data))
	{
		m_SyncingList.EndCacheSync(false);
		return false;
	}

	m_Data.Set(data);	

	return true;
}

void GDBTaskQuestDone::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(QUEST_DONE)))
		return;

	for (; !rs.IsEOF(); rs.MoveNext())
		m_Data.m_mapIUID.insert(map<int, int64>::value_type(rs.FieldW(L"SLOT_ID").AsInt(), rs.FieldW(L"NEW_IUID").AsInt64()));
}

mdb::MDB_THRTASK_RESULT GDBTaskQuestDone::_OnCompleted()
{
	Completer completer(m_Data, IsSuccess());
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT GDBTaskQuestDone::_OnFailed()
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_QuestComm.m_uidPlayer);
	if (NULL == pPlayer) return mdb::MDBTR_SUCESS;

	for each (const GDBT_QUEST_ADDITEM& addItem in m_Data.m_vecAddItem)
	{
		if (addItem.m_nStackAmt == addItem.m_nModStackAmt)
		{
			pPlayer->GetItemHolder()->CancelReserveSlot(SLOTTYPE_INVENTORY, addItem.m_nSlotID);
		}
	}

	gsys.pInteractionSystem->GetNPCInteractor().End(pPlayer);

	return mdb::MDBTR_SUCESS;
}

bool GDBTaskQuestDone::BeginCacheSync( GDBT_QEUST_DONE& data )
{
	GCharacterDBCache* cc = gsys.pDBCacheSystem->FindCharCache(data.m_QuestComm.m_uidPlayer);
	if (NULL == cc)
		return false;

	if (!cc->BeginCacheSync())
		return false;

	if (NULL == m_SyncingList.PushChar())
	{
		cc->EndCacheSync(false);	
		return false;
	}

	for each (const GITEM_STACK_AMT& amt in data.m_vecRemoveItem)
	{
		GItemDBCache* pi = gsys.pDBCacheSystem->FindItemCache(data.m_QuestComm.m_uidPlayer, amt.nSlotType, amt.nSlotID, amt.nIUID);
		if (NULL == pi)
			return false;

		if (!pi->BeginCacheSync())
			return false;
		
		if (NULL == m_SyncingList.PushItem(pi->GetIUID()))
		{
			pi->EndCacheSync(false);
			return false;
		}
	}

	return true;
}

void GDBTaskQuestDone::Completer::Do()
{
	gsys.pDBManager->QuestDoneLog(m_Data);

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_QuestComm.m_uidPlayer);
	if (NULL == pPlayer)
		return;

	if (!m_bSuccess)
	{
		gsys.pInteractionSystem->GetNPCInteractor().End(pPlayer);
		return;
	}

	gsys.pQuestSystem->GetRewarderForDBTask().Reward(m_Data);		
}


void GDBTaskQuestDone::_OnPreCompleted()
{
	__super::_OnPreCompleted();
	m_SyncingList.EndCacheSync(IsSuccess());
}