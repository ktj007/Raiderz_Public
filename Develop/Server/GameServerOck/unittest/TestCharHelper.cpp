#include "stdafx.h"
#include "CSCharHelper.h"

SUITE(CharHelper)
{
	TEST(TestCharHelper_IsDeveloperGrade)
	{
		CHECK_EQUAL(false, CSCharHelper::IsDeveloperGrade(PLAYER_GRADE_NORMAL));
		CHECK_EQUAL(false, CSCharHelper::IsDeveloperGrade(PLAYER_GRADE_GM));
		CHECK_EQUAL(true, CSCharHelper::IsDeveloperGrade(PLAYER_GRADE_TESTER));
		CHECK_EQUAL(true, CSCharHelper::IsDeveloperGrade(PLAYER_GRADE_DEVELOPER));
	}

	TEST(TestCharHelper_CalcValidInteractionDistance)
	{
		// 1차
		float fTarColRadius = 40.0f;
		float fValidDist = CSCharHelper::CalcValidInteractionDistance(fTarColRadius, 0);
		CHECK_EQUAL((fTarColRadius * GAME_INTERACTION_DISTANCE_XY_MULTIPLICATION) + GAME_INTERACTION_DISTANCE_XY_ADDITION , fValidDist);

		// 2차
		fTarColRadius = 0.0f;
		fValidDist = CSCharHelper::CalcValidInteractionDistance(fTarColRadius, 0);
		CHECK_EQUAL((fTarColRadius * GAME_INTERACTION_DISTANCE_XY_MULTIPLICATION) + GAME_INTERACTION_DISTANCE_XY_ADDITION , fValidDist);

		// 3차
		fTarColRadius = 500.0f;
		fValidDist = CSCharHelper::CalcValidInteractionDistance(fTarColRadius, 0);
		CHECK_EQUAL((fTarColRadius * GAME_INTERACTION_DISTANCE_XY_MULTIPLICATION) + GAME_INTERACTION_DISTANCE_XY_ADDITION , fValidDist);

	}

	TEST(TestCharHelper_CalcValidInteractionDistance_NPCInfoInteractionDistance)
	{
		// 1차
		float fTarColRadius = 40.0f;
		float fValidDist = CSCharHelper::CalcValidInteractionDistance(fTarColRadius, 100);
		CHECK_EQUAL(100.0f , fValidDist);

		// 2차
		fTarColRadius = 0.0f;
		fValidDist = CSCharHelper::CalcValidInteractionDistance(fTarColRadius, 1);
		CHECK_EQUAL(1.0f, fValidDist);

		// 3차
		fTarColRadius = 500.0f;
		fValidDist = CSCharHelper::CalcValidInteractionDistance(fTarColRadius, 200);
		CHECK_EQUAL(200.0f, fValidDist);

	}

}