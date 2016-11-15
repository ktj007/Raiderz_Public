#include "stdafx.h"
#include "SUnitTestUtil.h"
#include "GTestWrapper.h"
#include "FForCommandTest.h"
#include "GPartySystem.h"
#include "GPlayerFieldDynamic.h"
#include "GConfig.h"
#include "GPartyManager.h"
#include "GCommand.h"
#include "GDynamicFieldPolicy.h"
#include "GDynamicFieldMaster.h"
#include "GPlayerBattleArena.h"
#include "MockDuel.h"
#include "GUTHelper_Duel.h"
#include "GPlayerPVPArea.h"
#include "GFieldAttrGrid.h"
#include "GPlayerDoing.h"
#include "GTestForward.h"
#include "CCommandResultTable.h"
#include "GProxyParty.h"


SUITE(PartySystem)
{
	struct TestPartySystemStub: public GPartySystem
	{
		GParty* NewParty(MUID uidParty, MUID uidLeader, wstring strLeaderName, int nLeaderCID)
		{
			GParty* pParty = m_pPartyManager->NewParty(uidParty);

			pParty->AddMember(uidLeader, strLeaderName, nLeaderCID);
			pParty->SetLeader(uidLeader);
			
			m_pPartyManager->AddParty(pParty);

			gsys.pMasterServerFacade->AddProxyParty(uidParty, uidLeader, strLeaderName, nLeaderCID);

			return pParty;
		}
	};

	struct MockDynamicFieldPolicy: public GDynamicFieldPolicy
	{
		MUID m_uidPlayer;
		MUID m_uidField;
		vector<int> m_vecQuestID;

		MUID m_uidOldPlayer;
		MUID m_uidNewPlayer;

		MockDynamicFieldPolicy(GDynamicFieldMaster* pOwner): GDynamicFieldPolicy(pOwner)
		{
		}

		virtual void OnQuestSynch(MUID uidPlayer, MUID uidField, const vector<int>* pVecQuestID) override
		{
			m_uidPlayer = uidPlayer;
			m_uidField = uidField;
			m_vecQuestID = *pVecQuestID;
		}

		virtual void OnQuestEraseRunner(MUID uidPlayer) override
		{
			m_uidPlayer = uidPlayer;
		}

		virtual void ChangePlayerUID(MUID uidOlePlayer, MUID uidNewPlayer) override
		{
			m_uidOldPlayer = uidOlePlayer;
			m_uidNewPlayer = uidNewPlayer;
		}
	};

	struct Fixture: public FForCommandTest
	{
		GTestSysWrapper2<GPartySystem, TestPartySystemStub>	m_PartySystemWrapper;
		TestPartySystemStub* m_pPartySystemStub;

		GEntityPlayer* m_pTargetPlayer;
		MockLink* m_pTargetLink;

		Fixture()	
		{
			m_pPartySystemStub = m_PartySystemWrapper.Get();
			m_pNetAgent->IgnoreCmd(MMC_PLAYER_IN_FIELD_NOTIFY);


			m_pLink->AddIgnoreID(MC_FIELD_IN_PLAYER);
			m_pLink->AddIgnoreID(MC_FIELD_IN_NPC);
			m_pLink->AddIgnoreID(MC_NPCINTERACTION_ICON);

			
			m_pTargetPlayer = NewEntityPlayer(m_pField, vec3(100,100,0));
			m_pTargetLink = NewLink(m_pTargetPlayer);
			m_pTargetLink->AddIgnoreID(MC_FATIGUE_UPDATE);
			m_pTargetLink->AddIgnoreID(MC_TIME_SYNCH);		
			m_pTargetLink->AddIgnoreID(MC_FIELD_IN_PLAYER);
		}

		~Fixture()	{}
	};

	// Feature	//////////////////////////////////////////////////////////////////////////////////////////////
	TEST_FIXTURE(Fixture, TestInviteReq)
	{
		MUID uidTargetPlayer = SUnitTestUtil::NewUID();
		gsys.pPartySystem->InviteReq(uidTargetPlayer, m_pPlayer->GetUID());

		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_INVITE_REQ);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidTargetPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), m_pPlayer->GetUID());
	}

	TEST_FIXTURE(Fixture, TestInviteReq_UnkownRequestorFailure)	
	{
		MUID uidUnkownRequestPlayer = SUnitTestUtil::NewUID();
		MUID uidTargetPlayer = SUnitTestUtil::NewUID();
		gsys.pPartySystem->InviteReq(uidTargetPlayer, uidUnkownRequestPlayer);

		ASSERT_EQUAL(m_pNetAgent->GetSendCommandCount(), 0);
		CHECK_EQUAL(m_pLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestInviteReq_InvalidTargetFailure)	

	{
		MUID uidTargetPlayer = MUID::Invalid();
		gsys.pPartySystem->InviteReq(uidTargetPlayer, m_pPlayer->GetUID());

		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_INVITE);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_SYSTEM_INVALID_PC_UID);
	}

	TEST_FIXTURE(Fixture, TestInviteReq_InviteSelfFailure)	
	{
		gsys.pPartySystem->InviteReq(m_pPlayer->GetUID(), m_pPlayer->GetUID());

		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_INVITE);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_PARTY_NOT_INVITE_SELF);
	}

	TEST_FIXTURE(Fixture, TestInviteReq_NotLeaderFailure)	
	{
		MUID uidParty = SUnitTestUtil::NewUID();
		m_pPlayer->SetParty(uidParty);

		MUID uidTargetPlayer = SUnitTestUtil::NewUID();
		gsys.pPartySystem->InviteReq(uidTargetPlayer, m_pPlayer->GetUID());

		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_INVITE);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_PARTY_NOT_PARTY_LEADER);
	}	

	TEST_FIXTURE(Fixture, TestInviteReq_PartyFullFailure)	
	{
		m_pLink->AddIgnoreID(MC_PARTY_REFRESH_INFO_ALL);
		m_pLink->AddIgnoreID(MC_PARTY_NOTIFY_JOIN);

		MUID uidParty = SUnitTestUtil::NewUID();
		m_pPlayer->SetParty(uidParty);
		m_pPlayer->SetPartyLeader(true);			

		gsys.pMasterServerFacade->AddProxyParty(uidParty, m_pPlayer->GetUID(), m_pPlayer->GetName(), m_pPlayer->GetCID());
		gsys.pPartySystem->AddParty(uidParty, m_pPlayer->GetUID());
		
		for(int i = 0; i < 100; i++)
		{
			gsys.pPartySystem->AddMember(uidParty, SUnitTestUtil::NewUID(), L"TestMemberName", SUnitTestUtil::NewCID());
		}
	
		MUID uidTargetPlayer = SUnitTestUtil::NewUID();
		gsys.pPartySystem->InviteReq(uidTargetPlayer, m_pPlayer->GetUID());

		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_INVITE);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_PARTY_FULL_PARTY);
	}

	TEST_FIXTURE(Fixture, TestInviteRes)	
	{
		MUID uidTargetPlayer = SUnitTestUtil::NewUID();
		CCommandResultTable nResult = CR_FAIL;

		gsys.pPartySystem->InviteRes(m_pPlayer->GetUID(), uidTargetPlayer, nResult);

		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_INVITE);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), nResult);
	}

	TEST_FIXTURE(Fixture, TestInviteRes_UnkownRequestPlayerFailure)
	{
		MUID uidUnkownRequestPlayer = SUnitTestUtil::NewUID();
		MUID uidTargetPlayer = SUnitTestUtil::NewUID();
		CCommandResultTable nResult = CR_SUCCESS;

		gsys.pPartySystem->InviteRes(uidUnkownRequestPlayer, uidTargetPlayer, nResult);

		ASSERT_EQUAL(m_pLink->GetCommandCount(), 0);
	}
	
	TEST_FIXTURE(Fixture, TestAcceptReq)	
	{
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();
		wstring strRequestPlayerName = L"TestRequestPlayerName";

		gsys.pPartySystem->AcceptReq(m_pPlayer->GetUID(), uidRequestPlayer, strRequestPlayerName);


		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_INVITE_QUESTION);
		CHECK_EQUAL(m_pLink->GetParam<MUID>(0, 0), uidRequestPlayer);
		CHECK(m_pLink->GetParam<const wchar_t*>(0, 1) == strRequestPlayerName);
		CHECK_EQUAL(m_pLink->GetParam<bool>(0, 2), m_pPlayer->IsNowCombat());
	}

	TEST_FIXTURE(Fixture, TestAcceptReq_UnkownTargetPlayerFilure)	
	{
		MUID uidUnkownTargetPlayer = SUnitTestUtil::NewUID();
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();
		wstring strRequestPlayerName = L"TestRequestPlayerName";

		gsys.pPartySystem->AcceptReq(uidUnkownTargetPlayer, uidRequestPlayer, strRequestPlayerName);


		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_ACCEPT_RES);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), uidUnkownTargetPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 2), CR_FAIL_SYSTEM_INVALID_PC_UID);
	}

	TEST_FIXTURE(Fixture, TestAcceptReq_StayDynamicFieldFailure)	
	{
		GField* pField = gmgr.pFieldMgr->GetDynamicField(m_uidDynamicFieldGroup, 100);
		m_pPlayer->LeaveFromField();
		m_pPlayer->EnterToField(pField, vec3(100, 100, 100), vec3(0, 1, 0));

		MUID uidRequestPlayer = SUnitTestUtil::NewUID();
		wstring strRequestPlayerName = L"TestRequestPlayerName";

		gsys.pPartySystem->AcceptReq(m_pPlayer->GetUID(), uidRequestPlayer, strRequestPlayerName);


		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_ACCEPT_RES);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), m_pPlayer->GetUID());
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 2), CR_FAIL_PARTY_NOT_INVITE_TO_PLAYER_IN_TRIALFIELD);		
	}

	TEST_FIXTURE(Fixture, TestAcceptReq_AlreadyHasPartyFailure)	
	{
		MUID uidParty = SUnitTestUtil::NewUID();
		m_pPlayer->SetParty(uidParty);

		MUID uidRequestPlayer = SUnitTestUtil::NewUID();
		wstring strRequestPlayerName = L"TestRequestPlayerName";

		gsys.pPartySystem->AcceptReq(m_pPlayer->GetUID(), uidRequestPlayer, strRequestPlayerName);


		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_ACCEPT_RES);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), m_pPlayer->GetUID());
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 2), CR_FAIL_PARTY_TARGET_ALEADY_HAS_PARTY);	
	}

	TEST_FIXTURE(Fixture, TestAcceptReq_BusyFailure)	
	{
		m_pPlayer->GetDoing().ChangeDoing(CD_INTERACTING);

		MUID uidRequestPlayer = SUnitTestUtil::NewUID();
		wstring strRequestPlayerName = L"TestRequestPlayerName";

		gsys.pPartySystem->AcceptReq(m_pPlayer->GetUID(), uidRequestPlayer, strRequestPlayerName);


		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_ACCEPT_RES);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), m_pPlayer->GetUID());
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 2), CR_FAIL_PARTY_OPPONENT_IS_BUSY);	
	}

	TEST_FIXTURE(Fixture, TestAcceptRes_AceeptOK)	
	{
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();
		m_pPlayer->SetPartyInvitorUID(uidRequestPlayer);

		PARTY_RESPOND nRespond = PRS_OK;		
		gsys.pPartySystem->AcceptRes(m_pPlayer->GetUID(), nRespond);
		
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_ACCEPT_RES);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), m_pPlayer->GetUID());
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 2), CR_SUCCESS);
	}

	TEST_FIXTURE(Fixture, TestAcceptRes_Refuse)
	{
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();
		m_pPlayer->SetPartyInvitorUID(uidRequestPlayer);

		PARTY_RESPOND nRespond = PRS_REFUSE;		
		gsys.pPartySystem->AcceptRes(m_pPlayer->GetUID(), nRespond);
		
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_ACCEPT_RES);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), m_pPlayer->GetUID());
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 2), CR_FAIL_PARTY_REFUSE_INVITATION);
	}

	TEST_FIXTURE(Fixture, TestAcceptRes_InvalidRequestPlayerFailure)
	{
		PARTY_RESPOND nRespond = PRS_OK;
		gsys.pPartySystem->AcceptRes(m_pPlayer->GetUID(), nRespond);
		
		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_SYSTEM_INVALID_PC_UID);
	}

	TEST_FIXTURE(Fixture, TestAcceptRes_AlreadyHasPartyFailure)
	{
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();
		MUID uidParty = SUnitTestUtil::NewUID();

		m_pPlayer->SetPartyInvitorUID(uidRequestPlayer);
		m_pPlayer->SetParty(uidParty);

		PARTY_RESPOND nRespond = PRS_OK;		
		gsys.pPartySystem->AcceptRes(m_pPlayer->GetUID(), nRespond);
		
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_ACCEPT_RES);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), m_pPlayer->GetUID());
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 2), CR_FAIL_PARTY_TARGET_ALEADY_HAS_PARTY);

		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_PARTY_TARGET_ALEADY_HAS_PARTY);
	}

	TEST_FIXTURE(Fixture, TestAcceptRes_UnkownPlayerFailure)	
	{
		MUID uidUnkownPlayer = SUnitTestUtil::NewUID();
		PARTY_RESPOND nRespond = PRS_OK;
		gsys.pPartySystem->AcceptRes(uidUnkownPlayer, nRespond);
		
		CHECK_EQUAL(m_pNetAgent->GetSendCommandCount(), 0);
		CHECK_EQUAL(m_pLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestAcceptCancel)	
	{		
		CCommandResultTable nResult = CR_FAIL;
		gsys.pPartySystem->AcceptCancel(m_pPlayer->GetUID(), nResult);

		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_ACCEPT_CANCEL);		
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), nResult);
	}

	TEST_FIXTURE(Fixture, TestAcceptCancel_UnkownPlayerFailure)
	{
		MUID uidUnkownPlayer = SUnitTestUtil::NewUID();
		CCommandResultTable nResult = CR_FAIL;
		gsys.pPartySystem->AcceptCancel(uidUnkownPlayer, nResult);
		
		CHECK_EQUAL(m_pLink->GetCommandCount(), 0);
	}

	// Sync	//////////////////////////////////////////////////////////////////////////////////////////////////

	TEST_FIXTURE(Fixture, TestAddParty_LeaderServer)
	{
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();

		CHECK(NULL == gsys.pPartySystem->FindParty(uidParty));
		CHECK_EQUAL(m_pPlayer->GetPartyUID(), MUID::Invalid());
		CHECK(!m_pPlayer->IsPartyLeader());


		gsys.pMasterServerFacade->AddProxyParty(uidParty, uidLeader, m_pPlayer->GetName(), m_pPlayer->GetCID());
		gsys.pPartySystem->AddParty(uidParty, uidLeader);

		
		// 파티 생성 확인
		GParty* pParty = gsys.pPartySystem->FindParty(uidParty);
		ASSERT_CHECK(pParty != NULL);

		// 파티장 설정 확인
		CHECK_EQUAL(pParty->GetLeader(), uidLeader);
		CHECK_EQUAL(m_pPlayer->GetPartyUID(), uidParty);
		CHECK(m_pPlayer->IsPartyLeader());

		// 동기화 커맨드 확인
		CHECK_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_ADD_QUEST);
		CHECK_EQUAL(m_pNetAgent->GetSendCommandID(1), MMC_PARTY_UPDATE_QUEST_RUNNER);
		CHECK_EQUAL(m_pNetAgent->GetSendCommandID(2), MMC_PARTY_UPDATE);
	}

	TEST_FIXTURE(Fixture, TestAddParty_NoLeaderServer)
	{
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = SUnitTestUtil::NewUID();
		gsys.pPartySystem->AddParty(uidParty, uidLeader);

		CHECK(NULL == gsys.pPartySystem->FindParty(uidParty));
	}

	TEST_FIXTURE(Fixture, TestAddMember_MemberOnly)	
	{
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = SUnitTestUtil::NewUID();
		wstring strLeaderName = L"TestLeaderName";
		int nLeaderCID = SUnitTestUtil::NewCID();
		
		gsys.pMasterServerFacade->AddProxyParty(uidParty, uidLeader, strLeaderName, nLeaderCID);

		
		MUID uidMember = m_pPlayer->GetUID();
		gsys.pMasterServerFacade->AddProxyPartyMember(uidParty, uidMember, m_pPlayer->GetName(), m_pPlayer->GetCID());
		gsys.pPartySystem->AddMember(uidParty, uidMember, m_pPlayer->GetName(), m_pPlayer->GetCID()); 


		GParty* pParty = gsys.pPartySystem->FindParty(uidParty);
		ASSERT_CHECK(pParty != NULL);

		CHECK_EQUAL(pParty->GetMemberCount(), 2);
		CHECK_EQUAL(pParty->GetLeader(), uidLeader);
		CHECK(pParty->IsExistMember(uidLeader));
		CHECK(pParty->IsExistMember(uidMember));

		// 동기화 커맨드 확인
		CHECK_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_ADD_QUEST);
		CHECK_EQUAL(m_pNetAgent->GetSendCommandID(1), MMC_PARTY_UPDATE_QUEST_RUNNER);
		CHECK_EQUAL(m_pNetAgent->GetSendCommandID(2), MMC_PARTY_UPDATE);
		CHECK_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_NOTIFY_JOIN);
	}

	TEST_FIXTURE(Fixture, TestAddMember_PartyOnly)	
	{
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();

		gsys.pMasterServerFacade->AddProxyParty(uidParty, uidLeader,m_pPlayer->GetName(), m_pPlayer->GetCID());
		gsys.pPartySystem->AddParty(uidParty, uidLeader);

		GParty* pParty = gsys.pPartySystem->FindParty(uidParty);
		ASSERT_CHECK(pParty != NULL);


		MUID uidMember = SUnitTestUtil::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = SUnitTestUtil::NewCID();
		gsys.pMasterServerFacade->AddProxyPartyMember(uidParty, uidMember, strMemberName, nMemberCID);
		gsys.pPartySystem->AddMember(uidParty, uidMember, strMemberName, nMemberCID);


		// 파티워 추가 확인
		CHECK(pParty->IsExistMember(uidMember));

		// 파티원들에게 통보
		CHECK_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_NOTIFY_JOIN);
	}	

	TEST_FIXTURE(Fixture, TestAddMember_NoPartyNoMember)
	{
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidMember = SUnitTestUtil::NewUID();
		int nMemberCID = SUnitTestUtil::NewCID();
		gsys.pPartySystem->AddMember(uidParty, uidMember, L"TestMemberName", nMemberCID);

		CHECK(NULL == gsys.pPartySystem->FindParty(uidParty));
	}

	TEST_FIXTURE(Fixture, TestAddMember_PartyFullFailure)	
	{
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();

		gsys.pMasterServerFacade->AddProxyParty(uidParty, uidLeader,m_pPlayer->GetName(), m_pPlayer->GetCID());
		gsys.pPartySystem->AddParty(uidParty, uidLeader);

		GParty* pParty = gsys.pPartySystem->FindParty(uidParty);
		ASSERT_CHECK(pParty != NULL);

		while(!pParty->IsFull())
		{
			pParty->AddMember(SUnitTestUtil::NewUID(), L"TestMemberName", SUnitTestUtil::NewCID());
		}


		MUID uidMember = SUnitTestUtil::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = SUnitTestUtil::NewCID();
		gsys.pMasterServerFacade->AddProxyPartyMember(uidParty, uidMember, strMemberName, nMemberCID);
		gsys.pPartySystem->AddMember(uidParty, uidMember, strMemberName, nMemberCID);


		CHECK(!pParty->IsExistMember(uidMember));
	}

	TEST_FIXTURE(Fixture, TestSyncParty)
	{
		TD_PARTY tdParty;
		tdParty.m_uidParty = SUnitTestUtil::NewUID();
		tdParty.m_partySetting.m_uidLeader = m_pPlayer->GetUID();

		wstring strLeaderName = m_pPlayer->GetName();
		int nLeaderCID = m_pPlayer->GetCID();

		m_pPartySystemStub->NewParty(tdParty.m_uidParty, tdParty.m_partySetting.m_uidLeader, strLeaderName, nLeaderCID);

		
		ASSERT_EQUAL(5, MAX_PARTY_MEMBER_COUNT);
		auto_ptr<MCommand> pCommand(MakeNewCommand(MMC_PARTY_SYNC,
											7,
											NEW_BLOB(),
											NEW_BLOB(),
											NEW_BLOB(),
											NEW_BLOB(),
											NEW_BLOB(),
											NEW_BLOB(),
											NEW_BLOB())
									);
		
		gsys.pPartySystem->SyncParty(tdParty, pCommand.get());


		CHECK_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_REFRESH_INFO_ALL);
	}

	TEST_FIXTURE(Fixture, TestSyncParty_NoPartyServer)
	{
		TD_PARTY tdParty;
		tdParty.m_uidParty = SUnitTestUtil::NewUID();

		ASSERT_EQUAL(5, MAX_PARTY_MEMBER_COUNT);

		auto_ptr<MCommand> pCommand(MakeNewCommand(MMC_PARTY_SYNC,
											7,
											NEW_BLOB(),
											NEW_BLOB(),
											NEW_BLOB(),
											NEW_BLOB(),
											NEW_BLOB(),
											NEW_BLOB(),
											NEW_BLOB())
									);
		
		gsys.pPartySystem->SyncParty(tdParty, pCommand.get());

		CHECK_EQUAL(m_pLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestSyncMember)	
	{
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		int nLeaderCID = m_pPlayer->GetCID();

		m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);

		auto_ptr<MCommand> pCommand(MakeNewCommand(MMC_PARTY_MEMBER_SYNC,
											3,														
											NEW_UID(),
											NEW_BLOB(),
											NEW_BLOB())
									);
		
		gsys.pPartySystem->SyncMember(uidParty, pCommand.get());

		CHECK_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_REFRESH_INFO);
	}

	TEST_FIXTURE(Fixture, TestSyncMember_NoPartyServer)	
	{
		MUID uidParty = SUnitTestUtil::NewUID();
		auto_ptr<MCommand> pCommand(MakeNewCommand(MMC_PARTY_MEMBER_SYNC,
											3,														
											NEW_UID(),
											NEW_BLOB(),
											NEW_BLOB())
									);
		
		gsys.pPartySystem->SyncMember(uidParty, pCommand.get());

		CHECK_EQUAL(m_pLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestSyncQuest)	
	{
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		int nLeaderCID = m_pPlayer->GetCID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName,  nLeaderCID);

		CHECK(!pParty->HasQuest(100));
		CHECK(!pParty->HasQuest(200));
		CHECK(!pParty->HasQuest(300));


		map<int, int> mapQuests;
		mapQuests.insert(make_pair(100, 1));
		mapQuests.insert(make_pair(200, 3));
		mapQuests.insert(make_pair(300, 2));

		gsys.pPartySystem->SyncQuest(uidParty, mapQuests);


		CHECK(pParty->HasQuest(100));
		CHECK(pParty->HasQuest(200));
		CHECK(pParty->HasQuest(300));
	}

	TEST_FIXTURE(Fixture, TestSyncField) 
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		int nLeaderCID = m_pPlayer->GetCID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		pParty->CreateField(10, MUID(100));
		
		map<int, MUID> mapFields;
		mapFields.insert(make_pair(100, MUID(1)));
		mapFields.insert(make_pair(200, MUID(2)));
		mapFields.insert(make_pair(300, MUID(3)));


		// act
		gsys.pPartySystem->SyncField(uidParty, mapFields);
		
		
		// assert
		CHECK_EQUAL(pParty->FindField(10), MUID::Invalid());

		CHECK_EQUAL(pParty->FindField(100), MUID(1));
		CHECK_EQUAL(pParty->FindField(200), MUID(2));
		CHECK_EQUAL(pParty->FindField(300), MUID(3));		
	}

	TEST_FIXTURE(Fixture, TestSyncQuestRunner)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidMember = SUnitTestUtil::NewUID();
		MUID uidField = SUnitTestUtil::NewUID();
		vector<int> vecQuestID;
		vecQuestID.push_back(100);
		vecQuestID.push_back(200);
		vecQuestID.push_back(300);

		
		GDynamicFieldMaster* pDynamicFieldMaster = gmgr.pFieldMgr->GetDynamicFieldMaster(m_uidDynamicFieldGroup);
		ASSERT_CHECK(pDynamicFieldMaster != NULL);
		MockDynamicFieldPolicy* pMockPolicy = new MockDynamicFieldPolicy(pDynamicFieldMaster);
		pDynamicFieldMaster->SetPolicy(pMockPolicy);

		MUID uidLeader = m_pPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		int nLeaderCID = m_pPlayer->GetCID();
		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		pParty->CreateField(m_nDynamicFieldID, m_uidDynamicFieldGroup);


		// act
		gsys.pPartySystem->SyncQuestRunner(uidParty, uidMember, uidField, vecQuestID);


		// assert
		CHECK_EQUAL(pMockPolicy->m_uidPlayer, uidMember);
		CHECK_EQUAL(pMockPolicy->m_uidField, uidField);
		CHECK(pMockPolicy->m_vecQuestID == vecQuestID);
	}

	TEST_FIXTURE(Fixture, TestConveyanceLeader)		
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		MUID uidNewLeader = m_pTargetPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		wstring strNewLeaderName = m_pTargetPlayer->GetName();
		int nLeaderCID = m_pPlayer->GetCID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);
		pParty->AddMember(uidNewLeader, strNewLeaderName, nLeaderCID);		
		CHECK_EQUAL(pParty->GetLeader(), uidLeader);

		m_pPlayer->SetParty(uidParty);
		m_pPlayer->SetPartyLeader(true);
		m_pTargetPlayer->SetParty(uidParty);
		CHECK(m_pPlayer->HasParty());
		CHECK(m_pPlayer->IsPartyLeader());
		CHECK(m_pTargetPlayer->HasParty());
		CHECK(!m_pTargetPlayer->IsPartyLeader());


		// act
		gsys.pPartySystem->ChangePartyLeaderRes(uidParty, uidNewLeader);


		// assert
		CHECK_EQUAL(pParty->GetLeader(), uidNewLeader);
		CHECK(m_pPlayer->HasParty());
		CHECK(!m_pPlayer->IsPartyLeader());
		CHECK(m_pTargetPlayer->HasParty());
		CHECK(m_pTargetPlayer->IsPartyLeader());
	}

	TEST_FIXTURE(Fixture, TestConveyanceLeader_UnknownPartyFailure)		
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		MUID uidNewLeader = m_pTargetPlayer->GetUID();

		m_pPlayer->SetParty(uidParty);
		m_pPlayer->SetPartyLeader(true);
		m_pTargetPlayer->SetParty(uidParty);
		CHECK(m_pPlayer->HasParty());
		CHECK(m_pPlayer->IsPartyLeader());
		CHECK(m_pTargetPlayer->HasParty());
		CHECK(!m_pTargetPlayer->IsPartyLeader());


		// act
		gsys.pPartySystem->ChangePartyLeaderRes(uidParty, uidNewLeader);


		// assert
		CHECK(m_pPlayer->HasParty());
		CHECK(m_pPlayer->IsPartyLeader());
		CHECK(m_pTargetPlayer->HasParty());
		CHECK(!m_pTargetPlayer->IsPartyLeader());
	}	

	TEST_FIXTURE(Fixture, TestConveyanceLeader_NoLeaderServer)
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = SUnitTestUtil::NewUID();
		MUID uidNewLeader = m_pTargetPlayer->GetUID();
		wstring strLeaderName = L"TestLeaderName";
		wstring strNewLeaderName = m_pTargetPlayer->GetName();
		int nLeaderCID = SUnitTestUtil::NewCID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);
		pParty->AddMember(uidNewLeader, strNewLeaderName, nLeaderCID);
		CHECK_EQUAL(pParty->GetLeader(), uidLeader);

		m_pTargetPlayer->SetParty(uidParty);
		CHECK(m_pTargetPlayer->HasParty());
		CHECK(!m_pTargetPlayer->IsPartyLeader());


		// act
		gsys.pPartySystem->ChangePartyLeaderRes(uidParty, uidNewLeader);


		// assert
		CHECK_EQUAL(pParty->GetLeader(), uidNewLeader);
		CHECK(m_pTargetPlayer->HasParty());
		CHECK(m_pTargetPlayer->IsPartyLeader());
	}

	TEST_FIXTURE(Fixture, TestConveyanceLeader_NoNewLeaderServer)		
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		MUID uidNewLeader = SUnitTestUtil::NewUID();
		wstring strLeaderName = m_pPlayer->GetName();
		wstring strNewLeaderName = L"TestNewLeaderName";
		int nLeaderCID = m_pPlayer->GetCID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);
		pParty->AddMember(uidNewLeader, strNewLeaderName, nLeaderCID);
		CHECK_EQUAL(pParty->GetLeader(), uidLeader);

		m_pPlayer->SetParty(uidParty);
		m_pPlayer->SetPartyLeader(true);
		CHECK(m_pPlayer->HasParty());
		CHECK(m_pPlayer->IsPartyLeader());


		// act
		gsys.pPartySystem->ChangePartyLeaderRes(uidParty, uidNewLeader);


		// assert
		CHECK_EQUAL(pParty->GetLeader(), uidNewLeader);
		CHECK(m_pPlayer->HasParty());
		CHECK(!m_pPlayer->IsPartyLeader());
	}

	TEST_FIXTURE(Fixture, TestLeaveReq)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeavePlayer = m_pPlayer->GetUID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeavePlayer, m_pPlayer->GetName(), m_pPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);
		m_pPlayer->SetParty(uidParty);
		m_pPlayer->SetPartyLeader(true);
		CHECK(m_pPlayer->HasParty());
		CHECK(m_pPlayer->IsPartyLeader());

		
		// act
		gsys.pPartySystem->LeaveReq(uidLeavePlayer);


		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_LEAVE_REQ);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), uidLeavePlayer);
	}

	TEST_FIXTURE(Fixture, TestLeaveReq_UnknownPlayerFailure)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeavePlayer = SUnitTestUtil::NewUID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeavePlayer, m_pPlayer->GetName(), m_pPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);
		m_pPlayer->SetParty(uidParty);
		m_pPlayer->SetPartyLeader(true);
		CHECK(m_pPlayer->HasParty());
		CHECK(m_pPlayer->IsPartyLeader());

		
		// act
		gsys.pPartySystem->LeaveReq(uidLeavePlayer);


		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandCount(), 0);
		ASSERT_EQUAL(m_pLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestLeaveReq_NotHasPartyFailure)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeavePlayer = m_pPlayer->GetUID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeavePlayer, m_pPlayer->GetName(), m_pPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);

		
		// act
		gsys.pPartySystem->LeaveReq(uidLeavePlayer);


		// assert
		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_INVITE);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_PARTY_NOT_IN_PARTY);
	}
	
	TEST_FIXTURE(Fixture, TestLeaveRes)	
	{
		// arrange
		MUID uidLeavePlayer = m_pPlayer->GetUID();
		CCommandResultTable nResult = CR_SUCCESS;


		// act
		gsys.pPartySystem->LeaveRes(uidLeavePlayer, nResult);


		// assert
		CHECK_EQUAL(m_pLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestLeaveRes_Failure)	
	{
		// arrange
		MUID uidLeavePlayer = m_pPlayer->GetUID();
		CCommandResultTable nResult = CR_FAIL;


		// act
		gsys.pPartySystem->LeaveRes(uidLeavePlayer, nResult);


		// assert
		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_INVITE);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), nResult);
	}
	
	TEST_FIXTURE(Fixture, TestRemoveMember)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		MUID uidMember = m_pTargetPlayer->GetUID();
		wstring strMemberName = m_pTargetPlayer->GetName();
		int nLeaderCID = m_pPlayer->GetCID();
		int nMemberCID = m_pTargetPlayer->GetCID();

		m_pNetAgent->IgnoreCmd(MMC_PARTY_ADD_QUEST);
		m_pNetAgent->IgnoreCmd(MMC_PARTY_ADD_FIELD);
		m_pNetAgent->IgnoreCmd(MMC_PARTY_UPDATE_QUEST_RUNNER);
		m_pNetAgent->IgnoreCmd(MMC_PARTY_UPDATE);

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);
		m_pLink->AddIgnoreID(MC_PARTY_NOTIFY_JOIN);
		m_pTargetLink->AddIgnoreID(MC_PARTY_NOTIFY_JOIN);
		m_pPartySystemStub->AddMember(uidParty, uidMember, strMemberName, nMemberCID);

		m_pPlayer->SetParty(uidParty);
		m_pPlayer->SetPartyLeader(true);
		m_pTargetPlayer->SetParty(uidParty);
		CHECK(m_pPlayer->HasParty());
		CHECK(m_pPlayer->IsPartyLeader());
		CHECK(m_pTargetPlayer->HasParty());
		CHECK(!m_pTargetPlayer->IsPartyLeader());


		// act
		gsys.pPartySystem->RemoveMember(uidParty, uidMember);


		// assert
		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_NOTIFY_LEAVE);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MC_PARTY_NOTIFY_LEAVE);

		CHECK(!pParty->IsExistMember(uidMember));
		CHECK(!m_pTargetPlayer->HasParty());
	}

	TEST_FIXTURE(Fixture, TestRemoveMember_NoMemberInServer)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = SUnitTestUtil::NewUID();
		wstring strLeaderName = L"TestLeaderName";
		int nLeaderCID = SUnitTestUtil::NewCID();
		MUID uidMember = m_pTargetPlayer->GetUID();
		wstring strMemberName = m_pTargetPlayer->GetName();		
		int nMemberCID = m_pTargetPlayer->GetCID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);
		m_pPartySystemStub->AddMember(uidParty, uidMember, strMemberName, nMemberCID);

		m_pTargetPlayer->SetParty(uidParty);
		CHECK(m_pTargetPlayer->HasParty());
		CHECK(!m_pTargetPlayer->IsPartyLeader());


		// act
		gsys.pPartySystem->RemoveMember(uidParty, uidMember);


		// assert
		pParty = gsys.pPartySystem->FindParty(uidParty);
		CHECK(pParty == NULL);
	}

	TEST_FIXTURE(Fixture, TestRemoveMember_LastMember)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		int nLeaderCID = m_pPlayer->GetCID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);
		
		m_pPlayer->SetParty(uidParty);
		m_pPlayer->SetPartyLeader(true);
		CHECK(m_pPlayer->HasParty());
		CHECK(m_pPlayer->IsPartyLeader());


		// act
		gsys.pPartySystem->RemoveMember(uidParty, uidLeader);


		// assert
		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_NOTIFY_LEAVE);
		CHECK(!m_pPlayer->HasParty());
		CHECK(!m_pPlayer->IsPartyLeader());
	}

	TEST_FIXTURE(Fixture, TestSyncEraseQuestRunner)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidMember = SUnitTestUtil::NewUID();
		MUID uidField = SUnitTestUtil::NewUID();

		GDynamicFieldMaster* pDynamicFieldMaster = gmgr.pFieldMgr->GetDynamicFieldMaster(m_uidDynamicFieldGroup);
		ASSERT_CHECK(pDynamicFieldMaster != NULL);
		MockDynamicFieldPolicy* pMockPolicy = new MockDynamicFieldPolicy(pDynamicFieldMaster);
		pDynamicFieldMaster->SetPolicy(pMockPolicy);

		MUID uidLeader = m_pPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		int nLeaderCID = m_pPlayer->GetCID();
		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		pParty->CreateField(m_nDynamicFieldID, m_uidDynamicFieldGroup);


		// act
		gsys.pPartySystem->SyncEraseQuestRunner(uidParty, uidMember);


		// assert
		CHECK_EQUAL(pMockPolicy->m_uidPlayer, uidMember);
	}

	TEST_FIXTURE(Fixture, TestKickReq)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidRequestPlayer = m_pPlayer->GetUID();
		MUID uidTargetPlayer = m_pTargetPlayer->GetUID();
		wstring strRequestPlayerName = m_pPlayer->GetName();
		wstring strTargetPlayerName = m_pTargetPlayer->GetName();
		int nRequestPlayerCID = m_pPlayer->GetCID();
		int nTargetPlayerCID = m_pTargetPlayer->GetCID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidRequestPlayer, strRequestPlayerName, nRequestPlayerCID);
		ASSERT_CHECK(pParty != NULL);
		pParty->AddMember(uidTargetPlayer, strTargetPlayerName, nTargetPlayerCID);
		CHECK_EQUAL(pParty->GetLeader(), uidRequestPlayer);

		m_pPlayer->SetParty(uidParty);
		m_pPlayer->SetPartyLeader(true);
		m_pTargetPlayer->SetParty(uidParty);
		CHECK(m_pPlayer->HasParty());
		CHECK(m_pPlayer->IsPartyLeader());
		CHECK(m_pTargetPlayer->HasParty());
		CHECK(!m_pTargetPlayer->IsPartyLeader());


		// act
		gsys.pPartySystem->KickReq(uidRequestPlayer, uidTargetPlayer);


		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_KICK_REQ);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 2), uidTargetPlayer);
	}

	TEST_FIXTURE(Fixture, TestKickReq_UnknownPlayerFailure)
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();
		MUID uidTargetPlayer = m_pTargetPlayer->GetUID();
		wstring strRequestPlayerName = L"TestUnknownPlayerName";
		wstring strTargetPlayerName = m_pTargetPlayer->GetName();
		int nRequestPlayerCID = SUnitTestUtil::NewCID();
		int nTargetPlayerCID = SUnitTestUtil::NewCID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidRequestPlayer, strRequestPlayerName, nRequestPlayerCID);
		ASSERT_CHECK(pParty != NULL);
		pParty->AddMember(uidTargetPlayer, strTargetPlayerName, nTargetPlayerCID);
		CHECK_EQUAL(pParty->GetLeader(), uidRequestPlayer);


		// act
		gsys.pPartySystem->KickReq(uidRequestPlayer, uidTargetPlayer);


		// assert
		CHECK_EQUAL(m_pNetAgent->GetSendCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestKickReq_NotLeaderFailure)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidRequestPlayer = m_pPlayer->GetUID();
		MUID uidTargetPlayer = m_pTargetPlayer->GetUID();
		wstring strRequestPlayerName = m_pPlayer->GetName();
		wstring strTargetPlayerName = m_pTargetPlayer->GetName();
		int nReqeustPlayerCID = m_pPlayer->GetCID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidTargetPlayer, strTargetPlayerName, nReqeustPlayerCID);
		ASSERT_CHECK(pParty != NULL);
		pParty->AddMember(uidRequestPlayer, strRequestPlayerName, nReqeustPlayerCID);
		CHECK_EQUAL(pParty->GetLeader(), uidTargetPlayer);

		m_pTargetPlayer->SetParty(uidParty);
		m_pTargetPlayer->SetPartyLeader(true);
		m_pPlayer->SetParty(uidParty);
		CHECK(m_pTargetPlayer->HasParty());
		CHECK(m_pTargetPlayer->IsPartyLeader());
		CHECK(m_pPlayer->HasParty());
		CHECK(!m_pPlayer->IsPartyLeader());


		// act
		gsys.pPartySystem->KickReq(uidRequestPlayer, uidTargetPlayer);


		// assert
		CHECK_EQUAL(m_pNetAgent->GetSendCommandCount(), 0);

		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_INVITE);		
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_PARTY_NOT_PARTY_LEADER);
	}

	TEST_FIXTURE(Fixture, TestKickReq_SelfKickFailure)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidRequestPlayer = m_pPlayer->GetUID();
		wstring strRequestPlayerName = m_pPlayer->GetName();
		int nReqeustPlayerCID = m_pPlayer->GetCID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidRequestPlayer, strRequestPlayerName, nReqeustPlayerCID);
		ASSERT_CHECK(pParty != NULL);

		m_pPlayer->SetParty(uidParty);
		m_pPlayer->SetPartyLeader(true);


		// act
		gsys.pPartySystem->KickReq(uidRequestPlayer, uidRequestPlayer);


		// assert
		CHECK_EQUAL(m_pNetAgent->GetSendCommandCount(), 0);

		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_INVITE);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_PARTY_KICK_SELF);
	}

	TEST_FIXTURE(Fixture, TestKickReq_NoPartyFailure)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidRequestPlayer = m_pPlayer->GetUID();
		MUID uidTargetPlayer = m_pTargetPlayer->GetUID();
		wstring strRequestPlayerName = m_pPlayer->GetName();
		wstring strTargetPlayerName = m_pTargetPlayer->GetName();

		m_pPlayer->SetParty(uidParty);
		m_pPlayer->SetPartyLeader(true);
		m_pTargetPlayer->SetParty(uidParty);
		CHECK(m_pPlayer->HasParty());
		CHECK(m_pPlayer->IsPartyLeader());
		CHECK(m_pTargetPlayer->HasParty());
		CHECK(!m_pTargetPlayer->IsPartyLeader());


		// act
		gsys.pPartySystem->KickReq(uidRequestPlayer, uidTargetPlayer);


		// assert
		CHECK_EQUAL(m_pNetAgent->GetSendCommandCount(), 0);

		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_INVITE);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_PARTY_INVALID_PARTY);
	}

	TEST_FIXTURE(Fixture, TestKickReq_NotMemberTargetFailure)
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidRequestPlayer = m_pPlayer->GetUID();
		wstring strRequestPlayerName = m_pPlayer->GetName();
		int nRequestPlayerCID = m_pPlayer->GetCID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidRequestPlayer, strRequestPlayerName, nRequestPlayerCID);
		ASSERT_CHECK(pParty != NULL);

		m_pPlayer->SetParty(uidParty);
		m_pPlayer->SetPartyLeader(true);
		CHECK(m_pPlayer->HasParty());
		CHECK(m_pPlayer->IsPartyLeader());

		MUID uidMember = SUnitTestUtil::NewUID();


		// act
		gsys.pPartySystem->KickReq(uidRequestPlayer, uidMember);


		// assert
		CHECK_EQUAL(m_pNetAgent->GetSendCommandCount(), 0);

		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_INVITE);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_PARTY_TARGET_NOT_IN_PARTY);
	}

	TEST_FIXTURE(Fixture, TestKickRes)	
	{
		// arrange
		MUID uidRequestPlayer = m_pPlayer->GetUID();
		MUID uidTargetPlayer = m_pTargetPlayer->GetUID();
		CCommandResultTable nResult = CR_SUCCESS;


		// act
		gsys.pPartySystem->KickRes(uidRequestPlayer, uidTargetPlayer, nResult);


		// assert
		CHECK_EQUAL(m_pLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestKickRes_Failure)	
	{
		// arrange
		MUID uidRequestPlayer = m_pPlayer->GetUID();
		MUID uidTargetPlayer = m_pTargetPlayer->GetUID();
		CCommandResultTable nResult = CR_FAIL;


		// act
		gsys.pPartySystem->KickRes(uidRequestPlayer, uidTargetPlayer, nResult);


		// assert
		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_INVITE);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), nResult);
	}

	TEST_FIXTURE(Fixture, TestDoOffline)
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidMember = m_pPlayer->GetUID();
		wstring strMemberName = m_pPlayer->GetName();
		int nMemberCID = m_pPlayer->GetCID();
		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidMember, strMemberName, nMemberCID);
		m_pPlayer->SetParty(uidParty);		
		CHECK_EQUAL(m_pPlayer->IsMoveServer(), false);
	

		// act
		gsys.pPartySystem->DoOffline(m_pPlayer);


		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_DO_OFFLINE);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), uidMember);		
	}

	TEST_FIXTURE(Fixture, TestDoOffline_NotHavePartyFailure)		
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidMember = m_pPlayer->GetUID();


		// act
		gsys.pPartySystem->DoOffline(m_pPlayer);


		// assert
		CHECK_EQUAL(m_pNetAgent->GetSendCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestDoOffline_UnknownPartyFailure)		
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidMember = m_pPlayer->GetUID();
		m_pPlayer->SetParty(uidParty);


		// act
		gsys.pPartySystem->DoOffline(m_pPlayer);


		// assert
		CHECK_EQUAL(m_pNetAgent->GetSendCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestDoOffline_NotMemberFailure)		
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = SUnitTestUtil::NewUID();
		wstring strLeaderName = L"TestLeaderName";
		int nLeaderCID = SUnitTestUtil::NewCID();
		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName,  nLeaderCID);
		
		MUID uidMember = m_pPlayer->GetUID();
		m_pPlayer->SetParty(uidParty);


		// act
		gsys.pPartySystem->DoOffline(m_pPlayer);


		// assert
		CHECK_EQUAL(m_pNetAgent->GetSendCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, AddOfflineMember)		
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		int nLeaderCID = m_pPlayer->GetCID();
		
		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);
		
		MUID uidMember = SUnitTestUtil::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = SUnitTestUtil::NewCID();
		pParty->AddMember(uidMember, strMemberName, nMemberCID);
		
		MUID uidMember2 = SUnitTestUtil::NewUID();
		wstring strMemberName2 = L"TestMember2Name";
		int nMemberCID2 = SUnitTestUtil::NewCID();
		pParty->AddMember(uidMember2, strMemberName2, nMemberCID2);


		// act
		gsys.pPartySystem->AddOfflineMember(uidParty, uidMember);


		// assert		
		CHECK_EQUAL(pParty->GetMemberCount(), 3);
		CHECK_EQUAL(pParty->GetOfflineMemberCount(), 1);
		CHECK_EQUAL(pParty->GetOnlineMemberCount(), 2);
	}

	TEST_FIXTURE(Fixture, AddOfflineMember_NoMemberInServer)		
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = SUnitTestUtil::NewUID();
		wstring strLeaderName = L"TestLeaderName";
		int nLeaderCID = SUnitTestUtil::NewCID();
		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);
		
		MUID uidMember = SUnitTestUtil::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = SUnitTestUtil::NewCID();
		pParty->AddMember(uidMember, strMemberName, nMemberCID);


		// act
		gsys.pPartySystem->AddOfflineMember(uidParty, uidMember);


		// assert		
		pParty = gsys.pPartySystem->FindParty(uidParty);
		CHECK(pParty == NULL);
	}

	TEST_FIXTURE(Fixture, TestDoOnline)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidOffline = SUnitTestUtil::NewUID();
		wstring strMemberName = m_pPlayer->GetName();
		int nMemberCID = m_pPlayer->GetCID();
		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidOffline, strMemberName, nMemberCID);
		ASSERT_CHECK(pParty != NULL);

		MUID uidMember = m_pPlayer->GetUID();
		m_pPlayer->SetParty(uidParty);


		// act
		gsys.pPartySystem->DoOnline(uidMember);


		// assert
		CHECK(!m_pPlayer->HasParty());
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_DO_ONLINE);
	}

	TEST_FIXTURE(Fixture, TestDoOnline_UnknownPlayerFailure)		
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidOffline = SUnitTestUtil::NewUID();
		wstring strMemberName = m_pPlayer->GetName();
		int nMemberCID = m_pPlayer->GetCID();
		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidOffline, strMemberName, nMemberCID);
		ASSERT_CHECK(pParty != NULL);

		MUID uidMember = SUnitTestUtil::NewUID();
		m_pPlayer->SetParty(uidParty);


		// act
		gsys.pPartySystem->DoOnline(uidMember);


		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestDoOnline_NoHavePartyFailure)		
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidOffline = SUnitTestUtil::NewUID();
		wstring strMemberName = m_pPlayer->GetName();
		int nMemberCID = m_pPlayer->GetCID();
		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidOffline, strMemberName, nMemberCID);
		ASSERT_CHECK(pParty != NULL);

		MUID uidMember = m_pPlayer->GetUID();


		// act
		gsys.pPartySystem->DoOnline(uidMember);


		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestDoOnline_RemovePartyFailure)		
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidOffline = SUnitTestUtil::NewUID();
		wstring strMemberName = m_pPlayer->GetName();
		int nMemberCID = m_pPlayer->GetCID();
		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidOffline, strMemberName, nMemberCID);
		ASSERT_CHECK(pParty != NULL);

		MUID uidMember = m_pPlayer->GetUID();
		m_pPlayer->SetParty(uidParty);
		
		gsys.pMasterServerFacade->RemoveProxyParty(uidParty);


		// act
		gsys.pPartySystem->DoOnline(uidMember);


		// assert
		CHECK(!m_pPlayer->HasParty());
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestDoOnline_NotMemberFailure)
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidOffline = SUnitTestUtil::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = SUnitTestUtil::NewCID();
		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidOffline, strMemberName, nMemberCID);
		ASSERT_CHECK(pParty != NULL);

		MUID uidMember = m_pPlayer->GetUID();
		m_pPlayer->SetParty(uidParty);
		
		gsys.pMasterServerFacade->RemoveProxyParty(uidParty);


		// act
		gsys.pPartySystem->DoOnline(uidMember);


		// assert
		CHECK(!m_pPlayer->HasParty());
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestRemoveOfflineMember)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		int nLeaderCID = m_pPlayer->GetCID();
		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);

		MUID uidMember = SUnitTestUtil::NewUID();
		wstring strMemberName = L"TestMemberName";
		MUID uidOffline = SUnitTestUtil::NewUID();
		int nOffileCID = SUnitTestUtil::NewCID();

		pParty->AddMember(uidOffline, strMemberName, nOffileCID);
		pParty->AddOfflineMember(uidOffline);


		// act
		gsys.pPartySystem->RemoveOfflineMember(uidParty, uidMember, uidOffline);


		// assert
		CHECK_EQUAL(pParty->GetOnlineMemberCount(), 2);
		CHECK(pParty->IsExistMember(uidMember));
		CHECK(!pParty->IsExistMember(uidOffline));

		CHECK_EQUAL(m_pNetAgent->GetSendCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestRemoveOfflineMember_MemberServer)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = SUnitTestUtil::NewUID();
		wstring strLeaderName = L"TestLeaderName";
		int nLeaderCID = SUnitTestUtil::NewCID();
		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);

		MUID uidMember = m_pPlayer->GetUID();
		wstring strMemberName = m_pPlayer->GetName();
		MUID uidOffline = SUnitTestUtil::NewUID();
		int nOfflineCID = SUnitTestUtil::NewCID();

		pParty->AddMember(uidOffline, strMemberName, nOfflineCID);
		pParty->AddOfflineMember(uidOffline);


		// act
		gsys.pPartySystem->RemoveOfflineMember(uidParty, uidMember, uidOffline);


		// assert
		CHECK_EQUAL(pParty->GetOnlineMemberCount(), 2);
		CHECK(pParty->IsExistMember(uidMember));
		CHECK(!pParty->IsExistMember(uidOffline));

		CHECK_EQUAL(m_pNetAgent->GetSendCommandCount(), 3);
	}

	TEST_FIXTURE(Fixture, TestJoinInviteReq)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidRequestPlayer = m_pPlayer->GetUID();

		MUID uidLeader = SUnitTestUtil::NewUID();
		wstring strLeaderName = L"TestLeaderName";
		int nLeaderCID = SUnitTestUtil::NewCID();
		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);


		// act
		gsys.pPartySystem->JoinInviteReq(uidParty, uidRequestPlayer);


		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_JOIN_INVITE_REQ);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), uidRequestPlayer);
	}

	TEST_FIXTURE(Fixture, TestJoinInviteReq_UnknownRequestPlayerFailure)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();

		MUID uidLeader = SUnitTestUtil::NewUID();
		wstring strLeaderName = L"TestLeaderName";
		int nLeaderCID = SUnitTestUtil::NewCID();
		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);


		// act
		gsys.pPartySystem->JoinInviteReq(uidParty, uidRequestPlayer);


		// assert
		ASSERT_EQUAL(m_pLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestJoinInviteReq_BusyFailure)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidRequestPlayer = m_pPlayer->GetUID();

		MUID uidLeader = SUnitTestUtil::NewUID();
		wstring strLeaderName = L"TestLeaderName";
		int nLeaderCID = SUnitTestUtil::NewCID();
		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);

		m_pPlayer->GetDoing().ChangeDoing(CD_INTERACTING);


		// act
		gsys.pPartySystem->JoinInviteReq(uidParty, uidRequestPlayer);


		// assert
		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_INVITE);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_PARTY_PLAYER_IS_BUSY);
	}

	TEST_FIXTURE(Fixture, TestJoinInviteReq_DuelFailure)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidRequestPlayer = m_pPlayer->GetUID();

		MUID uidLeader = SUnitTestUtil::NewUID();
		wstring strLeaderName = L"TestLeaderName";
		int nLeaderCID = SUnitTestUtil::NewCID();
		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);
		
		MockDuel* pDuel = GUTHelper_Duel::NewMockDuel_Fighting(m_pPlayer, m_pTargetPlayer);
		m_pPlayer->SetDuel(pDuel);


		// act
		gsys.pPartySystem->JoinInviteReq(uidParty, uidRequestPlayer);


		// assert
		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_INVITE);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_PARTY_REQUESTER_IS_DUEL);
	}

	TEST_FIXTURE(Fixture, TestJoinInviteReq_PvPAreaFailure)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidRequestPlayer = m_pPlayer->GetUID();

		MUID uidLeader = SUnitTestUtil::NewUID();
		wstring strLeaderName = L"TestLeaderName";
		int nLeaderCID = SUnitTestUtil::NewCID();
		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);

		PVP_AREA_INFO infoPVP;
		infoPVP.nFaction1 = 100;
		infoPVP.nFaction2 = 200;
		infoPVP.rcArea = MRect(-500, -500, 500, 500);
		GFieldInfo* pFieldinfo = const_cast<GFieldInfo*>(m_pField->GetInfo());
		pFieldinfo->Init(L"", L"");
		pFieldinfo->m_PvPAreas.push_back(infoPVP);
		pFieldinfo->Cooking();

		struct PvPAreaCleanup
		{
			GField* m_pField;
			PvPAreaCleanup(GField* pField)
			{
				m_pField = pField;
			}

			~PvPAreaCleanup()
			{
				m_pField->GetInfo()->GetAttrGrid()->Fini();
			}

		} localCleanup(m_pField);


		// act
		gsys.pPartySystem->JoinInviteReq(uidParty, uidRequestPlayer);


		// assert
		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_INVITE);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_PARTY_REQUESTER_IN_PVPAREA);
	}

	TEST_FIXTURE(Fixture, TestJoinInviteReq_BattleArenaFailure)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidRequestPlayer = m_pPlayer->GetUID();

		MUID uidLeader = SUnitTestUtil::NewUID();
		wstring strLeaderName = L"TestLeaderName";
		int nLeaderCID = SUnitTestUtil::NewCID();
		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);

		MUID uidBattleArena = SUnitTestUtil::NewUID();
		m_pPlayer->GetPlayerBattleArena().SetBattleArenaUID(uidBattleArena);


		// act
		gsys.pPartySystem->JoinInviteReq(uidParty, uidRequestPlayer);


		// assert
		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_INVITE);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_PARTY_REQUESTER_IN_BATTLEARENA);
	}
	TEST_FIXTURE(Fixture, TestJoinInviteReq_UnknownPartyFailure)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidRequestPlayer = m_pPlayer->GetUID();


		// act
		gsys.pPartySystem->JoinInviteReq(uidParty, uidRequestPlayer);


		// assert
		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_INVITE);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_PARTY_INVALID_PARTY);
	}

	TEST_FIXTURE(Fixture, TestJoinInviteReq_FullPartyFailure)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidRequestPlayer = m_pPlayer->GetUID();

		MUID uidLeader = SUnitTestUtil::NewUID();
		wstring strLeaderName = L"TestLeaderName";
		int nLeaderCID = SUnitTestUtil::NewCID();
		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);

		for(int i = 1; i < MAX_PARTY_MEMBER_COUNT; i++)
		{
			MUID uidMember = SUnitTestUtil::NewUID();
			int nMemberCID = SUnitTestUtil::NewCID();
			gsys.pMasterServerFacade->AddProxyPartyMember(uidParty, uidMember, L"TestMemberName", nMemberCID);
		}


		// act
		gsys.pPartySystem->JoinInviteReq(uidParty, uidRequestPlayer);


		// assert
		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_INVITE);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_PARTY_FULL_PARTY);
	}

	TEST_FIXTURE(Fixture, TestJoinInviteRes_Fail)	
	{
		// arrange
		MUID uidRequestPlayer = m_pPlayer->GetUID();
		CCommandResultTable nResult = CR_FAIL;


		// act
		gsys.pPartySystem->JoinInviteRes(uidRequestPlayer, nResult);


		// assert
		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_INVITE);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), nResult);
	}

	TEST_FIXTURE(Fixture, TestJoinInviteRes_Success)	
	{
		// arrange
		MUID uidRequestPlayer = m_pPlayer->GetUID();
		CCommandResultTable nResult = CR_SUCCESS;


		// act
		gsys.pPartySystem->JoinInviteRes(uidRequestPlayer, nResult);


		// assert
		ASSERT_EQUAL(m_pLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestJoinInviteRes_UnknownPlayer)
	{
		// arrange
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();
		CCommandResultTable nResult = CR_FAIL;


		// act
		gsys.pPartySystem->JoinInviteRes(uidRequestPlayer, nResult);


		// assert
		ASSERT_EQUAL(m_pLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptReq)
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		int nLeaderCID = m_pPlayer->GetCID();
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();
		wstring strRequestPlayerName = L"TestRequestPlayerName";
		

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);


		// act
		gsys.pPartySystem->JoinAcceptReq(uidParty, uidLeader, uidRequestPlayer, strRequestPlayerName);


		// assert
		CHECK(!m_pPlayer->GetDoing().IsBusy());
		CHECK_EQUAL(m_pPlayer->GetPartyInviteeUID(), uidRequestPlayer);

		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_INVITE_FOR_ME_QUESTION);
		CHECK_EQUAL(m_pLink->GetParam<MUID>(0, 0), uidRequestPlayer);
		CHECK(m_pLink->GetParam<const wchar_t*>(0, 1) == strRequestPlayerName);
		CHECK_EQUAL(m_pLink->GetParam<bool>(0, 2), m_pPlayer->IsNowCombat());
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptReq_UnknownPartyFailure)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();
		wstring strRequestPlayerName = L"TestRequestPlayerName";


		// act
		gsys.pPartySystem->JoinAcceptReq(uidParty, uidLeader, uidRequestPlayer, strRequestPlayerName);


		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_JOIN_ACCEPT_RES);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), uidLeader);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 2), uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 3), CR_FAIL_PARTY_INVALID_PARTY);
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptReq_NotLeaderFailure)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();
		wstring strRequestPlayerName = L"TestRequestPlayerName";
		int nRequestPlayerCID = SUnitTestUtil::NewCID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidRequestPlayer, strRequestPlayerName, nRequestPlayerCID);
		ASSERT_CHECK(pParty != NULL);


		// act
		gsys.pPartySystem->JoinAcceptReq(uidParty, uidLeader, uidRequestPlayer, strRequestPlayerName);


		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_JOIN_ACCEPT_RES);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), uidLeader);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 2), uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 3), CR_FAIL_PARTY_INVALID_PARTY);
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptReq_UnknownLeaderFailure)
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = SUnitTestUtil::NewUID();
		wstring strLeaderName = L"TestLeaderName";
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();
		wstring strRequestPlayerName = L"TestRequestPlayerName";
		int nRequestPlayerCID = SUnitTestUtil::NewCID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName,  nRequestPlayerCID);
		ASSERT_CHECK(pParty != NULL);


		// act
		gsys.pPartySystem->JoinAcceptReq(uidParty, uidLeader, uidRequestPlayer, strRequestPlayerName);


		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_JOIN_ACCEPT_RES);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), uidLeader);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 2), uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 3), CR_FAIL_PARTY_INVALID_PARTY);
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptReq_NotEndableBusyFailure)	
	{
		// arrange
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();
		wstring strRequestPlayerName = L"TestRequestPlayerName";
		int nLeaderCID = m_pPlayer->GetCID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);

		m_pPlayer->GetDoing().ChangeDoing(CD_CUTSCENING);

		// act
		gsys.pPartySystem->JoinAcceptReq(uidParty, uidLeader, uidRequestPlayer, strRequestPlayerName);


		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_JOIN_ACCEPT_RES);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), uidLeader);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 2), uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 3), CR_FAIL_PARTY_PLAYER_IS_BUSY);
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptRes)	
	{
		// arrange		
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		PARTY_RESPOND nRespond = PRS_OK;
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();
		int nLeaderCID = m_pPlayer->GetCID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);

		m_pPlayer->SetPartyInviteeUID(uidRequestPlayer);
		m_pPlayer->SetParty(uidParty);


		// act
		gsys.pPartySystem->JoinAcceptRes(uidLeader, nRespond);


		// assert
		CHECK_EQUAL(m_pPlayer->GetPartyInviteeUID(), MUID::Invalid());
		CHECK(!m_pPlayer->GetDoing().IsBusy());

		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_JOIN_ACCEPT_RES);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), uidLeader);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 2), uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 3), CR_SUCCESS);
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptRes_UnknownLeaderFailure)	
	{
		// arrange		
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = SUnitTestUtil::NewUID();		
		wstring strLeaderName = L"TestLeaderName";
		int nLeaderCID = SUnitTestUtil::NewCID();
		PARTY_RESPOND nRespond = PRS_OK;
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);


		// act
		gsys.pPartySystem->JoinAcceptRes(uidLeader, nRespond);


		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandCount(), 0);
		ASSERT_EQUAL(m_pLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptRes_UnknownRequestPlayerFailure)
	{
		// arrange		
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		int nLeaderCID = m_pPlayer->GetCID();
		PARTY_RESPOND nRespond = PRS_OK;
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);

		m_pPlayer->SetPartyInviteeUID(MUID::Invalid());
		m_pPlayer->SetParty(uidParty);


		// act
		gsys.pPartySystem->JoinAcceptRes(uidLeader, nRespond);


		// assert
		CHECK_EQUAL(m_pPlayer->GetPartyInviteeUID(), MUID::Invalid());
		CHECK(!m_pPlayer->GetDoing().IsBusy());

		ASSERT_EQUAL(m_pNetAgent->GetSendCommandCount(), 0);		

		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_SYSTEM_INVALID_PC_UID);
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptRes_UnknownPartyFailure)	
	{
		// arrange		
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		PARTY_RESPOND nRespond = PRS_OK;
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();

		m_pPlayer->SetPartyInviteeUID(uidRequestPlayer);
		m_pPlayer->SetParty(uidParty);


		// act
		gsys.pPartySystem->JoinAcceptRes(uidLeader, nRespond);


		// assert
		CHECK_EQUAL(m_pPlayer->GetPartyInviteeUID(), MUID::Invalid());
		CHECK(!m_pPlayer->GetDoing().IsBusy());

		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_JOIN_ACCEPT_RES);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), uidLeader);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 2), uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 3), CR_FAIL_PARTY_INVALID_PARTY);

		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_PARTY_INVALID_PARTY);
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptRes_FullPartyFailure)	
	{
		// arrange		
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		int nLeaderCID = m_pPlayer->GetCID();
		PARTY_RESPOND nRespond = PRS_OK;
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName,  nLeaderCID);
		ASSERT_CHECK(pParty != NULL);

		m_pPlayer->SetPartyInviteeUID(uidRequestPlayer);
		m_pPlayer->SetParty(uidParty);

		for(int i = 1; i < MAX_PARTY_MEMBER_COUNT; i++)
		{
			MUID uidMember = SUnitTestUtil::NewUID();
			int nMemberCID = SUnitTestUtil::NewCID();
			pParty->AddMember(uidMember, L"TestMemberName", nMemberCID);
		}


		// act
		gsys.pPartySystem->JoinAcceptRes(uidLeader, nRespond);


		// assert
		CHECK_EQUAL(m_pPlayer->GetPartyInviteeUID(), MUID::Invalid());
		CHECK(!m_pPlayer->GetDoing().IsBusy());

		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_JOIN_ACCEPT_RES);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), uidLeader);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 2), uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 3), CR_FAIL_PARTY_FULL_PARTY);

		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_PARTY_FULL_PARTY);
	}
	
	TEST_FIXTURE(Fixture, TestJoinAcceptRes_NotEndableBusyFailure_RebirthingSoulbiding)	
	{
		// arrange		
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		int nLeaderCID = m_pPlayer->GetCID();
		PARTY_RESPOND nRespond = PRS_OK;
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);

		m_pPlayer->SetPartyInviteeUID(uidRequestPlayer);
		m_pPlayer->SetParty(uidParty);		
		m_pPlayer->GetDoing().ChangeDoing(CD_CUTSCENING);


		// act
		gsys.pPartySystem->JoinAcceptRes(uidLeader, nRespond);


		// assert
		CHECK_EQUAL(m_pPlayer->GetPartyInviteeUID(), MUID::Invalid());
		CHECK(m_pPlayer->GetDoing().IsBusy());

		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_JOIN_ACCEPT_RES);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), uidLeader);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 2), uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 3), CR_FAIL_PARTY_RESPOND_PLAYER_IS_BUSY);

		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_PARTY_RESPOND_PLAYER_IS_BUSY);
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptRes_NotEndableBusyFailure_CutScene)	
	{
		// arrange		
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		int nLeaderCID = m_pPlayer->GetCID();
		PARTY_RESPOND nRespond = PRS_OK;
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);

		m_pPlayer->SetPartyInviteeUID(uidRequestPlayer);
		m_pPlayer->SetParty(uidParty);		
		m_pPlayer->GetDoing().ChangeDoing(CD_CUTSCENING);


		// act
		gsys.pPartySystem->JoinAcceptRes(uidLeader, nRespond);


		// assert
		CHECK_EQUAL(m_pPlayer->GetPartyInviteeUID(), MUID::Invalid());
		CHECK(m_pPlayer->GetDoing().IsBusy());

		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_JOIN_ACCEPT_RES);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), uidLeader);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 2), uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 3), CR_FAIL_PARTY_RESPOND_PLAYER_IS_BUSY);

		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_PARTY_RESPOND_PLAYER_IS_BUSY);
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptRes_DuelFailure)	
	{
		// arrange		
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		int nLeaderCID = m_pPlayer->GetCID();
		PARTY_RESPOND nRespond = PRS_OK;
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);

		m_pPlayer->SetPartyInviteeUID(uidRequestPlayer);
		m_pPlayer->SetParty(uidParty);

		MockDuel* pDuel = GUTHelper_Duel::NewMockDuel_Fighting(m_pPlayer, m_pTargetPlayer);
		m_pPlayer->SetDuel(pDuel);
		
		
		// act
		gsys.pPartySystem->JoinAcceptRes(uidLeader, nRespond);


		// assert
		CHECK_EQUAL(m_pPlayer->GetPartyInviteeUID(), MUID::Invalid());
		CHECK(!m_pPlayer->GetDoing().IsBusy());

		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_JOIN_ACCEPT_RES);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), uidLeader);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 2), uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 3), CR_FAIL_PARTY_RESPONDER_IS_DUEL);

		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_PARTY_RESPONDER_IS_DUEL);
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptRes_PvPAreaFailure)	
	{
		// arrange		
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		int nLeaderCID = m_pPlayer->GetCID();
		PARTY_RESPOND nRespond = PRS_OK;
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);

		m_pPlayer->SetPartyInviteeUID(uidRequestPlayer);
		m_pPlayer->SetParty(uidParty);

		
		PVP_AREA_INFO infoPVP;
		infoPVP.nFaction1 = 100;
		infoPVP.nFaction2 = 200;
		infoPVP.rcArea = MRect(-500, -500, 500, 500);
		GFieldInfo* pFieldinfo = const_cast<GFieldInfo*>(m_pField->GetInfo());
		pFieldinfo->Init(L"", L"");
		pFieldinfo->m_PvPAreas.push_back(infoPVP);
		pFieldinfo->Cooking();

		struct PvPAreaCleanup
		{
			GField* m_pField;
			PvPAreaCleanup(GField* pField)
			{
				m_pField = pField;
			}

			~PvPAreaCleanup()
			{
				m_pField->GetInfo()->GetAttrGrid()->Fini();
			}
		} localCleanup(m_pField);


		// act
		gsys.pPartySystem->JoinAcceptRes(uidLeader, nRespond);


		// assert
		CHECK_EQUAL(m_pPlayer->GetPartyInviteeUID(), MUID::Invalid());
		CHECK(!m_pPlayer->GetDoing().IsBusy());

		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_JOIN_ACCEPT_RES);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), uidLeader);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 2), uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 3), CR_FAIL_PARTY_RESPONDER_IN_PVPAREA);

		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_PARTY_RESPONDER_IN_PVPAREA);
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptRes_BattleArenaFailure)	
	{
		// arrange		
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		int nLeaderCID = m_pPlayer->GetCID();
		PARTY_RESPOND nRespond = PRS_OK;
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);

		m_pPlayer->SetPartyInviteeUID(uidRequestPlayer);
		m_pPlayer->SetParty(uidParty);

		MUID uidBattleArena = SUnitTestUtil::NewUID();
		m_pPlayer->GetPlayerBattleArena().SetBattleArenaUID(uidBattleArena);
		

		// act
		gsys.pPartySystem->JoinAcceptRes(uidLeader, nRespond);


		// assert
		CHECK_EQUAL(m_pPlayer->GetPartyInviteeUID(), MUID::Invalid());
		CHECK(!m_pPlayer->GetDoing().IsBusy());

		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_JOIN_ACCEPT_RES);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), uidLeader);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 2), uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 3), CR_FAIL_PARTY_RESPONDER_IN_BATTLEARENA);

		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), CR_FAIL_PARTY_RESPONDER_IN_BATTLEARENA);
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptRes_Refuse)	
	{
		// arrange		
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidLeader = m_pPlayer->GetUID();
		wstring strLeaderName = m_pPlayer->GetName();
		int nLeaderCID = m_pPlayer->GetCID();
		PARTY_RESPOND nRespond = PRS_REFUSE;
		MUID uidRequestPlayer = SUnitTestUtil::NewUID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidLeader, strLeaderName, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);

		m_pPlayer->SetPartyInviteeUID(uidRequestPlayer);
		m_pPlayer->SetParty(uidParty);


		// act
		gsys.pPartySystem->JoinAcceptRes(uidLeader, nRespond);


		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_JOIN_ACCEPT_RES);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), uidLeader);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 2), uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 3), CR_FAIL_PARTY_REFUSE_INVITATION_FOR_ME);
	}
	
	TEST_FIXTURE(Fixture, TestJoinAcceptCancel)
	{
		// arrange
		MUID uidLeader = m_pPlayer->GetUID();
		CCommandResultTable nResult = CR_FAIL;

		
		// act
		gsys.pPartySystem->JoinAcceptCancel(uidLeader, nResult);


		// assert
		ASSERT_EQUAL(m_pLink->GetCommandID(0), MC_PARTY_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pLink->GetParam<int>(0, 0), nResult);
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptCancel_UnkownLeader)	
	{
		// arrange
		MUID uidLeader = SUnitTestUtil::NewUID();
		CCommandResultTable nResult = CR_FAIL;

		
		// act
		gsys.pPartySystem->JoinAcceptCancel(uidLeader, nResult);


		// assert
		CHECK_EQUAL(m_pLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestMoveServerSync)	
	{
		// arrange
		GDynamicFieldMaster* pDynamicFieldMaster = gmgr.pFieldMgr->GetDynamicFieldMaster(m_uidDynamicFieldGroup);
		ASSERT_CHECK(pDynamicFieldMaster != NULL);
		MockDynamicFieldPolicy* pMockPolicy = new MockDynamicFieldPolicy(pDynamicFieldMaster);
		pDynamicFieldMaster->SetPolicy(pMockPolicy);

		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidOffline = SUnitTestUtil::NewUID();
		wstring strLeaderName = L"TestMemberName";
		int nLeaderCID = SUnitTestUtil::NewCID();
		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidOffline, strLeaderName, nLeaderCID);
		pParty->CreateField(m_nDynamicFieldID, m_uidDynamicFieldGroup);

		MUID uidMember = m_pPlayer->GetUID();		


		// act
		gsys.pPartySystem->MoveServerSync(uidParty, uidMember, uidOffline);


		// assert
		CHECK(!pParty->IsExistMember(uidOffline));
		CHECK(pParty->IsExistMember(uidMember));		
		CHECK_EQUAL(pParty->GetLeader(), uidMember);
		CHECK_EQUAL(m_pPlayer->GetPartyUID(), uidParty);
		CHECK(m_pPlayer->IsPartyLeader());
		CHECK_EQUAL(pMockPolicy->m_uidOldPlayer, uidOffline);
		CHECK_EQUAL(pMockPolicy->m_uidNewPlayer, uidMember);
	}

	TEST_FIXTURE(Fixture, TestFixedPartyLogOn_NewLogIn)
	{
		MUID uidMember = m_pPlayer->GetUID();
		wstring strName = m_pPlayer->GetName();

		MUID uidParty = MUID::Invalid();
		MUID uidOffline = MUID::Invalid();


		m_pPartySystemStub->FixedPartyLogOn(uidMember, strName);


		ASSERT_EQUAL(MMC_PARTY_FIXED_LOG_ON, m_pNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(uidParty, m_pNetAgent->GetSendParam<MUID>(0, 0));
		CHECK_EQUAL(uidMember, m_pNetAgent->GetSendParam<MUID>(0, 1));
		CHECK_EQUAL(uidOffline, m_pNetAgent->GetSendParam<MUID>(0, 2));
	}		

	TEST_FIXTURE(Fixture, TestFixedPartyLogOn_Online)
	{
		MUID uidParty = SUnitTestUtil::NewUID();
		MUID uidOffline = SUnitTestUtil::NewUID();
		wstring strName = m_pPlayer->GetName();
		CID nCID = m_pPlayer->GetCID();

		GParty* pParty = m_pPartySystemStub->NewParty(uidParty, uidOffline, strName, nCID);
		m_pPlayer->SetParty(uidParty);


		MUID uidMember = m_pPlayer->GetUID();
		m_pPartySystemStub->FixedPartyLogOn(uidMember, strName);


		ASSERT_EQUAL(MMC_PARTY_FIXED_LOG_ON, m_pNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(uidParty, m_pNetAgent->GetSendParam<MUID>(0, 0));
		CHECK_EQUAL(uidMember, m_pNetAgent->GetSendParam<MUID>(0, 1));
		CHECK_EQUAL(uidOffline, m_pNetAgent->GetSendParam<MUID>(0, 2));
	}	
}
