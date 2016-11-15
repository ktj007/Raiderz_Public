#include "stdafx.h"
#include "GPlayerQuests.h"
#include "GSTLUtil.h"
#include "GQuestInfo.h"
#include "GGlobal.h"
#include "GQuestInfoMgr.h"
#include "GSystem.h"
#include "GPlayerQuest.h"
#include "GEntityPlayer.h"
#include "GPartySystem.h"

GPlayerQuests::GPlayerQuests( GEntityPlayer* pOwner ) :m_pOwner(pOwner)
{
	memset(m_bSlot, 0, sizeof(m_bSlot));
}

GPlayerQuests::~GPlayerQuests()
{
	Clear();
}

void GPlayerQuests::Clear()
{
	SAFE_DELETE_MAP(m_mapDoingQuest);
}

void GPlayerQuests::ClearDoneCount()
{
	vector<int> vecClerableDoneQuestID;
	for each (MAP_DONE_COUNT::value_type val in  m_mapDoneCount)
	{
		if (false == IsDoingQuest(val.first))
		{
			vecClerableDoneQuestID.push_back(val.first);
		}
	}

	for each (int nClearableDoneQuestID in vecClerableDoneQuestID)
	{
		m_mapDoneCount.erase(nClearableDoneQuestID);
	}	
}

GPlayerQuest* GPlayerQuests::Insert(const uint8 nSlotID, int nQuestID, const wstring& strAcceptDt, const wstring& strExpiDt)
{
	if (!IsValidSlotID(nSlotID))
		return NULL;

	GPlayerQuest* pPlayerQuest = Get(nQuestID);
	_ASSERT(NULL == pPlayerQuest);
	if (NULL != pPlayerQuest) return NULL;

	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(nQuestID);
	if (NULL ==pQuestInfo) return NULL;	

	GPlayerQuest* pNewPlayerQuest = new GPlayerQuest(pQuestInfo, m_pOwner, nSlotID, gsys.pSystem->GetNowTime(), strAcceptDt, strExpiDt);

	m_mapDoingQuest.insert(MAP_PLAYER_QUEST::value_type(pQuestInfo->nID, pNewPlayerQuest));
	if (m_mapDoneCount.end() == m_mapDoneCount.find(pQuestInfo->nID))
	{
		m_mapDoneCount.insert(MAP_DONE_COUNT::value_type(pQuestInfo->nID, 0));
	}

	if (gsys.pPartySystem->IsPartyMember(m_pOwner))
	{
		MUID uidMember = m_pOwner->GetUID();
		MUID uidParty = m_pOwner->GetPartyUID();
		gsys.pPartySystem->AddQuest(uidParty, uidMember, nQuestID);
	}

	m_pOwner->OnSynchQuest();

	SlotMarking(nSlotID);
	
	return pNewPlayerQuest;
}

void GPlayerQuests::Cancel(int nQuestID)
{
	GPlayerQuest* pPlayerQuest = Get(nQuestID);
	if (NULL == pPlayerQuest) return;

	m_mapDoingQuest.erase(nQuestID);
	ResetSlot(pPlayerQuest->GetSlotID());

	SAFE_DELETE(pPlayerQuest);
}

void GPlayerQuests::Update(float fDelta)
{
	PFC_THISFUNC;
	for each (pair<int, GPlayerQuest*> pairEach in m_mapDoingQuest)
	{
		pairEach.second->Update(fDelta);
	}
}

void GPlayerQuests::Done(int nQuestID)
{
	GPlayerQuest* pPlayerQuest = Get(nQuestID);
	if (NULL == pPlayerQuest) return;

	MAP_DONE_COUNT::iterator itor = m_mapDoneCount.find(nQuestID);
	if (m_mapDoneCount.end() == itor)
	{
		m_mapDoneCount.insert(MAP_DONE_COUNT::value_type(nQuestID, 1));
	}
	else
	{
		int nDoneCount = (*itor).second;
		if (INT_MAX > nDoneCount)
		{
			itor->second = nDoneCount + 1;
		}		
	}
	
	// Script 콜백에서 GPlayerQuest를 사용하고 있어서 Done에서 바로 Delete하지 않고, 콜백이 끝난후 Delete한다.
	m_mapDoingQuest.erase(nQuestID);
	ResetSlot(pPlayerQuest->GetSlotID());
}

