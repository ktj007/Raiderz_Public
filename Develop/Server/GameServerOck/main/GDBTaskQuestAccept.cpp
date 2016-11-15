#include "stdafx.h"
#include "GDBTaskQuestAccept.h"
#include "GDBTaskGlobal.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "GInteractionSystem.h"
#include "GQuestSystem.h"
#include "GNPCInteractor.h"
#include "GQuestInfoMgr.h"
#include "GChallengerQuestMgr.h"
#include "GQuestInfo.h"
#include "GPlayerQuests.h"
#include "GDBManager.h"
#include "GQuestGiver.h"
#include "GItemHolder.h"


GDBTaskQuestAccept::GDBTaskQuestAccept(const MUID& uidReqPlayer) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_QUESTACCEPT), m_SyncingList(uidReqPlayer)
{
	
}

GDBTaskQuestAccept::~GDBTaskQuestAccept()
{

}



void GDBTaskQuestAccept::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(QUEST_ACCEPT)))
		return;

	for (; !rs.IsEOF(); rs.MoveNext())
		m_Data.m_mapIUID.insert(map<int, int64>::value_type(rs.FieldW(L"SLOT_ID").AsInt(), rs.FieldW(L"NEW_IUID").AsInt64()));	
}

bool GDBTaskQuestAccept::Input( GDBT_QUEST_ACCEPT& data )
{
	for each(const GITEM_STACK_AMT& amt in data.m_vecRemoveItem)
	{
		GItemDBCache* ic = gsys.pDBCacheSystem->FindItemCache(data.m_QuestComm.m_uidPlayer, amt.nSlotType, amt.nSlotID, amt.nIUID);
		if (NULL != ic)
		{
			if (ic->BeginCacheSync())
			{
				if (NULL != m_SyncingList.PushItem(amt.nIUID))
					continue;				
			}
		}

		m_SyncingList.EndCacheSync(false);
		return false;
	}

	m_Data = data;

	return true;
}

mdb::MDB_THRTASK_RESULT GDBTaskQuestAccept::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT GDBTaskQuestAccept::_OnFailed()
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_QuestComm.m_uidPlayer);
	if (NULL == pPlayer)
		return mdb::MDBTR_SUCESS;

	pPlayer->GetQuests().ResetSlot(m_Data.m_QuestComm.m_nSlotID);

	for each (const GDBT_QUEST_ADDITEM& addItem in m_Data.m_vecAddItem)
	{
		if (addItem.m_nStackAmt == addItem.m_nModStackAmt)
		{
			pPlayer->GetItemHolder()->CancelReserveSlot(SLOTTYPE_INVENTORY, addItem.m_nSlotID);
		}
	}

	GQuestInfo*pQuestInfo = gmgr.pQuestInfoMgr->Get(m_Data.m_QuestComm.m_nQuestID);
	VALID_RET(pQuestInfo, mdb::MDBTR_SUCESS);
	if (!pQuestInfo->bChallengerQuest)
	{
		gsys.pInteractionSystem->GetNPCInteractor().End(pPlayer);
	}

	return mdb::MDBTR_SUCESS;
}

void GDBTaskQuestAccept::_OnPreCompleted()
{
	__super::_OnPreCompleted();

	m_SyncingList.EndCacheSync(IsSuccess());
}

GDBTaskQuestAccept::Completer::Completer( GDBT_QUEST_ACCEPT& Data ) : m_Data(Data)
{
}

void GDBTaskQuestAccept::Completer::Do()
{
	gsys.pDBManager->QuestAcceptLog(m_Data);

	if (m_Data.m_bComplete)
		gsys.pDBManager->QuestCompleteLog(m_Data.m_QuestComm);	

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_QuestComm.m_uidPlayer);
	if (NULL == pPlayer)
		return;

	gsys.pQuestSystem->GetGiver().GiveForDBTask(m_Data);

	GQuestInfo*pQuestInfo = gmgr.pQuestInfoMgr->Get(m_Data.m_QuestComm.m_nQuestID);
	VALID(pQuestInfo);
	if (pQuestInfo->bChallengerQuest)
	{
		gmgr.pChallengerQuestMgr->OnChallengerQuestAccept(pPlayer, pQuestInfo);
	}
	else
	{
		gsys.pInteractionSystem->GetNPCInteractor().End(pPlayer);
	}
}