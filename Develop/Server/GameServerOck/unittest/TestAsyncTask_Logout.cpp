#include "stdafx.h"
#include "GAsyncTask_Logout.h"
#include "GUnitTestUtil.h"
#include "GUTHelper.h"
#include "MockDBTask.h"
#include "FBaseNetClient.h"
#include "CCommandTable_Master.h"
#include "GDBManager.h"
#include "SAccountInfo.h"
#include "GPlayerObjectManager.h"
#include "GServer.h"
#include "GConfig.h"
#include "GDBTaskDataConnection.h"
#include "GAsyncTask_MoveGameServer.h"
#include "GAsyncTask_MoveLoginServer.h"
#include "GAsyncTaskFactory.h"


SUITE(AsyncTask_Logout)
{
	class MockLogoutTask: public GAsyncTask_Logout
	{
	public:
		MockLogoutTask(MUID uid, uint32 nNowTime, MUID uidPlayer)
			: GAsyncTask_Logout(uid, nNowTime, uidPlayer)
		{
		}

		MOCK_METHOD0(OnStartLogout, bool());
		MOCK_METHOD0(OnLogoutDBExecuteState, bool());
		MOCK_METHOD0(OnCheckMoveServerAsyncTaskState, bool());
		MOCK_METHOD0(OnPublisherLogoutTask, bool());
		MOCK_METHOD0(OnDestroyPlayer, bool());
	};

	
	class MockDBManager: public GDBManager
	{
	public:
		MockDBManager()	{}

		MOCK_METHOD1(DisconnLogInsert, void(const GDBT_DISCONN_LOG& data));
		MOCK_METHOD5(Logout, bool(int64 nAID, int64 nCID, GEntityPlayer* pPlayer, vector<REMAIN_BUFF_TIME>& vecBuffRemainTime, vector<pair<int, float>>& vecTalentCoolTime));
	};


	struct TestAsyncTask_Logout: public FBaseNetClient
	{
		TestAsyncTask_Logout()
		{
			m_uidTask = GUnitTestUtil::NewUID();			
			m_nNowTime = gsys.pSystem->GetNowTime();

			GEntityPlayer* pEntityPlayer = GUTHelper::NewEntityPlayer();
			m_uidPlayer = pEntityPlayer->GetUID();
			
			m_pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(m_uidPlayer);			
		}
		~TestAsyncTask_Logout()
		{
			gsys.pServer->GetAsyncTaskManager()->Clear();
		}

		template<typename T>
		T* NewLogoutTask()
		{
			return new T(m_uidTask, m_nNowTime, m_uidPlayer);
		}

		GAsyncTask_Logout* NewLogoutTask()
		{
			return NewLogoutTask<GAsyncTask_Logout>();
		}

		MUID m_uidTask;
		uint32 m_nNowTime;
		MUID m_uidPlayer;

		GPlayerObject* m_pPlayerObject;

		GTestSysWrapper2<GDBManager, MockDBManager> m_DBManagerWrapper;
	};


	TEST_FIXTURE(TestAsyncTask_Logout, LogoutTaskID)
	{	
		auto_ptr<GAsyncTask_Logout> pTask(NewLogoutTask());
		
		CHECK_EQUAL(ASYNC_TASK_LOGOUT, pTask->GetID());		
	}

	TEST_FIXTURE(TestAsyncTask_Logout, NewAsyncTask)
	{
		auto_ptr<GAsyncTask_Logout> pTask(NewLogoutTask());		

		CHECK_EQUAL(GAsyncTask_Logout::StartLogoutState, pTask->GetState());
	}

	TEST_FIXTURE(TestAsyncTask_Logout, InvalidPlayerUID)
	{		
		m_uidPlayer = GUnitTestUtil::NewUID();
		auto_ptr<GAsyncTask_Logout> pTask(NewLogoutTask());

		CHECK(pTask->IsDone());
	}

	TEST_FIXTURE(TestAsyncTask_Logout, UpdateStartLogoutState)
	{		
		auto_ptr<MockLogoutTask> pTask(NewLogoutTask<MockLogoutTask>());		
		EXPECT_CALL(*pTask, OnStartLogout());
		
		pTask->SetState(GAsyncTask_Logout::StartLogoutState);
		pTask->Update();
	}

	TEST_FIXTURE(TestAsyncTask_Logout, UpdateLogoutDBExecuteState)
	{		
		auto_ptr<MockLogoutTask> pTask(NewLogoutTask<MockLogoutTask>());		
		EXPECT_CALL(*pTask, OnLogoutDBExecuteState());

		pTask->SetState(GAsyncTask_Logout::LogoutDBExecuteState);
		pTask->Update();
	}

	TEST_FIXTURE(TestAsyncTask_Logout, UpdateWaitMoveServerAsyncTaskState)
	{		
		auto_ptr<MockLogoutTask> pTask(NewLogoutTask<MockLogoutTask>());		
		EXPECT_CALL(*pTask, OnCheckMoveServerAsyncTaskState());

		pTask->SetState(GAsyncTask_Logout::CheckMoveServerAsyncTaskState);
		pTask->Update();
	}

	TEST_FIXTURE(TestAsyncTask_Logout, UpdatePublisherLogoutTaskState)
	{		
		auto_ptr<MockLogoutTask> pTask(NewLogoutTask<MockLogoutTask>());
		EXPECT_CALL(*pTask, OnPublisherLogoutTask());

		pTask->SetState(GAsyncTask_Logout::PublisherLogoutTaskState);		
		pTask->Update();
	}

	TEST_FIXTURE(TestAsyncTask_Logout, UpdateDestroyPlayerState)
	{		
		auto_ptr<MockLogoutTask> pTask(NewLogoutTask<MockLogoutTask>());
		EXPECT_CALL(*pTask, OnDestroyPlayer());

		pTask->SetState(GAsyncTask_Logout::DestroyPlayerState);		
		pTask->Update();
	}

	TEST_FIXTURE(TestAsyncTask_Logout, UpdateFinalState)
	{		
		auto_ptr<MockLogoutTask> pTask(NewLogoutTask<MockLogoutTask>());
		
		pTask->SetState(GAsyncTask_Logout::FinalState);		
		pTask->Update();

		CHECK(pTask->IsDone());
	}

	TEST_FIXTURE(TestAsyncTask_Logout, OnStartLogout)
	{		
		auto_ptr<GAsyncTask_Logout> pTask(NewLogoutTask());
		m_pPlayerObject->OnStartGame();

		pTask->OnStartLogout();
		
		CHECK(pTask->IsLeaveGame());
		CHECK_EQUAL(POS_LOGOUT, m_pPlayerObject->GetState());
		CHECK_EQUAL(GAsyncTask_Logout::LogoutDBExecuteState, pTask->GetState());
	}

	TEST_FIXTURE(TestAsyncTask_Logout, OnStartLogout_ForSelectCharacter_InStandAlone)
	{
		bool bOldStandAlone = GConfig::m_bStandAlone;
		GConfig::m_bStandAlone = true;

		m_pPlayerObject->OnEndLogin();
		CHECK_EQUAL(POS_SELECT_CHAR, m_pPlayerObject->GetState());

		auto_ptr<GAsyncTask_Logout> pTask(NewLogoutTask());
		
		pTask->OnStartLogout();
		
		CHECK_EQUAL(GAsyncTask_Logout::DestroyPlayerState, pTask->GetState());

		GConfig::m_bStandAlone = bOldStandAlone;
	}

	//TEST_FIXTURE(TestAsyncTask_Logout, OnStartLogout_ForDuplicate)
	//{
	//	auto_ptr<GAsyncTask_Logout> pTask(NewLogoutTask());	
	//	
	//	m_pPlayerObject->OnStartGame();
	//	pTask->OnStartLogout();
	//	

	//	auto_ptr<GAsyncTask_Logout> pSamePlayerAnotherTask(NewLogoutTask());

	//	pSamePlayerAnotherTask->OnStartLogout();

	//	CHECK_EQUAL(GAsyncTask_Logout::FinalState, pSamePlayerAnotherTask->GetState());
	//}

	TEST_FIXTURE(TestAsyncTask_Logout, OnLogoutDBExecuteState_Success)
	{
		auto_ptr<GAsyncTask_Logout> pTask(NewLogoutTask());
		pTask->SetState(GAsyncTask_Logout::LogoutDBExecuteState);

		MockDBTask dbTask(m_uidPlayer);
		dbTask.IncreaseRefCount(m_uidPlayer);
		pTask->OnLogoutDBExecuteState();

		CHECK_EQUAL(GAsyncTask_Logout::LogoutDBExecuteState, pTask->GetState());

		dbTask.DecreaseRefCount(m_uidPlayer);
		pTask->OnLogoutDBExecuteState();

		CHECK_EQUAL(GAsyncTask_Logout::PublisherLogoutTaskState, pTask->GetState());
	}

	TEST_FIXTURE(TestAsyncTask_Logout, OnCheckMoveServerAsyncTaskState_Success)
	{
		auto_ptr<GAsyncTask_Logout> pTask(NewLogoutTask());
		pTask->SetState(GAsyncTask_Logout::CheckMoveServerAsyncTaskState);

		pTask->OnCheckMoveServerAsyncTaskState();

		CHECK_EQUAL(GAsyncTask_Logout::PublisherLogoutTaskState, pTask->GetState());
	}

	TEST_FIXTURE(TestAsyncTask_Logout, OnCheckMoveServerAsyncTaskState_Success_Exist_AsyncTask_MoveGameServer_Done)
	{
		auto_ptr<GAsyncTask_Logout> pTask(NewLogoutTask());
		pTask->SetState(GAsyncTask_Logout::CheckMoveServerAsyncTaskState);

		GAsyncTask_MoveGameServer* pMoveServerTask = GAsyncTaskFactory::NewMoveGameServerTask(gsys.pServer->GetAsyncTaskManager());
		m_pPlayerObject->SetMoveGameServerAsyncTaskUID(pMoveServerTask->GetUID());

		pTask->OnCheckMoveServerAsyncTaskState();
		CHECK_EQUAL(GAsyncTask_Logout::CheckMoveServerAsyncTaskState, pTask->GetState());

		pMoveServerTask->Done();

		// Act
		pTask->OnCheckMoveServerAsyncTaskState();

		// Assert
		CHECK_EQUAL(GAsyncTask_Logout::PublisherLogoutTaskState, pTask->GetState());
	}

	TEST_FIXTURE(TestAsyncTask_Logout, OnCheckMoveServerAsyncTaskState_Failure_Exist_AsyncTask_MoveGameServer_State_MTS_INITIAL)
	{
		auto_ptr<GAsyncTask_Logout> pTask(NewLogoutTask());
		pTask->SetState(GAsyncTask_Logout::CheckMoveServerAsyncTaskState);

		GAsyncTask_MoveGameServer* pMoveServerTask = GAsyncTaskFactory::NewMoveGameServerTask(gsys.pServer->GetAsyncTaskManager());
		m_pPlayerObject->SetMoveGameServerAsyncTaskUID(pMoveServerTask->GetUID());

		CHECK_EQUAL(GAsyncTask_MoveGameServer::MTS_INITIAL, pMoveServerTask->GetState());

		// Act
		pTask->OnCheckMoveServerAsyncTaskState();

		// Assert
		CHECK_EQUAL(GAsyncTask_Logout::CheckMoveServerAsyncTaskState, pTask->GetState());
	}

	TEST_FIXTURE(TestAsyncTask_Logout, OnCheckMoveServerAsyncTaskState_Failure_Exist_AsyncTask_MoveGameServer_State_MTS_LOGOUT_DB_EXECUTE)
	{
		auto_ptr<GAsyncTask_Logout> pTask(NewLogoutTask());
		pTask->SetState(GAsyncTask_Logout::CheckMoveServerAsyncTaskState);

		GAsyncTask_MoveGameServer* pMoveServerTask = GAsyncTaskFactory::NewMoveGameServerTask(gsys.pServer->GetAsyncTaskManager());
		m_pPlayerObject->SetMoveGameServerAsyncTaskUID(pMoveServerTask->GetUID());

		pMoveServerTask->SetState(GAsyncTask_MoveGameServer::MTS_LOGOUT_DB_EXECUTE);

		// Act
		pTask->OnCheckMoveServerAsyncTaskState();

		// Assert
		CHECK_EQUAL(GAsyncTask_Logout::CheckMoveServerAsyncTaskState, pTask->GetState());
	}

	TEST_FIXTURE(TestAsyncTask_Logout, OnCheckMoveServerAsyncTaskState_Success_Exist_AsyncTask_MoveGameServer_State_MTS_LOGOUT_DB_EXECUTE_Over)
	{
		auto_ptr<GAsyncTask_Logout> pTask(NewLogoutTask());
		pTask->SetState(GAsyncTask_Logout::CheckMoveServerAsyncTaskState);

		GAsyncTask_MoveGameServer* pMoveServerTask = GAsyncTaskFactory::NewMoveGameServerTask(gsys.pServer->GetAsyncTaskManager());
		m_pPlayerObject->SetMoveGameServerAsyncTaskUID(pMoveServerTask->GetUID());

		int nState = GAsyncTask_MoveGameServer::MTS_LOGOUT_DB_EXECUTE + 1;
		pMoveServerTask->SetState((GAsyncTask_MoveGameServer::MoveTaskState)nState);

		// Act
		pTask->OnCheckMoveServerAsyncTaskState();

		// Assert
		CHECK_EQUAL(GAsyncTask_Logout::PublisherLogoutTaskState, pTask->GetState());
	}

	TEST_FIXTURE(TestAsyncTask_Logout, OnCheckMoveServerAsyncTaskState_Success_Exist_AsyncTask_MoveLoginServer_Done)
	{
		auto_ptr<GAsyncTask_Logout> pTask(NewLogoutTask());
		pTask->SetState(GAsyncTask_Logout::CheckMoveServerAsyncTaskState);

		GAsyncTask_MoveLoginServer* pMoveServerTask = GAsyncTaskFactory::NewMoveLoginServerTask(gsys.pServer->GetAsyncTaskManager());
		m_pPlayerObject->SetMoveLoginServerAsyncTaskUID(pMoveServerTask->GetUID());

		pTask->OnCheckMoveServerAsyncTaskState();
		CHECK_EQUAL(GAsyncTask_Logout::CheckMoveServerAsyncTaskState, pTask->GetState());

		pMoveServerTask->Done();

		pTask->OnCheckMoveServerAsyncTaskState();
		CHECK_EQUAL(GAsyncTask_Logout::PublisherLogoutTaskState, pTask->GetState());
	}

	TEST_FIXTURE(TestAsyncTask_Logout, OnCheckMoveServerAsyncTaskState_Failure_Exist_AsyncTask_MoveLoginServer_State_MTS_INITIAL)
	{
		auto_ptr<GAsyncTask_Logout> pTask(NewLogoutTask());
		pTask->SetState(GAsyncTask_Logout::CheckMoveServerAsyncTaskState);

		GAsyncTask_MoveLoginServer* pMoveServerTask = GAsyncTaskFactory::NewMoveLoginServerTask(gsys.pServer->GetAsyncTaskManager());
		m_pPlayerObject->SetMoveLoginServerAsyncTaskUID(pMoveServerTask->GetUID());

		CHECK_EQUAL(GAsyncTask_MoveLoginServer::MTS_INITIAL, pMoveServerTask->GetState());

		// Act
		pTask->OnCheckMoveServerAsyncTaskState();

		// Assert
		CHECK_EQUAL(GAsyncTask_Logout::CheckMoveServerAsyncTaskState, pTask->GetState());
	}

	TEST_FIXTURE(TestAsyncTask_Logout, OnCheckMoveServerAsyncTaskState_Failure_Exist_AsyncTask_MoveLoginServer_State_MTS_LOGOUT_DB_EXECUTE)
	{
		auto_ptr<GAsyncTask_Logout> pTask(NewLogoutTask());
		pTask->SetState(GAsyncTask_Logout::CheckMoveServerAsyncTaskState);

		GAsyncTask_MoveLoginServer* pMoveServerTask = GAsyncTaskFactory::NewMoveLoginServerTask(gsys.pServer->GetAsyncTaskManager());
		m_pPlayerObject->SetMoveLoginServerAsyncTaskUID(pMoveServerTask->GetUID());

		pMoveServerTask->SetState(GAsyncTask_MoveLoginServer::MTS_LOGOUT_DB_EXECUTE);

		// Act
		pTask->OnCheckMoveServerAsyncTaskState();

		// Assert
		CHECK_EQUAL(GAsyncTask_Logout::CheckMoveServerAsyncTaskState, pTask->GetState());
	}

	TEST_FIXTURE(TestAsyncTask_Logout, OnCheckMoveServerAsyncTaskState_Success_Exist_AsyncTask_MoveLoginServer_State_MTS_LOGOUT_DB_EXECUTE_Over)
	{
		auto_ptr<GAsyncTask_Logout> pTask(NewLogoutTask());
		pTask->SetState(GAsyncTask_Logout::CheckMoveServerAsyncTaskState);

		GAsyncTask_MoveLoginServer* pMoveServerTask = GAsyncTaskFactory::NewMoveLoginServerTask(gsys.pServer->GetAsyncTaskManager());
		m_pPlayerObject->SetMoveLoginServerAsyncTaskUID(pMoveServerTask->GetUID());

		int nState = GAsyncTask_MoveLoginServer::MTS_LOGOUT_DB_EXECUTE + 1;
		pMoveServerTask->SetState((GAsyncTask_MoveLoginServer::MoveTaskState)nState);

		// Act
		pTask->OnCheckMoveServerAsyncTaskState();

		// Assert
		CHECK_EQUAL(GAsyncTask_Logout::PublisherLogoutTaskState, pTask->GetState());
	}

	TEST_FIXTURE(TestAsyncTask_Logout, OnPublisherLogoutTask_Success)
	{
		auto_ptr<GAsyncTask_Logout> pTask(NewLogoutTask());
		pTask->SetState(GAsyncTask_Logout::PublisherLogoutTaskState);

		pTask->OnPublisherLogoutTask();

		CHECK_EQUAL(GAsyncTask_Logout::DestroyPlayerState, pTask->GetState());
	}

	TEST_FIXTURE(TestAsyncTask_Logout, OnDestroyPlayer_Success)	
	{
		AID nAID = (int)m_pPlayerObject->GetAccountInfo().nAID;

		GDBT_DISCONN_LOG data(m_pPlayerObject->GetAccountInfo().nAID
			, m_pPlayerObject->GetEntity()->GetCID()
			, m_pPlayerObject->GetEntity()->GetPlayerInfo()->nPlayTimeSec
			, m_pPlayerObject->GetEntity()->GetPlayerInfo()->GetDeltaPlayTime()
			, m_pPlayerObject->GetEntity()->GetMoney()
			, m_pPlayerObject->GetEntity()->GetPlayerInfo()->nLootGainMoney
			, m_pPlayerObject->GetEntity()->GetFieldID());

		EXPECT_CALL(*m_DBManagerWrapper.Get(), DisconnLogInsert(data));

		auto_ptr<GAsyncTask_Logout> pTask(NewLogoutTask());
		pTask->SetState(GAsyncTask_Logout::DestroyPlayerState);

		pTask->OnDestroyPlayer();

		CHECK_EQUAL(MMC_PLAYER_DELETE_PLAYER, m_pNetAgent->GetSendCommandID(0));

		CHECK(NULL == gmgr.pPlayerObjectManager->GetPlayer(m_uidPlayer));
		CHECK_EQUAL(GAsyncTask_Logout::FinalState, pTask->GetState());		
	}
}
