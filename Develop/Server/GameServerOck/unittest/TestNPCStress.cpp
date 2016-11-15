#include "stdafx.h"
#include "GTestForward.h"
#include "FBaseGame.h"
#include "GNPCStress.h"

SUITE(NPCStress)
{
	struct Fixture : FBaseGame
	{
		Fixture()
		{
			pNPC = NewNPC(m_pField);
		}
		~Fixture()
		{
		}

		GEntityNPC* pNPC;
	};

	TEST_FIXTURE(Fixture, StressResetWhenHit)
	{
		pNPC->GetNPCStress().SetStress(100);
		CHECK_EQUAL(100, pNPC->GetStress());
		Attack(pNPC, m_pPlayer);
		CHECK_EQUAL(0, pNPC->GetStress());
	}
}