#include "stdafx.h"
#include "GTestForward.h"
#include "FBaseGame.h"
#include "GPlayerRide.h"
#include "GModuleBuff.h"
#include "GBuff.h"

SUITE(PlayerRide)
{
	struct Fixture : FBaseGame
	{
		Fixture()
		{
			GConst::DISMOUNT_BUFFID = 777;
			pDismountBuff = test.buff.NewBuffInfo(GConst::DISMOUNT_BUFFID);
			pDismountBuff->m_nActiveExtraAttrib = BUFAEA_DISMOUNT;
		}
		~Fixture()
		{
			GConst::DISMOUNT_BUFFID = INVALID_ID;
		}

		GBuffInfo* Mount(int nTestMode = 0)
		{
			GBuffInfo* pBuff = test.buff.NewBuffInfo();
			pBuff->m_nPassiveExtraAttrib = BUFPEA_RIDE;
			pBuff->m_fPassiveExtraAttribValue = (float)nTestMode;
			test.buff.GainBuffSelf(m_pPlayer, pBuff);
			return pBuff;
		}

		void DIsmount()
		{
			m_pPlayer->GetRide().Dismount();
			m_pPlayer->GetModuleBuff()->Update(0.1f);
		}

		GBuffInfo* pDismountBuff;
	};

	TEST_FIXTURE(Fixture, Cooking)
	{
		GBuffInfo* pBuff = test.buff.NewBuffInfo();
		CHECK_EQUAL(false, pBuff->IsRideBuff());
		
		pBuff->m_nPassiveExtraAttrib = BUFPEA_RIDE;
		CHECK_EQUAL(true, pBuff->IsRideBuff());

		pBuff->m_nPassiveExtraAttrib = BUFPEA_NONE;
		pBuff->m_nPassiveExtraAttrib2 = BUFPEA_RIDE;
		CHECK_EQUAL(true, pBuff->IsRideBuff());
	}

	TEST_FIXTURE(Fixture, LifeCycle)
	{
		GBuffInfo* pBuff = test.buff.NewBuffInfo();
		pBuff->m_nPassiveExtraAttrib = BUFPEA_RIDE;
		CHECK_EQUAL(false, m_pPlayer->GetRide().IsMounted());
		test.buff.GainBuffSelf(m_pPlayer, pBuff);
		CHECK_EQUAL(true, m_pPlayer->GetRide().IsMounted());
		DIsmount();
		CHECK_EQUAL(false, m_pPlayer->GetRide().IsMounted());
	}

	TEST_FIXTURE(Fixture, CheckRideSpeed)
	{
		CHECK_EQUAL(375, m_pPlayer->GetMoveSpeed());
		GBuffInfo* pBuffMount = 
			Mount();
		pBuffMount->m_ActorModifier.fMoveSpeed = 777;
		CHECK_EQUAL(777, m_pPlayer->GetMoveSpeed());
		DIsmount();
		CHECK_EQUAL(375, m_pPlayer->GetMoveSpeed());
	}

	TEST_FIXTURE(Fixture, DIsmountBuff)
	{
		Mount();
		CHECK_EQUAL(true, m_pPlayer->GetRide().IsMounted());
		test.buff.GainBuffSelf(m_pPlayer, pDismountBuff);
		m_pPlayer->GetModuleBuff()->Update(0.1f);
		CHECK_EQUAL(false, m_pPlayer->GetRide().IsMounted());
	}

	TEST_FIXTURE(Fixture, RestoreWalkingSpeedWhileCombat)
	{
		GBuffInfo* pBuffMount = 
			Mount(1); // mode1만 유효
		pBuffMount->m_ActorModifier.fMoveSpeed = 777;
		CHECK_EQUAL(true, m_pPlayer->GetRide().IsMounted());
		GEntityNPC* pNPC = test.npc.SpawnNPC(m_pField);
		CHECK_EQUAL(777, m_pPlayer->GetMoveSpeed());
		MakeCombat(m_pPlayer, pNPC);
		CHECK_EQUAL(375, m_pPlayer->GetMoveSpeed());
		MakePeace(m_pPlayer, pNPC);
		CHECK_EQUAL(777, m_pPlayer->GetMoveSpeed());
	}

	TEST_FIXTURE(Fixture, DismountWhenBeginCombat)
	{
		Mount(2); // mode2만 유효
		CHECK_EQUAL(true, m_pPlayer->GetRide().IsMounted());
		GEntityNPC* pNPC = test.npc.SpawnNPC(m_pField);
		MakeCombat(m_pPlayer, pNPC);
		m_pPlayer->GetModuleBuff()->Update(0.1f);
		CHECK_EQUAL(false, m_pPlayer->GetRide().IsMounted());
	}

	TEST_FIXTURE(Fixture, DismountWhenHit)
	{
		Mount(3); // mode3만 유효
		CHECK_EQUAL(true, m_pPlayer->GetRide().IsMounted());
		GEntityNPC* pNPC = test.npc.SpawnNPC(m_pField);
		test.actor.Hit(pNPC, m_pPlayer);
		m_pPlayer->GetModuleBuff()->Update(0.1f);

		class FixedDismountHandler : public GPlayerRide::RandomDismountHandler
		{
		public:
			virtual float CalcDismountRate( GEntityActor* pAttacker, GEntityActor* pVictim ) override
			{
				return 100.0f; // 무조건 내림
			}
		};

		m_pPlayer->GetRide().SetRandomHandler(new FixedDismountHandler());
		CHECK_EQUAL(false, m_pPlayer->GetRide().IsMounted());
	}

	TEST_FIXTURE(Fixture, DismountAfterCombatElapsedFiveSecond)
	{
		Mount(105);
		GEntityNPC* pNPC = test.npc.SpawnNPC(m_pField);
		MakeCombat(m_pPlayer, pNPC);
		GBuff* pBuff = m_pPlayer->GetModuleBuff()->FindBuffByID(GConst::DISMOUNT_BUFFID);
		TVALID(pBuff);
		CHECK_EQUAL(5, pBuff->GetDurationTime());
	}

	TEST_FIXTURE(Fixture, CalcDismountRate)
	{
		GEntityNPC* pNPC = test.npc.SpawnNPC(m_pField);

		struct CheckData
		{
			int nLevelFactor;
			float fExpected;
		} cases[] = {
			{0, 25.0f},
			{1, 25.0f},
			{2, 25.0f},
			{3, 30.0f},
			{5, 50.0f},
			{8, 80.0f},
			{9, 90.0f},
			{10, 90.0f},
		};

		GPlayerRide::RandomDismountHandler random_handler;

		for (int i=0; i<ArraySize(cases); ++i) 
		{
			CheckData data = cases[i];
			MakeLevelFactor(data.nLevelFactor, m_pPlayer, pNPC);
			CHECK_EQUAL(data.fExpected, random_handler.CalcDismountRate(pNPC, m_pPlayer));
		}
	}
}