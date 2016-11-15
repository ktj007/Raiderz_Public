#include "stdafx.h"
#include "XBattleArenaInfo.h"
#include "XGlobalVar_MyInfo.h"

SUITE(BattleArena)
{
	TEST(TestBattleArenaInfo)
	{
		XBattleArenaInfo info;
		info.SetInBattle();

		CHECK_EQUAL(true, info.IsInBattleArena());

		info.SetOutBattle();
		CHECK_EQUAL(false, info.IsInBattleArena());
	}
}