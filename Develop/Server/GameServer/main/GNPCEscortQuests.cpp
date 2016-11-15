#include "StdAfx.h"
#include "GNPCEscortQuests.h"
#include "GNPCEscortQuest.h"
#include "GSTLUtil.h"

GNPCEscortQuests::GNPCEscortQuests(GEntityNPC* pOwner)
: m_pOwner(pOwner)
{
}

GNPCEscortQuests::~GNPCEscortQuests(void)
{
	Clear();
}

void GNPCEscortQuests::Clear()
{
	SAFE_DELETE_MAP(m_mapNPCEscortQuest);
}

GNPCEscortQuest* GNPCEscortQuests::Get(int nQuestID, int nQObjectiveID)
{
	MAP_NPCESCORTQUEST::iterator itor = m_mapNPCEscortQuest.find(pair<int, int>(nQuestID, nQObjectiveID));

	if (m_mapNPCEscortQuest.end() == itor) return NULL;

	return (*itor).second;	
}

void GNPCEscortQuests::InsertEscortPlayer(MUID uidPlayer, int nQuestID, int nQObjectiveID)
{
	GNPCEscortQuest* pNPCEscortQuest = Get(nQuestID, nQObjectiveID);
	if (NULL == pNPCEscortQuest)
	{
		pNPCEscortQuest = New(nQuestID, nQObjectiveID);
		Insert(pNPCEscortQuest);
	}

	pNPCEscortQuest->InsertEscortPlayer(uidPlayer);
}

void GNPCEscortQuests::DeleteEscortPlayer(MUID uidPlayer, int nQuestID, int nQObjectiveID)
{
	GNPCEscortQuest* pNPCEscortQuest = Get(nQuestID, nQObjectiveID);
	if (NULL == pNPCEscortQuest) return;

	pNPCEscortQuest->DeleteEscortPlayer(uidPlayer);

	if (true == pNPCEscortQuest->IsEscortPlayerEmpty())
	{
		Delete(pNPCEscortQuest);
	}
}

GNPCEscortQuest* GNPCEscortQuests::New(int nQuestID, int nQOjectiveID)
{
	return new GNPCEscortQuest(m_pOwner, nQuestID, nQOjectiveID);
}

void GNPCEscortQuests::Insert(GNPCEscortQuest* pNPCEscortQuest)
{
	m_mapNPCEscortQuest.insert(MAP_NPCESCORTQUEST::value_type(pair<int,int>(pNPCEscortQuest->GetQuestID(), pNPCEscortQuest->GetQObjectiveID()), pNPCEscortQuest));
}

void GNPCEscortQuests::Delete(GNPCEscortQuest* pNPCEscortQuest)
{
	if (NULL == pNPCEscortQuest) return;

	m_mapNPCEscortQuest.erase(pair<int,int>(pNPCEscortQuest->GetQuestID(), pNPCEscortQuest->GetQObjectiveID()));
	SAFE_DELETE(pNPCEscortQuest);
}

vector<MUID> GNPCEscortQuests::GetEscortPlayerUID()
{
	set<MUID> setEscortPlayerUID;

	for each (MAP_NPCESCORTQUEST::value_type val in m_mapNPCEscortQuest)
	{
		GNPCEscortQuest* pNPCEscortQuest = val.second;
		pNPCEscortQuest->GetEscortPlayerUID(setEscortPlayerUID);
	}

	GVectorMUID vecEscortPlayerUID;
	vecEscortPlayerUID.Vector().insert(vecEscortPlayerUID.Vector().end(), setEscortPlayerUID.begin(), setEscortPlayerUID.end());

	return vecEscortPlayerUID.Vector();
}