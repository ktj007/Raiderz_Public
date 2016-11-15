#pragma once

#include "XItemRelatedNPCInfoMgr.h"

class XItem_QuestRewarderInfoMgr : public XItemRelatedNPCInfoMgr
{
public:
	XItem_QuestRewarderInfoMgr(void);
	~XItem_QuestRewarderInfoMgr(void);
	MWLua::table GetNPCsName( int nItemID );
};
