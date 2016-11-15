#include "stdafx.h"
#include "XPlayer.h"
#include "XMyPlayer.h"
#include "XNetPlayer.h"

SUITE(Player)
{
	TEST(TestPlayer_IsCollidableTo)
	{
		XNetPlayer		player1(MUID(34,4), 0);
		XMyPlayer		player2(MUID(34,5), 0);

		// 기본은 충돌 안함
		CHECK_EQUAL(false, player1.IsCollidableTo(&player2));
		CHECK_EQUAL(false, player2.IsCollidableTo(&player1));

		// Duel 중이면 충돌체크 함
		player1.SetDuel(true, player2.GetUID());
		player2.SetDuel(true, player1.GetUID());

		CHECK_EQUAL(true, player1.IsCollidableTo(&player2));
		CHECK_EQUAL(true, player2.IsCollidableTo(&player1));

		player1.SetDuel(false, MUID::ZERO);
		player2.SetDuel(false, MUID::ZERO);

		CHECK_EQUAL(false, player1.IsCollidableTo(&player2));
		CHECK_EQUAL(false, player2.IsCollidableTo(&player1));


		// battle arena에 있으면 충돌 체크 함
		gvar.MyInfo.BattleArenaInfo.SetInBattle();

		CHECK_EQUAL(true, player1.IsCollidableTo(&player2));
		CHECK_EQUAL(true, player2.IsCollidableTo(&player1));

		gvar.MyInfo.BattleArenaInfo.SetOutBattle();
		CHECK_EQUAL(false, player1.IsCollidableTo(&player2));
		CHECK_EQUAL(false, player2.IsCollidableTo(&player1));

	}
}