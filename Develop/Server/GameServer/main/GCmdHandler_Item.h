#ifndef _GCMD_HANDLER_ITEM_H
#define _GCMD_HANDLER_ITEM_H

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Item : public MCommandHandler, public MTestMemPool<GCmdHandler_Item>
{
public:
	GCmdHandler_Item(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnRequestEquip);
	DECL_CMD_HANDLER(OnRequestUnEquip);	
	DECL_CMD_HANDLER(OnRequestDyePrepare);
	DECL_CMD_HANDLER(OnRequestDyePrepareCancel);
	DECL_CMD_HANDLER(OnRequestDye);
	DECL_CMD_HANDLER(OnRequestUse);
	DECL_CMD_HANDLER(OnRequestDrop);
	DECL_CMD_HANDLER(OnRequestSortInventorySlot);
	DECL_CMD_HANDLER(OnRequestItemInteractNPC);
	DECL_CMD_HANDLER(OnRequestMove);
	DECL_CMD_HANDLER(OnRequestEnchant);
	DECL_CMD_HANDLER(OnRequestEnchantCheck);
	DECL_CMD_HANDLER(OnRequestEnchantPrepare);
	DECL_CMD_HANDLER(OnRequestEnchantCancel);
	DECL_CMD_HANDLER(OnRequestAttune);
	DECL_CMD_HANDLER(OnRequestAttunePrepare);
	DECL_CMD_HANDLER(OnRequestAttuneCheck);
	DECL_CMD_HANDLER(OnRequestAttuneCancel);
};

#endif//#define _GCMD_HANDLER_ITEM_H