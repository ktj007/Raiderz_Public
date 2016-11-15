#include "stdafx.h"
#include "XPlayerPVP.h"

SUITE(PlayerPVP)
{
	TEST(TestPlayerFaction_BattleArena)
	{
		XPlayerPVP f1;
		XPlayerPVP f2;

		CHECK_EQUAL(true, f1.IsAlly(&f2));

		f2.SetBattleArenaEnemy();

		CHECK_EQUAL(false, f1.IsAlly(&f2));

		f2.ReleaseBattleArenaEnemy();
		CHECK_EQUAL(true, f1.IsAlly(&f2));
	}

	TEST(TestPlayerFaction_Duel)
	{
		XPlayerPVP f1;
		XPlayerPVP f2;

		CHECK_EQUAL(true, f1.IsAlly(&f2));

		f1.SetPvPAreaFaction(true, false);
		f2.SetPvPAreaFaction(false, true);

		CHECK_EQUAL(false, f1.IsAlly(&f2));

		f2.ReleasePvPAreaFaction();
		CHECK_EQUAL(true, f1.IsAlly(&f2));

		// duel이 true이면 무조건 적
		f1.SetPvPAreaFaction(true, false);
		f2.SetPvPAreaFaction(false, true);

		CHECK_EQUAL(false, f1.IsAlly(&f2));
	}

}