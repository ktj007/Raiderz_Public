#include "stdafx.h"
#include "XMyActionDieTransChecker.h"
#include "FTestGame.h"

SUITE(MyActionDieTransChecker)
{
	class TestMyActionDieTransChecker : public XMyActionDieTransChecker
	{
	public:
		using XMyActionDieTransChecker::CheckReservedHitByTalent;
		using XMyActionDieTransChecker::CheckMotion;
		using XMyActionDieTransChecker::CheckDead;
		using XMyActionDieTransChecker::CheckForceToDead;
		using XMyActionDieTransChecker::_IsReady;
		using XMyActionDieTransChecker::_IsReadyForHitState;
	};

	class FMyActionDieTransChecker : public FTestGame
	{
	public:
		FMyActionDieTransChecker()
		{

		}
		virtual ~FMyActionDieTransChecker()
		{

		}

		TestMyActionDieTransChecker		m_TransChecker;
	};

	TEST_FIXTURE(FMyActionDieTransChecker, TestMyActionDieTransChecker_CheckReservedHitByTalent)
	{
		CHECK_EQUAL(true, m_TransChecker.CheckReservedHitByTalent(false));
		CHECK_EQUAL(false, m_TransChecker.CheckReservedHitByTalent(true));
	}

	TEST_FIXTURE(FMyActionDieTransChecker, TestMyActionDieTransChecker_CheckMotion)
	{
		CHECK_EQUAL(true, m_TransChecker.CheckMotion(true, wstring(L"idle")));
		CHECK_EQUAL(true, m_TransChecker.CheckMotion(true, wstring(L"idle2")));
		CHECK_EQUAL(true, m_TransChecker.CheckMotion(false , wstring(MOTION_NAME_DIE)));

		CHECK_EQUAL(false, m_TransChecker.CheckMotion(false, wstring(L"idle")));
	}


	TEST_FIXTURE(FMyActionDieTransChecker, TestMyActionDieTransChecker_CheckDead)
	{
		CHECK_EQUAL(true, m_TransChecker.CheckDead(true));
		CHECK_EQUAL(false, m_TransChecker.CheckDead(false));
	}

	TEST_FIXTURE(FMyActionDieTransChecker, TestMyActionDieTransChecker_CheckForceToDead)
	{
		CHECK_EQUAL(false, m_TransChecker.CheckForceToDead(0.0f));
		CHECK_EQUAL(false, m_TransChecker.CheckForceToDead(0.5f));
		CHECK_EQUAL(true, m_TransChecker.CheckForceToDead(300.0f));
	}

	TEST_FIXTURE(FMyActionDieTransChecker, TestMyActionDieTransChecker__IsReady)
	{
		CHECK_EQUAL(true, m_TransChecker._IsReady(0.01f, true, false));


		CHECK_EQUAL(true, m_TransChecker._IsReady(300.0f, true, true));
	}

	TEST_FIXTURE(FMyActionDieTransChecker, TestMyActionDieTransChecker__IsReady_Negative)
	{
		CHECK_EQUAL(false, m_TransChecker._IsReady(0.01f, false, false));
		CHECK_EQUAL(false, m_TransChecker._IsReady(0.01f, true, true));
		CHECK_EQUAL(false, m_TransChecker._IsReady(300.0f, false, false));
	}

	TEST_FIXTURE(FMyActionDieTransChecker, TestMyActionDieTransChecker__IsReadyForHitState)
	{
		CHECK_EQUAL(true, m_TransChecker._IsReadyForHitState(0.01f, false , true, false, false, wstring(L"")));
		CHECK_EQUAL(true, m_TransChecker._IsReadyForHitState(0.01f, false , true, false, true, wstring(L"")));

		// Check Motion
		CHECK_EQUAL(true, m_TransChecker._IsReadyForHitState(0.01f, true , true, false, true, wstring(L"")));
		CHECK_EQUAL(true, m_TransChecker._IsReadyForHitState(0.01f, true , true, false, false, wstring(MOTION_NAME_DIE)));
	}


	TEST_FIXTURE(FMyActionDieTransChecker, TestMyActionDieTransChecker__IsReadyForHitState_Negative)
	{
		CHECK_EQUAL(false, m_TransChecker._IsReadyForHitState(0.01f, false , false, false, false, wstring(L"")));
		CHECK_EQUAL(false, m_TransChecker._IsReadyForHitState(0.01f, false , true, true, true, wstring(L"")));

		// Check Motion
		CHECK_EQUAL(false, m_TransChecker._IsReadyForHitState(0.01f, true , true, false, false, wstring(L"")));
	}

}