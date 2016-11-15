#pragma once

#include "GBehaviorState.h"

class GStateIDIndexer
{
	typedef map<string, AI_BEHAVIOR_STATE>		BEHAVIOR_ID_MAP;
	typedef map<string, AI_COMBAT_STATE>	COMBAT_ID_MAP;
	
	BEHAVIOR_ID_MAP		m_mapBehaviorStrId;
	COMBAT_ID_MAP		m_mapCombatStrId;

public:
	GStateIDIndexer(void);
	~GStateIDIndexer(void);

	AI_BEHAVIOR_STATE& GetBehaviorID(string strName);
	AI_COMBAT_STATE& GetCombatID(string strName);

	static GStateIDIndexer& GetInstance();
};
