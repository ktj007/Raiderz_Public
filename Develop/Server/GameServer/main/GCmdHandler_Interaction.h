#ifndef _GCMD_HANDLER_INTERACTION_H
#define _GCMD_HANDLER_INTERACTION_H

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Interaction : public MCommandHandler, public MTestMemPool<GCmdHandler_Interaction>
{
public:
	GCmdHandler_Interaction(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnInteraction);
	DECL_CMD_HANDLER(OnNPCInteractionElement);
	DECL_CMD_HANDLER(OnNPCInteractionEnd);
};

#endif//_GCMD_HANDLER_INTERACTION_H
