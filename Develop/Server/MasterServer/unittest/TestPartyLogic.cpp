#include "stdafx.h"
#include "ZFixtureHelper.h"
#include "ZParty.h"
#include "ZPartyManager.h"
#include "ZPartyLogic.h"
#include "STransData_M2G.h"
#include "CCommandTable_Master.h"
#include "ZPartyCommandRouter.h"
#include "ZPartyMember.h"
#include "CTransData.h"
#include "ZDynamicFieldMaster.h"
#include "SCommandTable.h"
#include "ZGameServerObject.h"
#include "ZFixedPartyInfoManager.h"
#include "ZConfig.h"

SUITE(PartyLogic)
{
	struct MockFixedPartyInfoManager: public ZFixedPartyInfoManager
	{
		MOCK_METHOD1(FindFixedPartyID, int(wstring strName));
	};

	struct Fixture : FBaseServerField, FBaseMockLink, FBasePlayer
	{
		ZTestMgrWrapper<ZPartyManager>	m_PartyManagerWrapper;
		ZTestMgrWrapper2<ZFixedPartyInfoManager, MockFixedPartyInfoManager> m_FixedPartyInfoManagerWrapper;
		MockFixedPartyInfoManager* m_pFixedPartyInfoManager;

		MUID m_uidRequest;
		MUID m_uidRequestPlayer;	
		ZPlayer* m_pRequestPlayer;
		wstring m_strRequestPlayerName;
		MockLink* m_pRequestLink;
		
		MUID m_uidTarget;
		MUID m_uidTargetPlayer;		
		wstring m_strTargetPlayerName;
		ZPlayer* m_pTargetPlayer;
		MockLink* m_pTargetLink;

		int m_nRequestServerID;
		int m_nTargetServerID;

		Fixture()
		{
			m_pFixedPartyInfoManager = m_FixedPartyInfoManagerWrapper.Get();

			m_nRequestServerID = 2;
			m_nTargetServerID = 3;

			m_uidRequest = AddGameServerObject(m_nRequestServerID, 3)->GetUID();
			m_pRequestLink = NewLink(m_uidRequest);

			m_uidTarget = AddGameServerObject(m_nTargetServerID, 107)->GetUID();
			m_pTargetLink = NewLink(m_uidTarget);

			
			m_pRequestPlayer = AddNewPlayerInWorld(m_nRequestServerID);
			m_uidRequestPlayer = m_pRequestPlayer->GetUID();
			m_strRequestPlayerName = m_pRequestPlayer->GetPlayerName();

			m_pTargetPlayer = AddNewPlayerInWorld(m_nTargetServerID);
			m_uidTargetPlayer = m_pTargetPlayer->GetUID();
			m_strTargetPlayerName = m_pTargetPlayer->GetPlayerName();

			ZConfig::m_nServerMode = SERVER_MODE_NORMAL;		
		}

		~Fixture()
		{
		}
	};


	TEST_FIXTURE(Fixture, TestInviteReq)	
	{
		gsys.pPartyLogic->InviteReq(m_uidRequest, m_uidTargetPlayer, m_uidRequestPlayer);
		
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_ACCEPT_REQ);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), m_uidTargetPlayer);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 1), m_uidRequestPlayer);

		wstring strRequestPlayerName;
		MCommand& cmd = m_pTargetLink->GetCommand(0);
		cmd.GetParameter(strRequestPlayerName, 2, MPT_WSTR);
		CHECK(strRequestPlayerName == m_strRequestPlayerName);
	}

	TEST_FIXTURE(Fixture, TestInviteReq_UnknownTargetPlayerFailure)	
	{
		MUID uidUnknownTagetPlayer = ZTestHelper::NewUID();

		gsys.pPartyLogic->InviteReq(m_uidRequest, uidUnknownTagetPlayer, m_uidRequestPlayer);

		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), uidUnknownTagetPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 2), CR_FAIL_SYSTEM_INVALID_PC_UID);
	}

	TEST_FIXTURE(Fixture, TestInviteReq_NoTargetServerFailure)	
	{
		int nTestServerID = 4;
		MUID uidNoServerPlayer = ZTestHelper::NewUID();
		//auto_ptr<ZPlayer> pNoServerPlayer(gmgr.pPlayerManager->NewPlayer(3, 4, uidNoServerPlayer));
		//pNoServerPlayer->Init(L"TestNoServerPlayerName");

		auto_ptr<ZPlayer> pNoServerPlayer(gmgr.pPlayerManager->NewPlayer(ZTestHelper::NewUID(), ZTestHelper::NewID(), ZTestHelper::NewName()));
		pNoServerPlayer->SetWorldInfo(ZTestHelper::NewID(), nTestServerID, uidNoServerPlayer, L"TestNoServerPlayerName", ZTestHelper::NewID());

		gsys.pPartyLogic->InviteReq(m_uidRequest, uidNoServerPlayer, m_uidRequestPlayer);

		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), uidNoServerPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 2), CR_FAIL_SYSTEM_INVALID_PC_UID);
	}

	TEST_FIXTURE(Fixture, TestInviteReq_AleadyHasPartyFailure)	
	{
		ZParty* pParty = gmgr.pPartyManager->AddParty(m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());
		m_pTargetPlayer->SetPartyUID(pParty->GetUID());

		gsys.pPartyLogic->InviteReq(m_uidRequest, m_uidTargetPlayer, m_uidRequestPlayer);


		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), m_uidTargetPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 2), CR_FAIL_PARTY_TARGET_ALEADY_HAS_PARTY);
	}

	TEST_FIXTURE(Fixture, TestInviteReq_UnknownRequestPlayerFailure)	
	{
		MUID uidUnknownRequestPlayer = ZTestHelper::NewUID();

		gsys.pPartyLogic->InviteReq(m_uidRequest, m_uidTargetPlayer, uidUnknownRequestPlayer);


		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), uidUnknownRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), m_uidTargetPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 2), CR_FAIL_SYSTEM_INVALID_PC_UID);	
	}

	TEST_FIXTURE(Fixture, TestInviteReq_NotLeaderFailure)	
	{
		MUID uidLeader = ZTestHelper::NewUID();
		wstring strLeaderName = L"TestLeaderName";
		int nLeaderCID = ZTestHelper::NewCID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidLeader, strLeaderName, m_nRequestServerID, nLeaderCID);
		gmgr.pPartyManager->AddMember(pParty->GetUID(), m_uidRequestPlayer, m_strRequestPlayerName, m_nTargetServerID, m_pRequestPlayer->GetCID());
		m_pRequestPlayer->SetPartyUID(pParty->GetUID());

		gsys.pPartyLogic->InviteReq(m_uidRequest, m_uidTargetPlayer, m_uidRequestPlayer);


		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), m_uidTargetPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 2), CR_FAIL_PARTY_NOT_PARTY_LEADER);
	}

	TEST_FIXTURE(Fixture, TestInviteReq_FullPartyFailure)	
	{
		ZParty* pParty = gmgr.pPartyManager->AddParty(m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		m_pRequestPlayer->SetPartyUID(pParty->GetUID());

		for(int i = 1; i < MAX_PARTY_MEMBER_COUNT; i++)
		{
			gmgr.pPartyManager->AddMember(pParty->GetUID(), ZTestHelper::NewUID(), L"TestMemberName", m_nTargetServerID, ZTestHelper::NewCID());
		}

		gsys.pPartyLogic->InviteReq(m_uidRequest, m_uidTargetPlayer, m_uidRequestPlayer);


		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), m_uidTargetPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 2), CR_FAIL_PARTY_FULL_PARTY);
	}

	TEST_FIXTURE(Fixture, TestAcceptRes_ExistParty)	
	{
		CCommandResultTable nResult = CR_SUCCESS;
		ZParty* pParty = gmgr.pPartyManager->AddParty(m_uidRequestPlayer, m_pRequestPlayer->GetPlayerName(), m_nRequestServerID, m_pRequestPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);
		m_pRequestPlayer->SetPartyUID(pParty->GetUID());	
		
		gsys.pPartyLogic->AcceptRes(m_uidTarget, m_uidRequestPlayer, m_uidTargetPlayer, nResult);

		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MSC_PROXY_PLAYER_UPDATE);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MSC_PROXY_PLAYER_UPDATE);

		ASSERT_EQUAL(m_pRequestLink->GetCommandID(1), MMC_PARTY_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(1, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(1, 1), m_uidTargetPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(1, 2), nResult);
		
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(2), MMC_PARTY_ADD_MEMBER);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(1), MMC_PARTY_ADD_MEMBER);
	}

	TEST_FIXTURE(Fixture, TestAcceptRes)	
	{
		CCommandResultTable nResult = CR_SUCCESS;
		
		gsys.pPartyLogic->AcceptRes(m_uidTarget, m_uidRequestPlayer, m_uidTargetPlayer, nResult);

		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MSC_PROXY_PLAYER_UPDATE);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(1), MSC_PROXY_PLAYER_UPDATE);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MSC_PROXY_PLAYER_UPDATE);		
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(1), MSC_PROXY_PLAYER_UPDATE);

		ASSERT_EQUAL(m_pRequestLink->GetCommandID(2), MMC_PARTY_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(2, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(2, 1), m_uidTargetPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(2, 2), nResult);

		ASSERT_EQUAL(m_pRequestLink->GetCommandID(3), MMC_PARTY_ADD);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(4), MMC_PARTY_ADD_MEMBER);

		ASSERT_EQUAL(m_pTargetLink->GetCommandID(2), MMC_PARTY_ADD);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(3), MMC_PARTY_ADD_MEMBER);
	}

	TEST_FIXTURE(Fixture, TestAcceptRes_UnknownRequestPlayerFailure)	
	{		
		MUID uidUnknownRequestPlayer = ZTestHelper::NewUID();
		CCommandResultTable nResult = CR_SUCCESS;		
	
		gsys.pPartyLogic->AcceptRes(m_uidTarget, uidUnknownRequestPlayer, m_uidTargetPlayer, nResult);

		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), m_uidTargetPlayer);
		CHECK_EQUAL(m_pTargetLink->GetParam<int>(0, 1), CR_FAIL_SYSTEM_INVALID_PC_UID);
	}

	TEST_FIXTURE(Fixture, TestAcceptRes_NoRequestServerFailure)	
	{
		MUID uidNoServerPlayer = ZTestHelper::NewUID();
		auto_ptr<ZPlayer> pNoServerPlayer(gmgr.pPlayerManager->NewPlayer(ZTestHelper::NewUID(), ZTestHelper::NewID(), ZTestHelper::NewName()));
		pNoServerPlayer->SetWorldInfo(ZTestHelper::NewID(), ZTestHelper::NewID(), uidNoServerPlayer, L"TestNoServerPlayerName", ZTestHelper::NewID());

		MUID uidUnknownRequestPlayer = ZTestHelper::NewUID();
		CCommandResultTable nResult = CR_SUCCESS;		
		
		gsys.pPartyLogic->AcceptRes(m_uidTarget, uidNoServerPlayer, m_uidTargetPlayer, nResult);

		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), m_uidTargetPlayer);
		CHECK_EQUAL(m_pTargetLink->GetParam<int>(0, 1), CR_FAIL_SYSTEM_INVALID_PC_UID);
	}

	TEST_FIXTURE(Fixture, TestAcceptRes_NotLeaderFailure)
	{
		MUID uidLeader = ZTestHelper::NewUID();
		wstring strLeaderName = L"TestLeaderName";
		int nLeaderCID = ZTestHelper::NewCID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidLeader, strLeaderName, m_nTargetServerID, nLeaderCID);
		gmgr.pPartyManager->AddMember(pParty->GetUID(), m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		m_pRequestPlayer->SetPartyUID(pParty->GetUID());

		CCommandResultTable nResult = CR_SUCCESS;
		gsys.pPartyLogic->AcceptRes(m_uidTarget, m_uidRequestPlayer, m_uidTargetPlayer, nResult);

		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), m_uidTargetPlayer);
		CHECK_EQUAL(m_pTargetLink->GetParam<int>(0, 1), CR_FAIL_PARTY_NOT_PARTY_LEADER);

		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), m_uidTargetPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 2), CR_FAIL_PARTY_NOT_PARTY_LEADER);
	}

	TEST_FIXTURE(Fixture, TestAcceptRes_FullPartyFailure)
	{		
		ZParty* pParty = gmgr.pPartyManager->AddParty(m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		m_pRequestPlayer->SetPartyUID(pParty->GetUID());

		for(int i = 1; i < MAX_PARTY_MEMBER_COUNT; i++)
		{
			gmgr.pPartyManager->AddMember(pParty->GetUID(), ZTestHelper::NewUID(), L"TestMemberName", m_nTargetServerID, ZTestHelper::NewCID());
		}

		CCommandResultTable nResult = CR_SUCCESS;
		gsys.pPartyLogic->AcceptRes(m_uidTarget, m_uidRequestPlayer, m_uidTargetPlayer, nResult);

		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), m_uidTargetPlayer);
		CHECK_EQUAL(m_pTargetLink->GetParam<int>(0, 1), CR_FAIL_PARTY_FULL_PARTY);

		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), m_uidTargetPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 2), CR_FAIL_PARTY_FULL_PARTY);
	}

	TEST_FIXTURE(Fixture, TestAcceptRes_UnknownTargetPlayerFailure)
	{
		MUID uidUnknwonTargetPlayer = ZTestHelper::NewUID();

		CCommandResultTable nResult = CR_SUCCESS;
		gsys.pPartyLogic->AcceptRes(m_uidTarget, m_uidRequestPlayer, uidUnknwonTargetPlayer, nResult);

		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), uidUnknwonTargetPlayer);
		CHECK_EQUAL(m_pTargetLink->GetParam<int>(0, 1), CR_FAIL_SYSTEM_INVALID_PC_UID);

		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), uidUnknwonTargetPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 2), CR_FAIL_SYSTEM_INVALID_PC_UID);
	}

	TEST_FIXTURE(Fixture, TestAcceptRes_AlreadyHadPartyFailure)	
	{
		ZParty* pParty = gmgr.pPartyManager->AddParty(m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());
		m_pTargetPlayer->SetPartyUID(pParty->GetUID());

		CCommandResultTable nResult = CR_SUCCESS;
		gsys.pPartyLogic->AcceptRes(m_uidTarget, m_uidRequestPlayer, m_uidTargetPlayer, nResult);


		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), m_uidTargetPlayer);
		CHECK_EQUAL(m_pTargetLink->GetParam<int>(0, 1), CR_FAIL_PARTY_ALEADY_HAS_PARTY);

		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), m_uidTargetPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 2), CR_FAIL_PARTY_TARGET_ALEADY_HAS_PARTY);
	}
	
	TEST_FIXTURE(Fixture, TestAcceptRes_Refuse)	
	{
		CCommandResultTable nResult = CR_FAIL_PARTY_REFUSE_INVITATION;
		gsys.pPartyLogic->AcceptRes(m_uidTarget, m_uidRequestPlayer, m_uidTargetPlayer, nResult);


		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), m_uidTargetPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 2), nResult);
	}


	TEST_FIXTURE(Fixture, TestUpdateParty)
	{
		ZPartyMember partyLeader(m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());

		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		gmgr.pPartyManager->AddMember(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());
		
		gsys.pPartyLogic->UpdateParty(uidParty, &partyLeader);

		ASSERT_EQUAL(m_pRequestLink->GetCommandCount(), 0);
		ASSERT_EQUAL(m_pTargetLink->GetCommandCount(), 0);

		ZPartyMember partyMember(m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());
		gsys.pPartyLogic->UpdateParty(uidParty, &partyMember);

		
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

	TEST_FIXTURE(Fixture, TestUpdateParty_UnknownPartyFailure)	
	{
		MUID uidUnknownParty = ZTestHelper::NewUID();
		ZPartyMember partyMember(m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());

		gsys.pPartyLogic->UpdateParty(uidUnknownParty, &partyMember);

		
		ASSERT_EQUAL(m_pRequestLink->GetCommandCount(), 0);
		ASSERT_EQUAL(m_pTargetLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestUpdateParty_UnknownMemberFailure)		
	{
		MUID uidUnknownMember = ZTestHelper::NewUID();
		wstring strUnknownMember = L"TestUnknownMemberName";
		int nUnknownMemberCID = ZTestHelper::NewCID();
		ZPartyMember partyMember(uidUnknownMember, strUnknownMember, 3, nUnknownMemberCID);

		MUID uidParty = ZTestHelper::NewUID();
		gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());

		gsys.pPartyLogic->UpdateParty(uidParty, &partyMember);

		
		ASSERT_EQUAL(m_pRequestLink->GetCommandCount(), 0);
		ASSERT_EQUAL(m_pTargetLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestUpdateMember)	
	{
		MUID uidParty = ZTestHelper::NewUID();
		gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		gmgr.pPartyManager->AddMember(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());

		TD_PARTY_MEMBER tdPartyMember;
		tdPartyMember.m_uidPlayer = m_uidRequestPlayer;
		wcsncpy_s(tdPartyMember.szName, m_strRequestPlayerName.c_str(), _TRUNCATE);
		vector<int> vecBuff;
		vecBuff.push_back(100);
		vecBuff.push_back(200);
		vecBuff.push_back(300);

		ZPartyMember partyMember;
		partyMember.Assign(tdPartyMember, vecBuff);

		gsys.pPartyLogic->UpdateMember(uidParty, &partyMember);

		
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
	
	TEST_FIXTURE(Fixture, TestUpdateMember_UnknownPartyFailure)	
	{
		MUID uidUnknownParty = ZTestHelper::NewUID();
		ZPartyMember partyMember(m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());

		gsys.pPartyLogic->UpdateMember(uidUnknownParty, &partyMember);

		
		ASSERT_EQUAL(m_pRequestLink->GetCommandCount(), 0);
		ASSERT_EQUAL(m_pTargetLink->GetCommandCount(), 0);
	}
	
	TEST_FIXTURE(Fixture, TestUpdateMember_UnknownMemberFailure)	
	{
		MUID uidUnknownMember = ZTestHelper::NewUID();
		wstring strUnknownMember = L"TestUnknownMemberName";
		int nUnkownMemberCID = ZTestHelper::NewCID();
		ZPartyMember partyMember(uidUnknownMember, strUnknownMember, 3, nUnkownMemberCID);

		MUID uidParty = ZTestHelper::NewUID();
		gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());

		gsys.pPartyLogic->UpdateMember(uidParty, &partyMember);

		
		ASSERT_EQUAL(m_pRequestLink->GetCommandCount(), 0);
		ASSERT_EQUAL(m_pTargetLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestAddQuest)		
	{
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);
		gmgr.pPartyManager->AddMember(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());

		vector<int> vecQuestID;
		vecQuestID.push_back(100);
		vecQuestID.push_back(200);
		vecQuestID.push_back(300);

		gsys.pPartyLogic->AddQuest(uidParty, m_uidRequestPlayer, vecQuestID);

		ZPartyMember* pPartyMember = pParty->GetMember(m_uidRequestPlayer);
		ASSERT_CHECK(pPartyMember != NULL);

		CHECK(pPartyMember->IsExistQuest(vecQuestID[0]));
		CHECK(pPartyMember->IsExistQuest(vecQuestID[1]));
		CHECK(pPartyMember->IsExistQuest(vecQuestID[2]));

		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_QUEST_SYNC);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_QUEST_SYNC);

		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), uidParty);
		ASSERT_EQUAL(m_pRequestLink->GetParam<MBlob*>(0, 1)->GetBlobCount(), 3);

		TD_PARTY_QUEST* pPartyQuest = (TD_PARTY_QUEST*)m_pRequestLink->GetParam<MBlob*>(0, 1)->GetBlobElement(0);
		CHECK_EQUAL(pPartyQuest->nQuestID, 100);
		CHECK_EQUAL(pPartyQuest->nQuestCount, 1);

		pPartyQuest = (TD_PARTY_QUEST*)m_pRequestLink->GetParam<MBlob*>(0, 1)->GetBlobElement(1);
		CHECK_EQUAL(pPartyQuest->nQuestID, 200);
		CHECK_EQUAL(pPartyQuest->nQuestCount, 1);

		pPartyQuest = (TD_PARTY_QUEST*)m_pRequestLink->GetParam<MBlob*>(0, 1)->GetBlobElement(2);
		CHECK_EQUAL(pPartyQuest->nQuestID, 300);
		CHECK_EQUAL(pPartyQuest->nQuestCount, 1);
	}
	
	TEST_FIXTURE(Fixture, TestAddQuest_UnknownPartyFailure)		
	{
		MUID uidUnknownParty = ZTestHelper::NewUID();
		vector<int> vecQuestID;
		vecQuestID.push_back(100);
		vecQuestID.push_back(200);
		vecQuestID.push_back(300);


		gsys.pPartyLogic->AddQuest(uidUnknownParty, m_uidRequestPlayer, vecQuestID);


		ASSERT_EQUAL(m_pRequestLink->GetCommandCount(), 0);
		ASSERT_EQUAL(m_pTargetLink->GetCommandCount(), 0);		
	}

	TEST_FIXTURE(Fixture, TestAddField)		
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);
		gmgr.pPartyManager->AddMember(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());

		int nFieldGroupID = 100;
		FIELD_GROUP_INFO* pFieldGroupInfo = new FIELD_GROUP_INFO();
		pFieldGroupInfo->nID = nFieldGroupID;
		gmgr.pFieldInfoManager->InsertFieldGroup(pFieldGroupInfo);

		ZDynamicFieldMaster* pFieldGroup = gmgr.pFieldManager->CreateDynamicFieldMaster(nFieldGroupID, 1, DFT_TRIAL, m_uidRequestPlayer);
		ASSERT_CHECK(pFieldGroup != NULL);
		MUID uidFieldGroup = pFieldGroup->GetUID();

		
		// act
		gsys.pPartyLogic->AddField(uidParty, m_uidRequestPlayer, uidFieldGroup);


		// assert
		CHECK_EQUAL(pParty->GetFieldGroupUID(nFieldGroupID), uidFieldGroup);

		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_FIELD_SYNC);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_FIELD_SYNC);

		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), uidParty);
	
		MCommand& cmd = m_pRequestLink->GetCommand(0);
		vector<TD_PARTY_FIELD> vecPartyField;
		ASSERT_CHECK(cmd.GetBlob(vecPartyField, 1));
		ASSERT_EQUAL(vecPartyField.size(), 1);
		
		for each(TD_PARTY_FIELD tdPartyField in vecPartyField)
		{
			int nFieldGroupID = tdPartyField.nFieldGroupID;
			MUID uidFieldGroup = tdPartyField.uidFieldGroup;

			CHECK_EQUAL(pParty->GetFieldGroupUID(nFieldGroupID), uidFieldGroup);
		}
	}

	TEST_FIXTURE(Fixture, TestAddField_UnknownPartyFailure)		
	{
		// arrange
		MUID uidUnknownParty = ZTestHelper::NewUID();
		MUID uidFieldGroup = ZTestHelper::NewUID();


		// act
		gsys.pPartyLogic->AddField(uidUnknownParty, m_uidRequestPlayer, uidFieldGroup);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandCount(), 0);
		ASSERT_EQUAL(m_pTargetLink->GetCommandCount(), 0);		
	}

	TEST_FIXTURE(Fixture, TestUpdateQuestRunner)		
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);
		gmgr.pPartyManager->AddMember(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());

		MUID uidField = ZTestHelper::NewUID();
		vector<int> vecQuestID;
		vecQuestID.push_back(100);
		vecQuestID.push_back(200);
		vecQuestID.push_back(300);


		// act
		gsys.pPartyLogic->UpdateQuestRunner(uidParty, m_uidRequestPlayer, uidField, vecQuestID);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_QUEST_RUNNER_SYNC);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_QUEST_RUNNER_SYNC);

		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 2), uidField);
		CHECK_EQUAL(m_pRequestLink->GetParam<MBlob*>(0, 3)->GetBlobCount(), 3);
		CHECK_EQUAL(*(int*)m_pRequestLink->GetParam<MBlob*>(0, 3)->GetBlobElement(0), 100);
		CHECK_EQUAL(*(int*)m_pRequestLink->GetParam<MBlob*>(0, 3)->GetBlobElement(1), 200);
		CHECK_EQUAL(*(int*)m_pRequestLink->GetParam<MBlob*>(0, 3)->GetBlobElement(2), 300);
	}

	TEST_FIXTURE(Fixture, TestUpdateQuestRunner_UnknownPartyFailure)		
	{
		// arrange
		MUID uidUnknownParty = ZTestHelper::NewUID();
		MUID uidField = ZTestHelper::NewUID();
		vector<int> vecQuestID;
		vecQuestID.push_back(100);
		vecQuestID.push_back(200);
		vecQuestID.push_back(300);


		// act
		gsys.pPartyLogic->UpdateQuestRunner(uidUnknownParty, m_uidRequestPlayer, uidField, vecQuestID);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandCount(), 0);
		ASSERT_EQUAL(m_pTargetLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestUpdateQuestRunner_UnknownMememberFailure)		
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);
		gmgr.pPartyManager->AddMember(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());

		MUID uidUnknownMember = ZTestHelper::NewUID();
		MUID uidField = ZTestHelper::NewUID();
		vector<int> vecQuestID;
		vecQuestID.push_back(100);
		vecQuestID.push_back(200);
		vecQuestID.push_back(300);


		// act
		gsys.pPartyLogic->UpdateQuestRunner(uidParty, uidUnknownMember, uidField, vecQuestID);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandCount(), 0);
		ASSERT_EQUAL(m_pTargetLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestLeaveReq_NormalMember)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);
		gmgr.pPartyManager->AddMember(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());

		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		gmgr.pPartyManager->AddMember(uidParty, uidMember, strMemberName, 3, nMemberCID);

		m_pTargetPlayer->SetPartyUID(uidParty);

		CHECK_EQUAL(pParty->GetCount(), 3);
		CHECK(pParty->IsExistMember(m_uidTargetPlayer));
		CHECK_EQUAL(m_pTargetPlayer->GetPartyUID(), uidParty);


		// act
		gsys.pPartyLogic->LeaveReq(m_uidTarget, uidParty, m_uidTargetPlayer);


		// assert
		pParty = gmgr.pPartyManager->Find(uidParty);
		ASSERT_CHECK(pParty != NULL);
		CHECK_EQUAL(pParty->GetCount(), 2);
		CHECK(!pParty->IsExistMember(m_uidTargetPlayer));
		CHECK_EQUAL(m_pTargetPlayer->GetPartyUID(), MUID::Invalid());

		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MSC_PROXY_PLAYER_UPDATE);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MSC_PROXY_PLAYER_UPDATE);

		ASSERT_EQUAL(m_pTargetLink->GetCommandID(1), MMC_PARTY_LEAVE_RES);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(2), MMC_PARTY_REMOVE_MEMBER);		
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(3), MMC_PARTY_ERASE_QUEST_RUNNER_SYNC);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(4), MMC_PARTY_FIELD_SYNC);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(5), MMC_PARTY_QUEST_SYNC);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(6), MMC_PARTY_SYNC);

		ASSERT_EQUAL(m_pRequestLink->GetCommandID(1), MMC_PARTY_REMOVE_MEMBER);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(2), MMC_PARTY_ERASE_QUEST_RUNNER_SYNC);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(3), MMC_PARTY_FIELD_SYNC);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(4), MMC_PARTY_QUEST_SYNC);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(5), MMC_PARTY_SYNC);

		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(1, 0), m_uidTargetPlayer);
		CHECK_EQUAL(m_pTargetLink->GetParam<int>(1, 1), CR_SUCCESS);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(2, 0), uidParty);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(2, 1), m_uidTargetPlayer);
	}

	TEST_FIXTURE(Fixture, TestLeaveReq_AloneMember)
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);		

		m_pRequestPlayer->SetPartyUID(uidParty);

		CHECK_EQUAL(pParty->GetCount(), 1);
		CHECK(pParty->IsExistMember(m_uidRequestPlayer));
		CHECK_EQUAL(pParty->GetLeader(), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestPlayer->GetPartyUID(), uidParty);


		// act
		gsys.pPartyLogic->LeaveReq(m_uidRequest, uidParty, m_uidRequestPlayer);


		// assert
		pParty = gmgr.pPartyManager->Find(uidParty);
		ASSERT_CHECK(pParty == NULL);
		CHECK_EQUAL(m_pRequestPlayer->GetPartyUID(), MUID::Invalid());

		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MSC_PROXY_PLAYER_UPDATE);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MSC_PROXY_PLAYER_UPDATE);

		ASSERT_EQUAL(m_pRequestLink->GetCommandID(1), MMC_PARTY_LEAVE_RES);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(2), MMC_PARTY_REMOVE_MEMBER);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(3), MMC_PARTY_REMOVE);

		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(1, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(1, 1), CR_SUCCESS);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(2, 0), uidParty);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(2, 1), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(3, 0), uidParty);
	}

	TEST_FIXTURE(Fixture, TestLeaveReq_Leader)
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);
		gmgr.pPartyManager->AddMember(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());

		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		gmgr.pPartyManager->AddMember(uidParty, uidMember, strMemberName, 3, nMemberCID);

		m_pRequestPlayer->SetPartyUID(uidParty);

		CHECK_EQUAL(pParty->GetCount(), 3);
		CHECK(pParty->IsExistMember(m_uidRequestPlayer));
		CHECK_EQUAL(pParty->GetLeader(), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestPlayer->GetPartyUID(), uidParty);


		// act
		gsys.pPartyLogic->LeaveReq(m_uidRequest, uidParty, m_uidRequestPlayer);


		// assert
		pParty = gmgr.pPartyManager->Find(uidParty);
		ASSERT_CHECK(pParty != NULL);
		CHECK_EQUAL(pParty->GetCount(), 2);
		CHECK(!pParty->IsExistMember(m_uidRequestPlayer));
		CHECK(m_uidRequestPlayer != pParty->GetLeader());
		CHECK_EQUAL(m_pRequestPlayer->GetPartyUID(), MUID::Invalid());

		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_CHANGE_LEADER);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(1), MSC_PROXY_PLAYER_UPDATE);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(2), MMC_PARTY_LEAVE_RES);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(3), MMC_PARTY_REMOVE_MEMBER);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(4), MMC_PARTY_ERASE_QUEST_RUNNER_SYNC);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(5), MMC_PARTY_FIELD_SYNC);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(6), MMC_PARTY_QUEST_SYNC);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(7), MMC_PARTY_SYNC);

		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_CHANGE_LEADER);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(1), MSC_PROXY_PLAYER_UPDATE);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(2), MMC_PARTY_REMOVE_MEMBER);		
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(3), MMC_PARTY_ERASE_QUEST_RUNNER_SYNC);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(4), MMC_PARTY_FIELD_SYNC);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(5), MMC_PARTY_QUEST_SYNC);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(6), MMC_PARTY_SYNC);

		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(2, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(2, 1), CR_SUCCESS);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(3, 0), uidParty);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(3, 1), m_uidRequestPlayer);
	}

	TEST_FIXTURE(Fixture, TestLeaveReq_Looter)
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);
		gmgr.pPartyManager->AddMember(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());

		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		gmgr.pPartyManager->AddMember(uidParty, uidMember, strMemberName, 3, nMemberCID);

		m_pRequestPlayer->SetPartyUID(uidParty);
		pParty->ChangeLeader(m_uidTargetPlayer);
		

		CHECK_EQUAL(pParty->GetCount(), 3);
		CHECK(pParty->IsExistMember(m_uidRequestPlayer));
		CHECK_EQUAL(m_pRequestPlayer->GetPartyUID(), uidParty);


		// act
		gsys.pPartyLogic->LeaveReq(m_uidRequest, uidParty, m_uidRequestPlayer);


		// assert
		pParty = gmgr.pPartyManager->Find(uidParty);
		ASSERT_CHECK(pParty != NULL);
		CHECK_EQUAL(pParty->GetCount(), 2);
		CHECK(!pParty->IsExistMember(m_uidRequestPlayer));
		CHECK(m_uidRequestPlayer != pParty->GetLeader());
		CHECK_EQUAL(m_pRequestPlayer->GetPartyUID(), MUID::Invalid());

		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MSC_PROXY_PLAYER_UPDATE);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(1), MMC_PARTY_LEAVE_RES);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(2), MMC_PARTY_REMOVE_MEMBER);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(3), MMC_PARTY_ERASE_QUEST_RUNNER_SYNC);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(4), MMC_PARTY_FIELD_SYNC);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(5), MMC_PARTY_QUEST_SYNC);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(6), MMC_PARTY_SYNC);

		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MSC_PROXY_PLAYER_UPDATE);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(1), MMC_PARTY_REMOVE_MEMBER);		
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(2), MMC_PARTY_ERASE_QUEST_RUNNER_SYNC);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(3), MMC_PARTY_FIELD_SYNC);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(4), MMC_PARTY_QUEST_SYNC);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(5), MMC_PARTY_SYNC);
		
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(1, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(1, 1), CR_SUCCESS);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(2, 0), uidParty);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(2, 1), m_uidRequestPlayer);
	}

	TEST_FIXTURE(Fixture, TestLeaveReq_LeaderAndAloneMember)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);
		gmgr.pPartyManager->AddMember(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());

		m_pRequestPlayer->SetPartyUID(uidParty);
		m_pTargetPlayer->SetPartyUID(uidParty);

		CHECK_EQUAL(pParty->GetCount(), 2);
		CHECK(pParty->IsExistMember(m_uidRequestPlayer));
		CHECK_EQUAL(pParty->GetLeader(), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestPlayer->GetPartyUID(), uidParty);
		CHECK_EQUAL(m_pTargetPlayer->GetPartyUID(), uidParty);


		// act
		gsys.pPartyLogic->LeaveReq(m_uidRequest, uidParty, m_uidRequestPlayer);


		// assert
		pParty = gmgr.pPartyManager->Find(uidParty);
		ASSERT_CHECK(pParty == NULL);
		CHECK_EQUAL(m_pRequestPlayer->GetPartyUID(), MUID::Invalid());
		CHECK_EQUAL(m_pTargetPlayer->GetPartyUID(), MUID::Invalid());

		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_CHANGE_LEADER);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(1), MSC_PROXY_PLAYER_UPDATE);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(2), MMC_PARTY_LEAVE_RES);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(3), MMC_PARTY_REMOVE_MEMBER);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(4), MSC_PROXY_PLAYER_UPDATE);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(5), MMC_PARTY_REMOVE_MEMBER);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(6), MMC_PARTY_REMOVE);

		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_CHANGE_LEADER);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(1), MSC_PROXY_PLAYER_UPDATE);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(2), MMC_PARTY_REMOVE_MEMBER);		
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(3), MSC_PROXY_PLAYER_UPDATE);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(4), MMC_PARTY_REMOVE_MEMBER);		
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(5), MMC_PARTY_REMOVE);

		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), m_uidTargetPlayer);		
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(2, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(2, 1), CR_SUCCESS);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(3, 0), uidParty);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(3, 1), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(5, 0), uidParty);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(5, 1), m_uidTargetPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(6, 0), uidParty);
	}

	TEST_FIXTURE(Fixture, TestLeaveReq_UnknownPartyFailure)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();

		
		// act
		gsys.pPartyLogic->LeaveReq(m_uidTarget, uidParty, m_uidTargetPlayer);


		// assert
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_LEAVE_RES);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), m_uidTargetPlayer);
		CHECK_EQUAL(m_pTargetLink->GetParam<int>(0, 1), CR_FAIL_PARTY_INVALID_PARTY);
	}

	TEST_FIXTURE(Fixture, TestLeaveReq_NotMemberFailure)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);

		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		gmgr.pPartyManager->AddMember(uidParty, uidMember, strMemberName, 3, nMemberCID);


		// act
		gsys.pPartyLogic->LeaveReq(m_uidTarget, uidParty, m_uidTargetPlayer);


		// assert
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_LEAVE_RES);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), m_uidTargetPlayer);
		CHECK_EQUAL(m_pTargetLink->GetParam<int>(0, 1), CR_FAIL_PARTY_NOT_IN_PARTY);
	}

	TEST_FIXTURE(Fixture, TestLeaveReq_UnknownPlayerFailure)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);

		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		gmgr.pPartyManager->AddMember(uidParty, uidMember, strMemberName, 3,  nMemberCID);


		// act
		gsys.pPartyLogic->LeaveReq(m_uidTarget, uidParty, uidMember);


		// assert
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_LEAVE_RES);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), uidMember);
		CHECK_EQUAL(m_pTargetLink->GetParam<int>(0, 1), CR_FAIL_SYSTEM_INVALID_PC_UID);
	}

	TEST_FIXTURE(Fixture, TestRemoveQuest)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);
		gmgr.pPartyManager->AddMember(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());

		ZPartyMember* pPartyMember = pParty->GetMember(m_uidRequestPlayer);
		ASSERT_CHECK(pPartyMember != NULL);
		pPartyMember->AddQuest(100);
		pPartyMember->AddQuest(200);
		pPartyMember->AddQuest(300);

		vector<int> vecQuestID;
		vecQuestID.push_back(100);
		vecQuestID.push_back(200);
		vecQuestID.push_back(300);


		// act
		gsys.pPartyLogic->RemoveQuest(uidParty, m_uidRequestPlayer, vecQuestID);


		// assert
		CHECK(pPartyMember->IsEmptyQuest());

		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_QUEST_SYNC);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_QUEST_SYNC);
	}

	TEST_FIXTURE(Fixture, TestRemoveField)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);
		gmgr.pPartyManager->AddMember(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());

		int nFieldGroupID = 100;
		MUID uidFieldGroup = ZTestHelper::NewUID();

		ZPartyMember* pPartyMember = pParty->GetMember(m_uidRequestPlayer);
		ASSERT_CHECK(pPartyMember != NULL);
		pPartyMember->SetFieldGroup(nFieldGroupID, uidFieldGroup);

		
		// act
		gsys.pPartyLogic->RemoveField(uidParty, m_uidRequestPlayer, nFieldGroupID);


		// assert		
		CHECK_EQUAL(pParty->GetFieldGroupUID(nFieldGroupID), MUID::Invalid());

		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_FIELD_SYNC);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_FIELD_SYNC);
	}

	TEST_FIXTURE(Fixture, TestEraseQuestRunner)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);
		gmgr.pPartyManager->AddMember(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());


		// act
		gsys.pPartyLogic->EraseQuestRunner(uidParty, m_uidRequestPlayer);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_ERASE_QUEST_RUNNER_SYNC);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_ERASE_QUEST_RUNNER_SYNC);

		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), m_uidRequestPlayer);
	}

	TEST_FIXTURE(Fixture, TestKickReq_NoramlMember)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);
		gmgr.pPartyManager->AddMember(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());
		m_pTargetPlayer->SetPartyUID(uidParty);

		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		gmgr.pPartyManager->AddMember(uidParty, uidMember, strMemberName, 3, nMemberCID);


		// act
		gsys.pPartyLogic->KickReq(m_uidRequest, uidParty, m_uidRequestPlayer, m_uidTargetPlayer);

		// assert
		CHECK_EQUAL(pParty->GetCount(), 2);
		CHECK_EQUAL(m_pTargetPlayer->GetPartyUID(), MUID::Invalid());

		ASSERT_EQUAL(m_pRequestLink->GetCommandCount(), 7);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(0), MSC_PROXY_PLAYER_UPDATE);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(1), MMC_PARTY_KICK_RES);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(2), MMC_PARTY_REMOVE_MEMBER);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(3), MMC_PARTY_ERASE_QUEST_RUNNER_SYNC);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(4), MMC_PARTY_FIELD_SYNC);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(5), MMC_PARTY_QUEST_SYNC);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(6), MMC_PARTY_SYNC);
		
		ASSERT_EQUAL(m_pTargetLink->GetCommandCount(), 6);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(0), MSC_PROXY_PLAYER_UPDATE);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(1), MMC_PARTY_REMOVE_MEMBER);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(2), MMC_PARTY_ERASE_QUEST_RUNNER_SYNC);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(3), MMC_PARTY_FIELD_SYNC);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(4), MMC_PARTY_QUEST_SYNC);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(5), MMC_PARTY_SYNC);

		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(1, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(1, 1), m_uidTargetPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(1, 2), CR_SUCCESS);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(2, 0), uidParty);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(2, 1), m_uidTargetPlayer);
	}

	TEST_FIXTURE(Fixture, TestKickReq_AloneMember)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);
		gmgr.pPartyManager->AddMember(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());
		m_pTargetPlayer->SetPartyUID(uidParty);


		// act
		gsys.pPartyLogic->KickReq(m_uidRequest, uidParty, m_uidRequestPlayer, m_uidTargetPlayer);


		// assert
		pParty = gmgr.pPartyManager->Find(uidParty);
		CHECK(pParty == NULL);
		CHECK_EQUAL(m_pTargetPlayer->GetPartyUID(), MUID::Invalid());

		ASSERT_EQUAL(m_pRequestLink->GetCommandCount(), 6);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(0), MSC_PROXY_PLAYER_UPDATE);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(1), MMC_PARTY_KICK_RES);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(2), MMC_PARTY_REMOVE_MEMBER);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(3), MSC_PROXY_PLAYER_UPDATE);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(4), MMC_PARTY_REMOVE_MEMBER);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(5), MMC_PARTY_REMOVE);

		ASSERT_EQUAL(m_pTargetLink->GetCommandCount(), 5);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(0), MSC_PROXY_PLAYER_UPDATE);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(1), MMC_PARTY_REMOVE_MEMBER);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(2), MSC_PROXY_PLAYER_UPDATE);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(3), MMC_PARTY_REMOVE_MEMBER);		
		CHECK_EQUAL(m_pTargetLink->GetCommandID(4), MMC_PARTY_REMOVE);

		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(1, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(1, 1), m_uidTargetPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(1, 2), CR_SUCCESS);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(2, 0), uidParty);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(2, 1), m_uidTargetPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(4, 0), uidParty);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(4, 1), m_uidRequestPlayer);
	}

	TEST_FIXTURE(Fixture, TestKickReq_UnknownPartyFailure)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();


		// act
		gsys.pPartyLogic->KickReq(m_uidRequest, uidParty, m_uidRequestPlayer, m_uidTargetPlayer);


		// assert		
		CHECK_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_KICK_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 1), m_uidTargetPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 2), CR_FAIL_PARTY_INVALID_PARTY);
	}

	TEST_FIXTURE(Fixture, TestKickReq_NotMemberFailure)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);


		// act
		gsys.pPartyLogic->KickReq(m_uidTarget, uidParty, m_uidTargetPlayer, m_uidRequestPlayer);


		// assert		
		CHECK_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_KICK_RES);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), m_uidTargetPlayer);
		CHECK_EQUAL(m_pTargetLink->GetParam<int>(0, 2), CR_FAIL_PARTY_NOT_IN_PARTY);
	}

	TEST_FIXTURE(Fixture, TestKickReq_NotLeaderFailure)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);
		gmgr.pPartyManager->AddMember(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());


		// act
		gsys.pPartyLogic->KickReq(m_uidTarget, uidParty, m_uidTargetPlayer, m_uidRequestPlayer);


		// assert		
		CHECK_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_KICK_RES);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), m_uidTargetPlayer);
		CHECK_EQUAL(m_pTargetLink->GetParam<int>(0, 2), CR_FAIL_PARTY_NOT_PARTY_LEADER);
	}

	TEST_FIXTURE(Fixture, TestKickReq_SelfKickFailure)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);


		// act
		gsys.pPartyLogic->KickReq(m_uidRequest, uidParty, m_uidRequestPlayer, m_uidRequestPlayer);


		// assert		
		CHECK_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_KICK_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 2), CR_FAIL_PARTY_KICK_SELF);
	}

	TEST_FIXTURE(Fixture, TestKickReq_NotMemberTargetFailure)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);


		// act
		gsys.pPartyLogic->KickReq(m_uidRequest, uidParty, m_uidRequestPlayer, m_uidTargetPlayer);


		// assert		
		CHECK_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_KICK_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 2), CR_FAIL_PARTY_TARGET_NOT_IN_PARTY);
	}

	TEST_FIXTURE(Fixture, TestDoOffline)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		wstring strMessage = L"TestMessage";
		
		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		pParty->AddMember(uidMember, strMemberName, 3, nMemberCID);


		// act
		gsys.pPartyLogic->DoOffline(uidParty, uidMember);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandCount(), 5);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_ADD_OFFLINE_MEMBER);		
		CHECK_EQUAL(m_pRequestLink->GetCommandID(1), MMC_PARTY_ERASE_QUEST_RUNNER_SYNC);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(2), MMC_PARTY_FIELD_SYNC);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(3), MMC_PARTY_QUEST_SYNC);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(4), MMC_PARTY_SYNC);

		ASSERT_EQUAL(m_pTargetLink->GetCommandCount(), 5);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_ADD_OFFLINE_MEMBER);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(1), MMC_PARTY_ERASE_QUEST_RUNNER_SYNC);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(2), MMC_PARTY_FIELD_SYNC);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(3), MMC_PARTY_QUEST_SYNC);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(4), MMC_PARTY_SYNC);
	}

	TEST_FIXTURE(Fixture, TestDoOffline_Leader)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		wstring strMessage = L"TestMessage";
		
		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();
		pParty->AddMember(uidMember, strMemberName, 3, nMemberCID);

		// act
		gsys.pPartyLogic->DoOffline(uidParty, m_uidRequestPlayer);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandCount(), 6);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_CHANGE_LEADER);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(1), MMC_PARTY_ADD_OFFLINE_MEMBER);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(2), MMC_PARTY_ERASE_QUEST_RUNNER_SYNC);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(3), MMC_PARTY_FIELD_SYNC);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(4), MMC_PARTY_QUEST_SYNC);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(5), MMC_PARTY_SYNC);
		
		ASSERT_EQUAL(m_pTargetLink->GetCommandCount(), 6);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_CHANGE_LEADER);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(1), MMC_PARTY_ADD_OFFLINE_MEMBER);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(2), MMC_PARTY_ERASE_QUEST_RUNNER_SYNC);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(3), MMC_PARTY_FIELD_SYNC);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(4), MMC_PARTY_QUEST_SYNC);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(5), MMC_PARTY_SYNC);
	}

	
	TEST_FIXTURE(Fixture, TestDoOffline_Empty)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		wstring strMessage = L"TestMessage";

		// act
		gsys.pPartyLogic->DoOffline(uidParty, m_uidRequestPlayer);


		// assert
		pParty = gmgr.pPartyManager->Find(uidParty);
		ASSERT_CHECK(pParty == NULL);

		ASSERT_EQUAL(m_pRequestLink->GetCommandCount(), 2);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_ADD_OFFLINE_MEMBER);
		CHECK_EQUAL(m_pRequestLink->GetCommandID(1), MMC_PARTY_REMOVE);
		
		ASSERT_EQUAL(m_pTargetLink->GetCommandCount(), 2);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_ADD_OFFLINE_MEMBER);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(1), MMC_PARTY_REMOVE);
	}

	TEST_FIXTURE(Fixture, TestDoOffline_UnknownPartyFailure)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();


		// act
		gsys.pPartyLogic->DoOffline(uidParty, m_uidRequestPlayer);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandCount(), 0);		
		ASSERT_EQUAL(m_pTargetLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestDoOffline_NotMemberFailure)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		wstring strMessage = L"TestMessage";
		
		MUID uidMember = ZTestHelper::NewUID();


		// act
		gsys.pPartyLogic->DoOffline(uidParty, uidMember);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandCount(), 0);		
		ASSERT_EQUAL(m_pTargetLink->GetCommandCount(), 0);
	}	

	TEST_FIXTURE(Fixture, TestDoOnline)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		MUID uidOffline = ZTestHelper::NewUID();

		pParty->AddMember(uidOffline, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());


		// act
		gsys.pPartyLogic->DoOnline(uidParty, m_uidTargetPlayer, uidOffline);


		// assert
		CHECK_EQUAL(m_pTargetPlayer->GetPartyUID(), uidParty);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MSC_PROXY_PLAYER_UPDATE);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MSC_PROXY_PLAYER_UPDATE);

		ASSERT_EQUAL(m_pRequestLink->GetCommandID(1), MMC_PARTY_REMOVE_OFFLINE_MEMBER);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(1), MMC_PARTY_REMOVE_OFFLINE_MEMBER);

		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(1, 0), uidParty);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(1, 1), m_uidTargetPlayer);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(1, 2), uidOffline);
	}

	TEST_FIXTURE(Fixture, TestDoOnline_MoveServerComplate)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		MUID uidOffline = ZTestHelper::NewUID();

		pParty->AddMember(uidOffline, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());

		ZPartyMember* pPartyMember = pParty->GetMember(uidOffline);
		ASSERT_CHECK(pPartyMember != NULL);
		
		pPartyMember->SetMoveServer(true);


		// act
		gsys.pPartyLogic->DoOnline(uidParty, m_uidTargetPlayer, uidOffline);


		// assert
		CHECK(!pPartyMember->IsMoveServer());
		CHECK(!pParty->IsExistMember(uidOffline));
		CHECK(pParty->IsExistMember(m_uidTargetPlayer));
		CHECK_EQUAL(m_pTargetPlayer->GetPartyUID(), uidParty);

		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MSC_PROXY_PLAYER_UPDATE);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(1), MMC_PARTY_MOVE_SERVER_SYNC);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(1, 0), uidParty);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(1, 1), m_uidTargetPlayer);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(1, 2), uidOffline);

		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MSC_PROXY_PLAYER_UPDATE);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(1), MMC_PARTY_MOVE_SERVER_SYNC);
	}

	TEST_FIXTURE(Fixture, TestDoOnline_UnknownPartyFailure)		
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		MUID uidOffline = ZTestHelper::NewUID();


		// act
		gsys.pPartyLogic->DoOnline(uidParty, m_uidTargetPlayer, uidOffline);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandCount(), 0);
		ASSERT_EQUAL(m_pTargetLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestDoOnline_NotMemberFailure)		
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		MUID uidOffline = ZTestHelper::NewUID();


		// act
		gsys.pPartyLogic->DoOnline(uidParty, m_uidTargetPlayer, uidOffline);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandCount(), 0);
		ASSERT_EQUAL(m_pTargetLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestDoOnline_UnknownPlayerFailure)		
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		MUID uidOffline = ZTestHelper::NewUID();
		MUID uidMember = ZTestHelper::NewUID();

		pParty->AddMember(uidOffline, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());


		// act
		gsys.pPartyLogic->DoOnline(uidParty, uidMember, uidOffline);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandCount(), 0);
		ASSERT_EQUAL(m_pTargetLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestJoinInviteReq)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);


		// act
		gsys.pPartyLogic->JoinInviteReq(m_uidRequest, uidParty, m_uidRequestPlayer);


		// assert
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_JOIN_ACCEPT_REQ);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 1), m_uidTargetPlayer);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 2), m_uidRequestPlayer);		

		wstring strRequestPlayerName;
		MCommand& cmd = m_pTargetLink->GetCommand(0);
		cmd.GetParameter(strRequestPlayerName, 3, MPT_WSTR);
		CHECK(strRequestPlayerName == m_strRequestPlayerName);
	}

	TEST_FIXTURE(Fixture, TestJoinInviteReq_UnknownPartyFailure)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();


		// act
		gsys.pPartyLogic->JoinInviteReq(m_uidRequest, uidParty, m_uidRequestPlayer);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_JOIN_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 1), CR_FAIL_PARTY_INVALID_PARTY);
	}

	TEST_FIXTURE(Fixture, TestJoinInviteReq_FullPartyFailure)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);

		for(int i = 1; i < MAX_PARTY_MEMBER_COUNT; i++)
		{
			MUID uidMember = ZTestHelper::NewUID();
			int nMemberCID = ZTestHelper::NewCID();
			pParty->AddMember(uidMember, L"TestMemberName", 3, nMemberCID);
		}


		// act
		gsys.pPartyLogic->JoinInviteReq(m_uidRequest, uidParty, m_uidRequestPlayer);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_JOIN_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 1), CR_FAIL_PARTY_FULL_PARTY);
	}

	TEST_FIXTURE(Fixture, TestJoinInviteReq_AleadyMemberFailure)
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);
		pParty->AddMember(m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());


		// act
		gsys.pPartyLogic->JoinInviteReq(m_uidRequest, uidParty, m_uidRequestPlayer);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_JOIN_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 1), CR_FAIL_PARTY_ALEADY_HAS_PARTY);
	}

	TEST_FIXTURE(Fixture, TestJoinInviteReq_UnknownLeaderFailure)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		MUID uidLeader = ZTestHelper::NewUID();
		wstring strLeaderName = L"TestLeaderName";
		int nLeaderCID = ZTestHelper::NewCID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, uidLeader, strLeaderName, m_nTargetServerID, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);


		// act
		gsys.pPartyLogic->JoinInviteReq(m_uidRequest, uidParty, m_uidRequestPlayer);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_JOIN_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 1), CR_FAIL_PARTY_INVALID_PARTY);
	}

	TEST_FIXTURE(Fixture, TestJoinInviteReq_NoServerFailure)	
	{
		// arrange
		MUID uidNoServerPlayer = ZTestHelper::NewUID();
		auto_ptr<ZPlayer> pNoServerPlayer(gmgr.pPlayerManager->NewPlayer(ZTestHelper::NewUID(), ZTestHelper::NewID(), ZTestHelper::NewName()));
		pNoServerPlayer->SetWorldInfo(ZTestHelper::NewID(), ZTestHelper::NewID(), uidNoServerPlayer, L"TestNoServerPlayerName", ZTestHelper::NewID());


		MUID uidParty = ZTestHelper::NewUID();

		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, uidNoServerPlayer, pNoServerPlayer->GetPlayerName(), m_nTargetServerID, pNoServerPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);


		// act
		gsys.pPartyLogic->JoinInviteReq(m_uidRequest, uidParty, m_uidRequestPlayer);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_JOIN_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 1), CR_FAIL_PARTY_INVALID_PARTY);
	}

	TEST_FIXTURE(Fixture, TestJoinInviteReq_UnknownRequestPlayerFailure)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		MUID uidRequestPlayer = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);


		// act
		gsys.pPartyLogic->JoinInviteReq(m_uidRequest, uidParty, uidRequestPlayer);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_JOIN_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 1), CR_FAIL_SYSTEM_INVALID_PC_UID);
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptRes)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		CCommandResultTable nResult = CR_SUCCESS;
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);


		// act
		gsys.pPartyLogic->JoinAcceptRes(m_uidTarget, uidParty, m_uidTargetPlayer, m_uidRequestPlayer, nResult);


		// assert
		CHECK(pParty->IsExistMember(m_uidRequestPlayer));
		CHECK_EQUAL(m_pRequestPlayer->GetPartyUID(), uidParty);

		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MSC_PROXY_PLAYER_UPDATE);
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(1), MMC_PARTY_JOIN_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(1, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(1, 1), nResult);

		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MSC_PROXY_PLAYER_UPDATE);
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(1), MMC_PARTY_ADD_MEMBER);		
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(1, 0), uidParty);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(1, 1), m_uidRequestPlayer);
		
		wstring strRequestPlayerName;
		MCommand& cmd = m_pTargetLink->GetCommand(1);
		cmd.GetParameter(strRequestPlayerName, 2, MPT_WSTR);
		CHECK(strRequestPlayerName == m_strRequestPlayerName);
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptRes_UnknownRequestPlayerFailure)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		MUID uidRequestPlayer = ZTestHelper::NewUID();
		CCommandResultTable nResult = CR_SUCCESS;
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);

		// act
		gsys.pPartyLogic->JoinAcceptRes(m_uidTarget, uidParty, m_uidTargetPlayer, uidRequestPlayer, nResult);


		// assert
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_JOIN_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), m_uidTargetPlayer);
		CHECK_EQUAL(m_pTargetLink->GetParam<int>(0, 1), CR_FAIL_SYSTEM_INVALID_PC_UID);
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptRes_NoServerFailure)	
	{
		// arrange
		MUID uidNoServerPlayer = ZTestHelper::NewUID();
		auto_ptr<ZPlayer> pNoServerPlayer(gmgr.pPlayerManager->NewPlayer(ZTestHelper::NewUID(), ZTestHelper::NewID(), ZTestHelper::NewName()));
		pNoServerPlayer->SetWorldInfo(ZTestHelper::NewID(), ZTestHelper::NewID(), uidNoServerPlayer, L"TestNoServerPlayerName", ZTestHelper::NewID());


		MUID uidParty = ZTestHelper::NewUID();
		CCommandResultTable nResult = CR_SUCCESS;
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);


		// act
		gsys.pPartyLogic->JoinAcceptRes(m_uidTarget, uidParty, m_uidTargetPlayer, uidNoServerPlayer, nResult);


		// assert
		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_JOIN_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), m_uidTargetPlayer);
		CHECK_EQUAL(m_pTargetLink->GetParam<int>(0, 1), CR_FAIL_SYSTEM_INVALID_PC_UID);
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptRes_UnknownPartyFailure)
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		CCommandResultTable nResult = CR_SUCCESS;


		// act
		gsys.pPartyLogic->JoinAcceptRes(m_uidTarget, uidParty, m_uidTargetPlayer, m_uidRequestPlayer, nResult);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_JOIN_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 1), CR_FAIL_PARTY_INVALID_PARTY);

		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_JOIN_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), m_uidTargetPlayer);
		CHECK_EQUAL(m_pTargetLink->GetParam<int>(0, 1), CR_FAIL_PARTY_INVALID_PARTY);
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptRes_NotPartyLeaderFailure)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		CCommandResultTable nResult = CR_SUCCESS;
		MUID uidLeader = ZTestHelper::NewUID();
		wstring strLeaderName = L"TestLeaderName";
		int nLeaderCID = ZTestHelper::NewCID();

		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, uidLeader, strLeaderName, m_nTargetServerID, nLeaderCID);
		ASSERT_CHECK(pParty != NULL);


		// act
		gsys.pPartyLogic->JoinAcceptRes(m_uidTarget, uidParty, m_uidTargetPlayer, m_uidRequestPlayer, nResult);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_JOIN_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 1), CR_FAIL_PARTY_NOT_PARTY_LEADER);

		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_JOIN_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), m_uidTargetPlayer);
		CHECK_EQUAL(m_pTargetLink->GetParam<int>(0, 1), CR_FAIL_PARTY_NOT_PARTY_LEADER);
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptRes_FullPartyFailure)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		CCommandResultTable nResult = CR_SUCCESS;
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);

		for(int i = 1; i < MAX_PARTY_MEMBER_COUNT; i++)
		{
			MUID uidMember = ZTestHelper::NewUID();
			int nMemberCID = ZTestHelper::NewCID();
			pParty->AddMember(uidMember, L"TestMemberName", 3, nMemberCID);
		}


		// act
		gsys.pPartyLogic->JoinAcceptRes(m_uidTarget, uidParty, m_uidTargetPlayer, m_uidRequestPlayer, nResult);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_JOIN_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 1), CR_FAIL_PARTY_FULL_PARTY);

		ASSERT_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_JOIN_ACCEPT_CANCEL);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), m_uidTargetPlayer);
		CHECK_EQUAL(m_pTargetLink->GetParam<int>(0, 1), CR_FAIL_PARTY_FULL_PARTY);
	}

	TEST_FIXTURE(Fixture, TestJoinAcceptRes_Refuse)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		CCommandResultTable nResult = CR_FAIL_PARTY_REFUSE_INVITATION_FOR_ME;
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);


		// act
		gsys.pPartyLogic->JoinAcceptRes(m_uidTarget, uidParty, m_uidTargetPlayer, m_uidRequestPlayer, nResult);


		// assert
		ASSERT_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_JOIN_INVITE_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pRequestLink->GetParam<int>(0, 1), nResult);

		ASSERT_EQUAL(m_pTargetLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestMoveServer)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, m_uidRequestPlayer, m_strRequestPlayerName, m_nTargetServerID, m_pRequestPlayer->GetCID());
		ASSERT_CHECK(pParty != NULL);

		ZPartyMember* pPartyMember = pParty->GetMember(m_uidRequestPlayer);
		ASSERT_CHECK(pPartyMember != NULL);


		// act
		gsys.pPartyLogic->MoveServer(uidParty, m_uidRequestPlayer);


		// assert
		CHECK(pPartyMember->IsMoveServer());
	}

	TEST_FIXTURE(Fixture, TestPartyInfoAllReq)
	{
		// arrange
		MUID uidParty1 = ZTestHelper::NewUID();
		MUID uidParty2 = ZTestHelper::NewUID();
		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty1, m_uidRequestPlayer, m_strRequestPlayerName, m_nTargetServerID, m_pRequestPlayer->GetCID());
		gmgr.pPartyManager->AddMember(pParty->GetUID(), m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());
		pParty = gmgr.pPartyManager->AddParty(uidParty2, m_uidRequestPlayer, m_strRequestPlayerName, m_nRequestServerID, m_pRequestPlayer->GetCID());
		gmgr.pPartyManager->AddMember(pParty->GetUID(), m_uidTargetPlayer, m_strTargetPlayerName, m_nRequestServerID, m_pTargetPlayer->GetCID());


		// act
		gsys.pPartyLogic->PartyInfoAllReq(m_uidRequest);


		// assert
		CHECK_EQUAL(m_pRequestLink->GetCommandID(0), MMC_PARTY_INFO_ALL_RES);
		CHECK_EQUAL(m_pRequestLink->GetParam<MBlob*>(0, 0)->GetBlobCount(), 2);
		TD_PARTY_INFO* pPartyInfo = (TD_PARTY_INFO*)m_pRequestLink->GetParam<MBlob*>(0, 0)->GetBlobPointer();
		ASSERT_CHECK(pPartyInfo != NULL);
		CHECK_EQUAL(pPartyInfo->uidParty, uidParty1);
		CHECK_EQUAL(pPartyInfo->nMemberCount, 2);
		CHECK_EQUAL(pPartyInfo[1].uidParty, uidParty2);
		CHECK_EQUAL(pPartyInfo[1].nMemberCount, 2);
	}

	TEST_FIXTURE(Fixture, TestAllMemberOfflineInGameServer_NoServerParty)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		MUID uidLeader = ZTestHelper::NewUID();
		wstring strLeaderName = L"TestLeaderName";
		int nLeaderCID = ZTestHelper::NewCID();

		gmgr.pPartyManager->AddParty(uidParty, uidLeader, strLeaderName, 3, nLeaderCID);


		// act
		gsys.pPartyLogic->AllMemberOfflineInGameServer(m_nRequestServerID);


		// assert
		CHECK_EQUAL(m_pTargetLink->GetCommandCount(), 0);
	}

	TEST_FIXTURE(Fixture, TestAllMemberOfflineInGameServer_SingleServerParty)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		MUID uidLeader = ZTestHelper::NewUID();
		wstring strLeaderName = L"TestLeaderName";
		int nLeaderCID = ZTestHelper::NewCID();

		gmgr.pPartyManager->AddParty(uidParty, uidLeader, strLeaderName, m_nRequestServerID, nLeaderCID);


		// act
		gsys.pPartyLogic->AllMemberOfflineInGameServer(m_nRequestServerID);


		// assert
		CHECK_EQUAL(m_pTargetLink->GetCommandCount(), 1);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_REMOVE);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), uidParty);

		ZParty* pParty = gmgr.pPartyManager->Find(uidParty);
		CHECK(pParty == NULL);
	}

	TEST_FIXTURE(Fixture, TestAllMemberOfflineInGameServer_LeaderOffline)	
	{
		// arrange
		MUID uidParty = ZTestHelper::NewUID();
		MUID uidLeader = ZTestHelper::NewUID();
		wstring strLeaderName = L"TestLeaderName";
		int nLeaderCID = ZTestHelper::NewCID();
		MUID uidMember = ZTestHelper::NewUID();
		wstring strMemberName = L"TestMemberName";
		int nMemberCID = ZTestHelper::NewCID();

		ZParty* pParty = gmgr.pPartyManager->AddParty(uidParty, uidLeader, strLeaderName, m_nRequestServerID, nLeaderCID);
		pParty->AddMember(uidMember, strMemberName, m_nTargetServerID, nMemberCID);		


		// act
		gsys.pPartyLogic->AllMemberOfflineInGameServer(m_nRequestServerID);


		// assert
		CHECK_EQUAL(m_pTargetLink->GetCommandCount(), 6);

		CHECK_EQUAL(m_pTargetLink->GetCommandID(0), MMC_PARTY_CHANGE_LEADER);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 0), uidParty);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(0, 1), uidMember);

		CHECK_EQUAL(m_pTargetLink->GetCommandID(1), MMC_PARTY_ADD_OFFLINE_MEMBER);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(1, 0), uidParty);
		CHECK_EQUAL(m_pTargetLink->GetParam<MUID>(1, 1), uidLeader);

		CHECK_EQUAL(m_pTargetLink->GetCommandID(2), MMC_PARTY_ERASE_QUEST_RUNNER_SYNC);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(3), MMC_PARTY_FIELD_SYNC);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(4), MMC_PARTY_QUEST_SYNC);
		CHECK_EQUAL(m_pTargetLink->GetCommandID(5), MMC_PARTY_SYNC);
	}

	TEST_FIXTURE(Fixture, FixedPartyLogOn)
	{
		// arrange
		int nFixedPartyID = ZTestHelper::NewID();
		MUID uidDummy = MUID::Invalid();		


		// arrange stub
		using testing::_;
		using testing::Return;		
		EXPECT_CALL(*m_pFixedPartyInfoManager, FindFixedPartyID(_))
			.WillRepeatedly(Return(nFixedPartyID));


		// assert
		CHECK(gsys.pPartyLogic->FixedPartyLogOn(uidDummy, m_uidRequestPlayer, uidDummy));
		CHECK_EQUAL(0, gmgr.pPartyManager->GetCount());

		CHECK(gsys.pPartyLogic->FixedPartyLogOn(uidDummy, m_uidTargetPlayer, uidDummy));
		CHECK_EQUAL(1, gmgr.pPartyManager->GetCount());

		ZPlayer* pPlayer = AddNewPlayer();
		CHECK(gsys.pPartyLogic->FixedPartyLogOn(uidDummy, pPlayer->GetUID(), uidDummy));
		CHECK_EQUAL(1, gmgr.pPartyManager->GetCount());
	}

	TEST_FIXTURE(Fixture, FixedPartyLogOn_PartyFull)
	{
		// arrange
		int nFixedPartyID = ZTestHelper::NewID();
		MUID uidDummy = MUID::Invalid();		

		using testing::_;
		using testing::Return;		
		EXPECT_CALL(*m_pFixedPartyInfoManager, FindFixedPartyID(_))
			.WillRepeatedly(Return(nFixedPartyID));


		ZPlayer* pPlayer[MAX_PARTY_MEMBER_COUNT];

		for(int i = 0; i < MAX_PARTY_MEMBER_COUNT; i++)
		{
			pPlayer[i] = AddNewPlayerInWorld();
			CHECK(gsys.pPartyLogic->FixedPartyLogOn(uidDummy, pPlayer[i]->GetUID(), uidDummy));
		}


		ZPlayer* pAddPlayer = AddNewPlayer();
		CHECK(!gsys.pPartyLogic->FixedPartyLogOn(uidDummy, pAddPlayer->GetUID(), uidDummy));


		MUID uidParty = pPlayer[0]->GetPartyUID();
		MUID uidMember = pPlayer[0]->GetUID();		
		gsys.pPartyLogic->DoOffline(uidParty, uidMember);

		CHECK(gsys.pPartyLogic->FixedPartyLogOn(uidDummy, pAddPlayer->GetUID(), uidDummy));
	}

	TEST_FIXTURE(Fixture, FixedPartyLogOn_UnknownPlayer)
	{
		MUID uidPlayer = ZTestHelper::NewUID();
		MUID uidDummy = MUID::Invalid();

		CHECK(!gsys.pPartyLogic->FixedPartyLogOn(uidDummy, uidPlayer, uidDummy));		
	}

	TEST_FIXTURE(Fixture, FixedPartyLogOn_NonFixedPartyMember)
	{
		// arrange
		int nFixedPartyID = INVALID_ID;
		MUID uidDummy = MUID::Invalid();

		// arrange stub
		using testing::_;
		using testing::Return;		
		EXPECT_CALL(*m_pFixedPartyInfoManager, FindFixedPartyID(_))
			.WillRepeatedly(Return(nFixedPartyID));

		// assert
		CHECK(!gsys.pPartyLogic->FixedPartyLogOn(uidDummy, m_uidRequestPlayer, uidDummy));		
	}

	TEST_FIXTURE(Fixture, CreateAutoPartyReq)
	{
		vector<MUID> vecMemberUID;
		vecMemberUID.push_back(m_uidRequestPlayer);
		vecMemberUID.push_back(m_uidTargetPlayer);

		CHECK_EQUAL(0, gmgr.pPartyManager->GetCount());	

		gsys.pPartyLogic->CreateAutoPartyReq(0, vecMemberUID);
		
		CHECK_EQUAL(1, gmgr.pPartyManager->GetCount());	
	}

	TEST_FIXTURE(Fixture, CreateAutoPartyReq_NotEnoughMember_NotExist)
	{
		MUID uidUnknown = ZTestHelper::NewUID();

		vector<MUID> vecMemberUID;
		vecMemberUID.push_back(m_uidRequestPlayer);
		vecMemberUID.push_back(uidUnknown);

		CHECK_EQUAL(0, gmgr.pPartyManager->GetCount());	

		gsys.pPartyLogic->CreateAutoPartyReq(0, vecMemberUID);

		CHECK_EQUAL(0, gmgr.pPartyManager->GetCount());	
	}

	TEST_FIXTURE(Fixture, CreateAutoPartyReq_NotEnoughMember_AleadyParty)
	{
		ZParty* pParty = gmgr.pPartyManager->AddParty(m_uidTargetPlayer, m_strTargetPlayerName, m_nTargetServerID, m_pTargetPlayer->GetCID());
		m_pTargetPlayer->SetPartyUID(pParty->GetUID());

		vector<MUID> vecMemberUID;
		vecMemberUID.push_back(m_uidRequestPlayer);
		vecMemberUID.push_back(m_uidTargetPlayer);

		CHECK_EQUAL(1, gmgr.pPartyManager->GetCount());	

		gsys.pPartyLogic->CreateAutoPartyReq(0, vecMemberUID);

		CHECK_EQUAL(1, gmgr.pPartyManager->GetCount());	
	}
}
