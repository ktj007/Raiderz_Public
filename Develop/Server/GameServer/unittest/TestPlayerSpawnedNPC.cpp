#include "stdafx.h"
#include "FBaseGame.h"
#include "GTestForward.h"
#include "GPlayerSpawnedNPC.h"
#include "GModuleAI.h"

SUITE(PlayerSpawnedNPC)
{
	struct Fixture : FBaseGame
	{
		Fixture()
		: m_PlayerSpawnedNPC(m_pPlayer->GetSpawnedNPC())
		{
			GNPCInfo* pNPCInfo = test.npc.NewNPCInfo();
			m_nNPCID = pNPCInfo->nID;
		}

		GPlayerSpawnedNPC&	m_PlayerSpawnedNPC;	
		int					m_nNPCID;
	};

	TEST_FIXTURE(Fixture, Spawn)
	{
		m_PlayerSpawnedNPC.Spawn(m_nNPCID, 10);

		CHECK_EQUAL(true, m_PlayerSpawnedNPC.IsSpawend(m_nNPCID));
		CHECK_EQUAL(1, m_pPlayer->GetField()->GetNPCQty(m_nNPCID));
	}

	TEST_FIXTURE(Fixture, Spawn_Warp)
	{
		m_PlayerSpawnedNPC.Spawn(m_nNPCID, 10);
		m_pPlayer->SetPos(vec3(100.0f, 100.0f, 100.0f));
		m_PlayerSpawnedNPC.Spawn(m_nNPCID, 10);

		GEntityNPC* pNPC = m_pPlayer->GetField()->FindOneNPCByID(m_nNPCID);
		pNPC->GetModuleAI()->Update(1.0f);		

		CHECK_EQUAL(1, m_pPlayer->GetField()->GetNPCQty(m_nNPCID));
		CHECK(m_pPlayer->GetPos() == pNPC->GetPos());
	}

	TEST_FIXTURE(Fixture, DespawnAll)
	{
		m_PlayerSpawnedNPC.Spawn(m_nNPCID, 10);
		m_PlayerSpawnedNPC.DespawnAll();

		CHECK_EQUAL(0, m_pPlayer->GetField()->GetNPCQty(m_nNPCID));
	}
}