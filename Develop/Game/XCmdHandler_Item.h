#ifndef _XCMD_HANDLER_ITEM_H
#define _XCMD_HANDLER_ITEM_H

#include "MCommandHandler.h"
using namespace minet;

/// 아이템 커맨드 처리 핸들러
class XCmdHandler_Item  : public MCommandHandler
{
private:
protected:
	DECL_CMD_HANDLER(OnEquip);
	DECL_CMD_HANDLER(OnEquipSwapToInven);
	DECL_CMD_HANDLER(OnChangeInvenSlot);
	DECL_CMD_HANDLER(OnSortInvenSlot);
	DECL_CMD_HANDLER(OnUnEquip);
	DECL_CMD_HANDLER(OnItemTalent);
	DECL_CMD_HANDLER(OnAdd);
	DECL_CMD_HANDLER(OnRemove);
	DECL_CMD_HANDLER(OnChangeLookEquipItem);
	DECL_CMD_HANDLER(OnChangeLookUnEquipItem);
	DECL_CMD_HANDLER(OnSwitchingWeaponSet);
	DECL_CMD_HANDLER(OnSwitchingWeaponSetBegin);
	DECL_CMD_HANDLER(OnDye);
	DECL_CMD_HANDLER(OnUpdateDurability);
	DECL_CMD_HANDLER(OnMove);
	DECL_CMD_HANDLER(OnUseFail);
public:
	XCmdHandler_Item(MCommandCommunicator* pCC);

};



#endif