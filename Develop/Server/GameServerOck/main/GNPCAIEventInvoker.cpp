#include "stdafx.h"
#include "GNPCAIEventInvoker.h"
#include "GModuleAI.h"

GNPCAIEventInvoker::GNPCAIEventInvoker(GModuleAI* pOwnerModuleAI)
: m_pOwnerModuleAI(pOwnerModuleAI)
{
	_ASSERT(pOwnerModuleAI);
}

GNPCAIEventInvoker::~GNPCAIEventInvoker()
{
	// do nothing
}

void GNPCAIEventInvoker::CombatBegin(void)
{
	InvokeEvent(GAIEvent::EVENT_ID_COMBAT_BEGIN);
}

void GNPCAIEventInvoker::CombatEnd(void)
{
	InvokeEvent(GAIEvent::EVENT_ID_COMBAT_END);
}

void GNPCAIEventInvoker::SpawnEmptyField(void)
{
	InvokeEvent(GAIEvent::EVENT_ID_SPAWN_EMPTY_FIELD);
}

void GNPCAIEventInvoker::SpawnEmptySector(void)
{
	InvokeEvent(GAIEvent::EVENT_ID_SPAWN_EMPTY_SECTOR);
}

void GNPCAIEventInvoker::SpawnPlayerSector(void)
{
	InvokeEvent(GAIEvent::EVENT_ID_SPAWN_PLAYER_SECTOR);
}

void GNPCAIEventInvoker::MakeMajorLodNPC(void)
{
	InvokeEvent(GAIEvent::EVENT_ID_MAKE_MAJOR_LOD_NPC);
}

void GNPCAIEventInvoker::InvokeEvent(GAIEvent::AI_EVENT_ID eventID)
{
	GAIEvent event;
	event.eEventID = eventID;
	event.uidNPC = m_pOwnerModuleAI->GetOwnerUID();

	GAIEventInvoker::InvokeEvent(event);
}
