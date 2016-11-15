#include "stdafx.h"
#include "ZFixedPartyLogic.h"
#include "ZFixtureHelper.h"
#include "ZFixedPartyInfoManager.h"

SUITE(FixedPartyLogic)
{
	struct MockFixedPartyInfoManager: public ZFixedPartyInfoManager
	{
		MOCK_METHOD1(FindFixedPartyID, int(wstring strName));
	};

	struct TestFixedPartyLogic: public FBaseServerField, FBaseMockLink, FBasePlayer
	{
		ZTestMgrWrapper2<ZFixedPartyInfoManager, MockFixedPartyInfoManager> m_FixedPartyInfoManagerWrapper;
		MockFixedPartyInfoManager* m_pFixedPartyInfoManager;
		ZFixedPartyLogic m_FixedPartyLogic;
		ZPlayer* m_pPlayer;

		TestFixedPartyLogic()
		{
			m_pFixedPartyInfoManager = m_FixedPartyInfoManagerWrapper.Get();
			m_pPlayer = AddNewPlayer(100, 1);
		}
	};

	TEST_FIXTURE(TestFixedPartyLogic, GetFixedPartyID)
	{
		// arrange
		wstring strName = m_pPlayer->GetPlayerName();
		int nFixedPartyID = ZTestHelper::NewID();

		// arrange stub
		using testing::Return;
		EXPECT_CALL(*m_pFixedPartyInfoManager, FindFixedPartyID(strName))
			.WillOnce(Return(nFixedPartyID));

		// assert
		CHECK_EQUAL(nFixedPartyID, m_FixedPartyLogic.GetFixedPartyID(strName));
	}

	TEST_FIXTURE(TestFixedPartyLogic, AddWating)
	{
		// 파티멤버 2명 대기
		int nFixedPartyID = ZTestHelper::NewID();
		MUID uidMember = ZTestHelper::NewUID();
		MUID uidMember2 = ZTestHelper::NewUID();

		m_FixedPartyLogic.AddWaiting(nFixedPartyID, uidMember);
		m_FixedPartyLogic.AddWaiting(nFixedPartyID, uidMember2);

		CHECK_EQUAL(1, m_FixedPartyLogic.GetWaitingPartyCount());
		CHECK_EQUAL(2, m_FixedPartyLogic.GetWaitingMemberCount(nFixedPartyID));


		// 파티멤버 1명 대기
		int nFixedPartyID2 = ZTestHelper::NewID();
		MUID uidMember3 = ZTestHelper::NewUID();

		m_FixedPartyLogic.AddWaiting(nFixedPartyID2, uidMember3);

		CHECK_EQUAL(2, m_FixedPartyLogic.GetWaitingPartyCount());
		CHECK_EQUAL(1, m_FixedPartyLogic.GetWaitingMemberCount(nFixedPartyID2));

		
		// 대기 없음
		int nFixedPartyID3 = ZTestHelper::NewID();
		CHECK_EQUAL(0, m_FixedPartyLogic.GetWaitingMemberCount(nFixedPartyID3));
	}

	TEST_FIXTURE(TestFixedPartyLogic, CreateFixedParty)
	{
		// arrange
		int nFixedPartyID = ZTestHelper::NewID();		
		ZPlayer* pPlayer2 = AddNewPlayer(200, 2);
		
		MUID uidPlayer = m_pPlayer->GetUID();
		MUID uidPlayer2 = pPlayer2->GetUID();


		m_FixedPartyLogic.AddWaiting(nFixedPartyID, uidPlayer);
		m_FixedPartyLogic.AddWaiting(nFixedPartyID, uidPlayer2);

		// act
		m_FixedPartyLogic.CreateFixedParty(nFixedPartyID);

		// assert		
		ZParty* pParty = m_FixedPartyLogic.FindParty(nFixedPartyID);
		ASSERT_CHECK(pParty != NULL);		
	}

	TEST_FIXTURE(TestFixedPartyLogic, CreateFixedParty_NotEnoughMember)
	{
		// arrange
		int nFixedPartyID = ZTestHelper::NewID();		
		ZPlayer* pPlayer2 = AddNewPlayer(200, 2);

		MUID uidPlayer = m_pPlayer->GetUID();
		MUID uidPlayer2 = ZTestHelper::NewUID();	// UnknownPlayer

		m_FixedPartyLogic.AddWaiting(nFixedPartyID, uidPlayer);
		m_FixedPartyLogic.AddWaiting(nFixedPartyID, uidPlayer2);
		
		CHECK_EQUAL(2, m_FixedPartyLogic.GetWaitingMemberCount(nFixedPartyID));


		// act
		m_FixedPartyLogic.CreateFixedParty(nFixedPartyID);


		// assert		
		ZParty* pParty = m_FixedPartyLogic.FindParty(nFixedPartyID);
		ASSERT_CHECK(pParty == NULL);
		
		CHECK_EQUAL(1, m_FixedPartyLogic.GetWaitingMemberCount(nFixedPartyID));
	}
}
