#include "stdafx.h"
#include "CSBasicAttributeCalculator.h"

SUITE(BasicAttributeCalculator)
{
	TEST(TestCalculator_CalcMaxHP)
	{
		// BASE_HP_RACE_HUMAN = 50
		// http://iworks2.maiet.net/wiki/sh:formulas:hp

		const int nMaxLevel = 50;

		// 소울헌트-각종수식시뮬레이션.xls

int nExpectedHP[nMaxLevel] = 
{
300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 
400, 410, 420, 430, 440, 450, 460, 470, 480, 490,
500, 510, 520, 530, 540, 550, 560, 570, 580, 590,
600, 610, 620, 630, 640, 650, 660, 670, 680, 690,
700, 710, 720, 730, 740, 750, 760, 770, 780, 790
};

		for (int nLevel = 1; nLevel <= nMaxLevel; nLevel++)
		{
			CHECK_EQUAL(nExpectedHP[nLevel-1], CSBasicAttributeCalculator::CalcMaxHP(nLevel, PLAYER_DEFAULT_CON));
		}
	}

	TEST(TestCalculator_CalcMaxHP_CON)
	{
		// http://iworks2.maiet.net/wiki/sh:formulas:hp

		int nLevel = 1; int nCon = 50;
		CHECK_EQUAL(300, CSBasicAttributeCalculator::CalcMaxHP(nLevel, nCon));

		nLevel = 1; nCon = 25;
		CHECK_EQUAL(BASE_HP_RACE_HUMAN + (10 * 0) + (0 * 10), CSBasicAttributeCalculator::CalcMaxHP(nLevel, nCon));

		nLevel = 10; nCon = 60;
		CHECK_EQUAL(BASE_HP_RACE_HUMAN + (10 * 9) + (35 * 10), CSBasicAttributeCalculator::CalcMaxHP(nLevel, nCon));

		nLevel = 35; nCon = 100;
		CHECK_EQUAL(BASE_HP_RACE_HUMAN + (10 * 34) + (75 * 10), CSBasicAttributeCalculator::CalcMaxHP(nLevel, nCon));

		nLevel = 49; nCon = 90;
		CHECK_EQUAL(BASE_HP_RACE_HUMAN + (10 * 48) + (65 * 10), CSBasicAttributeCalculator::CalcMaxHP(nLevel, nCon));

	}


	TEST(TestCalculator_CalcMaxEN)
	{
		// BASE_EN_RACE_HUMAN = 50
		// http://iworks2.maiet.net/wiki/sh:formulas:en_n_sp

		const int nMaxLevel = 50;

		// 소울헌트-각종수식시뮬레이션.xls

int nExpectedEN[nMaxLevel] = 
{
200, 202, 204, 206, 208, 210, 212, 214, 216, 218, 
220, 222, 224, 226, 228, 230, 232, 234, 236, 238, 
240, 242, 244, 246, 248, 250, 252, 254, 256, 258, 
260, 262, 264, 266, 268, 270, 272, 274, 276, 278, 
280, 282, 284, 286, 288, 290, 292, 294, 296, 298
};

		for (int nLevel = 1; nLevel <= nMaxLevel; nLevel++)
		{
			CHECK_EQUAL(nExpectedEN[nLevel-1], CSBasicAttributeCalculator::CalcMaxEN(nLevel, PLAYER_DEFAULT_INT));
		}
	}

	TEST(TestCalculator_CalcMaxEN_INT)
	{
		// BASE_EN_RACE_HUMAN = 50
		// http://iworks2.maiet.net/wiki/sh:formulas:en_n_sp

		int nLevel = 1; int nINT = 50;
		CHECK_EQUAL(BASE_EN_RACE_HUMAN + (2 * 0) + (30 * 5), CSBasicAttributeCalculator::CalcMaxEN(nLevel, nINT));

		nLevel = 1; nINT = 60;
		CHECK_EQUAL(BASE_EN_RACE_HUMAN + (2 * 0) + (40 * 5), CSBasicAttributeCalculator::CalcMaxEN(nLevel, nINT));

		nLevel = 15; nINT = 50;
		CHECK_EQUAL(BASE_EN_RACE_HUMAN + (2 * 14) + (30 * 5), CSBasicAttributeCalculator::CalcMaxEN(nLevel, nINT));

		nLevel = 31; nINT = 74;
		CHECK_EQUAL(BASE_EN_RACE_HUMAN + (2 * 30) + (54 * 5), CSBasicAttributeCalculator::CalcMaxEN(nLevel, nINT));

		nLevel = 50; nINT = 92;
		CHECK_EQUAL(BASE_EN_RACE_HUMAN + (2 * 49) + (72 * 5), CSBasicAttributeCalculator::CalcMaxEN(nLevel, nINT));

	}


	TEST(TestCalculator_CalcMaxSP)
	{
		// BASE_SP_RACE_HUMAN = 50
		// http://iworks2.maiet.net/wiki/sh:formulas:en_n_sp

		//const int nMaxLevel = 50;

		//// 소울헌트-각종수식시뮬레이션.xls

		//int nExpectedSP[nMaxLevel] = 
		//{
		//	200, 202, 204, 206, 208, 210, 212, 214, 216, 218, 
		//	220, 222, 224, 226, 228, 230, 232, 234, 236, 238, 
		//	240, 242, 244, 246, 248, 250, 252, 254, 256, 258, 
		//	260, 262, 264, 266, 268, 270, 272, 274, 276, 278, 
		//	280, 282, 284, 286, 288, 290, 292, 294, 296, 298
		//};

		//for (int nLevel = 1; nLevel <= nMaxLevel; nLevel++)
		//{
		//	CHECK_EQUAL(nExpectedSP[nLevel-1], CSBasicAttributeCalculator::CalcMaxSP(nLevel, PLAYER_DEFAULT_DEX));
		//}

		CHECK_EQUAL(100, CSBasicAttributeCalculator::CalcMaxSP(50, PLAYER_DEFAULT_DEX));
	}

	TEST(TestCalculator_CalcMaxEN_DEX)
	{
		// BASE_SP_RACE_HUMAN = 50
		// http://iworks2.maiet.net/wiki/sh:formulas:en_n_sp

		int nLevel = 1; int nDEX = 50;
		CHECK_EQUAL(BASE_SP_RACE_HUMAN + (2 * 0) + (30 * 5), CSBasicAttributeCalculator::CalcMaxEN(nLevel, nDEX));

		nLevel = 1; nDEX = 55;
		CHECK_EQUAL(BASE_SP_RACE_HUMAN + (2 * 0) + (35 * 5), CSBasicAttributeCalculator::CalcMaxEN(nLevel, nDEX));

		nLevel = 18; nDEX = 50;
		CHECK_EQUAL(BASE_SP_RACE_HUMAN + (2 * 17) + (30 * 5), CSBasicAttributeCalculator::CalcMaxEN(nLevel, nDEX));

		nLevel = 50; nDEX = 92;
		CHECK_EQUAL(BASE_SP_RACE_HUMAN + (2 * 49) + (72 * 5), CSBasicAttributeCalculator::CalcMaxEN(nLevel, nDEX));

	}
}