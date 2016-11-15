#include "stdafx.h"
#include "XGamePlayState.h"
#include "XGame.h"
#include "TestGame.h"
#include "FTestGame.h"
#include "XGlobal.h"
#include "XUnitTestHelper.h"
#include "XModuleEntity.h"
#include "XCharacter.h"
#include "MockCharacter.h"
#include "MockSystem.h"
#include "XMyPlayer.h"
#include "XNetPlayer.h"
#include "XNonPlayer.h"

SUITE(PlayerInOut)
{
	class FPlayerInOut
	{
	public:
		FPlayerInOut()
		{
			m_pGame = tg.GetGame();
			
			m_Helper.SetAniDataSet_Player(&m_PCAniDataSet);
		}
		virtual ~FPlayerInOut()
		{

		}
		XPlayer* NewInPlayer()
		{
			MUID uidPlayer = global.system->GenerateLocalUID();
			TD_UPDATE_CACHE_PLAYER td_update_cache_player = XUnitTestHelper::MakeSamle_TD_UPDATE_CACHE_PLAYER(uidPlayer);

			XPlayer* pPlayer = m_pGame->GetCurrentGameState()->InPlayer(uidPlayer, &td_update_cache_player);
			pPlayer->GetActor()->SetAnimationUpdated(true);

			return pPlayer;
		}
		void OutPlayer(MUID uidPlayer)
		{
			m_pGame->GetCurrentGameState()->OutPlayer(uidPlayer);
		}

		TestGame		tg;
		XGame*			m_pGame;
		XUnitTestHelper	m_Helper;
		TestAniDataSet			m_PCAniDataSet;
	};

	TEST_FIXTURE(FPlayerInOut, TestPlayerInOutFlood)
	{
		set<MUID>	setWillDelete;
		for (int i = 0; i < 100; i++)
		{
			XPlayer* pPlayer = NewInPlayer();
			MUID uidPlayer = pPlayer->GetUID();

			CHECK(pPlayer != NULL);

			if (XMath::RandomNumber(1, 10) < 3)
			{
				setWillDelete.insert(uidPlayer);
			}

			MUID uidDeletePlayer = MUID::ZERO;
			if (XMath::RandomNumber(1, 10) < 2)
			{
				for (set<MUID>::iterator itor = setWillDelete.begin(); itor != setWillDelete.end(); ++itor)
				{
					uidDeletePlayer = (*itor);

					OutPlayer(uidDeletePlayer);
				}
			}

			// 플레이어가 바로 지워지지는 않는다.
			static unsigned int nNowTime = 0;
			nNowTime += 10000;
			XExpectNowTime(nNowTime);

			gg.omgr->Update(10.0f);


			if (uidDeletePlayer != MUID::ZERO)
			{
				XObject* pObject = gg.omgr->Find(uidDeletePlayer);
				CHECK(pObject == NULL);
			}
		}
	}

	TEST_FIXTURE(FPlayerInOut, PlayerOutButNotDeleteImmediately)
	{
		CHECK_EQUAL(1, gg.omgr->GetCount());		// myplayer가 있으므로 1

		XPlayer* pPlayer = NewInPlayer();

		CHECK_EQUAL(2, gg.omgr->GetCount());

		OutPlayer(pPlayer->GetUID());

		CHECK_EQUAL(2, gg.omgr->GetCount());

		static unsigned int nNowTime = 0;
		nNowTime += 10000;
		XExpectNowTime(nNowTime);

		gg.omgr->Update(10.0f);

		CHECK_EQUAL(1, gg.omgr->GetCount());
	}


	//////////////////////////////////////////////////////////////////////////
	class FPlayerInOut2 : public FTestGame
	{
	public:
		FPlayerInOut2() : FTestGame(), m_pNPC(NULL)
		{
			m_pMyPlayer = GetMyPlayer();
			m_pNetPlayer = NewNetPlayer();

		}
		virtual ~FPlayerInOut2() {}

		void SpawnNPC()
		{
			XUnitTestHelper::NewNPCInfo(1);
			m_pNPC = NewNonPlayer(1);

		}
		void InNPC(MUID uidNPC, UIID nUIID)
		{
			TD_UPDATE_CACHE_NPC td_update_cache_npc = XUnitTestHelper::Make_TD_UPDATE_CACHE_NPC(uidNPC, 1);
			td_update_cache_npc.nUIID = nUIID;

			m_pNetAgent->OnRecv( MC_FIELD_IN_NPC, 1, NEW_SINGLE_BLOB(&td_update_cache_npc, sizeof(TD_UPDATE_CACHE_NPC)));
		}

		XMyPlayer*	m_pMyPlayer;
		XNetPlayer*	m_pNetPlayer;
		XNonPlayer*	m_pNPC;
	};

	TEST_FIXTURE(FPlayerInOut2, TestObjectCacheDelete)
	{
		vec3 vNewPos = vec3(10000.0f, 10000.0f, 0.0f);

		CHECK_EQUAL(2, gg.omgr->GetCount());

		m_pNetAgent->OnRecv( MC_ENTITY_WARP, 3, NEW_UID(m_pMyPlayer->GetUID()), NEW_VEC(vNewPos), NEW_VEC(vec3::AXISY));

		Update(0.1f);

		CHECK_CLOSE(10000.0f, m_pMyPlayer->GetPosition().x, 0.001f);
		CHECK_CLOSE(10000.0f, m_pMyPlayer->GetPosition().y, 0.001f);

		XExpectNowTime(XGetNowTime() + 10000);
		Update(10.0f);

		// 멀리 있는 NetPlayer가 지워졌다.
		CHECK_EQUAL(1, gg.omgr->GetCount());

	}

	TEST_FIXTURE(FPlayerInOut2, TestNetPlayerReInPlayer_PosSync)
	{
		Update(0.1f);

		vec3 vNewPos = vec3(10000.0f, 10000.0f, 0.0f);

		CHECK_EQUAL(2, gg.omgr->GetCount());

		m_pNetAgent->OnRecv( MC_ENTITY_WARP, 3, NEW_UID(m_pMyPlayer->GetUID()), NEW_VEC(vNewPos), NEW_VEC(vec3::AXISY));

		Update(0.1f);

		CHECK_CLOSE(10000.0f, m_pMyPlayer->GetPosition().x, 0.001f);
		CHECK_CLOSE(10000.0f, m_pMyPlayer->GetPosition().y, 0.001f);

		// 0.5초 흐르고
		XExpectNowTime(XGetNowTime() + 500);
		Update(0.5f);

		CHECK_EQUAL(2, gg.omgr->GetCount());

		// NetPlayer도 따라 왔다.
		CHECK_EQUAL(2, gg.omgr->GetCount());

		MUID uidNet = m_pNetPlayer->GetUID();
		UIID nNetUIID = m_pNetPlayer->GetUIID();

		TD_UPDATE_CACHE_PLAYER td_update_cache_player = XUnitTestHelper::MakeSamle_TD_UPDATE_CACHE_PLAYER(uidNet);
		td_update_cache_player.nUIID = nNetUIID;
		td_update_cache_player.vPos = vNewPos;

		m_pNetAgent->OnRecv( MC_FIELD_IN_PLAYER, 2, NEW_SINGLE_BLOB(&td_update_cache_player, sizeof(TD_UPDATE_CACHE_PLAYER)), NEW_SINGLE_BLOB(TD_PLAYER_FEATURE_TATTOO()));

		// 10초 흐르고
		XExpectNowTime(XGetNowTime() + 10000);
		Update(10.0f);

		CHECK_EQUAL(2, gg.omgr->GetCount());
	}


	TEST_FIXTURE(FPlayerInOut2, TestNPC_Hide_And_In_Slowly)
	{
		SpawnNPC();

		const MUID uidNPC = m_pNPC->GetUID();
		const UIID nNPCUIID = m_pNPC->GetUIID();

		Update(0.1f);
		CHECK_EQUAL(3, gg.omgr->GetCount());

		m_pNetAgent->OnRecv( MC_ACTION_HIDE, 1, NEW_UID(uidNPC));

		Update(0.1f);

		// 다 사라질 때까지 10초 흐르고
		XExpectNowTime(XGetNowTime() + 10000);
		Update(10.0f);

		// npc 없어졌다. 
		CHECK_EQUAL(2, gg.omgr->GetCount());


		// 다시 들어왔다.
		InNPC(uidNPC, nNPCUIID);

		CHECK_EQUAL(3, gg.omgr->GetCount());

	}

	TEST_FIXTURE(FPlayerInOut2, TestNPC_Hide_And_In_Immediately)
	{
		SpawnNPC();

		const MUID uidNPC = m_pNPC->GetUID();
		const UIID nNPCUIID = m_pNPC->GetUIID();

		Update(0.1f);
		CHECK_EQUAL(3, gg.omgr->GetCount());

		m_pNetAgent->OnRecv( MC_ACTION_HIDE, 1, NEW_UID(uidNPC));

		Update(0.1f);

		// 다 사라지기 전에 다시 In 커맨드 날라온다.
		XExpectNowTime(XGetNowTime() + 100);
		Update(0.1f);

		// npc가 아직 있다.
		CHECK_EQUAL(3, gg.omgr->GetCount());


		// 다시 들어왔다.
		InNPC(uidNPC, nNPCUIID);

		CHECK_EQUAL(3, gg.omgr->GetCount());


		// 시간이 한참 지나도 여전히 남아있음
		XExpectNowTime(XGetNowTime() + 10000);
		Update(10.0f);

		CHECK_EQUAL(3, gg.omgr->GetCount());
	}

}