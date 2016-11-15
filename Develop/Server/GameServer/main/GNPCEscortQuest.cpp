#include "StdAfx.h"
#include "GNPCEscortQuest.h"
#include "GPlayerQuest.h"
#include "GPlayerQuests.h"
#include "GPlayerQObjective.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GGlobal.h"
#include "GQuestSystem.h"
#include "GQObjUpdater.h"

GNPCEscortQuest::GNPCEscortQuest(GEntityNPC* pOwner, int nQuestID, int nQObjectiveID)
: m_pOwner(pOwner)
, m_nQuestID(nQuestID)
, m_nQObjectiveID(nQObjectiveID)
{
}

void GNPCEscortQuest::InsertEscortPlayer(MUID uidEscortPlayer)
{
	m_setEscortPlayerUID.insert(uidEscortPlayer);
}

void GNPCEscortQuest::DeleteEscortPlayer(MUID uidEscortPlayer)
{
	m_setEscortPlayerUID.erase(uidEscortPlayer);
}

void GNPCEscortQuest::CompleteEscortQuestObjective()
{
	for each (const MUID& uidEscrotPlayerUID in m_setEscortPlayerUID)
	{
		GEntityPlayer* pPlayer = m_pOwner->FindPlayer(uidEscrotPlayerUID);
		if (NULL == pPlayer) continue;

		gsys.pQuestSystem->GetQObjUpdater().UpdEscortQObj(pPlayer, m_nQObjectiveID, 1);
	}
}

void GNPCEscortQuest::GetEscortPlayerUID(set<MUID>& setEscortPlayerUID) const
{
	for each (const MUID& nEscortPlayerUID in m_setEscortPlayerUID)
	{
		setEscortPlayerUID.insert(nEscortPlayerUID);
	}
}