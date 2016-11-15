#include "stdafx.h"
#include "GTestForward.h"
#include "FBaseGame.h"
#include "GPlayerAFK.h"

SUITE(AFK)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{
		}

		~Fixture()
		{
		}
	};

	TEST_FIXTURE(Fixture, StatusCycle)
	{
		CHECK_EQUAL(false, m_pPlayer->GetAFK().IsNowAFK());
		m_pPlayer->Update(GConst::AFK_TIMEOUT);
		CHECK_EQUAL(true, m_pPlayer->GetAFK().IsNowAFK());
		m_pPlayer->OnDoSomething();
		CHECK_EQUAL(false, m_pPlayer->GetAFK().IsNowAFK());
	}

	TEST_FIXTURE(Fixture, Toggle)
	{
		CHECK_EQUAL(false, m_pPlayer->GetAFK().IsNowAFK());
		m_pPlayer->GetAFK().Toggle();
		CHECK_EQUAL(true, m_pPlayer->GetAFK().IsNowAFK());
		m_pPlayer->GetAFK().Toggle();
		CHECK_EQUAL(false, m_pPlayer->GetAFK().IsNowAFK());
	}

	TEST_FIXTURE(Fixture, TImerResetOnAFK)
	{
		m_pPlayer->Update(GConst::AFK_TIMEOUT/2);
		CHECK_EQUAL(false, m_pPlayer->GetAFK().IsNowAFK());
		CHECK_EQUAL(GConst::AFK_TIMEOUT/2, m_pPlayer->GetAFK().GetTimeoutSecond());
		m_pPlayer->Update(GConst::AFK_TIMEOUT/2);
		CHECK_EQUAL(true, m_pPlayer->GetAFK().IsNowAFK());
		CHECK_EQUAL(0.0f, m_pPlayer->GetAFK().GetTimeoutSecond());
	}

	TEST_FIXTURE(Fixture, TImerResetOnMoved)
	{
		m_pPlayer->Update(GConst::AFK_TIMEOUT/2);
		CHECK_EQUAL(false, m_pPlayer->GetAFK().IsNowAFK());
		CHECK_EQUAL(GConst::AFK_TIMEOUT/2, m_pPlayer->GetAFK().GetTimeoutSecond());
		m_pPlayer->OnDoSomething();
		CHECK_EQUAL(false, m_pPlayer->GetAFK().IsNowAFK());
		CHECK_EQUAL(0.0f, m_pPlayer->GetAFK().GetTimeoutSecond());
	}

	TEST_FIXTURE(Fixture, TImerResetOnDie)
	{
		m_pPlayer->Update(GConst::AFK_TIMEOUT/2);
		CHECK_EQUAL(false, m_pPlayer->GetAFK().IsNowAFK());
		CHECK_EQUAL(GConst::AFK_TIMEOUT/2, m_pPlayer->GetAFK().GetTimeoutSecond());
		m_pPlayer->doDie();
		CHECK_EQUAL(false, m_pPlayer->GetAFK().IsNowAFK());
		CHECK_EQUAL(0.0f, m_pPlayer->GetAFK().GetTimeoutSecond());
	}
}