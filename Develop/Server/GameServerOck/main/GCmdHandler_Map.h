#ifndef _GCMD_HANDLER_MAP_H
#define _GCMD_HANDLER_MAP_H

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Map : public MCommandHandler, public MTestMemPool<GCmdHandler_Map>
{
public:
	GCmdHandler_Map(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnRequestMiniMapMarkPos);
};

#endif//_GCMD_HANDLER_MAP_H
