#ifndef _GCMD_HANDLER_INN_H
#define _GCMD_HANDLER_INN_H

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Inn : public MCommandHandler, public MTestMemPool<GCmdHandler_Inn>
{
public:
	GCmdHandler_Inn(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnBeginSleepReq);
};

#endif//_GCMD_HANDLER_INN_H
