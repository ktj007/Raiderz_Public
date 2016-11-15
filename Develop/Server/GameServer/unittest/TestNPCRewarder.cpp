#include "stdafx.h"
#include "FBaseGame.h"
#include "GNPCRewarder.h"
#include "GUnitTestUtil.h"
#include "GExpSystem.h"
#include "GHateTable.h"
#include "GFactionSystem.h"
#include "GDropList.h"
#include "GNPCSetterForTest.h"
#include "GParty.h"
#include "GTestForward.h"
#include "GPlayerEmblem.h"
#include "GPlayerSetterForTest.h"
#include "GQuestSystem.h"
#include "GQObjUpdater.h"
#include "GQuestSystemSetterForTest.h"

SUITE(NPCRewarder)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{
			m_pNPCInfo = NewNPCInfo();
			m_pNPC = NewNPC(m_pField, m_pNPCInfo->nID);
		}

		GNPCInfo*	m_pNPCInfo;
		GEntityNPC* m_pNPC;

		GTestSysWrapper<GQuestSystem>	m_QuestSystem;
	};

	TEST_FIXTURE(Fixture, DivideContriubtorByServer)
	{
		int nOtherServerBeneficiaryCID = GUnitTestUtil::NewCID();

		vector<int> vecBeneficiaryCID;
		vecBeneficiaryCID.push_back(m_pPlayer->GetCID());
		vecBeneficiaryCID.push_back(nOtherServerBeneficiaryCID);

		vector<GEntityPlayer*> vecSameServerBeneficiary;
		vector<int>  vecOtherServerBeneficiaryCID;

		m_pNPC->GetNPCRewarder().DivideContriubtorByServer(vecBeneficiaryCID, vecSameServerBeneficiary, vecOtherServerBeneficiaryCID);

		CHECK_EQUAL(m_pPlayer, vecSameServerBeneficiary.front());
		CHECK_EQUAL(nOtherServerBeneficiaryCID, vecOtherServerBeneficiaryCID.front());
	}

	TEST_FIXTURE(Fixture, Exp)
	{
		class MockExpSystem : public GExpSystem
		{
		public:			
			MOCK_METHOD3(RewardExpByNPCKill, bool(const vector<GEntityPlayer*>& vecSameServerBeneficiary, int nAllBeneficiaryCount, GNPCInfo* pNPCInfo));
		};
		GTestSysWrapper2<GExpSystem, MockExpSystem> mockExpSystem;

		vector<GEntityPlayer*> vecSameServerBeneficiary;		
		vecSameServerBeneficiary.push_back(m_pPlayer);				
		EXPECT_CALL(*mockExpSystem.Get(), RewardExpByNPCKill(vecSameServerBeneficiary, 1, m_pNPCInfo));

		m_pNPC->GetNPCRewarder().Exp(vecSameServerBeneficiary, vecSameServerBeneficiary.size(), m_pNPCInfo);
	}

	TEST_FIXTURE(Fixture, Quest)
	{
		class MockQObjUpdater : public GQObjUpdater
		{
		public:			
			MOCK_METHOD4(UpdDestroyQObj, bool(GEntityPlayer* pPlayer, int nObjectiveParam1, int nVar, GEntityNPC* pNPC));
		};

		MockQObjUpdater* pMockQObjUpdater = new MockQObjUpdater;
		gsys.pQuestSystem->GetSetterForTest().SetQObjUpdater(pMockQObjUpdater);

		vector<GEntityPlayer*> vecSameServerBeneficiary;
		vecSameServerBeneficiary.push_back(m_pPlayer);			
		EXPECT_CALL(*pMockQObjUpdater, UpdDestroyQObj(m_pPlayer, m_pNPCInfo->nID, 1, m_pNPC));

		m_pNPC->GetNPCRewarder().Quest(vecSameServerBeneficiary, m_pNPC);
	}

	TEST_FIXTURE(Fixture, Faction)
	{
		class MockFactionSystem : public GFactionSystem
		{
		public:			
			MOCK_METHOD2(RewardFaction, void(const vector<GEntityPlayer*>& vecBeneficiary, GNPCInfo* pNPCInfo));
		};
		GTestSysWrapper2<GFactionSystem, MockFactionSystem> mockFactionSystem;

		vector<GEntityPlayer*> vecSameServerBeneficiary;		
		vecSameServerBeneficiary.push_back(m_pPlayer);				
		EXPECT_CALL(*mockFactionSystem.Get(), RewardFaction(vecSameServerBeneficiary, m_pNPC->GetNPCInfo()));

		m_pNPC->GetNPCRewarder().Faction(vecSameServerBeneficiary, m_pNPCInfo);
	}

	TEST_FIXTURE(Fixture, ItemByDie)
	{
		class MockDropList : public GDropList
		{
		public:
			MockDropList(GEntityNPC* pOwner)
			: GDropList(pOwner)	{}

			virtual ~MockDropList() {}

			MOCK_METHOD2(DropByKill, bool(const vector<int>& vecBeneficiaryCID, MUID nPartyUID));
		};

		MockDropList* pMockDropList =  new MockDropList(m_pNPC);
		m_pNPC->GetNPCSetterForTest().SetDropList(pMockDropList);

		vector<int> vecBeneficiaryCID;
		vecBeneficiaryCID.push_back(m_pPlayer->GetCID());

		EXPECT_CALL(*pMockDropList, DropByKill(vecBeneficiaryCID, MUID::ZERO));

		m_pNPC->GetNPCRewarder().ItemByDie(vecBeneficiaryCID, MUID::ZERO, m_pNPC);
	}

	TEST_FIXTURE(Fixture, ItemByGather)
	{
		class MockDropList : public GDropList
		{
		public:
			MockDropList(GEntityNPC* pOwner)
				: GDropList(pOwner)	{}

			MOCK_METHOD2(DropByGather, bool(int nGatherCID, int nLootID));
		};

		int nLootID = SUnitTestUtil::NewID();

		MockDropList* pMockDropList =  new MockDropList(m_pNPC);
		m_pNPC->GetNPCSetterForTest().SetDropList(pMockDropList);

		EXPECT_CALL(*pMockDropList, DropByGather(m_pPlayer->GetCID(), nLootID));

		m_pNPC->GetNPCRewarder().ItemByGather(m_pPlayer, nLootID, m_pNPC);
	}

	TEST_FIXTURE(Fixture, NotifyReward)
	{
		class MockPlayerEmblem : public GPlayerEmblem
		{
		public:
			MockPlayerEmblem(GEntityPlayer* pOwner)
			: GPlayerEmblem(pOwner)	{}
			MOCK_METHOD1(OnRewarded, void(int nNPCID));
		};

		MockPlayerEmblem* pMockPlayerEmblem = new MockPlayerEmblem(m_pPlayer);
		m_pPlayer->GetPlayerSetterForTest().SetPlayerEmblem(pMockPlayerEmblem);

		EXPECT_CALL(*pMockPlayerEmblem, OnRewarded(m_pNPC->GetID()));

		m_pNPC->GetNPCRewarder().NotifyReward(m_pPlayer, m_pNPCInfo->nID);
	}

	TEST_FIXTURE(Fixture, RewardOtherServerBeneficiary)
	{
		int nOtherServerBeneficiaryCID = SUnitTestUtil::NewCID();
		int nAlllBeneficiaryCount = 2;
		vector<int> vecOtherServerBeneficiaryCID;
		vecOtherServerBeneficiaryCID.push_back(nOtherServerBeneficiaryCID);

		m_pNetAgent->ResetSendCommands();
		m_pNPC->GetNPCRewarder().RewardOtherServerBeneficiary(vecOtherServerBeneficiaryCID, nAlllBeneficiaryCount, m_pNPCInfo->nID);
		
		ASSERT_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		ASSERT_EQUAL(MMC_CHAR_NPCDIE_REWARD_REQ, m_pNetAgent->GetSendCommandID(0));

		vector<int> vecOtherServerBeneficiaryCID_CMD;
		m_pNetAgent->GetSendCommand(0).GetBlob(vecOtherServerBeneficiaryCID_CMD, 0);
		CHECK_EQUAL(nOtherServerBeneficiaryCID, vecOtherServerBeneficiaryCID_CMD.front());		
		CHECK_EQUAL(nAlllBeneficiaryCount, m_pNetAgent->GetSendParam<int>(0, 1));			
		CHECK_EQUAL(m_pNPC->GetID(), m_pNetAgent->GetSendParam<int>(0, 2));			
	}	

	struct FChallengerFieldForReward : Fixture
	{
		class MockActorObserver : public GActorObserver
		{
		public:
			MockActorObserver(): m_bCallOnRewarded(false) {}

			// NPC를 죽여서 보상을 받을때 이벤트 (Player 전용)
			virtual void OnRewarded(int nNPCID) override { m_bCallOnRewarded = true; }

		public:
			bool m_bCallOnRewarded;
		};
	public:
		FChallengerFieldForReward()
		{
			m_pPlayer->AttachObserver(&actor_observer);
		}

		~FChallengerFieldForReward()
		{
			m_pPlayer->DetachObserver(&actor_observer);
		}

	public:
		MockActorObserver actor_observer;
	};

	TEST_FIXTURE(FChallengerFieldForReward, DoNotRewardInChallengerField)
	{
		FIELD_GROUP_INFO* pFieldGroup = new FIELD_GROUP_INFO;
		pFieldGroup->bChallengerQuest = true;
		const_cast<GFieldInfo*>(m_pNPC->GetFieldInfo())->m_pFieldGroup = pFieldGroup;
		m_pNPC->doAttack(m_pPlayer->GetUID(), 1);
		CHECK_EQUAL(0, m_pPlayer->GetPlayerInfo()->nXP);
		m_pNPC->GetNPCRewarder().RewardByDie(m_pNPC);
		CHECK_EQUAL(0, m_pPlayer->GetPlayerInfo()->nXP); // 경험치 얻지 않음
		const_cast<GFieldInfo*>(m_pNPC->GetFieldInfo())->m_pFieldGroup = NULL;
		SAFE_DELETE(pFieldGroup);
	}	

	//TEST_FIXTURE(Fixture, DoNotRewardInChallengerField_Failed)
	//{
	//	m_pNPC->doAttack(m_pPlayer->GetUID(), 1);
	//	CHECK_EQUAL(0, m_pPlayer->GetPlayerInfo()->nXP);
	//	m_pNPC->GetNPCRewarder().RewardByDie(m_pNPC);
	//	CHECK(0 < m_pPlayer->GetPlayerInfo()->nXP); // 경험치는 얻음
	//}
}

