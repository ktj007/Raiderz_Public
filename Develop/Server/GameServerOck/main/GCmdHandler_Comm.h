#ifndef _GCMD_HANDLER_COMM_H
#define _GCMD_HANDLER_COMM_H

#include "MCommandHandler.h"
using namespace minet;

/// 통신관련 커맨드 핸들러
class GCmdHandler_Comm : public MCommandHandler, public MTestMemPool<GCmdHandler_Comm>
{
private:

public:
	GCmdHandler_Comm(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnRequestPrepareChangeChannel);
	DECL_CMD_HANDLER(OnRequestChangeChannel);
	DECL_CMD_HANDLER(OnRequestCancelChangeChannel);
	DECL_CMD_HANDLER(OnRequestChannelList);
	DECL_CMD_HANDLER(OnTriggerTouchSensorEnter);
	DECL_CMD_HANDLER(OnTriggerTouchSensorLeave);
	DECL_CMD_HANDLER(OnTriggerSensorDestinationSelection);
	DECL_CMD_HANDLER(OnTriggerSensorInteraction);
	DECL_CMD_HANDLER(OnTriggerSensorFollowPartyRes);	
	
	DECL_CMD_HANDLER(OnRequestEnterDynamicField);
	DECL_CMD_HANDLER(OnRequestLeaveDynamicField);

	DECL_CMD_HANDLER(OnRequestDialogSelect);
	DECL_CMD_HANDLER(OnRequestBuffLost);

	DECL_CMD_HANDLER(OnRequestWhoIs);
	DECL_CMD_HANDLER(OnRequestWhoIsUIID);
	DECL_CMD_HANDLER(OnRequestPlayerExtraInfo);

	DECL_CMD_HANDLER(OnFieldLoadingComplete);

	DECL_CMD_HANDLER(OnStartMoveToLoginServerReq);

	// 게임가드
	DECL_CMD_HANDLER(OnGameGuardAuthRes);
	DECL_CMD_HANDLER(OnGameGuardEnable);

	DECL_CMD_HANDLER(OnNetClear);
};

#endif
