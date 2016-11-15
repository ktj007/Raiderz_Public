#include "stdafx.h"
#include "GTestForward.h"
#include "GEnemyFinder.h"
#include "GPlayerObjectManager.h"
#include "GPlayerDoing.h"
#include "GPlayerCutscene.h"
#include "GCutsceneInfoMgr.h"

SUITE(Cutscene)
{
	struct Fixture
	{
		Fixture()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pCutscenePlayer = m_Helper.player.NewPlayer(m_pField);
			m_pPartyMemberPlayer = m_Helper.player.NewPlayer(m_pField);
			m_pNeighborPlayer = m_Helper.player.NewPlayer(m_pField);
			m_pNeighborNPC = GUTHelper_NPC::SpawnNPC(m_pField);

			m_Helper.party.CreateParty(m_pCutscenePlayer, m_pPartyMemberPlayer);

			m_pCutscenePlayerLink = m_Helper.network.NewLink(m_pCutscenePlayer);
			m_pPartyMemberPlayerLink = m_Helper.network.NewLink(m_pPartyMemberPlayer);
			m_pNeighborPlayerLink = m_Helper.network.NewLink(m_pNeighborPlayer);
			
			m_pCutscenePlayerLink->AddIgnoreID(MC_PARTY_REFRESH_INFO);
			m_pPartyMemberPlayerLink->AddIgnoreID(MC_PARTY_REFRESH_INFO);
			m_pNeighborPlayerLink->AddIgnoreID(MC_PARTY_REFRESH_INFO);
			
			m_nCutsceneID = 1;
		}

		~Fixture()
		{
			gmgr.pPlayerObjectManager->DeleteAllPlayer();
			m_pField->Destroy();
		}

		GTestHelpers	m_Helper;
		MockField*		m_pField;
		GEntityPlayer*	m_pCutscenePlayer;
		GEntityPlayer*	m_pPartyMemberPlayer;
		GEntityPlayer*	m_pNeighborPlayer;
		GEntityNPC*		m_pNeighborNPC;

		MockLink*		m_pCutscenePlayerLink;
		MockLink*		m_pPartyMemberPlayerLink;
		MockLink*		m_pNeighborPlayerLink;

		int				m_nCutsceneID;

		GEnemyFinder	m_EnemyFinder;

		GTestMgrWrapper<GCutsceneInfoMgr>	m_wrapperCutsceneInfoMgr;
	};

	namespace Begin
	{
		TEST_FIXTURE(Fixture, AlreadyCutscening)
		{
			m_pCutscenePlayer->GetDoing().ChangeDoing(CD_CUTSCENING);

			CHECK_EQUAL(false, m_pCutscenePlayer->GetCutscene().BeginCutscene(m_nCutsceneID));
		}

		TEST_FIXTURE(Fixture, Begin)
		{
			CHECK_EQUAL(true, m_pCutscenePlayer->GetCutscene().BeginCutscene(m_nCutsceneID));

			CHECK_EQUAL(true, m_pCutscenePlayer->GetDoing().IsNowCutscening());
			CHECK_EQUAL(true, m_pCutscenePlayer->IsNowInvincibility());
			CHECK_EQUAL(true, m_pCutscenePlayer->IsNowInvisibilityToNPC());

			CHECK_EQUAL(1, m_pCutscenePlayerLink->GetCommandCount());
			CHECK_EQUAL(MC_CUTSCENE_BEGIN_ME, m_pCutscenePlayerLink->GetCommand(0).GetID());
			CHECK_EQUAL(m_nCutsceneID, m_pCutscenePlayerLink->GetParam<int>(0, 0));

			CHECK_EQUAL(1, m_pNeighborPlayerLink->GetCommandCount());
			CHECK_EQUAL(MC_CUTSCENE_BEGIN_OTHER, m_pNeighborPlayerLink->GetCommand(0).GetID());
			CHECK_EQUAL(m_pCutscenePlayer->GetUID(), m_pNeighborPlayerLink->GetParam<MUID>(0, 0));
		}

		TEST_FIXTURE(Fixture, BeginParty)
		{
			CHECK_EQUAL(true, m_pCutscenePlayer->GetCutscene().BeginPartyCutscene(m_nCutsceneID));

			CHECK_EQUAL(true, m_pCutscenePlayer->GetDoing().IsNowCutscening());
			CHECK_EQUAL(true, m_pCutscenePlayer->IsNowInvincibility());
			CHECK_EQUAL(true, m_pCutscenePlayer->IsNowInvisibilityToNPC());

			CHECK_EQUAL(true, m_pPartyMemberPlayer->GetDoing().IsNowCutscening());
			CHECK_EQUAL(true, m_pPartyMemberPlayer->IsNowInvincibility());
			CHECK_EQUAL(true, m_pPartyMemberPlayer->IsNowInvisibilityToNPC());

			CHECK_EQUAL(2, m_pCutscenePlayerLink->GetCommandCount());
			CHECK_EQUAL(MC_CUTSCENE_BEGIN_ME, m_pCutscenePlayerLink->GetCommand(0).GetID());
			CHECK_EQUAL(m_nCutsceneID, m_pCutscenePlayerLink->GetParam<int>(0, 0));
			CHECK_EQUAL(MC_CUTSCENE_BEGIN_OTHER, m_pCutscenePlayerLink->GetCommand(1).GetID());
			CHECK_EQUAL(m_pPartyMemberPlayer->GetUID(), m_pCutscenePlayerLink->GetParam<MUID>(1, 0));

			CHECK_EQUAL(2, m_pPartyMemberPlayerLink->GetCommandCount());
			CHECK_EQUAL(MC_CUTSCENE_BEGIN_OTHER, m_pPartyMemberPlayerLink->GetCommand(0).GetID());
			CHECK_EQUAL(m_pCutscenePlayer->GetUID(), m_pPartyMemberPlayerLink->GetParam<MUID>(0, 0));
			CHECK_EQUAL(MC_CUTSCENE_BEGIN_ME, m_pPartyMemberPlayerLink->GetCommand(1).GetID());
			CHECK_EQUAL(m_nCutsceneID, m_pPartyMemberPlayerLink->GetParam<int>(1, 0));

			CHECK_EQUAL(2, m_pNeighborPlayerLink->GetCommandCount());
			CHECK_EQUAL(MC_CUTSCENE_BEGIN_OTHER, m_pNeighborPlayerLink->GetCommand(0).GetID());
			CHECK_EQUAL(m_pCutscenePlayer->GetUID(), m_pNeighborPlayerLink->GetParam<MUID>(0, 0));
			CHECK_EQUAL(MC_CUTSCENE_BEGIN_OTHER, m_pNeighborPlayerLink->GetCommand(1).GetID());
			CHECK_EQUAL(m_pPartyMemberPlayer->GetUID(), m_pNeighborPlayerLink->GetParam<MUID>(1, 0));
		}
	}

	namespace End
	{
		TEST_FIXTURE(Fixture, NotCutscening)
		{
			CHECK_EQUAL(false, m_pCutscenePlayer->GetCutscene().EndCutscene(m_nCutsceneID));
		}

		TEST_FIXTURE(Fixture, End_RestoringCommands)
		{
			m_pCutscenePlayer->GetCutscene().BeginCutscene(m_nCutsceneID);
			m_pCutscenePlayerLink->ResetCommands();

			CHECK_EQUAL(true, m_pCutscenePlayer->GetCutscene().EndCutscene(m_nCutsceneID));
			
			CHECK_EQUAL(5, m_pCutscenePlayerLink->GetCommandCount());

			CHECK_EQUAL(MC_CUTSCENE_END_ME, m_pCutscenePlayerLink->GetCommandID(0));
			CHECK_EQUAL(MC_CHAR_MYTIMEINFO, m_pCutscenePlayerLink->GetCommandID(1));
			CHECK_EQUAL(MC_FIELD_SECTOR_ENTITY_INFO, m_pCutscenePlayerLink->GetCommandID(2));
			CHECK_EQUAL(MC_NPCINTERACTION_ICON, m_pCutscenePlayerLink->GetCommandID(3));	///< 근처에 Interaction 가능한 NPC가 있을경우.
			CHECK_EQUAL(MC_FIELD_SET_TIME_WEATHER, m_pCutscenePlayerLink->GetCommandID(4));
		}

		TEST_FIXTURE(Fixture, End)
		{
			m_pCutscenePlayerLink->AddIgnoreID(MC_CUTSCENE_BEGIN_ME);
			m_pCutscenePlayerLink->AddIgnoreID(MC_CHAR_MYTIMEINFO);
			m_pCutscenePlayerLink->AddIgnoreID(MC_FIELD_SECTOR_ENTITY_INFO);
			m_pCutscenePlayerLink->AddIgnoreID(MC_NPCINTERACTION_ICON);
			m_pCutscenePlayerLink->AddIgnoreID(MC_FIELD_SET_TIME_WEATHER);
			m_pNeighborPlayerLink->AddIgnoreID(MC_CUTSCENE_BEGIN_OTHER);
			m_pCutscenePlayer->GetCutscene().BeginCutscene(m_nCutsceneID);

			CHECK_EQUAL(true, m_pCutscenePlayer->GetCutscene().EndCutscene(m_nCutsceneID));

			CHECK_EQUAL(false, m_pCutscenePlayer->GetDoing().IsNowCutscening());
			CHECK_EQUAL(false, m_pCutscenePlayer->IsNowInvincibility());
			CHECK_EQUAL(false, m_pCutscenePlayer->IsNowInvisibilityToNPC());

			CHECK_EQUAL(1, m_pCutscenePlayerLink->GetCommandCount());
			CHECK_EQUAL(MC_CUTSCENE_END_ME, m_pCutscenePlayerLink->GetCommand(0).GetID());
			CHECK_EQUAL(m_nCutsceneID, m_pCutscenePlayerLink->GetParam<int>(0, 0));

			CHECK_EQUAL(1, m_pNeighborPlayerLink->GetCommandCount());
			CHECK_EQUAL(MC_CUTSCENE_END_OTHER, m_pNeighborPlayerLink->GetCommand(0).GetID());
			CHECK_EQUAL(m_pCutscenePlayer->GetUID(), m_pNeighborPlayerLink->GetParam<MUID>(0, 0));
		}
	}

	namespace Sawn
	{
		TEST_FIXTURE(Fixture, CutsceneIsSawn)
		{
			int nTestUnsawnCutsceneID = 9999;

			CHECK_EQUAL(false, m_pCutscenePlayer->GetCutscene().IsSawn(nTestUnsawnCutsceneID));


			std::vector<int> vecSawnCutscene;
			int nTestSawnCutsceneID_100 = 100;
			int nTestSawnCutsceneID_101 = 101;
			int nTestSawnCutsceneID_102 = 102;
			vecSawnCutscene.push_back(nTestSawnCutsceneID_100);
			vecSawnCutscene.push_back(nTestSawnCutsceneID_101);
			vecSawnCutscene.push_back(nTestSawnCutsceneID_102);

			m_pCutscenePlayer->GetCutscene().AssignSawnCutscene(vecSawnCutscene);

			CHECK_EQUAL(true, m_pCutscenePlayer->GetCutscene().IsSawn(nTestSawnCutsceneID_100));
			CHECK_EQUAL(true, m_pCutscenePlayer->GetCutscene().IsSawn(nTestSawnCutsceneID_101));
			CHECK_EQUAL(true, m_pCutscenePlayer->GetCutscene().IsSawn(nTestSawnCutsceneID_102));
		}

		TEST_FIXTURE(Fixture, BeginCutscene_Failure_IsSawn)
		{
			int nTestSawnCutsceneID_100 = 100;
			int nTestSawnCutsceneID_101 = 101;

			GCutsceneInfo* pCutsceneInfo_100 = new GCutsceneInfo();
			pCutsceneInfo_100->nID = nTestSawnCutsceneID_100;
			pCutsceneInfo_100->bRepeatable = false;
			gmgr.pCutsceneInfoMgr->Insert(pCutsceneInfo_100);

			GCutsceneInfo* pCutsceneInfo_101 = new GCutsceneInfo();
			pCutsceneInfo_101->nID = nTestSawnCutsceneID_101;
			pCutsceneInfo_101->bRepeatable = false;
			gmgr.pCutsceneInfoMgr->Insert(pCutsceneInfo_101);


			std::vector<int> vecSawnCutscene;
			vecSawnCutscene.push_back(nTestSawnCutsceneID_100);
			vecSawnCutscene.push_back(nTestSawnCutsceneID_101);

			m_pCutscenePlayer->GetCutscene().AssignSawnCutscene(vecSawnCutscene);


			CHECK_EQUAL(false, m_pCutscenePlayer->GetCutscene().BeginCutscene(nTestSawnCutsceneID_100));
			CHECK_EQUAL(false, m_pCutscenePlayer->GetCutscene().BeginCutscene(nTestSawnCutsceneID_101));


			int nTestCutsceneID_102 = 102;

			CHECK_EQUAL(true, m_pCutscenePlayer->GetCutscene().BeginCutscene(nTestCutsceneID_102));
		}
	}

}