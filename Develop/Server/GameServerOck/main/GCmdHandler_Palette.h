#ifndef _GCMD_HANDLER_PALETTE_H
#define _GCMD_HANDLER_PALETTE_H

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Palette : public MCommandHandler, public MTestMemPool<GCmdHandler_Palette>
{
public:
	GCmdHandler_Palette(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnRequestSelect);
	DECL_CMD_HANDLER(OnRequestSetPrimary);
	DECL_CMD_HANDLER(OnRequestSetSecondary);
	DECL_CMD_HANDLER(OnRequestPutUp);
	DECL_CMD_HANDLER(OnRequestPutDown);
	DECL_CMD_HANDLER(OnRequestChange);
};

#endif
