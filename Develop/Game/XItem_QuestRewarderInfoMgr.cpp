#include "StdAfx.h"
#include "XItem_QuestRewarderInfoMgr.h"
#include "XItemRelatedNPCInfo.h"
#include "XNPCInfo.h"

XItem_QuestRewarderInfoMgr::XItem_QuestRewarderInfoMgr(void)
{
}

XItem_QuestRewarderInfoMgr::~XItem_QuestRewarderInfoMgr(void)
{
}

MWLua::table XItem_QuestRewarderInfoMgr::GetNPCsName( int nItemID )
{
	MWLua::table t(global.script->GetLua()->GetState());

	XItemRelatedNPCInfo* pRelatedNPC = Get(nItemID);
	if (pRelatedNPC == NULL) return t;

	char buffer[32];
	int i = 1;
	for each ( int npcID in  pRelatedNPC->m_setNPCID)
	{
		XNPCInfo* pNPCInof = info.npc->Get(npcID);
		if(pNPCInof == NULL) continue;

		sprintf(buffer,"%d", i);
		t.set(buffer, pNPCInof->GetName());

		i++;
	}

	return t;
}