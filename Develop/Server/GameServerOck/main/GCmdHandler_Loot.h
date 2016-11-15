#ifndef _GCMD_HANDLER_LOOT_H
#define _GCMD_HANDLER_LOOT_H

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Loot : public MCommandHandler, public MTestMemPool<GCmdHandler_Loot>
{
public:
	GCmdHandler_Loot(MCommandCommunicator* pCC);
	
	DECL_CMD_HANDLER(OnBegin);
	DECL_CMD_HANDLER(OnSeeItem);
	DECL_CMD_HANDLER(OnLootItem);
	DECL_CMD_HANDLER(OnMasterLootItem);
	DECL_CMD_HANDLER(OnRequestEnd);
};

#endif//_GCMD_HANDLER_LOOT_H
