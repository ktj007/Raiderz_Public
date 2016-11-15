#include "stdafx.h"
#include "GPlayerLogoutDBExecuter.h"
#include "GDBManager.h"
#include "GUnitTestUtil.h"
#include "GUTHelper.h"
#include "FBasePlayer.h"
#include "MockDBTask.h"


SUITE(PlayerLogoutDBExecuter)
{
	class MockPlayerLogoutDBExecuter: public GPlayerLogoutDBExecuter
	{
	public:
		MockPlayerLogoutDBExecuter(GPlayerObject* pPlayerObject)
			: GPlayerLogoutDBExecuter(pPlayerObject)
		{
		}

		MOCK_METHOD0(OnFlushDBChachState, bool());
		MOCK_METHOD0(OnLogoutDBTaskState, bool());
		MOCK_METHOD0(OnWaitingLogoutDBTaskState, bool());
	};

	class MockDBManager: public GDBManager
	{
	public:
		MockDBManager()	{}

		MOCK_METHOD5(Logout, bool(int64 nAID, int64 nCID, GEntityPlayer* pPlayer, vector<REMAIN_BUFF_TIME>& vecBuffRemainTime, vector<pair<int, float>>& vecTalentCoolTime));
	};

	struct FPlayerLogoutDBExecuter : public FBasePlayer
	{
		FPlayerLogoutDBExecuter()
		{
			GEntityPlayer* pEntityPlayer = NewEntityPlayer(NULL);
			m_uidPlayer = pEntityPlayer->GetUID();

			m_pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(m_uidPlayer);		
		}
		virtual ~FPlayerLogoutDBExecuter()
		{
		}

		template<typename T>
		T* NewExecuter(GPlayerObject* pPlayer)
		{
			return new T(pPlayer);
		}

		MUID			m_uidPlayer;
		GPlayerObject*	m_pPlayerObject;

		GTestSysWrapper2<GDBManager, MockDBManager> m_DBManagerWrapper;
	};

	TEST_FIXTURE(FPlayerLogoutDBExecuter, TestPlayerLogoutDBExecuter_InitialyState)
	{
		GPlayerLogoutDBExecuter executer(m_pPlayerObject);
		
		CHECK_EQUAL(GPlayerLogoutDBExecuter::FlushDBChachState, executer.GetState());
	}

	TEST_FIXTURE(FPlayerLogoutDBExecuter, TestPlayerLogoutDBExecuter_PlayerObjectIsNULL_ShouldFinalState)
	{
		GPlayerLogoutDBExecuter executer(NULL);

		CHECK_EQUAL(GPlayerLogoutDBExecuter::FinalState, executer.GetState());
	}

	TEST_FIXTURE(FPlayerLogoutDBExecuter, TestPlayerLogoutDBExecuter_UpdateWaitingDBTaskState)
	{
		auto_ptr<MockPlayerLogoutDBExecuter> executer(NewExecuter<MockPlayerLogoutDBExecuter>(m_pPlayerObject));

		EXPECT_CALL(*executer, OnFlushDBChachState());

		executer->SetState(GPlayerLogoutDBExecuter::FlushDBChachState);
		executer->Update();
	}

	TEST_FIXTURE(FPlayerLogoutDBExecuter, TestPlayerLogoutDBExecuter_UpdateLogoutDBTaskState)
	{
		auto_ptr<MockPlayerLogoutDBExecuter> executer(NewExecuter<MockPlayerLogoutDBExecuter>(m_pPlayerObject));

		EXPECT_CALL(*executer, OnLogoutDBTaskState());

		executer->SetState(GPlayerLogoutDBExecuter::LogoutDBTaskState);
		executer->Update();
	}

	TEST_FIXTURE(FPlayerLogoutDBExecuter, TestPlayerLogoutDBExecuter_UpdateWaitingLogoutDBTaskState)
	{
		auto_ptr<MockPlayerLogoutDBExecuter> executer(NewExecuter<MockPlayerLogoutDBExecuter>(m_pPlayerObject));

		EXPECT_CALL(*executer, OnWaitingLogoutDBTaskState());

		executer->SetState(GPlayerLogoutDBExecuter::WaitingLogoutDBTaskState);
		executer->Update();
	}

	
	TEST_FIXTURE(FPlayerLogoutDBExecuter, TestPlayerLogoutDBExecuter_OnFlushDBChachState)
	{
		auto_ptr<GPlayerLogoutDBExecuter> executer(NewExecuter<GPlayerLogoutDBExecuter>(m_pPlayerObject));
		executer->SetState(GPlayerLogoutDBExecuter::FlushDBChachState);

		MockDBTask dbTask(m_uidPlayer);

		GPlayerLogoutDBExecuter::State oldState = executer->GetState();

		
		dbTask.IncreaseRefCount(m_uidPlayer);
		executer->OnFlushDBChachState();

		CHECK_EQUAL(oldState, executer->GetState());

		
		dbTask.DecreaseRefCount(m_uidPlayer);
		executer->OnFlushDBChachState();

		CHECK_EQUAL(GPlayerLogoutDBExecuter::LogoutDBTaskState, executer->GetState());
	}
	
	TEST_FIXTURE(FPlayerLogoutDBExecuter, TestPlayerLogoutDBExecuter_OnLogoutDBTask)
	{
		auto_ptr<GPlayerLogoutDBExecuter> executer(NewExecuter<GPlayerLogoutDBExecuter>(m_pPlayerObject));

		GEntityPlayer* pEntityPlayer = m_pPlayerObject->GetEntity();
		CID nCID = pEntityPlayer->GetCID();

		using testing::_;
		EXPECT_CALL(*m_DBManagerWrapper.Get(), Logout(m_pPlayerObject->GetAccountInfo().nAID  , nCID, pEntityPlayer, _, _));
		
		executer->SetState(GPlayerLogoutDBExecuter::LogoutDBTaskState);
		executer->OnLogoutDBTaskState();
	}

	TEST_FIXTURE(FPlayerLogoutDBExecuter, TestPlayerLogoutDBExecuter_OnWaitingLogoutDBTask)	
	{
		auto_ptr<GPlayerLogoutDBExecuter> executer(NewExecuter<GPlayerLogoutDBExecuter>(m_pPlayerObject));
		executer->SetState(GPlayerLogoutDBExecuter::WaitingLogoutDBTaskState);

		MockDBTask dbTask(m_uidPlayer);

		GPlayerLogoutDBExecuter::State oldState = executer->GetState();


		dbTask.IncreaseRefCount(m_uidPlayer);
		executer->OnWaitingLogoutDBTaskState();

		CHECK_EQUAL(oldState, executer->GetState());


		dbTask.DecreaseRefCount(m_uidPlayer);
		executer->OnWaitingLogoutDBTaskState();

		CHECK_EQUAL(true, executer->IsDone());
		CHECK_EQUAL(GPlayerLogoutDBExecuter::FinalState, executer->GetState());
	}

}