GPlayerQuest* GPlayerQuests::Get(int nQuestID)
{
	MAP_PLAYER_QUEST::iterator itor = m_mapDoingQuest.find(nQuestID);
	if (itor == m_mapDoingQuest.end()) return NULL;

	GPlayerQuest* pPlayerQuest = (*itor).second;
	if (NULL == pPlayerQuest) return NULL;

	return pPlayerQuest;
}

bool GPlayerQuests::IsDoingQuest(int nQuestID)
{
	if (NULL == Get(nQuestID)) return false;

	return true;
}

bool GPlayerQuests::IsDoingQuest(const vector<int>& vecQuestID)
{
	for each (int nQuestID in vecQuestID)
	{
		GPlayerQuest* pPlayerQuest = Get(nQuestID);
		if (NULL == pPlayerQuest) return false;
	}

	return true;
}

bool GPlayerQuests::IsCompleteQuest(int nQuestID)
{
	GPlayerQuest* pPlayerQuest = Get(nQuestID);
	if (NULL == pPlayerQuest) return false;

	return pPlayerQuest->IsComplete();
}

bool GPlayerQuests::IsDoneQuest(int nQuestID)
{
	MAP_DONE_COUNT::iterator itor = m_mapDoneCount.find(nQuestID);
	if (itor == m_mapDoneCount.end())
	{		
		return false;
	}
	else
	{
		int nDoneCount = (*itor).second;
		if (0 == nDoneCount) return false;
	}

	return true;
}

int GPlayerQuests::GetDoneCount(int nQuestID)
{
	MAP_DONE_COUNT::iterator itor = m_mapDoneCount.find(nQuestID);
	if (itor == m_mapDoneCount.end())
	{		
		return 0;
	}
	
	return itor->second;
}

int GPlayerQuests::GetQuestNeedItemQuantity(const vector<int>& vecQuestID, int nItemID)
{
	int nNeedItemQuantity = 0;

	for each (int nQuestID in vecQuestID)
	{
		GPlayerQuest* pPlayerQuest = Get(nQuestID);
		if (NULL == pPlayerQuest) continue;

		nNeedItemQuantity += pPlayerQuest->GetQuestNeedItemQuantity(nItemID);
	}

	return nNeedItemQuantity;
}

bool GPlayerQuests::IsDBInsert( int nQuestID )
{	
	return (m_mapDoneCount.end() == m_mapDoneCount.find(nQuestID));
}

void GPlayerQuests::MakeActiveQuestID(vector<int>* pVecQuestID) const
{
	_ASSERT(pVecQuestID != NULL);
	
	pVecQuestID->clear();

	for each(MAP_PLAYER_QUEST::value_type quest in m_mapDoingQuest)
	{
		GPlayerQuest* pQuest = quest.second;
		_ASSERT(pQuest != NULL);

		if (!pQuest->IsActive())	continue;

		int nQuestID = quest.first;
		pVecQuestID->push_back(nQuestID);
	}
}

bool GPlayerQuests::Has( int nQuestID )
{
	return (Get(nQuestID) != NULL);
}

uint8 GPlayerQuests::FindAndMarkingEmptySlot()
{
	for (uint8 i = 0; i < MAX_QUEST_INSTACNE_COUNT; ++i)
	{
		if (!m_bSlot[i])
		{
			SlotMarking(i);
			return i;
		}
	}

	return INVALID_QUSET_SLOT_ID;
}

void GPlayerQuests::ResetSlot( const uint8 nSlotID )
{
	if (!IsValidSlotID(nSlotID))
		return;

	m_bSlot[nSlotID] = false;
}

bool GPlayerQuests::SlotMarking( const uint8 nSlotID )
{
	if (!IsValidSlotID(nSlotID))
		return false;

	m_bSlot[nSlotID] = true;

	return true;
}

bool GPlayerQuests::IsValidSlotID( const uint8 nSlotID )
{
	if (MAX_QUEST_INSTACNE_COUNT <= nSlotID)
		return false;

	return true;
}