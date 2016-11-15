#ifndef _XCMD_HANDLER_MAP_H
#define _XCMD_HANDLER_MAP_H

#include "MCommandHandler.h"
using namespace minet;

class XCmdHandler_Map : public MCommandHandler
{
protected:
	DECL_CMD_HANDLER(OnMiniMapMarkPos);	

public:
	XCmdHandler_Map(MCommandCommunicator* pCC);
};

#endif