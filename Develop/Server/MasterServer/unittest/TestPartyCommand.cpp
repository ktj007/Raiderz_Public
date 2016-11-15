#include "stdafx.h"
#include "ZFixtureHelper.h"
#include "ZParty.h"
#include "ZPartyManager.h"
#include "ZPartyLogic.h"
#include "STransData_M2G.h"
#include "CCommandTable_Master.h"
#include "ZPartyCommandRouter.h"
#include "CTransData.h"
#include "ZPartyMember.h"
#include "ZGameServerObject.h"

SUITE(PartyCommand)
{
	struct MockPartyLogic: public ZPartyLogic
	{
		MUID m_uidSender;
		MUID m_uidTargetPlayer;
		MUID m_uidRequestPlayer;		
		MUID m_uidParty;
		MUID m_uidNewLeader;
		MUID m_uidLeaveMember;
		MUID m_uidFieldGroup;
		MUID m_uidMember;
		MUID m_uidField;
		MUID m_uidOffline;

		CCommandResultTable m_nResult;		
		vector<int> m_vecQuestID;
		wstring m_strMessage;
		int m_nFieldGroupID;
		TD_PARTY_MEMBER m_tdPartyMember;
		vector<int> m_vecBuff;

		PARTY_SETTING m_partySetting;

		virtual void InviteReq(MUID uidSender, MUID uidTargetPlayer, MUID uidRequestPlayer) override	
		{
			m_uidSender = uidSender;
			m_uidTargetPlayer = uidTargetPlayer;
			m_uidRequestPlayer = uidRequestPlayer;
		}

		virtual void AcceptRes(MUID uidSender, MUID uidRequestPlayer, MUID uidTargetPlayer, CCommandResultTable nResult) override
		{
			m_uidSender = uidSender;
			m_uidRequestPlayer = uidRequestPlayer;
			m_uidTargetPlayer = uidTargetPlayer;
			m_nResult = nResult;
		}

		virtual void LeaveReq(MUID uidSender, MUID uidParty, MUID uidLeaveMember) override	
		{
			m_uidSender = uidSender;
			m_uidParty = uidParty;
			m_uidLeaveMember = uidLeaveMember;
		}

		virtual void KickReq(MUID uidSender, MUID uidParty, MUID uidRequestPlayer, MUID uidTargetPlayer) override	
		{
			m_uidSender = uidSender;
			m_uidParty = uidParty;
			m_uidRequestPlayer = uidRequestPlayer;
			m_uidTargetPlayer = uidTargetPlayer;
		}

		virtual void UpdateParty(MUID uidParty, const ZPartyMember* pMember) override	
		{
			m_uidParty = uidParty;
			pMember->Export(&m_tdPartyMember, &m_vecBuff);
		}

		virtual void UpdateMember(MUID uidParty, const ZPartyMember* pMember) override	
		{
			m_uidParty = uidParty;
			pMember->Export(&m_tdPartyMember, &m_vecBuff);
		}

		virtual void AddQuest(MUID uidParty, MUID uidMember, const vector<int>& vecQuestID) override	
		{
			m_uidParty = uidParty;
			m_uidMember = uidMember;
			m_vecQuestID = vecQuestID;
		}

		virtual void AddField(MUID uidParty, MUID uidMember, MUID uidFieldGroup) override	
		{
			m_uidParty = uidParty;
			m_uidMember = uidMember;
			m_uidFieldGroup = uidFieldGroup;
		}

		virtual void UpdateQuestRunner(MUID uidParty, MUID uidMember, MUID uidField, const vector<int>& vecQuestID) override	
		{
			m_uidParty = uidParty;
			m_uidMember = uidMember;
			m_uidField = uidField;
			m_vecQuestID = vecQuestID;
		}

		virtual void RemoveQuest(MUID uidParty, MUID uidMember, const vector<int>& vecQuestID) override	
		{
			m_uidParty = uidParty;
			m_uidMember = uidMember;
			m_vecQuestID = vecQuestID;
		}

		virtual void RemoveField(MUID uidParty, MUID uidMember, int nFieldGroupID) override	
		{
			m_uidParty = uidParty;
			m_uidMember = uidMember;
			m_nFieldGroupID = nFieldGroupID;
		}

		virtual void EraseQuestRunner(MUID uidParty, MUID uidMember) override	
		{
			m_uidParty = uidParty;
			m_uidMember = uidMember;
		}

		virtual void DoOffline(MUID uidParty, MUID uidMember) override	
		{
			m_uidParty = uidParty;
			m_uidMember = uidMember;
		}

		virtual bool DoOnline(MUID uidParty, MUID uidMember, MUID uidOffline) override	
		{
			m_uidParty = uidParty;
			m_uidMember = uidMember;
			m_uidOffline = uidOffline;

			return true;
		}
	
		virtual void JoinInviteReq(MUID uidSender, MUID uidParty, MUID uidRequestPlayer) override
		{
			m_uidParty = uidParty;
			m_uidRequestPlayer = uidRequestPlayer;
		}

		virtual void JoinAcceptRes(MUID uidSender, MUID uidParty, MUID uidLeader, MUID uidRequestPlayer, CCommandResultTable nResult) override
		{
			m_uidParty = uidParty;
			m_partySetting.m_uidLeader = uidLeader;
			m_uidRequestPlayer = uidRequestPlayer;
			m_nResult = nResult;
		}

		virtual void MoveServer(MUID uidParty, MUID uidMember) override
		{
			m_uidParty = uidParty;
			m_uidMember = uidMember;
		}
	};

	struct Fixture: public FBaseServerField, public FBaseMockLink, FBasePlayer
	{
		ZTestSysWrapper2<ZPartyLogic, MockPartyLogic>	m_PartyLogicWrapper;
		MockPartyLogic*	m_pPartyLogic;

		MUID m_uidRequest;
		MUID m_uidRequestPlayer;
		int	 m_nRequestPlayerCID;
		wstring m_strRequestPlayerName;
		MockLink* m_pRequestLink;
		ZPlayer* m_pRequestPlayer;
		
		MUID m_uidTarget;
		MUID m_uidTargetPlayer;
		wstring m_strTargetPlayerName;
		MockLink* m_pTargetLink;
		ZPlayer* m_pTargetPlayer;

		ZPartyCommandRouter* m_pPartyRouter;

		int m_nRequestServerID;
		int m_nTargetServerID;

		Fixture()
		{
			m_nRequestServerID = 2;
			m_nTargetServerID = 3;

			m_pPartyLogic = m_PartyLogicWrapper.Get();

			m_uidRequest = AddGameServerObject(m_nRequestServerID, 3)->GetUID();
			m_pRequestLink = NewLink(m_uidRequest);

			m_uidTarget = AddGameServerObject(m_nTargetServerID, 107)->GetUID();			
			m_pTargetLink = NewLink(m_uidTarget);


			m_pRequestPlayer = AddNewPlayerInWorld(m_nRequestServerID);
			m_uidRequestPlayer = m_pRequestPlayer->GetUID();
			m_nRequestPlayerCID = m_pRequestPlayer->GetCID();
			m_strRequestPlayerName = m_pRequestPlayer->GetPlayerName();

			m_pTargetPlayer = AddNewPlayerInWorld(m_nTargetServerID);
			m_uidTargetPlayer = m_pTargetPlayer->GetUID();
			m_strTargetPlayerName = m_pTargetPlayer->GetPlayerName();

			m_pPartyRouter = new ZPartyCommandRouter();
		}

		~Fixture()
		{
			delete m_pPartyRouter;
		}

		MCommandResult RecvInviteReq(MockLink* pLink, MUID uidTargetPlayer, MUID uidRequestPlayer)
		{
			return pLink->OnRecv(MMC_PARTY_INVITE_REQ,
									2,
									NEW_UID(uidTargetPlayer),
									NEW_UID(uidRequestPlayer)
									);
		}

		MCommandResult RecvAcceptRes(MockLink* pLink, MUID uidRequestPlayer, MUID uidTargetPlayer, CCommandResultTable nResult)
		{
			return pLink->OnRecv(MMC_PARTY_ACCEPT_RES,
									3,
									NEW_UID(uidRequestPlayer),
									NEW_UID(uidTargetPlayer),
									NEW_INT(nResult)
									);
		}

		MCommandResult RecvPartyUpdate(MockLink* pLink, MUID uidParty, TD_PARTY_MEMBER& tdPartyMember, vector<int>& vecBuff)
		{
			return pLink->OnRecv(MMC_PARTY_UPDATE,
									3,
									NEW_UID(uidParty),
									NEW_BLOB(&tdPartyMember, sizeof(tdPartyMember), 1),
									NEW_BLOB(vecBuff)
									);

		}

		MCommandResult RecvPartyUpdateMember(MockLink* pLink, MUID uidParty, TD_PARTY_MEMBER& tdPartyMember, vector<int>& vecBuff)
		{
			return pLink->OnRecv(MMC_PARTY_UPDATE_MEMBER,
									3,
									NEW_UID(uidParty),
									NEW_BLOB(&tdPartyMember, sizeof(tdPartyMember), 1),
									NEW_BLOB(vecBuff)
									);
		}

		MCommandResult RecvPartyAddQuest(MockLink* pLink, MUID uidParty, MUID uidMember, vector<int>& vecQuestID)
		{
			return pLink->OnRecv(MMC_PARTY_ADD_QUEST,
									3,
									NEW_UID(uidParty),
									NEW_UID(uidMember),
									NEW_BLOB(vecQuestID)
									);
		}
		
		MCommandResult RecvPartyAddField(MockLink* pLink, MUID uidParty, MUID uidMember, MUID uidFieldGroup)
		{
			return pLink->OnRecv(MMC_PARTY_ADD_FIELD,
									3,
									NEW_UID(uidParty),
									NEW_UID(uidMember),
									NEW_UID(uidFieldGroup)
									);
		}

		MCommandResult RecvPartyUpdateQuestRunner(MockLink* pLink, MUID uidParty, MUID uidMember, MUID uidField, vector<int>& vecQuestID)
		{
			return pLink->OnRecv(MMC_PARTY_UPDATE_QUEST_RUNNER,
									4,
									NEW_UID(uidParty),
									NEW_UID(uidMember),
									NEW_UID(uidField),
									NEW_BLOB(vecQuestID)
									);
		}


		MCommandResult RecvPartyLeaveReq(MockLink* pLink, MUID uidParty, MUID uidLeavePlayer)
		{
			return pLink->OnRecv(MMC_PARTY_LEAVE_REQ,
									2,
									NEW_UID(uidParty),
									NEW_UID(uidLeavePlayer)
									);
		}
		
		MCommandResult RecvPartyRemoveQuest(MockLink* pLink, MUID uidParty, MUID uidMember, vector<int>& vecQuestID)
		{
			return pLink->OnRecv(MMC_PARTY_REMOVE_QUEST,
									3,
									NEW_UID(uidParty),
									NEW_UID(uidMember),
									NEW_BLOB(vecQuestID)
									);
		}
		
		MCommandResult RecvPartyRemoveField(MockLink* pLink, MUID uidParty, MUID uidMember, int nFieldGroupID)
		{
			return pLink->OnRecv(MMC_PARTY_REMOVE_FIELD,
									3,
									NEW_UID(uidParty),
									NEW_UID(uidMember),
									NEW_INT(nFieldGroupID)
									);
		}

		MCommandResult RecvPartyEraseQuestRunner(MockLink* pLink, MUID uidParty, MUID uidMember)
		{
			return pLink->OnRecv(MMC_PARTY_ERASE_QUEST_RUNNER,
									2,
									NEW_UID(uidParty),
									NEW_UID(uidMember)
									);
		}

		MCommandResult RecvPartyKickReq(MockLink* pLink, MUID uidParty, MUID uidRequestPlayer, MUID uidTargetPlayer)
		{
			return pLink->OnRecv(MMC_PARTY_KICK_REQ,
									3,
									NEW_UID(uidParty),
									NEW_UID(uidRequestPlayer),
									NEW_UID(uidTargetPlayer)
									);
		}

		MCommandResult RecvDoOffline(MockLink* pLink, MUID uidParty, MUID uidMember)
		{
			return pLink->OnRecv(MMC_PARTY_DO_OFFLINE,
									2,
									NEW_UID(uidParty),
									NEW_UID(uidMember)
									);
		}

		MCommandResult RecvDoOnline(MockLink* pLink, MUID uidParty, MUID uidMember, MUID uidOffline)
		{
			return pLink->OnRecv(MMC_PARTY_DO_ONLINE,
									3,
									NEW_UID(uidParty),
									NEW_UID(uidMember),
									NEW_UID(uidOffline)
									);
		}

		MCommandResult RecvJoinInviteReq(MockLink* pLink, MUID uidParty, MUID uidRequestPlayer)
		{
			return pLink->OnRecv(MMC_PARTY_JOIN_INVITE_REQ,
									2,
									NEW_UID(uidParty),
									NEW_UID(uidRequestPlayer)
									);
		}

		MCommandResult RecvJoinAcceptRes(MockLink* pLink, MUID uidParty, MUID uidLeader, MUID uidRequestPlayer, CCommandResultTable nResult)
		{
			return pLink->OnRecv(MMC_PARTY_JOIN_ACCEPT_RES,
									4,
									NEW_UID(uidParty),
									NEW_UID(uidLeader),
									NEW_UID(uidRequestPlayer),
									NEW_INT(nResult)
									);
		}

		MCommandResult RecvMoveServer(MockLink* pLink, MUID uidParty, MUID uidMember)
		{
			return pLink->OnRecv(MMC_PARTY_MOVE_SERVER,
									2,
									NEW_UID(uidParty),
									NEW_UID(uidMember)
									);
		}	
	};


	// Router	//////////////////////////////////////////////////////////////////////////////////////////////
	TEST_FIXTURE(Fixture, TestPartyCommandRouter_PartyInviteRes)	
	{
		CCommandResultTable nResult = CR_SUCCESS;
		
		m_pPartyRouter->InviteRes(m_uidRequest, m_uidRequestPlayer, m_uidTargetPlayer, nResult);


		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), m_uidTargetPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 2), nResult);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandRouter_PartyAcceptReq)	
	{
		m_pPartyRouter->AcceptReq(m_uidTarget, m_uidTargetPlayer, m_uidRequestPlayer, m_strRequestPlayerName.c_str());


		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_ACCEPT_REQ);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), m_uidTargetPlayer);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 1), m_uidRequestPlayer);
				
		wstring strRequestPlayerName;
		MCommand& cmd = m_pTargetLink->GetCommand(0);
		cmd.GetParameter(strRequestPlayerName, 2, MPT_WSTR);

		CHECK(strRequestPlayerName == m_strRequestPlayerName);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandRouter_PartyAcceptCancel)	
	{
		CCommandResultTable nResult = CR_SUCCESS;
		m_pPartyRouter->AcceptCancel(m_uidTarget, m_uidTargetPlayer, nResult);


		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), m_uidTargetPlayer);
		CHECK_EQUAL(m_pTargetLink->GetParam<int>(0, 1), nResult);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandRouter_PartyAdd)	
	{
		MUID uidParty = ZTestHelper::NewUID();
		MUID uidLeader = ZTestHelper::NewUID();
		wstring strLeaderName = L"TestLeaderName";
		int nLeaderCID = ZTestHelper::NewCID();

		m_pPartyRouter->PartyAdd(uidParty, uidLeader, strLeaderName, nLeaderCID);

		
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_ADD);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), uidLeader);		
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 3), nLeaderCID);

		wstring strLeaderNameParam;
		MCommand& cmdRequest = m_pRequestLink->GetCommand(0);
		cmdRequest.GetParameter(strLeaderNameParam, 2, MPT_WSTR);
		CHECK(strLeaderNameParam == strLeaderName);


		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_ADD);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 1), uidLeader);		
		CHECK_EQUAL(m_pTargetLink->GetParam<int>(0, 3), nLeaderCID);
		
		MCommand& cmdTarget = m_pTargetLink->GetCommand(0);
		cmdRequest.GetParameter(strLeaderNameParam, 2, MPT_WSTR);
		CHECK(strLeaderNameParam == strLeaderName);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandRouter_PartyAddMember)
	{
		MUID uidParty = ZTestHelper::NewUID();
		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();

		m_pPartyRouter->AddMember(uidParty, uidMember, strMemberName, nMemberCID);


		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_ADD_MEMBER);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), uidMember);		
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 3), nMemberCID);

		wstring strMemberNameParam;
		MCommand& cmdRequest = m_pRequestLink->GetCommand(0);
		cmdRequest.GetParameter(strMemberNameParam, 2, MPT_WSTR);
		CHECK(strMemberNameParam == strMemberName);

		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_ADD_MEMBER);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 1), uidMember);
		CHECK_EQUAL(m_pTargetLink->GetParam<int>(0, 3), nMemberCID);

		MCommand& cmdTarget = m_pRequestLink->GetCommand(0);
		cmdRequest.GetParameter(strMemberNameParam, 2, MPT_WSTR);
		CHECK(strMemberNameParam == strMemberName);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandRouter_PartySync)		
	{
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		gmgr.pPartyManager->AddMember(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());

		
		m_pPartyRouter->PartySync(pParty);


		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_SYNC);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_SYNC);
		
		
		ASSERT_EQUAL(m_pRequestLink->GetParam<MBlob*>(0, 0)->GetBlobCount(), 1);
		TD_PARTY* pPartyInfo = (TD_PARTY*)m_pRequestLink->GetParam<MBlob*>(0, 0)->GetBlobPointer();
		CHECK_EQUAL(pPartyInfo->m_uidParty, uidParty);
		CHECK_EQUAL(pPartyInfo->m_partySetting.m_uidLeader, m_uidRequestPlayer);

		ASSERT_EQUAL(m_pRequestLink->GetParam<MBlob*>(0, 1)->GetBlobCount(), 2);
		
		TD_PARTY_MEMBER* pPartyMember = (TD_PARTY_MEMBER*)m_pRequestLink->GetParam<MBlob*>(0, 1)->GetBlobElement(0);
		CHECK_EQUAL(pPartyMember->m_uidPlayer, m_uidRequestPlayer);
		CHECK(pPartyMember->szName == m_strRequestPlayerName);

		pPartyMember = (TD_PARTY_MEMBER*)m_pRequestLink->GetParam<MBlob*>(0, 1)->GetBlobElement(1);
		CHECK_EQUAL(pPartyMember->m_uidPlayer, m_uidTargetPlayer);
		CHECK(pPartyMember->szName == m_strTargetPlayerName);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandRouter_MemberSync)	
	{
		MUID uidParty = ZTestHelper::NewUID();
		
		TD_PARTY_MEMBER tdPartyMember;
		tdPartyMember.m_uidPlayer = m_uidRequestPlayer;
		tdPartyMember.nStatusFlag = 0;
		tdPartyMember.nHP = 100;
		tdPartyMember.nEN = 100;
		tdPartyMember.nSTA = 100;		
		wcsncpy_s(tdPartyMember.szName, m_strRequestPlayerName.c_str(), _TRUNCATE);

		vector<int> vecBuff;
		vecBuff.push_back(100);
		vecBuff.push_back(200);
		vecBuff.push_back(300);

		
		ZPartyMember partyMember;
		partyMember.Assign(tdPartyMember, vecBuff);

		m_pPartyRouter->MemberSync(uidParty, &partyMember);


		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_MEMBER_SYNC);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_MEMBER_SYNC);		

		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), uidParty);
		ASSERT_EQUAL(m_pRequestLink->GetParam<MBlob*>(0, 1)->GetBlobCount(), 1);
		ASSERT_EQUAL(m_pRequestLink->GetParam<MBlob*>(0, 2)->GetBlobCount(), 3);

		TD_PARTY_MEMBER* pPartyMember = (TD_PARTY_MEMBER*)m_pRequestLink->GetParam<MBlob*>(0, 1)->GetBlobPointer();
		CHECK_EQUAL(pPartyMember->m_uidPlayer, tdPartyMember.m_uidPlayer);
		CHECK_EQUAL(pPartyMember->nHP, tdPartyMember.nHP);
		CHECK_EQUAL(pPartyMember->nEN, tdPartyMember.nEN);
		CHECK_EQUAL(pPartyMember->nSTA, tdPartyMember.nSTA);
		CHECK(0 == wcscmp(pPartyMember->szName, tdPartyMember.szName));

		CHECK_EQUAL(*(int*)m_pRequestLink->GetParam<MBlob*>(0, 2)->GetBlobElement(0), 100);
		CHECK_EQUAL(*(int*)m_pRequestLink->GetParam<MBlob*>(0, 2)->GetBlobElement(1), 200);
		CHECK_EQUAL(*(int*)m_pRequestLink->GetParam<MBlob*>(0, 2)->GetBlobElement(2), 300);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandRouter_QuestSync)		
	{
		MUID uidParty = ZTestHelper::NewUID();
		
		vector<TD_PARTY_QUEST> vecPartyQuest;
		TD_PARTY_QUEST tdPartyQuest[3];

		tdPartyQuest[0].nQuestID = 100;
		tdPartyQuest[0].nQuestCount = 1;
		tdPartyQuest[1].nQuestID = 200;
		tdPartyQuest[1].nQuestCount = 3;
		tdPartyQuest[2].nQuestID = 300;
		tdPartyQuest[2].nQuestCount = 2;

		vecPartyQuest.push_back(tdPartyQuest[0]);
		vecPartyQuest.push_back(tdPartyQuest[1]);
		vecPartyQuest.push_back(tdPartyQuest[2]);


		m_pPartyRouter->QuestSync(uidParty, vecPartyQuest);


		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_QUEST_SYNC);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_QUEST_SYNC);

		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), uidParty);
		ASSERT_EQUAL(m_pRequestLink->GetParam<MBlob*>(0, 1)->GetBlobCount(), 3);

		TD_PARTY_QUEST* pPartyQuest = (TD_PARTY_QUEST*)m_pRequestLink->GetParam<MBlob*>(0, 1)->GetBlobElement(0);
		CHECK_EQUAL(pPartyQuest->nQuestID, 100);
		CHECK_EQUAL(pPartyQuest->nQuestCount, 1);

		pPartyQuest = (TD_PARTY_QUEST*)m_pRequestLink->GetParam<MBlob*>(0, 1)->GetBlobElement(1);
		CHECK_EQUAL(pPartyQuest->nQuestID, 200);
		CHECK_EQUAL(pPartyQuest->nQuestCount, 3);

		pPartyQuest = (TD_PARTY_QUEST*)m_pRequestLink->GetParam<MBlob*>(0, 1)->GetBlobElement(2);
		CHECK_EQUAL(pPartyQuest->nQuestID, 300);
		CHECK_EQUAL(pPartyQuest->nQuestCount, 2);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandRouter_FieldSync)		
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		vector<TD_PARTY_FIELD> vecPartyField;
		TD_PARTY_FIELD tdPartyField;
		tdPartyField.nFieldGroupID = 100;
		tdPartyField.uidFieldGroup = ZTestHelper::NewUID();
		vecPartyField.push_back(tdPartyField);
		tdPartyField.nFieldGroupID = 200;
		tdPartyField.uidFieldGroup = ZTestHelper::NewUID();
		vecPartyField.push_back(tdPartyField);
		tdPartyField.nFieldGroupID = 300;
		tdPartyField.uidFieldGroup = ZTestHelper::NewUID();
		vecPartyField.push_back(tdPartyField);


		// act
		m_pPartyRouter->FieldSync(uidParty, vecPartyField);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_FIELD_SYNC);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_FIELD_SYNC);

		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), uidParty);
	
		MCommand& cmd = m_pRequestLink->GetCommand(0);
		vector<TD_PARTY_FIELD> vecRecvPartyField;
		ASSERT_CHECK(cmd.GetBlob(vecRecvPartyField, 1));
		ASSERT_EQUAL(vecRecvPartyField.size(), 3);
		
		for(int i = 0; i < 3; i++)
		{
			CHECK_EQUAL(vecRecvPartyField[i].nFieldGroupID, vecPartyField[i].nFieldGroupID);
			CHECK_EQUAL(vecRecvPartyField[i].uidFieldGroup, vecPartyField[i].uidFieldGroup);
		}
	}

	TEST_FIXTURE(Fixture, TestPartyCommandRouter_QuestRunnerSync)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		MUID uidMember = ZTestHelper::NewUID();
		MUID uidField = ZTestHelper::NewUID();
		vector<int> vecQuestID;
		vecQuestID.push_back(100);
		vecQuestID.push_back(200);
		vecQuestID.push_back(300);		


		// act
		m_pPartyRouter->QuestRunnerSync(uidParty, uidMember, uidField, vecQuestID);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_QUEST_RUNNER_SYNC);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_QUEST_RUNNER_SYNC);

		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), uidMember);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 2), uidField);
		CHECK_EQUAL(m_pRequestLink->GetParam<MBlob*>(0, 3)->GetBlobCount(), 3);
		CHECK_EQUAL(*(int*)m_pRequestLink->GetParam<MBlob*>(0, 3)->GetBlobElement(0), 100);
		CHECK_EQUAL(*(int*)m_pRequestLink->GetParam<MBlob*>(0, 3)->GetBlobElement(1), 200);
		CHECK_EQUAL(*(int*)m_pRequestLink->GetParam<MBlob*>(0, 3)->GetBlobElement(2), 300);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandRouter_PartyChangeLeader)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		gmgr.pPartyManager->AddMember(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());


		// act
		m_pPartyRouter->ChangeLeaderRes(uidParty, m_uidTargetPlayer);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_CHANGE_LEADER);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_CHANGE_LEADER);

		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), m_uidTargetPlayer);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandRouter_PartyLeaveRes)	
	{
		// arrange
		CCommandResultTable nResult = CR_SUCCESS;

		
		// act
		m_pPartyRouter->LeaveRes(m_uidRequest, m_uidRequestPlayer, nResult);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_LEAVE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 1), nResult);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandRouter_PartyRemoveMember)
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();


		// act
		m_pPartyRouter->RemoveMember(uidParty, m_uidTargetPlayer);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_REMOVE_MEMBER);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_REMOVE_MEMBER);
		
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), m_uidTargetPlayer);
	}
	
	TEST_FIXTURE(Fixture, TestPartyCommandRouter_PartyEraseQuestRunnerSync)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		

		// act
		m_pPartyRouter->EraseQuestRunnerSync(uidParty, m_uidTargetPlayer);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_ERASE_QUEST_RUNNER_SYNC);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_ERASE_QUEST_RUNNER_SYNC);

		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), m_uidTargetPlayer);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandRouter_PartyKickRes)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		CCommandResultTable nResult = CR_SUCCESS;


		// act
		m_pPartyRouter->KickRes(m_uidRequest, m_uidRequestPlayer, m_uidTargetPlayer, nResult);

		
		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_KICK_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), m_uidTargetPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 2), nResult);
	}
	
	TEST_FIXTURE(Fixture, TestPartyCommandRouter_PartyAddOfflineMember)	
	{
		// arrange 
		MUID uidParty = ZTestHelper::NewUID();


		// act
		m_pPartyRouter->AddOfflineMember(uidParty, m_uidRequestPlayer);

		
		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_ADD_OFFLINE_MEMBER);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_ADD_OFFLINE_MEMBER);

		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), m_uidRequestPlayer);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandRouter_PartyRemoveOfflineMember)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		MUID uidMember = ZTestHelper::NewUID();
		MUID uidOffline = ZTestHelper::NewUID();


		// act
		m_pPartyRouter->RemoveOfflineMember(uidParty, uidMember, uidOffline);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_REMOVE_OFFLINE_MEMBER);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_REMOVE_OFFLINE_MEMBER);

		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), uidMember);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 2), uidOffline);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandRouter_PartyJoinInviteRes)	
	{
		// arrange
		CCommandResultTable nResult = CR_SUCCESS;
		

		// act
		m_pPartyRouter->JoinInviteRes(m_uidRequest, m_uidRequestPlayer, nResult);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_JOIN_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 1), nResult);
	}

	TEST_FIXTURE(Fixture, TetsPartyCommandRouter_PartyJoinAcceptReq)
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		MUID uidLeader = ZTestHelper::NewUID();


		// act
		m_pPartyRouter->JoinAcceptReq(m_uidTarget, uidParty, uidLeader, m_uidRequestPlayer, m_strRequestPlayerName);


		// assert
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_JOIN_ACCEPT_REQ);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 1), uidLeader);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 2), m_uidRequestPlayer);
		
		wstring strRequestPlayerName;
		MCommand& cmd = m_pTargetLink->GetCommand(0);
		cmd.GetParameter(strRequestPlayerName, 3, MPT_WSTR);
		CHECK(strRequestPlayerName == m_strRequestPlayerName);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandRouter_PartyJoinAcceptCancel)
	{
		// arrange
		MUID uidLeader = ZTestHelper::NewUID();
		CCommandResultTable nResult = CR_SUCCESS;


		// act
		m_pPartyRouter->JoinAcceptCancel(m_uidTarget, uidLeader, nResult);


		// assert
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_JOIN_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), uidLeader);
		CHECK_EQUAL(m_pTargetLink->GetParam<int>(0, 1), nResult);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandRouter_PartyMoveServerSync)
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		MUID uidMember = ZTestHelper::NewUID();
		MUID uidOffline = ZTestHelper::NewUID();


		// act		
		m_pPartyRouter->MoveServerSync(uidParty, uidMember, uidOffline);


		// assert
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_MOVE_SERVER_SYNC);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 1), uidMember);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 2), uidOffline);
	}
	
	TEST_FIXTURE(Fixture, TestPartyCommandRouter_PartyInfoAllRes)
	{
		// arrange
		vector<TD_PARTY_INFO> vecPartyInfo;
		TD_PARTY_INFO tdPartyInfo;
		tdPartyInfo.uidParty = ZTestHelper::NewUID();
		tdPartyInfo.nMemberCount = 3;
		tdPartyInfo.nLeaderIndex = 0;

		tdPartyInfo.members[0].uidMember = ZTestHelper::NewUID();
		tdPartyInfo.members[0].nMemberCID = ZTestHelper::NewCID();
		wcscpy_s(tdPartyInfo.members[0].szMemberName, sizeof(tdPartyInfo.members[0].szMemberName), L"TestLeaderName");
		tdPartyInfo.members[1].uidMember = ZTestHelper::NewUID();
		tdPartyInfo.members[1].nMemberCID = ZTestHelper::NewCID();
		wcscpy_s(tdPartyInfo.members[1].szMemberName, sizeof(tdPartyInfo.members[1].szMemberName), L"TestMemberName_1");
		tdPartyInfo.members[2].uidMember = ZTestHelper::NewUID();
		tdPartyInfo.members[2].nMemberCID = ZTestHelper::NewCID();
		wcscpy_s(tdPartyInfo.members[2].szMemberName, sizeof(tdPartyInfo.members[2].szMemberName), L"TestMemberName_2");

		vecPartyInfo.push_back(tdPartyInfo);
		
		
		// act
		m_pPartyRouter->PartyInfoAllRes(m_uidRequest, vecPartyInfo);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_INFO_ALL_RES);	
		CHECK_EQUAL(m_pRequestLink->GetParam<MBlob*>(0, 0)->GetBlobCount(), 1);
		TD_PARTY_INFO* pPartyInfo = (TD_PARTY_INFO*)m_pRequestLink->GetParam<MBlob*>(0, 0)->GetBlobPointer();
		ASSERT_CHECK(pPartyInfo != NULL);
		CHECK_EQUAL(pPartyInfo->uidParty, tdPartyInfo.uidParty);
		CHECK_EQUAL(pPartyInfo->nMemberCount, tdPartyInfo.nMemberCount);
		CHECK_EQUAL(pPartyInfo->nLeaderIndex, tdPartyInfo.nLeaderIndex);
		
		for(int i = 0; i < 3; i++)
		{
			CHECK_EQUAL(pPartyInfo->members[i].nMemberCID, tdPartyInfo.members[i].nMemberCID);
			CHECK_EQUAL(pPartyInfo->members[i].uidMember, tdPartyInfo.members[i].uidMember);
			CHECK(0 == wcscmp(pPartyInfo->members[i].szMemberName, tdPartyInfo.members[i].szMemberName));
		}
	}


	// Handler	//////////////////////////////////////////////////////////////////////////////////////////////

	TEST_FIXTURE(Fixture, TestPartyCommandHandler_PartyInviteReq)	
	{
		MCommandResult nCommandResult = RecvInviteReq(m_pRequestLink, m_uidTargetPlayer, m_uidRequestPlayer);

		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartyLogic->m_uidSender, m_uidRequest);
		CHECK_EQUAL(m_pPartyLogic->m_uidTargetPlayer, m_uidTargetPlayer);
		CHECK_EQUAL(m_pPartyLogic->m_uidRequestPlayer, m_uidRequestPlayer);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandHandler_PartyAcceptRes)	
	{
		CCommandResultTable nResult = CR_SUCCESS;
		MCommandResult nCommadResult = RecvAcceptRes(m_pTargetLink, m_uidRequestPlayer, m_uidTargetPlayer, nResult);

		CHECK_EQUAL(nCommadResult, CR_TRUE);
		CHECK_EQUAL(m_pPartyLogic->m_uidSender, m_uidTarget);
		CHECK_EQUAL(m_pPartyLogic->m_uidRequestPlayer, m_uidRequestPlayer);
		CHECK_EQUAL(m_pPartyLogic->m_uidTargetPlayer, m_uidTargetPlayer);
		CHECK_EQUAL(m_pPartyLogic->m_nResult, nResult);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandHandler_PartyUpdate)
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();

		TD_PARTY_MEMBER tdPartyMember;
		tdPartyMember.m_uidPlayer = m_uidRequestPlayer;
		tdPartyMember.nStatusFlag = 0;
		tdPartyMember.nHP = 100;
		tdPartyMember.nEN = 100;
		tdPartyMember.nSTA = 100;
		wcsncpy_s(tdPartyMember.szName, m_strRequestPlayerName.c_str(), _TRUNCATE);
		

		vector<int> vecBuff;
		vecBuff.push_back(100);
		vecBuff.push_back(200);
		vecBuff.push_back(300);


		// act
		MCommandResult nCommandResult = RecvPartyUpdate(m_pRequestLink, uidParty, tdPartyMember, vecBuff);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartyLogic->m_uidParty, uidParty);		
		CHECK_EQUAL(m_pPartyLogic->m_tdPartyMember.m_uidPlayer, tdPartyMember.m_uidPlayer);
		CHECK_EQUAL(m_pPartyLogic->m_tdPartyMember.nStatusFlag, tdPartyMember.nStatusFlag);
		CHECK_EQUAL(m_pPartyLogic->m_tdPartyMember.nHP, tdPartyMember.nHP);
		CHECK_EQUAL(m_pPartyLogic->m_tdPartyMember.nEN, tdPartyMember.nEN);
		CHECK_EQUAL(m_pPartyLogic->m_tdPartyMember.nSTA, tdPartyMember.nSTA);
		CHECK(0 == wcscmp(m_pPartyLogic->m_tdPartyMember.szName, tdPartyMember.szName));
		CHECK(m_pPartyLogic->m_vecBuff == vecBuff);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandHandler_PartyUpdateMember)	
	{
		// arrance
		MUID uidParty = ZTestHelper::NewUID();

		TD_PARTY_MEMBER tdPartyMember;
		tdPartyMember.m_uidPlayer = m_uidRequestPlayer;
		tdPartyMember.nStatusFlag = 0;
		tdPartyMember.nHP = 100;
		tdPartyMember.nEN = 100;
		tdPartyMember.nSTA = 100;
		wcsncpy_s(tdPartyMember.szName, m_strRequestPlayerName.c_str(), _TRUNCATE);

		vector<int> vecBuff;
		vecBuff.push_back(100);
		vecBuff.push_back(200);
		vecBuff.push_back(300);


		// act
		MCommandResult nCommandResult = RecvPartyUpdateMember(m_pRequestLink, uidParty, tdPartyMember, vecBuff);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartyLogic->m_uidParty, uidParty);		
		CHECK_EQUAL(m_pPartyLogic->m_tdPartyMember.m_uidPlayer, tdPartyMember.m_uidPlayer);
		CHECK_EQUAL(m_pPartyLogic->m_tdPartyMember.nStatusFlag, tdPartyMember.nStatusFlag);
		CHECK_EQUAL(m_pPartyLogic->m_tdPartyMember.nHP, tdPartyMember.nHP);
		CHECK_EQUAL(m_pPartyLogic->m_tdPartyMember.nEN, tdPartyMember.nEN);
		CHECK_EQUAL(m_pPartyLogic->m_tdPartyMember.nSTA, tdPartyMember.nSTA);
		CHECK(0 == wcscmp(m_pPartyLogic->m_tdPartyMember.szName, tdPartyMember.szName));
		CHECK(m_pPartyLogic->m_vecBuff == vecBuff);		
	}

	TEST_FIXTURE(Fixture, TestPartyCommandHandler_PartyAddQuest)		
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		MUID uidMember = ZTestHelper::NewUID();

		vector<int> vecQuestID;
		vecQuestID.push_back(100);
		vecQuestID.push_back(200);
		vecQuestID.push_back(300);


		// act
		MCommandResult nCommandResult = RecvPartyAddQuest(m_pRequestLink, uidParty, uidMember, vecQuestID);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartyLogic->m_uidParty, uidParty);
		CHECK_EQUAL(m_pPartyLogic->m_uidMember, uidMember);
		CHECK(m_pPartyLogic->m_vecQuestID == vecQuestID);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandHandler_PartyAddField)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		MUID uidMember = ZTestHelper::NewUID();
		MUID uidFieldGroup = ZTestHelper::NewUID();		
		
		
		// act
		MCommandResult nCommandResult = RecvPartyAddField(m_pRequestLink, uidParty, uidMember, uidFieldGroup);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartyLogic->m_uidParty, uidParty);
		CHECK_EQUAL(m_pPartyLogic->m_uidMember, uidMember);
		CHECK_EQUAL(m_pPartyLogic->m_uidFieldGroup, uidFieldGroup);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandHandler_PartyUpdateQuestRunner)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		MUID uidField = ZTestHelper::NewUID();
		vector<int> vecQuestID;
		vecQuestID.push_back(100);
		vecQuestID.push_back(200);
		vecQuestID.push_back(300);


		// act
		MCommandResult nCommandResult = RecvPartyUpdateQuestRunner(m_pRequestLink, uidParty, m_uidRequestPlayer, uidField, vecQuestID);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartyLogic->m_uidParty, uidParty);
		CHECK_EQUAL(m_pPartyLogic->m_uidMember, m_uidRequestPlayer);
		CHECK_EQUAL(m_pPartyLogic->m_uidField, uidField);
		CHECK(m_pPartyLogic->m_vecQuestID == vecQuestID);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandHandler_PartyLeaveReq)
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();


		// act
		MCommandResult nCommandResult = RecvPartyLeaveReq(m_pTargetLink, uidParty, m_uidTargetPlayer); 


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartyLogic->m_uidSender, m_uidTarget);
		CHECK_EQUAL(m_pPartyLogic->m_uidParty, uidParty);
		CHECK_EQUAL(m_pPartyLogic->m_uidLeaveMember, m_uidTargetPlayer);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandHandler_PartyRemoveQuest)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		MUID uidMember = ZTestHelper::NewUID();

		vector<int> vecQuestID;
		vecQuestID.push_back(100);
		vecQuestID.push_back(200);
		vecQuestID.push_back(300);


		// act
		MCommandResult nCommandResult = RecvPartyRemoveQuest(m_pRequestLink, uidParty, uidMember, vecQuestID);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartyLogic->m_uidParty, uidParty);
		CHECK_EQUAL(m_pPartyLogic->m_uidMember, uidMember);
		CHECK(m_pPartyLogic->m_vecQuestID == vecQuestID);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandHandler_PartyRemoveField)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		MUID uidMember = ZTestHelper::NewUID();
		int nFieldGroupID = 100;
		MUID uidFieldGroup = ZTestHelper::NewUID();
		
		
		// act
		MCommandResult nCommandResult = RecvPartyRemoveField(m_pRequestLink, uidParty, uidMember, nFieldGroupID);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartyLogic->m_uidParty, uidParty);
		CHECK_EQUAL(m_pPartyLogic->m_uidMember, uidMember);
		CHECK_EQUAL(m_pPartyLogic->m_nFieldGroupID, nFieldGroupID);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandHandler_PartyEraseQuestRunner)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();


		// act
		MCommandResult nCommandResult = RecvPartyEraseQuestRunner(m_pRequestLink, uidParty, m_uidRequestPlayer);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartyLogic->m_uidParty, uidParty);
		CHECK_EQUAL(m_pPartyLogic->m_uidMember, m_uidRequestPlayer);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandHandler_PartyKickReq)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();


		// act
		MCommandResult nCommandResult = RecvPartyKickReq(m_pRequestLink, uidParty, m_uidRequestPlayer, m_uidTargetPlayer);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartyLogic->m_uidSender, m_uidRequest);
		CHECK_EQUAL(m_pPartyLogic->m_uidParty, uidParty);
		CHECK_EQUAL(m_pPartyLogic->m_uidRequestPlayer, m_uidRequestPlayer);
		CHECK_EQUAL(m_pPartyLogic->m_uidTargetPlayer, m_uidTargetPlayer);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandHandler_DoOffline)
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();


		// act
		MCommandResult nCommandResult = RecvDoOffline(m_pTargetLink, uidParty, m_uidTargetPlayer);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartyLogic->m_uidParty, uidParty);
		CHECK_EQUAL(m_pPartyLogic->m_uidMember, m_uidTargetPlayer);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandHandler_DoOnline)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		MUID uidOffline = ZTestHelper::NewUID();
		

		// act
		MCommandResult nCommandResult = RecvDoOnline(m_pTargetLink, uidParty, m_uidTargetPlayer, uidOffline);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartyLogic->m_uidParty, uidParty);
		CHECK_EQUAL(m_pPartyLogic->m_uidMember, m_uidTargetPlayer);
		CHECK_EQUAL(m_pPartyLogic->m_uidOffline, uidOffline);
	}

	TEST_FIXTURE(Fixture, TestPartyCommandHandler_JoinInviteReq)
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();


		// act
		MCommandResult nCommandResult = RecvJoinInviteReq(m_pRequestLink, uidParty, m_uidRequestPlayer);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartyLogic->m_uidParty, uidParty);
		CHECK_EQUAL(m_pPartyLogic->m_uidRequestPlayer, m_uidRequestPlayer);
	}
}
