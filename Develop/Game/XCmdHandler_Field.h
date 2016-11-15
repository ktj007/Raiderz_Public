#ifndef _XCMD_HANDLER_FIELD_H
#define _XCMD_HANDLER_FIELD_H

#include "MCommandHandler.h"
using namespace minet;

/// 필드 커맨드 처리 핸들러
class XCmdHandler_Field  : public MCommandHandler
{
public:
	XCmdHandler_Field(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnChannelList);
	DECL_CMD_HANDLER(OnPreparingToLoad);
	DECL_CMD_HANDLER(OnStartGame);
	DECL_CMD_HANDLER(OnChangeField);
	DECL_CMD_HANDLER(OnChannelChangePrepareRes);
	DECL_CMD_HANDLER(OnChannelChangePrepareFailed);
	DECL_CMD_HANDLER(OnCancelChangeField);

	DECL_CMD_HANDLER(OnDynamicFieldExpiring);
	DECL_CMD_HANDLER(OnDynamicFieldCancelExpire);
		
	DECL_CMD_HANDLER(OnSetTime);
	DECL_CMD_HANDLER(OnSetWeather);
	DECL_CMD_HANDLER(OnSetTimeWeather);

	DECL_CMD_HANDLER(OnInPlayer);
	DECL_CMD_HANDLER(OnOutPlayer);
	DECL_CMD_HANDLER(OnInNPC);
	DECL_CMD_HANDLER(OnOutNPC);
	DECL_CMD_HANDLER(OnInBPart);
	DECL_CMD_HANDLER(OnOutBPart);
	DECL_CMD_HANDLER(OnInMagicArea);
	DECL_CMD_HANDLER(OnOutMagicArea);
	DECL_CMD_HANDLER(OnTrapTriggered);
};



#endif