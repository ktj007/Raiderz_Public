#include "stdafx.h"
#include "GSharedField.h"
#include "GUTHelper.h"
#include "GUTHelper_CmdHandler.h"
#include "FNotStandAlone.h"
#include "FBaseFieldInfo.h"
#include "FBaseFieldMgr.h"
#include "FBaseMockLink.h"
#include "GDynamicFieldMaster.h"
#include "GPlayerField.h"
#include "GPlayerObjectManager.h"
#include "CCommandTable.h"
#include "GInnRoomInfo.h"
#include "GInnRoomInfoMgr.h"
#include "GPlayerFieldDynamic.h"
#include "FBasePlayer.h"
#include "GConfig.h"
#include "CCommandTable_Master.h"
#include "FBaseNetClient.h"
#include "GBattleArenaMgr.h"
#include "GGameRule_DeathMatch.h"
#include "GPlayerBattleArena.h"
#include "GParty.h"
#include "GUTHelper_Party.h"
#include "GMatchRule_SameTeamMember.h"
#include "GUTHelper_Field.h"
#include "GConst.h"

SUITE(GateToDynamicField)
{
	const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
	const vec3 VCommonDir = vec3(0,1,0);

	struct FGateToDynamicField : public FBaseFieldInfo, FBaseMockLink, FBasePlayer
	{
		FGateToDynamicField()
		{
			InitField();
			InitPlayer();
			InitInnRoomInfo();

			InitBattleArenaFieldInfo();
		}
		virtual ~FGateToDynamicField()
		{
			ClearLinks();
		}

		void InitField()
		{
			m_pTestSharedFieldInfo = AddFieldInfo(TEST_SHARED_FIELD_ID);

			m_pTestDynamicFieldInfo = AddFieldInfo(TEST_FIRST_FIELD_ID);
			m_pTestDynamicFieldInfo->m_bDynamic = true;
			m_pTestFieldGroupInfo = AddFieldGroupInfo(TEST_FIELD_GROUP_ID, TEST_FIRST_FIELD_ID);

			m_pTestSharedField = GUTHelper_Field::NewSharedField(m_pTestSharedFieldInfo);
		}
		void InitPlayer()
		{
			m_pTestPlayer = GUTHelper::NewEntityPlayer(m_pTestSharedField);
			m_pTsstLink = NewLink(m_pTestPlayer);
		}
		void InitInnRoomInfo()
		{
			m_pTestInnRoomInfo = gmgr.pInnRoomInfoMgr->NewInfo();
			m_pTestInnRoomInfo->m_nID = 1;
			m_pTestInnRoomInfo->m_nFieldGroupID = TEST_FIELD_GROUP_ID;
			m_pTestInnRoomInfo->m_nType = IRT_NORMAL;

			gmgr.pInnRoomInfoMgr->Insert(m_pTestInnRoomInfo);

			m_pTestPlayer->GetPlayerInfo()->nInnRoomID = 1;
		}

		void InitBattleArenaPlayers()
		{
			// 플레이어 생성
			MakePlayer(m_pPlayerBlueLeader, "BlueLeader");
			MakePlayer(m_pPlayerBlueMember1, "BlueMember1");
			MakePlayer(m_pPlayerRedLeader, "RedLeader");
			MakePlayer(m_pPlayerRedMember1, "RedMember1");

			// 파티 생성
			m_pBlueParty = MakeParty(m_pPlayerBlueLeader, m_pPlayerBlueMember1);
			m_pRedParty = MakeParty(m_pPlayerRedLeader, m_pPlayerRedMember1);
		}

		void InitBattleArenaFieldInfo()
		{
			// 투기장 필드그룹 정보
			FIELD_GROUP_INFO* pFieldGroupInfo = new FIELD_GROUP_INFO;
			pFieldGroupInfo->nID = GConst::ARENA_FIELDGROUP_ID;
			// 투기장 필드 정보
			GFieldInfo* pFieldInfo = new GFieldInfo;
			pFieldInfo->m_bDynamic = true;
			pFieldInfo->m_nFieldID = GConst::ARENA_FIELD_ID;
			pFieldInfo->m_fMaxX = 50000.0f;
			pFieldInfo->m_fMaxY = 50000.0f;
			// 시작 마커 정보
			MARKER_INFO markerStarting;
			markerStarting.nID = 1;
			markerStarting.nFieldID = GConst::ARENA_FIELD_ID;
			markerStarting.vPoint = vec3(1000,1000,1000);
			markerStarting.vDir = vec3(0,1,0);
			pFieldInfo->InsertMarkerInfo(markerStarting);	// 1번 시작점
			markerStarting.nID = 2;
			markerStarting.nFieldID = GConst::ARENA_FIELD_ID;
			markerStarting.vPoint = vec3(2000,2000,2000);
			markerStarting.vDir = vec3(0,0,1);
			pFieldInfo->InsertMarkerInfo(markerStarting);	// 2번 시작점
			pFieldGroupInfo->vecFieldInfo.push_back(pFieldInfo);
			gmgr.pFieldInfoMgr->Insert(pFieldInfo);
			gmgr.pFieldInfoMgr->InsertGroup(pFieldGroupInfo);
		}

		void MakePlayer(GEntityPlayer*& pPlayer, std::string strName)
		{
			pPlayer = NewEntityPlayer(m_pTestSharedField, vec3(100,100,0));
			sprintf_s(pPlayer->GetPlayerInfo()->szName, strName.c_str());
		}

		GParty* MakeParty(GEntityPlayer* pLeader, GEntityPlayer* pMember)
		{
			GParty* pParty = GUTHelper_Party::CreateParty(pLeader);
			GUTHelper_Party::AddMember(pParty, pMember);

			return pParty;
		}

		static const int TEST_SHARED_FIELD_ID = 1;
		static const int TEST_FIELD_GROUP_ID = 1;
		static const int TEST_FIRST_FIELD_ID = 10;

		GUTHelper			m_Helper;
		GSharedField*		m_pTestSharedField;
		GEntityPlayer*		m_pTestPlayer;
		MockLink*			m_pTsstLink;

		GInnRoomInfo*		m_pTestInnRoomInfo;
		GFieldInfo*			m_pTestSharedFieldInfo;
		GFieldInfo*			m_pTestDynamicFieldInfo;
		FIELD_GROUP_INFO*	m_pTestFieldGroupInfo;

		//////////////////////////////////////////////////////////////////////////
		/// 투기장 입장 테스트를 위한 변수
		GEntityPlayer*		m_pPlayerBlueLeader;
		GEntityPlayer*		m_pPlayerBlueMember1;
		GEntityPlayer*		m_pPlayerRedLeader;
		GEntityPlayer*		m_pPlayerRedMember1;

		GParty*				m_pBlueParty;
		GParty*				m_pRedParty;
		//////////////////////////////////////////////////////////////////////////

		GTestMgrWrapper<GFieldMgr>				m_FieldMgr;
		GTestMgrWrapper<GPlayerObjectManager>	m_PlayerObjectManagerWrapper;
		GTestMgrWrapper<GInnRoomInfoMgr>		m_ImmRoomInfoMgrWrapper;
	};

	TEST_FIXTURE(FGateToDynamicField, TestGateToDynamicField_TypeIsTrial)
	{
		CHECK_EQUAL(0, gmgr.pFieldMgr->GetDynamicFieldGroupQty());

		if (!m_pTestPlayer->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_TRIAL, TEST_FIELD_GROUP_ID, TEST_FIRST_FIELD_ID, vCommonPos, VCommonDir))
		{
			CHECK(FALSE && "동적 필드 생성 실패");
			return;
		}

		CHECK_EQUAL(1, gmgr.pFieldMgr->GetDynamicFieldGroupQty());

		GDynamicFieldMaster* pFieldMaster = gmgr.pFieldMgr->GetDynamicFieldMaster(m_pTestPlayer);
		CHECK_EQUAL(TEST_FIELD_GROUP_ID, pFieldMaster->GetInfo()->nID);
		CHECK_EQUAL(DFT_TRIAL, pFieldMaster->GetType());
		CHECK_EQUAL(1, pFieldMaster->GetQty());
		CHECK_EQUAL(pFieldMaster->GetUID(), m_pTestPlayer->GetPlayerField().GetDynamicFieldGroupUID());


		CHECK_EQUAL(2, m_pTsstLink->GetCommandCount());
		ASSERT_EQUAL(MC_FIELD_PREPARING_TO_GATE, m_pTsstLink->GetCommand(0).GetID());
		ASSERT_EQUAL(MC_FIELD_CHANGE_FIELD, m_pTsstLink->GetCommand(1).GetID());
		CHECK_EQUAL(TEST_FIRST_FIELD_ID, m_pTsstLink->GetParam<int>(1, 0));
	}

	TEST_FIXTURE(FGateToDynamicField, TestGateToDynamicField_TypeIsTrial_Party_Together)
	{
		GEntityPlayer*		pPlayerLeader;
		GEntityPlayer*		pPlayerMember;
		GParty*				pParty;

		MakePlayer(pPlayerLeader, "Leader");
		MakePlayer(pPlayerMember, "Member");
		pParty = MakeParty(pPlayerLeader, pPlayerMember);

		CHECK_EQUAL(0, gmgr.pFieldMgr->GetDynamicFieldGroupQty());
		if (!pPlayerLeader->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_TRIAL, TEST_FIELD_GROUP_ID, TEST_FIRST_FIELD_ID, vCommonPos, VCommonDir))
		{
			CHECK(FALSE && "동적 필드 생성 실패");
			return;
		}
		CHECK_EQUAL(1, gmgr.pFieldMgr->GetDynamicFieldGroupQty());
		if (!pPlayerMember->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_TRIAL, TEST_FIELD_GROUP_ID, TEST_FIRST_FIELD_ID, vCommonPos, VCommonDir))
		{
			CHECK(FALSE && "동적 필드 생성 실패");
			return;
		}
		CHECK_EQUAL(1, gmgr.pFieldMgr->GetDynamicFieldGroupQty());

		GDynamicFieldMaster* pFieldMaster1 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayerLeader);
		GDynamicFieldMaster* pFieldMaster2 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayerMember);

		CHECK_EQUAL(pFieldMaster1->GetUID(), pFieldMaster2->GetUID());
	}

	TEST_FIXTURE(FGateToDynamicField, TestGateToDynamicField_TypeIsTrial_Party_Each_SingleEnter)
	{
		GEntityPlayer*		pPlayerLeader;
		GEntityPlayer*		pPlayerMember;
		GParty*				pParty;

		MakePlayer(pPlayerLeader, "Leader");
		MakePlayer(pPlayerMember, "Member");
		pParty = MakeParty(pPlayerLeader, pPlayerMember);

		CHECK_EQUAL(0, gmgr.pFieldMgr->GetDynamicFieldGroupQty());
		if (!pPlayerLeader->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_TRIAL, TEST_FIELD_GROUP_ID, TEST_FIRST_FIELD_ID, vCommonPos, VCommonDir, true))
		{
			CHECK(FALSE && "동적 필드 생성 실패");
			return;
		}
		CHECK_EQUAL(1, gmgr.pFieldMgr->GetDynamicFieldGroupQty());
		if (!pPlayerMember->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_TRIAL, TEST_FIELD_GROUP_ID, TEST_FIRST_FIELD_ID, vCommonPos, VCommonDir, true))
		{
			CHECK(FALSE && "동적 필드 생성 실패");
			return;
		}
		CHECK_EQUAL(2, gmgr.pFieldMgr->GetDynamicFieldGroupQty());

		GDynamicFieldMaster* pFieldMaster1 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayerLeader);
		GDynamicFieldMaster* pFieldMaster2 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayerMember);

		CHECK(pFieldMaster1->GetUID() != pFieldMaster2->GetUID());
	}

	TEST_FIXTURE(FGateToDynamicField, TestGateToDynamicField_TypeIsInnRoom)
	{
		CHECK_EQUAL(0, gmgr.pFieldMgr->GetDynamicFieldGroupQty());

		if (!m_pTestPlayer->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_INNROOM, TEST_FIELD_GROUP_ID, TEST_FIRST_FIELD_ID, vCommonPos, VCommonDir))
		{
			CHECK(FALSE && "동적 필드 생성 실패");
			return;
		}

		CHECK_EQUAL(1, gmgr.pFieldMgr->GetDynamicFieldGroupQty());

		GDynamicFieldMaster* pFieldMaster = gmgr.pFieldMgr->GetDynamicFieldMaster(m_pTestPlayer);
		CHECK_EQUAL(DFT_INNROOM, pFieldMaster->GetType());
	}

	TEST_FIXTURE(FGateToDynamicField, TestGateToDynamicField_BattleArena_AlwaysCreateLocal)
	{
		InitBattleArenaPlayers();

		// 게임규칙 추가
		gmgr.pBattleArenaMgr->GetMatcher().ClearRules();
		gmgr.pBattleArenaMgr->GetMatcher().AddRule(new GMatchRule_SameTeamMember());
		gmgr.pBattleArenaMgr->ClearGameRules();
		GGameRule_DeathMatch* pFinishRule = new GGameRule_DeathMatch();
		pFinishRule->SetMaxLifeCounter(2);	// 데스매치 라이프를 1로 설정
		gmgr.pBattleArenaMgr->AddGameRule(pFinishRule);
		CHECK_EQUAL(true, m_pPlayerBlueLeader->GetPlayerBattleArena().RegisterEntry());
		CHECK_EQUAL(true, m_pPlayerRedLeader->GetPlayerBattleArena().RegisterEntry());

		CHECK_EQUAL(1, gmgr.pFieldMgr->GetDynamicFieldGroupQty());

		GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldGroupMap().begin()->second;
		CHECK_EQUAL(DFT_BATTLEARENA, pFieldGroup->GetType());
		CHECK_EQUAL(GConst::ARENA_FIELDGROUP_ID, pFieldGroup->GetInfo()->nID);
	}

	TEST_FIXTURE(FGateToDynamicField, TestGateToDynamicField_Failure)
	{
		CHECK_EQUAL(0, gmgr.pFieldMgr->GetDynamicFieldGroupQty());

		bool bRet = m_pTestPlayer->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_NONE, TEST_FIELD_GROUP_ID, TEST_FIRST_FIELD_ID, vCommonPos, VCommonDir);
		CHECK_EQUAL(false, bRet);
		CHECK_EQUAL(0, gmgr.pFieldMgr->GetDynamicFieldGroupQty());


		bRet = m_pTestPlayer->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_NONE, 0, TEST_FIRST_FIELD_ID, vCommonPos, VCommonDir);
		CHECK_EQUAL(false, bRet);
		CHECK_EQUAL(0, gmgr.pFieldMgr->GetDynamicFieldGroupQty());

		bRet = m_pTestPlayer->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_TRIAL, 0, TEST_FIRST_FIELD_ID, vCommonPos, VCommonDir);
		CHECK_EQUAL(false, bRet);
		CHECK_EQUAL(0, gmgr.pFieldMgr->GetDynamicFieldGroupQty());

		bRet = m_pTestPlayer->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_BATTLEARENA, 0, TEST_FIRST_FIELD_ID, vCommonPos, VCommonDir);
		CHECK_EQUAL(false, bRet);
		CHECK_EQUAL(0, gmgr.pFieldMgr->GetDynamicFieldGroupQty());

		bRet = m_pTestPlayer->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_INNROOM, 0, TEST_FIRST_FIELD_ID, vCommonPos, VCommonDir);
		CHECK_EQUAL(false, bRet);
		CHECK_EQUAL(0, gmgr.pFieldMgr->GetDynamicFieldGroupQty());


		bRet = m_pTestPlayer->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_NONE, TEST_FIELD_GROUP_ID, 0, vCommonPos, VCommonDir);
		CHECK_EQUAL(false, bRet);
		CHECK_EQUAL(0, gmgr.pFieldMgr->GetDynamicFieldGroupQty());

		bRet = m_pTestPlayer->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_TRIAL, TEST_FIELD_GROUP_ID, 0, vCommonPos, VCommonDir);
		CHECK_EQUAL(false, bRet);
		CHECK_EQUAL(0, gmgr.pFieldMgr->GetDynamicFieldGroupQty());

		bRet = m_pTestPlayer->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_BATTLEARENA, TEST_FIELD_GROUP_ID, 0, vCommonPos, VCommonDir);
		CHECK_EQUAL(false, bRet);
		CHECK_EQUAL(0, gmgr.pFieldMgr->GetDynamicFieldGroupQty());

		bRet = m_pTestPlayer->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_INNROOM, TEST_FIELD_GROUP_ID, 0, vCommonPos, VCommonDir);
		CHECK_EQUAL(false, bRet);
		CHECK_EQUAL(0, gmgr.pFieldMgr->GetDynamicFieldGroupQty());
	}


	//////////////////////////////////////////////////////////////////////////
	// No StandAlone Mode Test
	struct FGateToDynamicField_NotStandAlone : public FNotStandAlone, FGateToDynamicField, FBaseNetClient
	{
		FGateToDynamicField_NotStandAlone()
		{
		}
		~FGateToDynamicField_NotStandAlone()
		{
		}

		void OnRecv_MMC_FIELD_ADD_DYNAMIC_FIELD_GROUP(MUID uidFieldGroup, MUID uidCreator, int nFieldGroupID, int nFirstFieldID)
		{
			TDMG_DYNAMIC_FIELD_GROUP_INFO paramBlob1;
			paramBlob1.uidDynamicFieldGroup = uidFieldGroup;
			paramBlob1.uidCreator = uidCreator;
			paramBlob1.nType = DFT_TRIAL;
			paramBlob1.nFieldGroupID = nFieldGroupID;
			paramBlob1.nGameServerID = GConfig::m_nMyServerID;

			TDMG_DYNAMIC_FIELD_NODE paramBlob2;
			paramBlob2.uidFieldGroup = uidFieldGroup;
			paramBlob2.uidDynamicField = nFieldGroupID;
			paramBlob2.nFieldID = nFirstFieldID;

			std::vector<TDMG_DYNAMIC_FIELD_NODE > vecDynamicFieldNode;
			vecDynamicFieldNode.push_back(paramBlob2);

			m_pNetAgent->OnRecv(MMC_FIELD_ADD_DYNAMIC_FIELD_GROUP, 3, NEW_SINGLE_BLOB(&paramBlob1, sizeof(TDMG_DYNAMIC_FIELD_GROUP_INFO)), NEW_BLOB(vecDynamicFieldNode), NEW_UINT(0));
		}

		void OnRecv_MMC_PARTY_FIELD_SYNC(MUID uidParty, int nFieldGroupID, MUID uidFieldGroup)
		{
			vector<TD_PARTY_FIELD> vecPartyField;

			TD_PARTY_FIELD tdPartyField;
			tdPartyField.nFieldGroupID = nFieldGroupID;
			tdPartyField.uidFieldGroup = uidFieldGroup;

			vecPartyField.push_back(tdPartyField);

			m_pNetAgent->OnRecv(MMC_PARTY_FIELD_SYNC, 2, NEW_UID(uidParty), NEW_BLOB(vecPartyField));
		}

		GTestMgrWrapper<GBattleArenaMgr>	BattleArenaMgr;
	};

	TEST_FIXTURE(FGateToDynamicField_NotStandAlone, TestGateToDynamicField_NotStandAlone_AskToMasterServerByCommand_TypeTrial)
	{
		bool bRet = m_pTestPlayer->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_TRIAL, TEST_FIELD_GROUP_ID, TEST_FIRST_FIELD_ID, vCommonPos, VCommonDir);
		CHECK_EQUAL(true, bRet);

		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MMC_GATE_ASK_DYNAMIC_FIELD, m_pNetAgent->GetSendCommand(0).GetID());
	}

	TEST_FIXTURE(FGateToDynamicField_NotStandAlone, TestGateToDynamicField_NotStandAlone_AskToMasterServerByCommand_TypeInnRoom)
	{
		bool bRet = m_pTestPlayer->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_INNROOM, TEST_FIELD_GROUP_ID, TEST_FIRST_FIELD_ID, vCommonPos, VCommonDir);
		CHECK_EQUAL(true, bRet);

		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MMC_GATE_ASK_DYNAMIC_FIELD, m_pNetAgent->GetSendCommand(0).GetID());
	}

	TEST_FIXTURE(FGateToDynamicField_NotStandAlone, TestGateToDynamicField_NotStandAlone_Failure)
	{
		bool bRet = m_pTestPlayer->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_TRIAL, TEST_FIELD_GROUP_ID, 0, vCommonPos, VCommonDir);
		CHECK_EQUAL(false, bRet);

		bRet = m_pTestPlayer->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_INNROOM, TEST_FIELD_GROUP_ID, 0, vCommonPos, VCommonDir);
		CHECK_EQUAL(false, bRet);

		bRet = m_pTestPlayer->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_BATTLEARENA, TEST_FIELD_GROUP_ID, 0, vCommonPos, VCommonDir);
		CHECK_EQUAL(false, bRet);

		
		bRet = m_pTestPlayer->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_TRIAL, 0, TEST_FIRST_FIELD_ID, vCommonPos, VCommonDir);
		CHECK_EQUAL(false, bRet);

		bRet = m_pTestPlayer->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_INNROOM, 0, TEST_FIRST_FIELD_ID, vCommonPos, VCommonDir);
		CHECK_EQUAL(false, bRet);

		bRet = m_pTestPlayer->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_BATTLEARENA, 0, TEST_FIRST_FIELD_ID, vCommonPos, VCommonDir);
		CHECK_EQUAL(false, bRet);
	}

	TEST_FIXTURE(FGateToDynamicField_NotStandAlone, TestGateToDynamicField_NotStandAlone_Follow_PartyFieldGroup)
	{
		InitBattleArenaPlayers();

		CHECK_EQUAL(TEST_SHARED_FIELD_ID, m_pPlayerBlueLeader->GetFieldID());
		CHECK_EQUAL(TEST_SHARED_FIELD_ID, m_pPlayerBlueMember1->GetFieldID());

		MUID uidFieldGroup = SUnitTestUtil::NewUID();
		OnRecv_MMC_FIELD_ADD_DYNAMIC_FIELD_GROUP(uidFieldGroup, m_pPlayerBlueLeader->GetUID(), TEST_FIELD_GROUP_ID, TEST_FIRST_FIELD_ID);
		OnRecv_MMC_PARTY_FIELD_SYNC(m_pBlueParty->GetUID(), TEST_FIELD_GROUP_ID, uidFieldGroup);


		TD_GATE_INFO tdGateInfo;
		tdGateInfo.bDynamicField = true;
		tdGateInfo.nFieldID = TEST_FIRST_FIELD_ID;
		tdGateInfo.uidDynamicFieldGroup = uidFieldGroup;
		tdGateInfo.vPosition = vCommonPos;
		tdGateInfo.vDirection = VCommonDir;

		GUTHelper_CmdHandler::OnRecv_MMC_GATE_RES(m_pNetAgent, m_pPlayerBlueLeader->GetUID(), RGT_EXECUTE, &tdGateInfo);
		m_pPlayerBlueLeader->GetPlayerField().GetPlayerFieldEntry().OnEnterFieldComplete();
		CHECK_EQUAL(TEST_FIRST_FIELD_ID, m_pPlayerBlueLeader->GetFieldID());

		m_pPlayerBlueMember1->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_TRIAL, TEST_FIELD_GROUP_ID, TEST_FIRST_FIELD_ID, vCommonPos, VCommonDir);
		m_pPlayerBlueMember1->GetPlayerField().GetPlayerFieldEntry().OnEnterFieldComplete();
		CHECK_EQUAL(TEST_FIRST_FIELD_ID, m_pPlayerBlueMember1->GetFieldID());
	}

}