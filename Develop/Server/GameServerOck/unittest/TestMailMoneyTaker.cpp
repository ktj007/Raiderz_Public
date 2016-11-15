#include "stdafx.h"
#include "GMailSystem.h"
#include "GMailMoneyTaker.h"
#include "GUTHelper_Player.h"
#include "GPlayerMailBox.h"
#include "GEntityPlayer.h"
#include "GClientMailRouter.h"
#include "GTestWrapper.h"
#include "MockField.h"
#include "GUTHelper_Field.h"
#include "GUTHelper_NPC.h"
#include "GUTHelper_Interaction.h"
#include "GUnitTestUtil.h"
#include "GMail.h"
#include "GDBTaskData.h"
#include "GPlayerInteraction.h"
#include "MockLink.h"
#include "GUTHelper_Network.h"
#include "CCommandTable.h"


SUITE(MailMoneyTaker)
{
	struct MockDBManager: public GDBManager
	{
		MOCK_METHOD2(MailGetAppendedMoney, bool(const MUID& uidPlayer, const GDBT_MAIL_GET_MONEY& data));		
	};

	struct MockClientMailRouter: public GClientMailRouter
	{
		MOCK_METHOD2(TakeAppendedMoneyRes, void(GEntityPlayer* pPlayer, MUID uidMail));
	};

	struct Fixture
	{
		GTestSysWrapper2<GDBManager, MockDBManager> m_DBManagerWrapper;
		GEntityPlayer* m_pPlayer;
		GEntityNPC* m_pNPC;
		MockLink* m_pLink;
		MockDBManager* m_pDBManager;
		MockClientMailRouter* m_pClientRouter;
		GMailMoneyTaker m_taker;
		

		Fixture()
		{
			m_pDBManager = m_DBManagerWrapper.Get();

			m_pClientRouter = new MockClientMailRouter();
			m_taker.SetClientRouter(m_pClientRouter);

			MockField* pField = GUTHelper_Field::NewMockField();

			GUTHelper_Player helper;
			m_pPlayer = helper.NewPlayer(pField);
			m_pNPC = GUTHelper_NPC::SpawnNPC(pField);

			GNPCInteractionElementInfo* pInteractionElementInfo = GUTHelper_Interaction::GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_MAILBOX);
			GUTHelper_Interaction::MakeInteractionState(m_pPlayer, m_pNPC, pInteractionElementInfo);

			GUTHelper_Network networkHelper;
			m_pLink = networkHelper.NewLink(m_pPlayer);
			m_pLink->AddIgnoreID(MC_MAIL_MAILBOX_IS_CLOSED);

			m_pPlayer->GetPlayerInfo()->SetMoney(1000);
		}
	
		~Fixture()
		{
		}
		
		GMail* NewTestMail(int nAppendedMoney)
		{
			MUID uidMail = GUnitTestUtil::NewUID();

			GMail* pMail = new GMail(uidMail.Value);
			pMail->Init(SUnitTestUtil::NewCID()
				,MT_PLAYER
				, false
				, L"Tester"
				, L"TestMail"
				, false
				, nAppendedMoney
				, 0
				, 1000
				);
			return pMail;
		}

		bool CheckRouteFailure(CCommandResultTable nResult)
		{
			MCommand& cmd = m_pLink->GetCommand(0);

			if (MC_MSG_RES != cmd.GetID())
			{
				return false;
			}

			if (5 != cmd.GetParameterCount())
			{
				return false;
			}

			int nSendResult;
			if (!cmd.GetParameter(&nSendResult, 4, MPT_INT))
			{
				return false;
			}

			if (nSendResult != nResult)
			{
				return false;
			}

			return true;
		}
	};


	TEST_FIXTURE(Fixture, Take_Success)
	{	
		// arrange
		GPlayerMailBox& mailBox = m_pPlayer->GetMailbox();

		GMail* pMail = NewTestMail(100);
		mailBox.AddMail(pMail);

		// arrange mock
		MUID uidPlayer = m_pPlayer->GetUID();

		GDBT_MAIL_GET_MONEY mailMoney;
		mailMoney.m_nAID = m_pPlayer->GetAID();
		mailMoney.m_nCID = m_pPlayer->GetCID();
		mailMoney.m_nMailUID = pMail->GetUID();
		mailMoney.m_nCharPtm = m_pPlayer->GetPlayerInfo()->nPlayTimeSec;
		mailMoney.m_nDeltaCharPtm = m_pPlayer->GetPlayerInfo()->GetDeltaPlayTime();
		mailMoney.m_nLevel = m_pPlayer->GetLevel();
		mailMoney.m_nAppendedMoney = pMail->GetAppendedMoney();
		mailMoney.m_nResultMoney = m_pPlayer->GetMoney() + pMail->GetAppendedMoney();

		
		using testing::Return;
		EXPECT_CALL(*m_pDBManager, MailGetAppendedMoney(uidPlayer, mailMoney))
			.WillOnce(Return(true));


		// act
		CHECK(m_taker.TakeAppendedMoney(m_pPlayer, pMail->GetUID()));


		// assert
		CHECK_EQUAL(0, m_pLink->GetCommandCount());
	}

	TEST_FIXTURE(Fixture, Teke_EndInteractionFailure)
	{
		// arrange
		GPlayerMailBox& mailBox = m_pPlayer->GetMailbox();

		GMail* pMail = NewTestMail(100);
		mailBox.AddMail(pMail);

		m_pLink->AddIgnoreID(2505);
		m_pPlayer->GetInteraction().EndInteraction();		

		MUID nMailUID = pMail->GetUID();

		// act
		CHECK(!m_taker.TakeAppendedMoney(m_pPlayer, nMailUID));
		CHECK(CheckRouteFailure(CR_FAIL_MAIL_APPEND_ITEM_NOT_INTERATION));
	}

	TEST_FIXTURE(Fixture, Take_NotFindMail)
	{
		// arrange
		MUID nMailUID = GUnitTestUtil::NewUID();

		// act
		CHECK(!m_taker.TakeAppendedMoney(m_pPlayer, nMailUID));
		CHECK(CheckRouteFailure(CR_FAIL_MAIL_NOT_FIND_MAIL));
	}

	TEST_FIXTURE(Fixture, Take_NotAppendedMoney)
	{
		// arrange
		GPlayerMailBox& mailBox = m_pPlayer->GetMailbox();

		GMail* pMail = NewTestMail(0);
		mailBox.AddMail(pMail);

		MUID nMailUID = pMail->GetUID();

		// act
		CHECK(!m_taker.TakeAppendedMoney(m_pPlayer, nMailUID));
		CHECK(CheckRouteFailure(CR_FAIL_MAIL_NO_APPENDED_MONEY));
	}

	TEST_FIXTURE(Fixture, Take_DBFailure)
	{
		// arrange
		GPlayerMailBox& mailBox = m_pPlayer->GetMailbox();

		GMail* pMail = NewTestMail(100);
		mailBox.AddMail(pMail);


		// arrange mock
		MUID nMailUID = pMail->GetUID();

		using testing::_;
		using testing::Return;
		EXPECT_CALL(*m_pDBManager, MailGetAppendedMoney(_, _))
			.WillOnce(Return(false));


		// act
		CHECK(!m_taker.TakeAppendedMoney(m_pPlayer, nMailUID));
		CHECK(CheckRouteFailure(CR_FAIL_MAIL_TAKE_DB_ERROR));
	}
}
