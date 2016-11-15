#ifndef _XCMD_HANDLER_LOOT_H_
#define _XCMD_HANDLER_LOOT_H_

#include "McommandHandler.h"
using namespace minet;

/// 루팅에서 사용하는 커맨드 처리
class XCmdHandler_Loot : public MCommandHandler
{
protected:
	DECL_CMD_HANDLER(OnBegin);
	DECL_CMD_HANDLER(OnSeeItem);
	DECL_CMD_HANDLER(OnMasterLootUI);
	DECL_CMD_HANDLER(OnMasterLootNotify);
	DECL_CMD_HANDLER(OnRollResult);
	DECL_CMD_HANDLER(OnRefreshItem);
	DECL_CMD_HANDLER(OnOtherGainItem);
	DECL_CMD_HANDLER(OnGettableItemAdd);
	DECL_CMD_HANDLER(OnEnd);	

	DECL_CMD_HANDLER(OnInsertLootableNPC);
	DECL_CMD_HANDLER(OnRemoveLootableNPC);	

	static wstring GetLootItemList(const vector<TD_ITEM>& vecTDItem, wstring szText);
public:
	XCmdHandler_Loot(MCommandCommunicator* pCC);
};


#endif//_XCMD_HANDLER_LOOT_H_