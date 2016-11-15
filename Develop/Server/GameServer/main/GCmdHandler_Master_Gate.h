#pragma once

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Master_Gate: public MCommandHandler, public MTestMemPool<GCmdHandler_Master_Gate>
{
public:
	GCmdHandler_Master_Gate(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnGateMoveServer);
	DECL_CMD_HANDLER(OnGateRes);
	DECL_CMD_HANDLER(OnGatePrepareChangeChannelRes);
};
