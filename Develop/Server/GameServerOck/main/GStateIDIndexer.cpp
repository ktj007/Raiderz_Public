#include "StdAfx.h"
#include ".\gstateidindexer.h"

GStateIDIndexer::GStateIDIndexer(void)
{
	m_mapBehaviorStrId.insert(BEHAVIOR_ID_MAP::value_type("idle", AI_BEHAVIOR_IDLE));
	m_mapBehaviorStrId.insert(BEHAVIOR_ID_MAP::value_type("patrol", AI_BEHAVIOR_PATROL));
	m_mapBehaviorStrId.insert(BEHAVIOR_ID_MAP::value_type("combat", AI_BEHAVIOR_COMBAT));
	m_mapBehaviorStrId.insert(BEHAVIOR_ID_MAP::value_type("moveto", AI_BEHAVIOR_MOVE_TO));

	m_mapCombatStrId.insert(COMBAT_ID_MAP::value_type("combat", AI_COMBAT_COMBAT));
	m_mapCombatStrId.insert(COMBAT_ID_MAP::value_type("flee", AI_COMBAT_FLEE));
	m_mapCombatStrId.insert(COMBAT_ID_MAP::value_type("yell", AI_COMBAT_YELL));
	m_mapCombatStrId.insert(COMBAT_ID_MAP::value_type("recovering", AI_COMBAT_RECOVERING));
	m_mapCombatStrId.insert(COMBAT_ID_MAP::value_type("weaken", AI_COMBAT_WEAKEN));
	m_mapCombatStrId.insert(COMBAT_ID_MAP::value_type("neutralize", AI_COMBAT_NEUTRALIZE));
	m_mapCombatStrId.insert(COMBAT_ID_MAP::value_type("asphyxia", AI_COMBAT_ASPHYXIA));
}

GStateIDIndexer::~GStateIDIndexer(void)
{
}

AI_BEHAVIOR_STATE& GStateIDIndexer::GetBehaviorID(string strName)
{
	return m_mapBehaviorStrId[strName];
}

AI_COMBAT_STATE& GStateIDIndexer::GetCombatID(string strName)
{
	return m_mapCombatStrId[strName];
}

GStateIDIndexer& GStateIDIndexer::GetInstance()
{
	static GStateIDIndexer m_Instance;
	return m_Instance;
}
