#include "stdafx.h"
#include "GAIEventNotifier.h"
#include "GAIEventHandler.h"
#include "GGlobal.h"
#include "GFieldMgr.h"
#include "GField.h"
#include "GEntityMgr.h"
#include "GEntity.h"
#include "GNPCMgr.h"
#include "GEntityNPC.h"


GAIEventNotifier::GAIEventNotifier()
{
	// do nothing
}

GAIEventNotifier::~GAIEventNotifier()
{
	// do nothing
}

void GAIEventNotifier::Notify(GAIEvent aiEvent)
{
	switch(aiEvent.eEventID)
	{
	case GAIEvent::EVENT_ID_FIELD_ENTER_PLAYER:
	case GAIEvent::EVENT_ID_FIELD_LEAVE_PLAYER:
		NotifyField(aiEvent);
		break;
	case GAIEvent::EVENT_ID_SECTOR_ENTER_PLAYER:
	case GAIEvent::EVENT_ID_SECTOR_LEAVE_PLAYER:
		NotifyNeighborSector(aiEvent);
		break;
	case GAIEvent::EVENT_ID_COMBAT_BEGIN:
	case GAIEvent::EVENT_ID_COMBAT_END:
	case GAIEvent::EVENT_ID_SECTOR_MOVE_BE_PLAYER:
	case GAIEvent::EVENT_ID_SECTOR_MOVE_EMPTY_PLAYER:
	case GAIEvent::EVENT_ID_SPAWN_EMPTY_FIELD:
	case GAIEvent::EVENT_ID_SPAWN_EMPTY_SECTOR:
	case GAIEvent::EVENT_ID_SPAWN_PLAYER_SECTOR:
	case GAIEvent::EVENT_ID_MAKE_MAJOR_LOD_NPC:
		NotifyNPC(aiEvent);
		break;		
	default:
		NotifyAll(aiEvent);
		break;
	}	
}

void GAIEventNotifier::AddEventHandler(MUID uidNPC, GAIEventHandler* pHandler)
{
	_ASSERT(uidNPC != MUID::Invalid());
	_ASSERT(pHandler != NULL);
	_ASSERT(m_mapNPCHandler.find(uidNPC) == m_mapNPCHandler.end());

	m_mapNPCHandler.insert(map<MUID, GAIEventHandler*>::value_type(uidNPC, pHandler));	
}

void GAIEventNotifier::RemoveEventHandler(MUID uidNPC, GAIEventHandler* pHandler)
{	
	_ASSERT(uidNPC != MUID::Invalid());
	_ASSERT(pHandler != NULL);

	map<MUID, GAIEventHandler*>::iterator it = m_mapNPCHandler.find(uidNPC);

	if(it != m_mapNPCHandler.end())
	{
		it = m_mapNPCHandler.erase(it);
	}
}

void GAIEventNotifier::NotifyField(GAIEvent aiEvent)
{
	GField* pField = gmgr.pFieldMgr->GetField(aiEvent.uidField);

	if (pField == NULL)
	{
		return;
	}

	
	// 필드의 Entity 목록에서, NPC의 경우에만 통보한다.
	GEntityMgr::ENTITY_UID_MAP mapEntity =  pField->GetEntityMap();
	for each(const GEntityMgr::ENTITY_UID_MAP::value_type& pairEntity in mapEntity)
	{
		if (pairEntity.second->IsNPC())
		{
			NotifyToUID(pairEntity.second->GetUID(), aiEvent);
		}
	}	
}

void GAIEventNotifier::NotifyNeighborSector(GAIEvent aiEvent)
{
	GField* pField = gmgr.pFieldMgr->GetField(aiEvent.uidField);

	if (pField == NULL)
	{
		return;
	}


	GFieldGrid::EntitySelector* grid_selector = 
		pField->GetGrid().GetEntitySelector();

	GVectorMUID vecEntities;
	grid_selector->GetNPCs(vecEntities.Vector(), MPoint(aiEvent.nSectorX, aiEvent.nSectorY));
	for each(const MUID& uidNPC in vecEntities.Vector())
	{
		NotifyToUID(uidNPC, aiEvent);
	}
}

void GAIEventNotifier::NotifyNPC(GAIEvent aiEvent)
{
	GEntityNPC* pEntityNPC = gmgr.pNPCMgr->GetEntityNPC(aiEvent.uidNPC);

	if (pEntityNPC == NULL)
	{
		return;
	}

	NotifyToUID(pEntityNPC->GetUID(), aiEvent);
}

void GAIEventNotifier::NotifyAll(GAIEvent aiEvent)
{
	for each(const map<MUID, GAIEventHandler*>::value_type& pairHandler in m_mapNPCHandler)
	{
		pairHandler.second->OnNotify(aiEvent);
	}
}

void GAIEventNotifier::NotifyToUID(MUID uidNPC, GAIEvent aiEvent)
{
	map<MUID, GAIEventHandler*>::iterator it = m_mapNPCHandler.find(uidNPC);

	if (it != m_mapNPCHandler.end())
	{
		it->second->OnNotify(aiEvent);
	}
}
