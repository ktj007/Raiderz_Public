#ifndef _XCMD_HANDLER_INTERACTION_H
#define _XCMD_HANDLER_INTERACTION_H

#include "MCommandHandler.h"
using namespace minet;

class XCmdHandler_Interaction : public MCommandHandler
{
protected:
	DECL_CMD_HANDLER(OnNPCInteraction);
	DECL_CMD_HANDLER(OnNPCInteractionEnd);
	DECL_CMD_HANDLER(OnNPCICon);
	DECL_CMD_HANDLER(OnNPCInteractionLootStart);

	
public:
	XCmdHandler_Interaction(MCommandCommunicator* pCC);	
};

#endif//_XCMD_HANDLER_INTERACTION_H
