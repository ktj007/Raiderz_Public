#include "stdafx.h"
#include "MMath.h"
#include "MCsvParser.h"
#include "GCalculator.h"
#include "CSModifier.h"
#include "GConst.h"

SUITE(Calc)
{
	TEST(NPCRotatationTest)
	{
		vec3 v1 = vec3(0, -1, 0);
		vec3 v2 = vec3(1, 0, 0);
		float fRot = v1.AngleToXY(v2);

		mat4 matRot = mat4::IDENTITY;
		matRot.SetRotationZ(fRot);

		vec3 v3 = vec3(0, 1, 0);
		vec3 v4 = v3 * matRot;

		// v4는 (-1, 0, 0)이어야 한다.
		CHECK_CLOSE(v4.x, -1.0f, 0.001f);
		CHECK_CLOSE(v4.y, 0.0f, 0.001f);
		CHECK_CLOSE(v4.z, 0.0f, 0.001f);
	}


	TEST(TestCapsuleIntersectRay)
	{
		// test1
		vec3 t = vec3(0.0f, 2000.0f, 100.0f);
		vec3 b = vec3(0.0f, 2000.0f, 0.0f);

		float fDistance = 0.0f;
		MCapsule tCapsule(b, t, 50.0f);
		vec3 vRayOrigin = vec3(0.0f, 0.0f, 0.0f);
		vec3 vRayDir = vec3(0.0f, -1.0f, 0.0f);
		bool bPick = tCapsule.IntersectRay(vRayOrigin, vRayDir, fDistance);

		CHECK(bPick == false);
		CHECK(fDistance > 0.0f);

		// test2
		t = vec3(0.0f, 2000.0f, 100.0f);
		b = vec3(0.0f, 2000.0f, 0.0f);

		MCapsule tCapsule2(b, t, 50.0f);
		vRayOrigin = vec3(0.0f, 0.0f, 0.0f);
		vRayDir = vec3(0.0f, 1.0f, 0.0f);
		bPick = tCapsule2.IntersectRay(vRayOrigin, vRayDir, fDistance);

		CHECK(bPick == true);
		CHECK_CLOSE(fDistance, 1950.0f, 0.01f);

		// test3
		t = vec3(0.0f, 2000.0f, 100.0f);
		b = vec3(0.0f, 2000.0f, 0.0f);

		MCapsule tCapsule3(b, t, 100.0f);
		vRayOrigin = vec3(0.0f, 0.0f, 0.0f);
		vRayDir = vec3(101.0f, 2000.0f, 0.0f).Normalize();
		bPick = tCapsule3.IntersectRay(vRayOrigin, vRayDir, fDistance);

		CHECK(bPick == false);
		CHECK(fDistance < 1.0f);

	}


	TEST(TestMCSVParser)
	{
		MCsvParser parser;
		char szValue[1024];

		parser.LoadFromStream("");
		CHECK(parser.GetColCount(0) == 0);
		parser.Clear();

		{
			parser.LoadFromStream("aaa");
			CHECK(parser.GetColCount(0) == 1);
			parser.Clear();
		}

		{
			parser.LoadFromStream("111,2222,33333,444,\"555 5555\", 666");

			CHECK(parser.GetColCount(0) == 6);


			parser.GetData(0, 0, szValue, 1024);
			CHECK(!_stricmp(szValue, "111"));

			parser.GetData(4, 0, szValue, 1024);
			CHECK(!_stricmp(szValue, "555 5555"));

			CHECK(parser.GetData(999, 0, szValue, 1024) == 0);

			parser.Clear();
		}

		{
			parser.LoadFromStream(",,,,aaaaa");

			CHECK(parser.GetColCount(0) == 5);

			parser.GetData(4, 0, szValue, 1024);
			CHECK(!_stricmp(szValue, "aaaaa"));

			parser.Clear();
		}

	}

	TEST(TestMathFunctionTruncateToInt)
	{
		CHECK_EQUAL(GMath::TruncateToInt(0.0), 0);
		CHECK_EQUAL(GMath::TruncateToInt(5.6), 5);
		CHECK_EQUAL(GMath::TruncateToInt(13.2), 13);
		CHECK_EQUAL(GMath::TruncateToInt(13.2), 13);
		CHECK_EQUAL(GMath::TruncateToInt(-5.6), -6);
		CHECK_EQUAL(GMath::TruncateToInt(-2.1), -3);
		CHECK_EQUAL(GMath::TruncateToInt(-2.0), -2);
		CHECK_EQUAL(GMath::TruncateToInt(-2585858.0), -2585858);
		CHECK_EQUAL(GMath::TruncateToInt(-2533858.12312), -2533859);
	}

	TEST(TestCalcLevelFactor)
	{
		CHECK_EQUAL(GCalculator::CalcLevelFactor(1, 61), 10);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(1, 11), 10);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(1, 10), 10);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(1, 9), 10);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(1, 8), 10);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(1, 7), 10);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(1, 6), 9);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(1, 5), 8);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(1, 4), 7);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(1, 3), 6);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(1, 2), 5);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(1, 1), 5);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(2, 1), 5);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(3, 1), 4);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(4, 1), 3);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(5, 1), 2);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(6, 1), 1);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(7, 1), 0);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(8, 1), 0);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(9, 1), 0);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(10, 1), 0);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(11, 1), 0);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(91, 1), 0);
		CHECK_EQUAL(GCalculator::CalcLevelFactor(51, 13), 0);
	}

	TEST(TestCalcModGrade)
	{
		CHECK_CLOSE(GCalculator::CalcModGrade(0), 0.0f, 0.0003f);
		CHECK_CLOSE(GCalculator::CalcModGrade(1), 0.3f, 0.0003f);
		CHECK_CLOSE(GCalculator::CalcModGrade(2), 0.6f, 0.0003f);
		CHECK_CLOSE(GCalculator::CalcModGrade(3), 1.0f, 0.0003f);
		CHECK_CLOSE(GCalculator::CalcModGrade(4), 1.4f, 0.0003f);
		CHECK_CLOSE(GCalculator::CalcModGrade(5), 1.9f, 0.0003f);
		CHECK_CLOSE(GCalculator::CalcModGrade(6), 2.4f, 0.0003f);
		CHECK_CLOSE(GCalculator::CalcModGrade(7), 3.0f, 0.0003f);
		CHECK_CLOSE(GCalculator::CalcModGrade(8), 3.6f, 0.0003f);
		CHECK_CLOSE(GCalculator::CalcModGrade(9), 4.2f, 0.0003f);
		CHECK_CLOSE(GCalculator::CalcModGrade(10), 4.9f, 0.0003f);
	}


	TEST(IntModVar)
	{
		ModVar<int> nVar1;
		nVar1.nMod = 10;
		nVar1.fPer = 1.2f;

		ModVar<int> nVar2;
		nVar2.nMod = 20;
		nVar2.fPer = 1.7f;

		nVar1 += nVar2;
		CHECK_EQUAL(nVar1.nMod, 10+20);
		CHECK_EQUAL(nVar1.fPer, 1.2f+1.7f);

		nVar1 -= nVar2;
		CHECK_EQUAL(nVar1.nMod, 10);
		CHECK_EQUAL(nVar1.fPer, 1.2f);

		CHECK_EQUAL(nVar1.Calc(7), (int)((7+10)*(1.0f+1.2f)));
		int nRetVar = 8;
		nVar1.Modify(nRetVar);
		CHECK_EQUAL(nRetVar, (int)((8+10)*(1.0f+1.2f)));
	}

	TEST(TestCalcFallingDamageOnGround)
	{
		float fFallingHeight = .0f;
		int nMaxHP = 100;
		int nRetDamage = 0;

		// 땅에 떨어짐
		bool bFallenOnWater = false;

		fFallingHeight = 300;	// 3 미터
		nRetDamage = GCalculator::CalcFallingDamage(bFallenOnWater, fFallingHeight, nMaxHP);
		CHECK_EQUAL(0,		nRetDamage);

		fFallingHeight = 500;	// 5 미터
		nRetDamage = GCalculator::CalcFallingDamage(bFallenOnWater, fFallingHeight, nMaxHP);
		CHECK_EQUAL(0,		nRetDamage);

		fFallingHeight = 600;
		nRetDamage = GCalculator::CalcFallingDamage(bFallenOnWater, fFallingHeight, nMaxHP);
		CHECK_EQUAL(2,		nRetDamage);

		fFallingHeight = 1500;
		nRetDamage = GCalculator::CalcFallingDamage(bFallenOnWater, fFallingHeight, nMaxHP);
		CHECK_EQUAL(20,		nRetDamage);

		fFallingHeight = 1600;
		nRetDamage = GCalculator::CalcFallingDamage(bFallenOnWater, fFallingHeight, nMaxHP);
		CHECK_EQUAL(24,		nRetDamage);

		fFallingHeight = 3400;
		nRetDamage = GCalculator::CalcFallingDamage(bFallenOnWater, fFallingHeight, nMaxHP);
		CHECK_EQUAL(96,		nRetDamage);

		fFallingHeight = 3500;
		nRetDamage = GCalculator::CalcFallingDamage(bFallenOnWater, fFallingHeight, nMaxHP);
		CHECK_EQUAL(100,	nRetDamage);
	}
	TEST(TestCalcFallingDamageOnWater)
	{
		float fFallingHeight = .0f;
		int nMaxHP = 100;
		int nRetDamage = 0;
		bool bFallenOnWater = true;	// 물에 떨어짐

		fFallingHeight = 1000;	// 10 미터
		nRetDamage = GCalculator::CalcFallingDamage(bFallenOnWater, fFallingHeight, nMaxHP);
		CHECK_EQUAL(0,		nRetDamage);

		fFallingHeight = 1500;	// 15 미터
		nRetDamage = GCalculator::CalcFallingDamage(bFallenOnWater, fFallingHeight, nMaxHP);
		CHECK_EQUAL(0,		nRetDamage);

		fFallingHeight = 1600;
		nRetDamage = GCalculator::CalcFallingDamage(bFallenOnWater, fFallingHeight, nMaxHP);
		CHECK_EQUAL(2,		nRetDamage);

		fFallingHeight = 3500;
		nRetDamage = GCalculator::CalcFallingDamage(bFallenOnWater, fFallingHeight, nMaxHP);
		CHECK_EQUAL(40,		nRetDamage);

		fFallingHeight = 3600;
		nRetDamage = GCalculator::CalcFallingDamage(bFallenOnWater, fFallingHeight, nMaxHP);
		CHECK_EQUAL(43,		nRetDamage);

		fFallingHeight = 5400;
		nRetDamage = GCalculator::CalcFallingDamage(bFallenOnWater, fFallingHeight, nMaxHP);
		CHECK_EQUAL(97,		nRetDamage);

		fFallingHeight = 5500;
		nRetDamage = GCalculator::CalcFallingDamage(bFallenOnWater, fFallingHeight, nMaxHP);
		CHECK_EQUAL(100,	nRetDamage);
	}
	TEST(TestCalcFallingDebuffID)
	{
		float fFallingHeight = .0f;
		int nRetDebuffID = 0;

		fFallingHeight = 300;	// 3 미터
		nRetDebuffID = GCalculator::CalcFallingDebuffID(fFallingHeight);
		CHECK_EQUAL(0,	nRetDebuffID);

		fFallingHeight = 300;	// 5 미터
		nRetDebuffID = GCalculator::CalcFallingDebuffID(fFallingHeight);
		CHECK_EQUAL(0,	nRetDebuffID);

		fFallingHeight = 600;
		nRetDebuffID = GCalculator::CalcFallingDebuffID(fFallingHeight);
		CHECK_EQUAL(GConst::FALLING_DEBUFF_LV1_ID,	nRetDebuffID);

		fFallingHeight = 1500;
		nRetDebuffID = GCalculator::CalcFallingDebuffID(fFallingHeight);
		CHECK_EQUAL(GConst::FALLING_DEBUFF_LV1_ID,	nRetDebuffID);

		fFallingHeight = 1600;
		nRetDebuffID = GCalculator::CalcFallingDebuffID(fFallingHeight);
		CHECK_EQUAL(GConst::FALLING_DEBUFF_LV2_ID,	nRetDebuffID);

		fFallingHeight = 3400;
		nRetDebuffID = GCalculator::CalcFallingDebuffID(fFallingHeight);
		CHECK_EQUAL(GConst::FALLING_DEBUFF_LV2_ID,	nRetDebuffID);

		fFallingHeight = 3500;
		nRetDebuffID = GCalculator::CalcFallingDebuffID(fFallingHeight);
		CHECK_EQUAL(0,	nRetDebuffID);
	}

}