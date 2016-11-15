#pragma once

#include "MCommandHandler.h"
using namespace minet;

/// 필드 관련 커맨드 핸들러
class ZCmdHandler_Field : public MCommandHandler
{
private:

public:
	ZCmdHandler_Field(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnRequestFieldInfoAll);
	DECL_CMD_HANDLER(OnRequestFieldGroupInfoAll);

	DECL_CMD_HANDLER(OnRequestDelDynamicFieldGroup);

	DECL_CMD_HANDLER(OnGateAskSharedField);
	DECL_CMD_HANDLER(OnGateAskDynamicField);

	DECL_CMD_HANDLER(OnGatePrepareChangeChannelReq);
	DECL_CMD_HANDLER(OnGateCancelChangeChannel);
	DECL_CMD_HANDLER(OnGateAskChangeChannel);

};

