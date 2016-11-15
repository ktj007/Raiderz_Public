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
	DECL_CMD_HANDLER(OnRequsetItemInteractNPC);
	DECL_CMD_HANDLER(OnRequestMove);
	DECL_CMD_HANDLER(OnRequsetEnchant);
	DECL_CMD_HANDLER(OnRequsetEnchantCheck);
	DECL_CMD_HANDLER(OnRequsetEnchantPrepare);
	DECL_CMD_HANDLER(OnRequsetEnchantCancel);
};

#endif//#define _GCMD_HANDLER_ITEM_H