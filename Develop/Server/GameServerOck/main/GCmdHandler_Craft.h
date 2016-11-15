#ifndef _GCMD_HANDLER_CRAFT_H
#define _GCMD_HANDLER_CRAFT_H

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Craft : public MCommandHandler, public MTestMemPool<GCmdHandler_Craft>
{
public:
	GCmdHandler_Craft(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnRequestMake);
	DECL_CMD_HANDLER(OnRequestRecipeInsert);
	DECL_CMD_HANDLER(OnRequestRecipeDelete);
};

#endif//_GCMD_HANDLER_CRAFT_H
