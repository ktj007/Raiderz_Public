#ifndef _GCMD_HANDLER_TRAINING_H
#define _GCMD_HANDLER_TRAINING_H

#include "MCommandHandler.h"
using namespace minet;

/// 트레이닝 관련 커맨드 핸들러

class GCmdHandler_Training : public MCommandHandler, public MTestMemPool<GCmdHandler_Training>
{
public:
	GCmdHandler_Training(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnRequestLearnTalent);
};


#endif//_GCMD_HANDLER_TRAINING_H