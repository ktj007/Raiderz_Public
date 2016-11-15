#ifndef _XCMD_HANDLER_SOUL_H
#define _XCMD_HANDLER_SOUL_H

#include "MCommandHandler.h"
using namespace minet;

class XCmdHandler_Soul  : public MCommandHandler
{
private:

protected:
	DECL_CMD_HANDLER(OnDrainMe);
	DECL_CMD_HANDLER(OnDrainOther);

public:
	XCmdHandler_Soul(MCommandCommunicator* pCC);
};



#endif