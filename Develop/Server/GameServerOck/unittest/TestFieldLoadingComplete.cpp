#include "stdafx.h"
#include "FBasePlayer.h"
#include "FBaseFieldInfo.h"
#include "FBaseFieldMgr.h"
#include "MockLink.h"
#include "FBaseMockLink.h"
#include "GUTHelper_Network.h"
#include "GFieldEnteringMgr.h"
#include "CCommandTable.h"


SUITE(FieldLoadingComplete)
{
	struct Fixture : public FBasePlayer, FBaseFieldInfo, FBaseFieldMgr
	{
		Fixture()
		{
			InitTestFields();
		}
		~Fixture()
		{
		}
		void InitTestFields()
		{
			GFieldInfo* pFieldInfo = AddFieldInfo();
			int nChannelID = 1;
			m_pSharedField = AddSharedField(SUnitTestUtil::NewUID(), pFieldInfo->m_nFieldID, nChannelID);
		}

		GUTHelper_Network	m_helperNetwork;
		GSharedField*		m_pSharedField;
	};

	TEST_FIXTURE(Fixture, TestFieldLoadingComplete_Recv_FIELD_LOADING_COMPLETE)
	{
		/// Fixture 설치
		GEntityPlayer* pPlayer = NewEntityPlayer((GField*)m_pSharedField);
		MockLink* pLink = m_helperNetwork.NewLink(pPlayer);

		CHECK_EQUAL(0, gmgr.pFieldMgr->GetFieldEnteringQueue()->GetQueueSize());

		/// SUT 실행
		pLink->OnRecv(MC_FIELD_LOADING_COMPLETE, 0, NULL);

		/// 검증
		CHECK_EQUAL(1, gmgr.pFieldMgr->GetFieldEnteringQueue()->GetQueueSize());
	}

	TEST_FIXTURE(Fixture, TestFieldLoadingComplete_OnePlayer)
	{
		/// Fixture 설치
		GEntityPlayer* pPlayer = NewEntityPlayer((GField*)m_pSharedField);
		MockLink* pLink = m_helperNetwork.NewLink(pPlayer);

		pLink->OnRecv(MC_FIELD_LOADING_COMPLETE, 0, NULL);

		CHECK_EQUAL(1, gmgr.pFieldMgr->GetFieldEnteringQueue()->GetQueueSize());

		/// SUT 실행
		gmgr.pFieldMgr->Update(GFieldEnteringMgr::TIME_UPDATE_SEC);

		/// 검증
		CHECK_EQUAL(0, gmgr.pFieldMgr->GetFieldEnteringQueue()->GetQueueSize());
		CHECK_EQUAL(3, pLink->GetCommandCount());
		CHECK_EQUAL(MC_CHAR_MYTIMEINFO, pLink->GetCommandID(0));
		CHECK_EQUAL(MC_FIELD_SET_TIME_WEATHER, pLink->GetCommandID(1));
		CHECK_EQUAL(MC_FIELD_START_GAME, pLink->GetCommandID(2));
	}

	TEST_FIXTURE(Fixture, TestFieldLoadingComplete_TwoPlayer)
	{
		/// Fixture 설치
		GEntityPlayer* pPlayer1 = NewEntityPlayer((GField*)m_pSharedField);
		GEntityPlayer* pPlayer2 = NewEntityPlayer((GField*)m_pSharedField);

		MockLink* pLink1 = m_helperNetwork.NewLink(pPlayer1);
		MockLink* pLink2 = m_helperNetwork.NewLink(pPlayer2);

		pLink1->OnRecv(MC_FIELD_LOADING_COMPLETE, 0, NULL);
		pLink2->OnRecv(MC_FIELD_LOADING_COMPLETE, 0, NULL);

		CHECK_EQUAL(2, gmgr.pFieldMgr->GetFieldEnteringQueue()->GetQueueSize());

		/// SUT 실행
		gmgr.pFieldMgr->Update(GFieldEnteringMgr::TIME_UPDATE_SEC);

		/// 검증
		CHECK_EQUAL(0, gmgr.pFieldMgr->GetFieldEnteringQueue()->GetQueueSize());

		CHECK_EQUAL(3, pLink1->GetCommandCount());
		CHECK_EQUAL(MC_CHAR_MYTIMEINFO, pLink1->GetCommandID(0));
		CHECK_EQUAL(MC_FIELD_SET_TIME_WEATHER, pLink1->GetCommandID(1));
		CHECK_EQUAL(MC_FIELD_START_GAME, pLink1->GetCommandID(2));

		CHECK_EQUAL(3, pLink2->GetCommandCount());
		CHECK_EQUAL(MC_CHAR_MYTIMEINFO, pLink2->GetCommandID(0));
		CHECK_EQUAL(MC_FIELD_SET_TIME_WEATHER, pLink2->GetCommandID(1));
		CHECK_EQUAL(MC_FIELD_START_GAME, pLink2->GetCommandID(2));
	}

	TEST_FIXTURE(Fixture, TestFieldLoadingComplete_Player_LimitCount)
	{
		/// Fixture 설치
		const int LIMIT_PLAYER = GFieldEnteringMgr::LIMIT_COUNT_ENTER_PLAYER_A_UPDATE;

		vector<MockLink*> vecMockLink;
		for (int i = 0; i < LIMIT_PLAYER; ++i)
		{
			GEntityPlayer* pPlayer = NewEntityPlayer((GField*)m_pSharedField);
			MockLink* pLink = m_helperNetwork.NewLink(pPlayer);

			pLink->OnRecv(MC_FIELD_LOADING_COMPLETE, 0, NULL);

			vecMockLink.push_back(pLink);
		}

		CHECK_EQUAL(LIMIT_PLAYER, gmgr.pFieldMgr->GetFieldEnteringQueue()->GetQueueSize());

		/// SUT 실행
		gmgr.pFieldMgr->Update(GFieldEnteringMgr::TIME_UPDATE_SEC);

		/// 검증
		CHECK_EQUAL(0, gmgr.pFieldMgr->GetFieldEnteringQueue()->GetQueueSize());

		for (int i = 0; i < LIMIT_PLAYER; ++i)
		{
			CHECK(0 != vecMockLink[i]->GetCommandCount());
		}
	}

	TEST_FIXTURE(Fixture, TestFieldLoadingComplete_Player_LimitCount_Over)
	{
		/// Fixture 설치
		const int LIMIT_PLAYER = GFieldEnteringMgr::LIMIT_COUNT_ENTER_PLAYER_A_UPDATE * 2;

		vector<MockLink*> vecMockLink;
		for (int i = 0; i < LIMIT_PLAYER; ++i)
		{
			GEntityPlayer* pPlayer = NewEntityPlayer((GField*)m_pSharedField);
			MockLink* pLink = m_helperNetwork.NewLink(pPlayer);

			pLink->OnRecv(MC_FIELD_LOADING_COMPLETE, 0, NULL);

			vecMockLink.push_back(pLink);
		}

		CHECK_EQUAL(LIMIT_PLAYER, gmgr.pFieldMgr->GetFieldEnteringQueue()->GetQueueSize());

		/// SUT 실행
		gmgr.pFieldMgr->Update(GFieldEnteringMgr::TIME_UPDATE_SEC);

		/// 검증
		CHECK_EQUAL(LIMIT_PLAYER/2, gmgr.pFieldMgr->GetFieldEnteringQueue()->GetQueueSize());

		/// SUT 실행
		gmgr.pFieldMgr->Update(GFieldEnteringMgr::TIME_UPDATE_SEC);

		/// 검증
		CHECK_EQUAL(0, gmgr.pFieldMgr->GetFieldEnteringQueue()->GetQueueSize());
	}

}