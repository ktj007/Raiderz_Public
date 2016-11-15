#pragma once

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Master_Field: public MCommandHandler, public MTestMemPool<GCmdHandler_Master_Field>
{
public:
	GCmdHandler_Master_Field(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnAddSharedField);
	DECL_CMD_HANDLER(OnDelSharedField);
	DECL_CMD_HANDLER(OnFieldInfoAll);

	DECL_CMD_HANDLER(OnAddDynamicFieldGroup);
	DECL_CMD_HANDLER(OnDelDynamicFieldGroup);
	DECL_CMD_HANDLER(OnDynamicFieldGroupInfoAll);

	DECL_CMD_HANDLER(OnUpdateProxyFieldInfo);
};
