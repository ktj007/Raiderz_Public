#include "stdafx.h"
#include "SUnitTestUtil.h"
#include "FForCommandTest.h"
#include "MockPartySystem.h"
#include "GProxyParty.h"
#include "GMasterServerPartyRouter.h"
#include "GClientPartyRouter.h"
#include "GConfig.h"
#include "MockMasterServerFacade.h"
#include "MockMasterNetClient.h"
#include "CCommandResultTable.h"


SUITE(PartyCommand)
{
	struct MockCommandPartySystem: public MockPartySystem
	{
		MUID m_uidTargetPlayer;
		MUID m_uidRequestPlayer;
		wstring m_strRequestPlayerName;
		CCommandResultTable m_nResult;

		MUID m_uidParty;
		MUID m_uidMember;
		wstring m_strLeaderName;
		wstring m_strMemberName;

		int m_nMemberCID;

		MCommand* m_pCommand;
		map<int, int> m_mapQuests;
		map<int, MUID> m_mapFields;

		MUID m_uidField;
		vector<int> m_vecQuestID;

		wstring m_strMessage;
		
		MUID m_uidOffline;

		PARTY_SETTING m_partySetting;

		MockCommandPartySystem(): m_pCommand(NULL)	{}
		virtual ~MockCommandPartySystem()			{ SAFE_DELETE(m_pCommand); }

		virtual void InviteRes(MUID uidRequestPlayer, MUID uidTargetPlayer, CCommandResultTable nResult) override
		{
			m_uidRequestPlayer = uidRequestPlayer;
			m_uidTargetPlayer = uidTargetPlayer;
			m_nResult = nResult;
		}

		virtual void AcceptReq(MUID uidTargetPlayer, MUID uidRequestPlayer, wstring strRequestPlayerName) override
		{
			m_uidTargetPlayer = uidTargetPlayer;
			m_uidRequestPlayer = uidRequestPlayer;
			m_strRequestPlayerName = strRequestPlayerName;
		}

		virtual void AcceptCancel(MUID uidTargetPlayer, CCommandResultTable nResult) override
		{
			m_uidTargetPlayer = uidTargetPlayer;
			m_nResult = nResult;
		}

		virtual void AddParty(MUID uidParty, MUID uidLeader, QuestID nQuestID) override
		{
			m_uidParty = uidParty;
			m_partySetting.m_uidLeader = uidLeader;
			
			const GProxyParty* pProxyParty = gsys.pMasterServerFacade->FindProxyParty(uidParty);
			if (pProxyParty != NULL)
			{
				m_strLeaderName = pProxyParty->GetName(uidLeader);
			}
		}

		virtual void AddMember(MUID uidParty, MUID uidMember, wstring strMemberName, int nMemberCID) override
		{
			m_uidParty = uidParty;
			m_uidMember = uidMember;
			m_strMemberName = strMemberName;
			m_nMemberCID =nMemberCID;
		}

		virtual void SyncParty(const TD_PARTY& tdParty, MCommand* pPartySyncCommand) override
		{
			m_uidParty = tdParty.m_uidParty;
			
			m_partySetting.m_uidLeader = tdParty.m_partySetting.m_uidLeader;
			wcsncpy_s(m_partySetting.m_szName, tdParty.m_partySetting.m_szName, _TRUNCATE);
			
			m_partySetting.m_lootingRuleData.m_nLRC = (LOOTING_RULE_COMMON)tdParty.m_partySetting.m_nLRC;
			m_partySetting.m_lootingRuleData.m_nLRR = (LOOTING_RULE_RARE)tdParty.m_partySetting.m_nLRR;
			m_partySetting.m_lootingRuleData.m_nLRRF = (LOOTING_RULE_RARE_FOR)tdParty.m_partySetting.m_nLRRF;

			m_pCommand = pPartySyncCommand->Clone();
		}

		virtual void SyncMember(MUID uidParty, MCommand* pPartyMemberSyncCommand) override
		{
			m_uidParty = uidParty;
			m_pCommand = pPartyMemberSyncCommand->Clone();
		}

		virtual void SyncQuest(MUID uidParty, const map<int, int>& mapQuests) override
		{
			m_uidParty = uidParty;
			m_mapQuests = mapQuests;
		}

		virtual void SyncField(MUID uidParty, const map<int, MUID>& mapFields) override
		{
			m_uidParty = uidParty;
			m_mapFields = mapFields;
		}

		virtual void SyncQuestRunner(MUID uidParty, MUID uidMember, MUID uidField, const vector<int>& vecQuestID) override
		{
			m_uidParty = uidParty;
			m_uidMember = uidMember;
			m_uidField = uidField;
			m_vecQuestID = vecQuestID;
		}

		virtual void ChangePartyLeaderRes(MUID uidParty, MUID uidNewLeader) override
		{
			m_uidParty = uidParty;
			m_uidTargetPlayer = uidNewLeader;
		}

		virtual void LeaveRes(MUID uidLeavePlayer, CCommandResultTable nResult) override
		{
			m_uidRequestPlayer = uidLeavePlayer;
			m_nResult = nResult;
		}

		virtual void RemoveMember(MUID uidParty, MUID uidMember) override
		{
			m_uidParty = uidParty;
			m_uidMember = uidMember;
		}

		virtual void SyncEraseQuestRunner(MUID uidParty, MUID uidMember) override
		{
			m_uidParty = uidParty;
			m_uidMember = uidMember;
		}

		virtual void AddOfflineMember(MUID uidParty, MUID uidMember) override
		{
			m_uidParty = uidParty;
			m_uidMember = uidMember;
		}

		virtual void RemoveOfflineMember(MUID uidParty, MUID uidMember, MUID uidOffline) override
		{
			m_uidParty = uidParty;
			m_uidMember = uidMember;
			m_uidOffline = uidOffline;
		}

		virtual void JoinInviteRes(MUID uidRequestPlayer, CCommandResultTable nResult) override
		{
			m_uidRequestPlayer = uidRequestPlayer;
			m_nResult = nResult;
		}

		virtual void JoinAcceptReq(MUID uidParty, MUID uidLeader, MUID uidRequestPlayer, wstring strRequestPlayerName) override
		{
			m_uidParty = uidParty;
			m_partySetting.m_uidLeader = uidLeader;
			m_uidRequestPlayer = uidRequestPlayer;
			m_strRequestPlayerName = strRequestPlayerName;
		}	

		virtual void JoinAcceptCancel(MUID uidLeader, CCommandResultTable nResult) override
		{
			m_partySetting.m_uidLeader = uidLeader;
			m_nResult = nResult;
		}

		virtual void MoveServerSync(MUID uidParty, MUID uidMember, MUID uidOffline) override
		{
			m_uidParty = uidParty;
			m_uidMember = uidMember;
			m_uidOffline = uidOffline;
		}
	};

	struct Fixture: public FForCommandTest
	{
		GTestSysWrapper2<GPartySystem, MockCommandPartySystem> m_PartySystemWrapper;
		MockCommandPartySystem* m_pPartySystem;		

		MUID m_uidRequestPlayer;
		MUID m_uidTargetPlayer;
		wstring m_strRequestPlayerName;
		wstring m_strTargetPlayerName;
		int m_nRequestPlayerCID;
		int m_nTargetPlayerCID;

		GServerPartyRouter* m_pServerRouter;
		GClientPartyRouter* m_pClientRouter;
		GProxyPartyManager* m_pProxyPartyManager;

		MUID m_uidParty;

		Fixture()	
		{
			m_uidParty = SUnitTestUtil::NewUID();
			m_pPartySystem = m_PartySystemWrapper.Get();

			m_uidRequestPlayer = SUnitTestUtil::NewUID();
			m_strRequestPlayerName = L"RequestPlayerName";
			m_nRequestPlayerCID = SUnitTestUtil::NewCID();
			m_uidTargetPlayer = SUnitTestUtil::NewUID();
			m_strTargetPlayerName = L"TargetPlayerName";

			m_pServerRouter = new GMasterServerPartyRouter();
			m_pClientRouter = new GClientPartyRouter();

			m_pProxyPartyManager = m_pMasterServerFacade->GetProxyPartyManger();
		}

		~Fixture()	
		{
			delete m_pServerRouter;
			delete m_pClientRouter;
		}

		void MakePartyMember(TD_PARTY_MEMBER* pPartyMember, vector<int>* pVecBuff, MUID uidMember, wstring strName, int nBuffCount)
		{
			pPartyMember->m_uidPlayer = uidMember;
			pPartyMember->nStatusFlag = 0;
			pPartyMember->nHP = 100;
			pPartyMember->nEN = 100;
			pPartyMember->nSTA = 100;
			pPartyMember->nLevel = 15;
			wcsncpy_s(pPartyMember->szName, strName.c_str(), _TRUNCATE);

			for(int i = 0; i < nBuffCount; i++)
			{
				pVecBuff->push_back(100 + 100 * i);		// 100, 200, 300, ..., n
			}
		}

		void MakePartyInfo(vector<TD_PARTY_INFO>& vecPartyInfo)
		{
			TD_PARTY_INFO tdPartyInfo;
			
			MakePartyInfo(&tdPartyInfo, MAX_PARTY_MEMBER_COUNT);
			vecPartyInfo.push_back(tdPartyInfo);			

			MakePartyInfo(&tdPartyInfo, 1);
			vecPartyInfo.push_back(tdPartyInfo);			

			MakePartyInfo(&tdPartyInfo, 4);
			vecPartyInfo.push_back(tdPartyInfo);			
		}

		void MakePartyInfo(TD_PARTY_INFO* pPartyInfo, int nMemberCount)
		{
			_ASSERT(0 < nMemberCount);
			_ASSERT(nMemberCount <= MAX_PARTY_MEMBER_COUNT);

			pPartyInfo->uidParty = SUnitTestUtil::NewUID();
			pPartyInfo->nLeaderIndex = RandomNumber(0, nMemberCount - 1);
			pPartyInfo->nMemberCount = nMemberCount;

			for(int i = 0; i < nMemberCount; i++)
			{
				MUID uidMember = SUnitTestUtil::NewUID();
				int nMemberCID = SUnitTestUtil::NewCID();
				
				wstring strMemberName;
				if (i == pPartyInfo->nLeaderIndex)
				{
					strMemberName = L"TestLeaderName";
				}
				else
				{
					strMemberName = L"TestMemberName";
				}

				pPartyInfo->members[i].uidMember = uidMember;
				pPartyInfo->members[i].nMemberCID = nMemberCID;
				swprintf_s(pPartyInfo->members[i].szMemberName, L"%s_%02d", strMemberName.c_str(), i + 1);
			}
		}

		MCommandResult RecvPartyInviteRes(MUID uidRequestPlayer, MUID uidTargetPlayer, CCommandResultTable nResult)
		{
			return m_pNetAgent->OnRecv(MMC_PARTY_INVITE_RES,
										3,
										NEW_UID(uidRequestPlayer),
										NEW_UID(uidTargetPlayer),
										NEW_INT(nResult)
										);
		}

		MCommandResult RecvPartyAcceptReq(MUID uidTargetPlayer, MUID uidRequestPlayer, wstring strRequestPlayerName)
		{
			return m_pNetAgent->OnRecv(MMC_PARTY_ACCEPT_REQ,
										3,
										NEW_UID(uidTargetPlayer),
										NEW_UID(uidRequestPlayer),
										NEW_WSTR(strRequestPlayerName.c_str())
										);
		}

		MCommandResult RecvPartyAcceptCancel(MUID uidTargetPlayer, int nResult)
		{
			return m_pNetAgent->OnRecv(MMC_PARTY_ACCEPT_CANCEL,
										2,
										NEW_UID(uidTargetPlayer),
										NEW_INT(nResult)
										);
		}

		MCommandResult RecvPartyAdd(MUID uidParty, MUID uidLeader, wstring strLeaderName, int nLeaderCID)
		{
			return m_pNetAgent->OnRecv(MMC_PARTY_ADD,
										4,
										NEW_UID(uidParty),
										NEW_UID(uidLeader),
										NEW_WSTR(strLeaderName.c_str()),
										NEW_INT(nLeaderCID)
										);
		}

		MCommandResult RecvPartyAddMember(MUID uidParty, MUID uidMember, wstring strMemberName, int nMemberCID)
		{
			return m_pNetAgent->OnRecv(MMC_PARTY_ADD_MEMBER,
										4,
										NEW_UID(uidParty),
										NEW_UID(uidMember),
										NEW_WSTR(strMemberName.c_str()), 
										NEW_INT(nMemberCID)
										);
		}			

		MCommandResult RecvPartySync(TD_PARTY& tdParty, vector<TD_PARTY_MEMBER>& vecPartyMember, vector<vector<int> >& vecPartyBuff)
		{
			_ASSERT(MAX_PARTY_MEMBER_COUNT == 5);
			_ASSERT(vecPartyBuff.size() == 5);

			return m_pNetAgent->OnRecv(MMC_PARTY_SYNC,
										7,
										NEW_BLOB(&tdParty, sizeof(tdParty), 1),
										NEW_BLOB(vecPartyMember),
										NEW_BLOB(vecPartyBuff[0]),
										NEW_BLOB(vecPartyBuff[1]),
										NEW_BLOB(vecPartyBuff[2]),
										NEW_BLOB(vecPartyBuff[3]),
										NEW_BLOB(vecPartyBuff[4])
										);
		}

		MCommandResult RecvMemberSync(MUID uidParty, TD_PARTY_MEMBER& tdPartyMember, vector<int>& vecBuff)
		{
			return m_pNetAgent->OnRecv(MMC_PARTY_MEMBER_SYNC,
										3,
										NEW_UID(uidParty),
										NEW_BLOB(&tdPartyMember, sizeof(tdPartyMember), 1),
										NEW_BLOB(vecBuff)
										);
		}

		MCommandResult RecvQuestSync(MUID uidParty, vector<TD_PARTY_QUEST>& vecPartyQuests)
		{
			return m_pNetAgent->OnRecv(MMC_PARTY_QUEST_SYNC,
										2,
										NEW_UID(uidParty),
										NEW_BLOB(vecPartyQuests)
										);
		}

		MCommandResult RecvFieldSync(MUID uidParty, vector<TD_PARTY_FIELD>& vecPartyFields)
		{
			return m_pNetAgent->OnRecv(MMC_PARTY_FIELD_SYNC,
										2,
										NEW_UID(uidParty),
										NEW_BLOB(vecPartyFields)
										);
		}

		MCommandResult RecvQuestRunnerSync(MUID uidParty, MUID uidMember, MUID uidField, vector<int>& vecQuestID)
		{
			return m_pNetAgent->OnRecv(MMC_PARTY_QUEST_RUNNER_SYNC,
										4,
										NEW_UID(uidParty),
										NEW_UID(uidMember),
										NEW_UID(uidField),
										NEW_BLOB(vecQuestID)
										);
		}

		MCommandResult RecvChangeLeaderRes(MUID uidParty, MUID uidNewLeader)
		{
			return m_pNetAgent->OnRecv(MMC_PARTY_CHANGE_LEADER,
										2,
										NEW_UID(uidParty),
										NEW_UID(uidNewLeader)
										);
		}

		MCommandResult RecvLeaveRes(MUID uidLeavePlayer, CCommandResultTable nResult)
		{
			return m_pNetAgent->OnRecv(MMC_PARTY_LEAVE_RES,
										2,
										NEW_UID(uidLeavePlayer),
										NEW_INT(nResult)
										);
		}

		MCommandResult RecvRemoveMember(MUID uidParty, MUID uidMember)
		{
			return m_pNetAgent->OnRecv(MMC_PARTY_REMOVE_MEMBER,
										2,
										NEW_UID(uidParty),
										NEW_UID(uidMember)
										);
		}

		MCommandResult RecvEraseQuestRunnerSync(MUID uidParty, MUID uidMember)
		{
			return m_pNetAgent->OnRecv(MMC_PARTY_ERASE_QUEST_RUNNER_SYNC,
										2,
										NEW_UID(uidParty),
										NEW_UID(uidMember)
										);
		}

		MCommandResult RecvAddOfflineMember(MUID uidParty, MUID uidMember)
		{
			return m_pNetAgent->OnRecv(MMC_PARTY_ADD_OFFLINE_MEMBER,
										2,
										NEW_UID(uidParty),
										NEW_UID(uidMember)
										);
		}

		MCommandResult RecvRemoveOfflineMember(MUID uidParty, MUID uidMember, MUID uidOffline)
		{
			return m_pNetAgent->OnRecv(MMC_PARTY_REMOVE_OFFLINE_MEMBER,
										3,
										NEW_UID(uidParty),
										NEW_UID(uidMember),
										NEW_UID(uidOffline)
										);
		}

		MCommandResult RecvJoinAcceptReq(MUID uidParty, MUID uidLeader, MUID uidRequestPlayer, wstring strRequestPlayerName)
		{
			return m_pNetAgent->OnRecv(MMC_PARTY_JOIN_ACCEPT_REQ,
										4,
										NEW_UID(uidParty),
										NEW_UID(uidLeader),
										NEW_UID(uidRequestPlayer),
										NEW_WSTR(strRequestPlayerName.c_str())
										);
		}

		MCommandResult RecvJoinInviteRes(MUID uidRequestPlayer, CCommandResultTable nResult)
		{
			return m_pNetAgent->OnRecv(MMC_PARTY_JOIN_INVITE_RES,
										2,
										NEW_UID(uidRequestPlayer),
										NEW_INT(nResult)
										);
		}

		MCommandResult RecvJoinAcceptCancel(MUID uidLeader, CCommandResultTable nResult)
		{
			return m_pNetAgent->OnRecv(MMC_PARTY_JOIN_ACCEPT_CANCEL,
										2,
										NEW_UID(uidLeader),
										NEW_INT(nResult)
										);
		}

		MCommandResult RecvMoverServerSync(MUID uidParty, MUID uidMember, MUID uidOffline)
		{
			return m_pNetAgent->OnRecv(MMC_PARTY_MOVE_SERVER_SYNC,
										3,
										NEW_UID(uidParty),
										NEW_UID(uidMember),
										NEW_UID(uidOffline)
										);
		}
		
		MCommandResult RecvPartyInfoAllRes(vector<TD_PARTY_INFO>& vecPartyInfo)
		{
			return m_pNetAgent->OnRecv(MMC_PARTY_INFO_ALL_RES, 
										1,
										NEW_BLOB(vecPartyInfo)
										);
		}
	};


	// Party Router	//////////////////////////////////////////////////////////////////////////////////////////

	TEST_FIXTURE(Fixture, TestCommandRouter_PartyInviteReq)	
	{
		m_pServerRouter->InviteReq(m_uidTargetPlayer, m_uidRequestPlayer);

		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_INVITE_REQ);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), m_uidTargetPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), m_uidRequestPlayer);
	}

	TEST_FIXTURE(Fixture, TestCommandRouter_PartyAcceptRes)		
	{	
		CCommandResultTable nResult = CR_SUCCESS;
		m_pServerRouter->AcceptRes(m_uidRequestPlayer, m_uidTargetPlayer, nResult);

		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_ACCEPT_RES);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), m_uidTargetPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 2), nResult);
	}

	TEST_FIXTURE(Fixture, TestCommandRouter_UpdateParty)		
	{
		TD_PARTY_MEMBER tdPartyMember;
		vector<int> vecBuff;

		MakePartyMember(&tdPartyMember, &vecBuff, m_uidRequestPlayer, m_strRequestPlayerName, 3);		
		m_pServerRouter->UpdateParty(m_uidParty, tdPartyMember, vecBuff);


		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_UPDATE);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), m_uidParty);
				
		TD_PARTY_MEMBER* pPartyMember;
		int nCount;
		ASSERT_CHECK(m_pNetAgent->GetSendCommand(0).GetBlob(pPartyMember, nCount, 1));
		CHECK_EQUAL(pPartyMember->m_uidPlayer, tdPartyMember.m_uidPlayer);
		CHECK_EQUAL(pPartyMember->nStatusFlag, tdPartyMember.nStatusFlag);
		CHECK_EQUAL(pPartyMember->nHP, tdPartyMember.nHP);
		CHECK_EQUAL(pPartyMember->nEN, tdPartyMember.nEN);
		CHECK_EQUAL(pPartyMember->nSTA, tdPartyMember.nSTA);
		CHECK_EQUAL(pPartyMember->nLevel, tdPartyMember.nLevel);
		CHECK(0 == wcscmp(pPartyMember->szName, tdPartyMember.szName));

		vector<int> vecSendBuff;
		ASSERT_CHECK(m_pNetAgent->GetSendCommand(0).GetBlob(vecSendBuff, 2));
		CHECK(vecSendBuff == vecBuff);
	}

	TEST_FIXTURE(Fixture, TestCommandRouter_UpdateMember)	
	{
		TD_PARTY_MEMBER tdPartyMember;
		vector<int> vecBuff;

		MakePartyMember(&tdPartyMember, &vecBuff, m_uidRequestPlayer, m_strRequestPlayerName, 3);
		m_pServerRouter->UpdateMember(m_uidParty, tdPartyMember, vecBuff);


		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_UPDATE_MEMBER);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), m_uidParty);
				
		TD_PARTY_MEMBER* pPartyMember;
		int nCount;
		ASSERT_CHECK(m_pNetAgent->GetSendCommand(0).GetBlob(pPartyMember, nCount, 1));
		CHECK_EQUAL(pPartyMember->m_uidPlayer, tdPartyMember.m_uidPlayer);
		CHECK_EQUAL(pPartyMember->nStatusFlag, tdPartyMember.nStatusFlag);
		CHECK_EQUAL(pPartyMember->nHP, tdPartyMember.nHP);
		CHECK_EQUAL(pPartyMember->nEN, tdPartyMember.nEN);
		CHECK_EQUAL(pPartyMember->nSTA, tdPartyMember.nSTA);
		CHECK_EQUAL(pPartyMember->nLevel, tdPartyMember.nLevel);
		CHECK(0 == wcscmp(pPartyMember->szName, tdPartyMember.szName));

		vector<int> vecSendBuff;
		ASSERT_CHECK(m_pNetAgent->GetSendCommand(0).GetBlob(vecSendBuff, 2));
		CHECK(vecSendBuff == vecBuff);
	}

	TEST_FIXTURE(Fixture, TestCommandRouter_AddQuest)
	{
		vector<int> vecQuestID;
		vecQuestID.push_back(100);
		vecQuestID.push_back(200);
		vecQuestID.push_back(300);

		m_pServerRouter->AddQuest(m_uidParty, m_uidRequestPlayer, vecQuestID);


		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_ADD_QUEST);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), m_uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), m_uidRequestPlayer);

		vector<int> vecSendQuestID;
		ASSERT_CHECK(m_pNetAgent->GetSendCommand(0).GetBlob(vecSendQuestID, 2));
		CHECK(vecSendQuestID == vecQuestID);
	}

	TEST_FIXTURE(Fixture, TestCommandRouter_AddField)		
	{
		// arrange
		MUID uidMember = SUnitTestUtil::NewID();
		MUID uidFieldGroup = SUnitTestUtil::NewUID();


		// act		
		m_pServerRouter->AddField(m_uidParty, uidMember, uidFieldGroup);


		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_ADD_FIELD);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), m_uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), uidMember); 
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 2), uidFieldGroup);
	}

	TEST_FIXTURE(Fixture, TestCommandRouter_UpdateQuestRunner)		
	{
		// arrange
		MUID uidField = SUnitTestUtil::NewUID();
		vector<int> vecQuestID;
		vecQuestID.push_back(100);
		vecQuestID.push_back(200);
		vecQuestID.push_back(300);


		// act		
		m_pServerRouter->UpdateQuestRunner(m_uidParty, m_uidRequestPlayer, uidField, vecQuestID);


		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_UPDATE_QUEST_RUNNER);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), m_uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), m_uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 2), uidField);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MBlob*>(0, 3)->GetBlobCount(), 3);
		CHECK_EQUAL(*(int*)m_pNetAgent->GetSendParam<MBlob*>(0, 3)->GetBlobElement(0), 100);
		CHECK_EQUAL(*(int*)m_pNetAgent->GetSendParam<MBlob*>(0, 3)->GetBlobElement(1), 200);
		CHECK_EQUAL(*(int*)m_pNetAgent->GetSendParam<MBlob*>(0, 3)->GetBlobElement(2), 300);
	}

	TEST_FIXTURE(Fixture, TestCommandRouter_LeaveReq)	
	{
		// act
		m_pServerRouter->LeaveReq(m_uidParty, m_uidRequestPlayer);


		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_LEAVE_REQ);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), m_uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), m_uidRequestPlayer);
	}

	TEST_FIXTURE(Fixture, TestCommandRouter_RemoveQuest)	
	{
		// arrange
		vector<int> vecQuestID;
		vecQuestID.push_back(100);
		vecQuestID.push_back(200);
		vecQuestID.push_back(300);


		// act
		m_pServerRouter->RemoveQuest(m_uidParty, m_uidRequestPlayer, vecQuestID);


		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_REMOVE_QUEST);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), m_uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), m_uidRequestPlayer);
		
		vector<int> vecSendQuestID;
		ASSERT_CHECK(m_pNetAgent->GetSendCommand(0).GetBlob(vecSendQuestID, 2));
		CHECK(vecSendQuestID == vecQuestID);
	}

	TEST_FIXTURE(Fixture, TestCommandRouter_RemoveField)	
	{
		// arrange
		MUID uidMember = SUnitTestUtil::NewUID();
		int nFieldGroupID = 100;


		// act
		m_pServerRouter->RemoveField(m_uidParty, uidMember, nFieldGroupID);


		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_REMOVE_FIELD);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), m_uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), uidMember);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 2), nFieldGroupID);
	}

	TEST_FIXTURE(Fixture, TestCommandRouter_EraseQuestRunner)	
	{
		// arrange
		MUID uidMember = SUnitTestUtil::NewUID();


		// act
		m_pServerRouter->EraseQuestRunner(m_uidParty, uidMember);


		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_ERASE_QUEST_RUNNER);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), m_uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), uidMember);
	}

	TEST_FIXTURE(Fixture, TestCommandRouter_KickReq)
	{
		// act
		m_pServerRouter->KickReq(m_uidParty, m_uidRequestPlayer, m_uidTargetPlayer);


		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_KICK_REQ);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), m_uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), m_uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 2), m_uidTargetPlayer);
	}

	TEST_FIXTURE(Fixture, TestCommandRouter_DoOffline)		
	{
		// act
		m_pServerRouter->DoOffline(m_uidParty, m_uidRequestPlayer);


		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_DO_OFFLINE);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), m_uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), m_uidRequestPlayer);
	}

	TEST_FIXTURE(Fixture, TestCommandRouter_DoOnline)		
	{
		// arrange
		MUID uidOffline = SUnitTestUtil::NewUID();


		// act
		m_pServerRouter->DoOnline(m_uidParty, m_uidRequestPlayer, uidOffline);


		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_DO_ONLINE);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), m_uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), m_uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 2), uidOffline);
	}

	TEST_FIXTURE(Fixture, TestCommandRouter_JoinInviteReq)	
	{
		// act
		m_pServerRouter->JoinInviteReq(m_uidParty, m_uidRequestPlayer);

		// assert
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_JOIN_INVITE_REQ);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), m_uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), m_uidRequestPlayer);
	}

	TEST_FIXTURE(Fixture, TestCommandRouter_JoinAcceptRes)	
	{
		// arrange
		MUID uidLeader = SUnitTestUtil::NewUID();
		CCommandResultTable nResult = CR_SUCCESS;


		// act
		m_pServerRouter->JoinAcceptRes(m_uidParty, uidLeader, m_uidRequestPlayer, nResult);


		// act
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_JOIN_ACCEPT_RES);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), m_uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), uidLeader);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 2), m_uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 3), nResult);
	}

	TEST_FIXTURE(Fixture, TestCommandRouter_MoveServer)
	{
		// act
		m_pServerRouter->MoveServer(m_uidParty, m_uidRequestPlayer);


		// act
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_MOVE_SERVER);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), m_uidParty);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), m_uidRequestPlayer);
	}	

	TEST_FIXTURE(Fixture, TestCommandRouter_PartyInfoAllReq)
	{
		// act
		m_pServerRouter->PartyInfoAllReq();

		// act
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_PARTY_INFO_ALL_REQ);
	}


	// Party Handler	//////////////////////////////////////////////////////////////////////////////////////
	
	TEST_FIXTURE(Fixture, TestCommandHandler_PartyInviteRes)	
	{	
		CCommandResultTable nResult = CR_SUCCESS;
		MCommandResult nCommandResult = RecvPartyInviteRes(m_uidRequestPlayer, m_uidTargetPlayer, nResult);

		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartySystem->m_uidRequestPlayer, m_uidRequestPlayer);
		CHECK_EQUAL(m_pPartySystem->m_uidTargetPlayer, m_uidTargetPlayer);
		CHECK_EQUAL(m_pPartySystem->m_nResult, nResult);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_PartyAcceptReq)	
	{
		MCommandResult nCommandResult = RecvPartyAcceptReq(m_uidTargetPlayer, m_uidRequestPlayer, m_strRequestPlayerName);

		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartySystem->m_uidTargetPlayer, m_uidTargetPlayer);
		CHECK_EQUAL(m_pPartySystem->m_uidRequestPlayer, m_uidRequestPlayer);
		CHECK(m_pPartySystem->m_strRequestPlayerName == m_strRequestPlayerName);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_PartyAcceptReq_RequestPlayerNameBoundCheck)
	{
		MCommandResult nCommandResult;
		
		wstring strRequestPlayerName;		
		for(int i = 0; i < PLAYER_NAME_LEN; i++)
		{
			strRequestPlayerName += L"A";
		}

		nCommandResult = RecvPartyAcceptReq(m_uidTargetPlayer, m_uidRequestPlayer, strRequestPlayerName.c_str());
		CHECK_EQUAL(nCommandResult, CR_TRUE);


		// overflow
		strRequestPlayerName += L"B";
		nCommandResult = RecvPartyAcceptReq(m_uidTargetPlayer, m_uidRequestPlayer, strRequestPlayerName.c_str());
		CHECK_EQUAL(nCommandResult, CR_ERROR);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_PartyAcceptCancel)	
	{
		CCommandResultTable nResult = CR_FAIL;
		MCommandResult nCommandResult = RecvPartyAcceptCancel(m_uidTargetPlayer, nResult);

		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartySystem->m_uidTargetPlayer, m_uidTargetPlayer);
		CHECK_EQUAL(m_pPartySystem->m_nResult, nResult);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_PartyAdd)
	{
		MCommandResult nCommandResult = RecvPartyAdd(m_uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestPlayerCID);

		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartySystem->m_uidParty, m_uidParty);
		CHECK_EQUAL(m_pPartySystem->m_partySetting.m_uidLeader, m_uidRequestPlayer);
		CHECK(m_pPartySystem->m_strLeaderName == m_strRequestPlayerName);

		const GProxyParty* pParty = m_pProxyPartyManager->Find(m_uidParty);
		ASSERT_CHECK(pParty != NULL);
		CHECK_EQUAL(pParty->GetLeader(), m_uidRequestPlayer);
		CHECK(pParty->GetName(m_uidRequestPlayer) == m_strRequestPlayerName);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_PartyAddMember)
	{
		// arrange
		m_pProxyPartyManager->AddParty(m_uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestPlayerCID);


		// act
		MCommandResult nCommandResult = RecvPartyAddMember(m_uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetPlayerCID);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartySystem->m_uidParty, m_uidParty);
		CHECK_EQUAL(m_pPartySystem->m_uidMember, m_uidTargetPlayer);
		CHECK(m_pPartySystem->m_strMemberName == m_strTargetPlayerName);

		const GProxyParty* pParty = m_pProxyPartyManager->Find(m_uidParty);
		ASSERT_CHECK(pParty != NULL);		
		CHECK(pParty->GetName(m_uidTargetPlayer) == m_strTargetPlayerName);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_PartySync)	
	{
		TD_PARTY tdParty;
		vector<TD_PARTY_MEMBER> vecPartyMember;
		vector<vector<int> > vecPartyBuff(MAX_PARTY_MEMBER_COUNT);

		for(int i = 0; i < 3; i++)
		{
			TD_PARTY_MEMBER tdPartyMember;
			MUID uidMember = SUnitTestUtil::NewUID();
			wchar_t szName[PLAYER_NAME_LEN + 1];
			swprintf_s(szName, L"TestPartyMember%d", i);

			MakePartyMember(&tdPartyMember, &vecPartyBuff[i], uidMember, szName, i);

			vecPartyMember.push_back(tdPartyMember);
		}

		tdParty.m_uidParty = m_uidParty;

		LOOTING_RULE_DATA lootingRuleData(LRC_FREE_FOR_ALL, LRR_FREE_FOR_ALL, LRRF_EPIC);
		PARTY_SETTING partySetting(vecPartyMember[0].m_uidPlayer, lootingRuleData, L"PartyName");

		tdParty.m_partySetting.Import(partySetting);

		MCommandResult nCommandResult = RecvPartySync(tdParty, vecPartyMember, vecPartyBuff);


		CHECK_EQUAL(nCommandResult, CR_TRUE);		
		CHECK(m_pPartySystem->m_partySetting == partySetting);
		CHECK_EQUAL(m_pPartySystem->m_uidParty, m_uidParty);

		ASSERT_CHECK(m_pPartySystem->m_pCommand != NULL);

		MCommand* pCommand = m_pPartySystem->m_pCommand;
		CHECK_EQUAL(pCommand->GetID(), MMC_PARTY_SYNC);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_MemberSync)	
	{
		TD_PARTY_MEMBER tdPartyMember;
		vector<int> vecBuff;
		MakePartyMember(&tdPartyMember, &vecBuff, m_uidRequestPlayer, m_strRequestPlayerName, 3);

		MCommandResult nCommandResult = RecvMemberSync(m_uidParty, tdPartyMember, vecBuff);


		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartySystem->m_uidParty, m_uidParty);
		ASSERT_CHECK(m_pPartySystem->m_pCommand != NULL);

		MCommand* pCommand = m_pPartySystem->m_pCommand;
		CHECK_EQUAL(pCommand->GetID(), MMC_PARTY_MEMBER_SYNC);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_QuestSync)	
	{
		TD_PARTY_QUEST tdPartyQuest;
		vector<TD_PARTY_QUEST> vecPartyQuests;

		tdPartyQuest.nQuestID = 100;
		tdPartyQuest.nQuestCount = 1;
		vecPartyQuests.push_back(tdPartyQuest);

		tdPartyQuest.nQuestID = 200;
		tdPartyQuest.nQuestCount = 3;
		vecPartyQuests.push_back(tdPartyQuest);

		tdPartyQuest.nQuestID = 300;
		tdPartyQuest.nQuestCount = 2;
		vecPartyQuests.push_back(tdPartyQuest);		

		MCommandResult nCommandResult = RecvQuestSync(m_uidParty, vecPartyQuests);


		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartySystem->m_uidParty, m_uidParty);
		CHECK_EQUAL(m_pPartySystem->m_mapQuests.size(), 3);		
		CHECK_EQUAL(m_pPartySystem->m_mapQuests[100], 1);
		CHECK_EQUAL(m_pPartySystem->m_mapQuests[200], 3);
		CHECK_EQUAL(m_pPartySystem->m_mapQuests[300], 2);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_FieldSync)		
	{
		// arrange
		TD_PARTY_FIELD tdPartyField;
		vector<TD_PARTY_FIELD> vecPartyFields;
		
		tdPartyField.nFieldGroupID = 100;
		tdPartyField.uidFieldGroup = MUID(1);
		vecPartyFields.push_back(tdPartyField);

		tdPartyField.nFieldGroupID = 200;
		tdPartyField.uidFieldGroup = MUID(2);
		vecPartyFields.push_back(tdPartyField);

		tdPartyField.nFieldGroupID = 300;
		tdPartyField.uidFieldGroup = MUID(3);
		vecPartyFields.push_back(tdPartyField);


		// act
		MCommandResult nCommandResult = RecvFieldSync(m_uidParty, vecPartyFields);

		
		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartySystem->m_uidParty, m_uidParty);
		CHECK_EQUAL(m_pPartySystem->m_mapFields.size(), 3);
		CHECK_EQUAL(m_pPartySystem->m_mapFields[100], MUID(1));
		CHECK_EQUAL(m_pPartySystem->m_mapFields[200], MUID(2));
		CHECK_EQUAL(m_pPartySystem->m_mapFields[300], MUID(3));
	}

	TEST_FIXTURE(Fixture, TestCommandHanlder_QuestRunnerSync)	
	{
		// arrange
		MUID uidField = SUnitTestUtil::NewUID();
		vector<int> vecQuestID;
		vecQuestID.push_back(100);
		vecQuestID.push_back(200);
		vecQuestID.push_back(300);


		// act
		MCommandResult nCommandResult = RecvQuestRunnerSync(m_uidParty, m_uidRequestPlayer, uidField, vecQuestID);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartySystem->m_uidParty, m_uidParty);
		CHECK_EQUAL(m_pPartySystem->m_uidMember, m_uidRequestPlayer);
		CHECK_EQUAL(m_pPartySystem->m_uidField, uidField);
		CHECK(m_pPartySystem->m_vecQuestID == vecQuestID);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_ChangeLeader)	
	{
		// arrange
		m_pProxyPartyManager->AddParty(m_uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestPlayerCID);
		m_pProxyPartyManager->AddMember(m_uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetPlayerCID);

		const GProxyParty* pParty = m_pProxyPartyManager->Find(m_uidParty);
		ASSERT_CHECK(pParty != NULL);
		CHECK_EQUAL(pParty->GetLeader(), m_uidRequestPlayer);
		CHECK(pParty->GetName(m_uidRequestPlayer) == m_strRequestPlayerName);


		// act
		MCommandResult nCommandResult = RecvChangeLeaderRes(m_uidParty, m_uidTargetPlayer);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartySystem->m_uidParty, m_uidParty);		
		CHECK_EQUAL(m_pPartySystem->m_uidTargetPlayer, m_uidTargetPlayer);		

		pParty = m_pProxyPartyManager->Find(m_uidParty);
		ASSERT_CHECK(pParty != NULL);
		CHECK_EQUAL(pParty->GetLeader(), m_uidTargetPlayer);
		CHECK(pParty->GetName(m_uidTargetPlayer) == m_strTargetPlayerName);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_LeaveRes)			
	{
		// arrange
		CCommandResultTable nResult = CR_SUCCESS;


		// act
		MCommandResult nCommandResult = RecvLeaveRes(m_uidRequestPlayer, nResult);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartySystem->m_uidRequestPlayer, m_uidRequestPlayer);
		CHECK_EQUAL(m_pPartySystem->m_nResult, nResult);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_RemoveMember)	
	{
		// arrange
		const GProxyParty* pParty = m_pProxyPartyManager->AddParty(m_uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestPlayerCID);
		m_pProxyPartyManager->AddMember(m_uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetPlayerCID);
		ASSERT_CHECK(pParty != NULL);
		CHECK(pParty->IsExistMember(m_uidTargetPlayer));


		// act
		MCommandResult nCommandResult = RecvRemoveMember(m_uidParty, m_uidTargetPlayer);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartySystem->m_uidParty, m_uidParty);
		CHECK_EQUAL(m_pPartySystem->m_uidMember, m_uidTargetPlayer);

		pParty = m_pProxyPartyManager->Find(m_uidParty);
		ASSERT_CHECK(pParty != NULL);
		CHECK(!pParty->IsExistMember(m_uidTargetPlayer));
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_EraseQuestRunnerSync)	
	{
		// arrange
		MUID uidMember = SUnitTestUtil::NewUID();
		
	
		// act
		MCommandResult nCommandResult = RecvEraseQuestRunnerSync(m_uidParty, uidMember);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartySystem->m_uidParty, m_uidParty);
		CHECK_EQUAL(m_pPartySystem->m_uidMember, uidMember);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_AddOfflineMember)		
	{
		// act
		MCommandResult nCommandResult = RecvAddOfflineMember(m_uidParty, m_uidRequestPlayer);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartySystem->m_uidParty, m_uidParty);
		CHECK_EQUAL(m_pPartySystem->m_uidMember, m_uidRequestPlayer);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_RemoveOfflineMember)	
	{
		// arrange
		MUID uidOffline = SUnitTestUtil::NewUID();
		
		const GProxyParty* pParty = m_pProxyPartyManager->AddParty(m_uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestPlayerCID);
		m_pProxyPartyManager->AddMember(m_uidParty, uidOffline, m_strTargetPlayerName, m_nTargetPlayerCID);
		ASSERT_CHECK(pParty != NULL);
		CHECK(pParty->IsExistMember(uidOffline));


		// act
		MCommandResult nCommandResult = RecvRemoveOfflineMember(m_uidParty, m_uidTargetPlayer, uidOffline);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartySystem->m_uidParty, m_uidParty);
		CHECK_EQUAL(m_pPartySystem->m_uidMember, m_uidTargetPlayer);
		CHECK_EQUAL(m_pPartySystem->m_uidOffline, uidOffline);

		pParty = m_pProxyPartyManager->Find(m_uidParty);
		ASSERT_CHECK(pParty != NULL);
		CHECK(!pParty->IsExistMember(uidOffline));
		CHECK(pParty->IsExistMember(m_uidTargetPlayer));
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_JoinAcceptReq)	
	{
		// arrange
		MUID uidLeader = SUnitTestUtil::NewUID();


		// act
		MCommandResult nCommandResult = RecvJoinAcceptReq(m_uidParty, uidLeader, m_uidRequestPlayer, m_strRequestPlayerName);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartySystem->m_uidParty, m_uidParty);
		CHECK_EQUAL(m_pPartySystem->m_partySetting.m_uidLeader, uidLeader);
		CHECK_EQUAL(m_pPartySystem->m_uidRequestPlayer, m_uidRequestPlayer);
		CHECK(m_pPartySystem->m_strRequestPlayerName == m_strRequestPlayerName);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_JoinInviteRes)	
	{
		// arrange
		CCommandResultTable nResult = CR_SUCCESS;

		// act
		MCommandResult nCommandResult = RecvJoinInviteRes(m_uidRequestPlayer, nResult);

		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartySystem->m_uidRequestPlayer, m_uidRequestPlayer);
		CHECK_EQUAL(m_pPartySystem->m_nResult, nResult);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_AcceptCancel)	
	{
		// arrange
		MUID uidLeader = SUnitTestUtil::NewUID();
		CCommandResultTable nResult = CR_SUCCESS;


		// act
		MCommandResult nCommandResult = RecvJoinAcceptCancel(uidLeader, nResult);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartySystem->m_partySetting.m_uidLeader, uidLeader);
		CHECK_EQUAL(m_pPartySystem->m_nResult, nResult);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_MoveServerSync)
	{
		// arrange		
		MUID uidOffline = SUnitTestUtil::NewUID();

		const GProxyParty* pParty = m_pProxyPartyManager->AddParty(m_uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestPlayerCID);
		m_pProxyPartyManager->AddMember(m_uidParty, uidOffline, m_strTargetPlayerName, m_nTargetPlayerCID);
		ASSERT_CHECK(pParty != NULL);
		CHECK(pParty->IsExistMember(uidOffline));


		// act
		MCommandResult nCommandResult = RecvMoverServerSync(m_uidParty, m_uidTargetPlayer, uidOffline);

		
		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);
		CHECK_EQUAL(m_pPartySystem->m_uidParty, m_uidParty);
		CHECK_EQUAL(m_pPartySystem->m_uidMember, m_uidTargetPlayer);
		CHECK_EQUAL(m_pPartySystem->m_uidOffline, uidOffline);

		pParty = m_pProxyPartyManager->Find(m_uidParty);
		ASSERT_CHECK(pParty != NULL);
		CHECK(!pParty->IsExistMember(uidOffline));
		CHECK(pParty->IsExistMember(m_uidTargetPlayer));
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_PartyInfoAllRes)
	{
		// arrange
		vector<TD_PARTY_INFO> vecPartyInfo;
		MakePartyInfo(vecPartyInfo);


		// act
		MCommandResult nCommandResult = RecvPartyInfoAllRes(vecPartyInfo);


		// assert
		CHECK_EQUAL(nCommandResult, CR_TRUE);

		GProxyPartyManager* pProxyPartyManager = gsys.pMasterServerFacade->GetProxyPartyManger();
		ASSERT_CHECK(pProxyPartyManager != NULL);
		CHECK_EQUAL(pProxyPartyManager->GetCount(), vecPartyInfo.size());

		for each(TD_PARTY_INFO tdPartyInfo in vecPartyInfo)
		{
			const GProxyParty* pProxyParty = pProxyPartyManager->Find(tdPartyInfo.uidParty);
			ASSERT_CHECK(pProxyParty != NULL);
			CHECK_EQUAL(pProxyParty->GetUID(), tdPartyInfo.uidParty);
			CHECK_EQUAL(pProxyParty->GetCount(), tdPartyInfo.nMemberCount);			

			for(int i = 0; i < tdPartyInfo.nMemberCount; i++)
			{
				MUID uidMember = tdPartyInfo.members[i].uidMember;
				wstring strMemberName = tdPartyInfo.members[i].szMemberName;
				int nMemberCID = tdPartyInfo.members[i].nMemberCID;
				CHECK(pProxyParty->IsExistMember(uidMember));
				CHECK_EQUAL(pProxyParty->Find(strMemberName), uidMember);
				CHECK_EQUAL(pProxyParty->Find(nMemberCID), uidMember);

				if (i == tdPartyInfo.nLeaderIndex)
				{
					CHECK_EQUAL(pProxyParty->GetLeader(), uidMember);
				}
			}
		}
	}
};

