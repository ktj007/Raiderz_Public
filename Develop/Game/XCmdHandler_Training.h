#ifndef _XCMD_HANDLER_TRAININIG_H
#define _XCMD_HANDLER_TRAININIG_H

#include "MCommandHandler.h"
using namespace minet;

/// 트레이닝 커맨드 처리 핸들러
class XCmdHandler_Training : public MCommandHandler
{
protected:
	DECL_CMD_HANDLER(OnTrain);
	DECL_CMD_HANDLER(OnUntrainAll);

public:
	XCmdHandler_Training(MCommandCommunicator* pCC);
};

#endif //_XCMD_HANDLER_TRAININIG_H
