#include "stdafx.h"
#include "GNPCAILod.h"

GNPCAILod::GNPCAILod()
: m_aiLod(AI_LOD_LEVEL_5)
, m_aiOldLod(AI_LOD_LEVEL_5)
{
	// do nothing
}

GNPCAILod::~GNPCAILod()
{
	// do nothing
}

void GNPCAILod::OnNotify(GAIEvent aiEvent)
{
	// 전이표	
	const int AI_LOD_REMAINS	= -1;
	const int AI_LOD_COMBAT		= -2;
	const int AI_LOD_OLD		= -3;

	const int transitionTable[GAIEvent::MAX_EVENT_ID][MAX_AI_LOD_LEVEL] =
	{
		{ AI_LOD_REMAINS, AI_LOD_REMAINS, AI_LOD_REMAINS, AI_LOD_REMAINS, AI_LOD_LEVEL_4},	// EVENT_ID_FIELD_ENTER_PLAYER
		{ AI_LOD_REMAINS, AI_LOD_LEVEL_5, AI_LOD_REMAINS, AI_LOD_LEVEL_5, AI_LOD_REMAINS},	// EVENT_ID_FIELD_LEAVE_PLAYER,	
		{ AI_LOD_REMAINS, AI_LOD_REMAINS, AI_LOD_REMAINS, AI_LOD_LEVEL_2, AI_LOD_LEVEL_2},	// EVENT_ID_SECTOR_ENTER_PLAYER,	 
		{ AI_LOD_REMAINS, AI_LOD_LEVEL_4, AI_LOD_REMAINS, AI_LOD_REMAINS, AI_LOD_REMAINS},	// EVENT_ID_SECTOR_LEAVE_PLAYER,	 
		{ AI_LOD_COMBAT,  AI_LOD_COMBAT,  AI_LOD_COMBAT,  AI_LOD_COMBAT,  AI_LOD_COMBAT},	// EVENT_ID_BATTLE_BEGIN,
		{ AI_LOD_OLD,     AI_LOD_OLD,     AI_LOD_OLD,     AI_LOD_OLD,     AI_LOD_OLD},		// EVENT_ID_BATTLE_END,
		{ AI_LOD_REMAINS, AI_LOD_REMAINS, AI_LOD_REMAINS, AI_LOD_LEVEL_2, AI_LOD_LEVEL_2},	// EVENT_ID_SECTOR_MOVE_BE_PLAYER,	 
		{ AI_LOD_REMAINS, AI_LOD_LEVEL_4, AI_LOD_REMAINS, AI_LOD_REMAINS, AI_LOD_REMAINS},	// EVENT_ID_SECTOR_MOVE_EMPTY_PLAYER 
		{ AI_LOD_LEVEL_5, AI_LOD_LEVEL_5, AI_LOD_REMAINS, AI_LOD_LEVEL_5, AI_LOD_LEVEL_5},	// EVENT_ID_SPAWN_EMPTY_FIELD
		{ AI_LOD_LEVEL_4, AI_LOD_LEVEL_4, AI_LOD_REMAINS, AI_LOD_LEVEL_4, AI_LOD_LEVEL_4},	// EVENT_ID_SPAWN_EMPTY_SECTOR
		{ AI_LOD_LEVEL_2, AI_LOD_LEVEL_2, AI_LOD_REMAINS, AI_LOD_LEVEL_2, AI_LOD_LEVEL_2},	// EVENT_ID_SPAWN_PLAYER_SECTOR
		{ AI_LOD_LEVEL_3, AI_LOD_LEVEL_3, AI_LOD_REMAINS, AI_LOD_LEVEL_3, AI_LOD_LEVEL_3},	// EVENT_ID_MAKE_EVENT_NPC
	};


	// 전이 - 전투 중에도 처리 가능하도록, OldLOD로 계산한다.
	int nTransAILod = transitionTable[aiEvent.eEventID][m_aiOldLod];

	switch(nTransAILod)
	{
	case AI_LOD_REMAINS:	return;									// 현재 LOD 유지
	case AI_LOD_COMBAT:		m_aiLod = AI_LOD_LEVEL_1;	break;		// 전투 LOD 
	case AI_LOD_OLD:		m_aiLod = m_aiOldLod;		break;		// 전투 전 LOD로 복귀
	default:
		m_aiLod = (AI_LOD_LEVEL)nTransAILod;
		m_aiOldLod = m_aiLod;
		break;
	}
}

AI_LOD_LEVEL GNPCAILod::GetLodLevel(void) const
{
	return m_aiLod;
}

wstring GNPCAILod::GetName(AI_LOD_LEVEL aiLod) const
{
	switch(aiLod)
	{
	case AI_LOD_LEVEL_1:	return wstring(L"AI_LOD_LEVEL_1");
	case AI_LOD_LEVEL_2:	return wstring(L"AI_LOD_LEVEL_2");
	case AI_LOD_LEVEL_3:	return wstring(L"AI_LOD_LEVEL_3");
	case AI_LOD_LEVEL_4:	return wstring(L"AI_LOD_LEVEL_4");
	case AI_LOD_LEVEL_5:	return wstring(L"AI_LOD_LEVEL_5");
	default:
		break;
	}

	return wstring();
}
