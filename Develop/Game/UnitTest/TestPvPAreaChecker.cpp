#include "stdafx.h"
#include "XPvPAreaChecker.h"
#include "FTestGame.h"
#include "XGame.h"
#include "XWorld.h"
#include "XFieldInfo.h"

SUITE(PvPAreaChecker)
{
	class TestPvpAreaChecker : public XPvPAreaChecker
	{
	public:
		TestPvpAreaChecker() {}
		~TestPvpAreaChecker() {}
		using XPvPAreaChecker::CheckPvPArea;
	};

	class FPvPAreaChecker
	{
	public:
		FPvPAreaChecker()
		{
			PVP_AREA_INFO area;
			area.rcArea = MRect(100, 100, 1000, 1000);
			area.nFaction1 = 100;
			area.nFaction2 = 101;

			fieldInfo.m_PvPAreas.push_back(area);

			area.rcArea = MRect(1000, 1000, 2000, 2000);
			area.nFaction1 = 100;
			area.nFaction2 = 101;

			fieldInfo.m_PvPAreas.push_back(area);
		}
		~FPvPAreaChecker()
		{

		}
		XFieldInfo			fieldInfo;
		TestPvpAreaChecker checker;
	};

	TEST_FIXTURE(FPvPAreaChecker, TestPvPAreaChecker_IsPvPArea)
	{
		vec3 vPos = vec3::ZERO;

		bool bPvPArea = checker.CheckPvPArea(&fieldInfo, vPos);
		CHECK_EQUAL(false, bPvPArea);


		// area 지역으로 들어감.
		vPos = vec3(200.0f, 200.0f, 0.0f);
		bPvPArea = checker.CheckPvPArea(&fieldInfo, vPos);
		CHECK_EQUAL(true, bPvPArea);

		vPos = vec3(1200.0f, 1200.0f, 10000.0f);
		bPvPArea = checker.CheckPvPArea(&fieldInfo, vPos);
		CHECK_EQUAL(true, bPvPArea);

		vPos = vec3(-10000.0f, 1200.0f, 10000.0f);
		bPvPArea = checker.CheckPvPArea(&fieldInfo, vPos);
		CHECK_EQUAL(false, bPvPArea);
	}

	TEST_FIXTURE(FPvPAreaChecker, TestPvPAreaChecker_PvPStateChange)
	{
		vec3 vPos = vec3::ZERO;

		bool bChange = checker.CheckPvPStateChange(0.1f, &fieldInfo, vPos);
		CHECK_EQUAL(false, bChange);


		// area 지역으로 들어감.
		vPos = vec3(200.0f, 200.0f, 0.0f);
		bChange = checker.CheckPvPStateChange(10.f, &fieldInfo, vPos);
		CHECK_EQUAL(true, bChange);

		// 여전히 pvp 지역
		vPos = vec3(1200.0f, 1200.0f, 0.0f);
		bChange = checker.CheckPvPStateChange(10.f, &fieldInfo, vPos);
		CHECK_EQUAL(false, bChange);


		// pvp 지역을 벗어남
		vPos = vec3(10000.0f, 12000.0f, 0.0f);
		bChange = checker.CheckPvPStateChange(10.f, &fieldInfo, vPos);
		CHECK_EQUAL(true, bChange);

	}
}