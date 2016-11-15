#include "stdafx.h"
#include "GWarpInfoChecker.h"
#include "SUnitTestUtil.h"
#include "GTestWrapper.h"
#include "GUTHelper.h"
#include "GUTHelper_Party.h"
#include "GConditionsSystem.h"
#include "GQuestInfoMgr.h"
#include "GPartySystem.h"
#include "GPlayerObjectManager.h"
#include "GMasterServerFacade.h"
#include "GProxyFieldManager.h"
#include "GProxyDynamicFieldMaster.h"
#include "GProxyField.h"
#include "GConfig.h"
#include "GUTHelper_Field.h"
#include "GQuestInfo.h"

SUITE(WarpInfoChecker)
{
	struct ConditionsSystemStub: public GConditionsSystem
	{
		bool m_isCheck;
		bool m_isCheckPartyConjunction;
		bool m_isCheckPartyDisjunction;

		ConditionsSystemStub(): m_isCheck(false), m_isCheckPartyConjunction(false), m_isCheckPartyDisjunction(false)	{}

		virtual bool Check(GEntityPlayer* pPlayer, int nConditionID) override
		{
			return m_isCheck;
		}

		virtual bool CheckForPartyDisjunction(GEntityPlayer* pPlayer, int nConditionID) override
		{
			return m_isCheckPartyDisjunction;
		}
	};

	struct StandAloneModeHelper
	{
		bool m_bOldStandAloneMode;
		StandAloneModeHelper(bool bStandAloneMode)
		{
			m_bOldStandAloneMode = GConfig::m_bStandAlone;
			GConfig::m_bStandAlone = bStandAloneMode;
		}

		~StandAloneModeHelper()
		{
			GConfig::m_bStandAlone = m_bOldStandAloneMode;
		}
	};

	struct Fixture
	{
		GTestSysWrapper2<GConditionsSystem, ConditionsSystemStub>	m_ConditionsSystemWrapper;
		GTestSysWrapper<GPartySystem> m_PartySystemWrapper;
		GTestMgrWrapper<GQuestInfoMgr> m_QuestInfoMgrWrapper;
		GTestSysWrapper<GMasterServerFacade> m_MasterServerFacadeWrapper;
		
		SENSOR_WARP_INFO m_warpInfo;
		GWarpInfoChecker m_warpInfoChecker;
		GEntityPlayer* m_pPlayer;
		GEntityPlayer* m_pOtherPlayer;
		GProxyField* m_pProxyField;

		Fixture(): m_warpInfoChecker(m_warpInfo), m_pPlayer(NULL), m_pOtherPlayer(NULL), m_pProxyField(NULL)
		{
			m_pPlayer = GUTHelper::NewEntityPlayer();
			m_pOtherPlayer = GUTHelper::NewEntityPlayer();
			
			GMasterServerFacade* pMasterServerFacade = m_MasterServerFacadeWrapper.Get();
			pMasterServerFacade->CreateNetwork(0, 0);			
		}

		~Fixture()	
		{	
			SAFE_DELETE(m_pProxyField);
		}

		void SetTrialWarpInfo(int nFieldGroupID, GATE_TYPE eTrailGateType, int nQuestID, int nConditionID)
		{
			m_warpInfo.nFieldGroupID = nFieldGroupID;
			m_warpInfo.eGateType = eTrailGateType;
			m_warpInfo.nQuestID = nQuestID;
			m_warpInfo.nConditionID = nConditionID;
		}

		void SetSharedWarpInfo(int nFieldID, int nConditionID)
		{
			m_warpInfo.nFieldID = nFieldID;
			m_warpInfo.nConditionID = nConditionID;
		}

		void SetCheckResult(bool bResult)
		{
			ConditionsSystemStub* pConditionsSystem = m_ConditionsSystemWrapper.Get();
			pConditionsSystem->m_isCheck = bResult;
		}

		void SetCheckForPartyConjunction(bool bResult)
		{
			ConditionsSystemStub* pConditionsSystem = m_ConditionsSystemWrapper.Get();
			pConditionsSystem->m_isCheckPartyConjunction = bResult;
		}

		void SetCheckForPartyDisjunction(bool bResult)
		{
			ConditionsSystemStub* pConditionsSystem = m_ConditionsSystemWrapper.Get();
			pConditionsSystem->m_isCheckPartyDisjunction = bResult;
		}

		void SetQuestInfo(int nQuestID, QUEST_TRIAL_TYPE eTrialType, int nLimitPersonnel)
		{
			GQuestInfo* pQuestInfo = new GQuestInfo;
			pQuestInfo->nID = nQuestID;
			pQuestInfo->nTrialType = eTrialType;
			pQuestInfo->nLimitPersonnel = nLimitPersonnel;

			GQuestInfoMgr* pQuestInfoMgr = m_QuestInfoMgrWrapper.Get();
			pQuestInfoMgr->Insert(pQuestInfo);
		}

		void SetProxyFieldPlayerCount(MUID uidFieldGroup, int nPlayerCount)
		{
			if (m_pProxyField != NULL)
			{
				m_pProxyField->SetPlayerCount(nPlayerCount);
			}
			else
			{
				FIELD_GROUP_INFO* pFieldGroupInfo = GUTHelper_Field::NewFieldGroupInfo();

				GMasterServerFacade* pMasterServerFacade = m_MasterServerFacadeWrapper.Get();
				GProxyFieldManager* pProxyFieldManager = pMasterServerFacade->GetProxyFieldManager();
				pProxyFieldManager->AddFieldGroup(uidFieldGroup, pFieldGroupInfo, 2, DFT_TRIAL);
							
				m_pProxyField = new GProxyField(MUID(), NULL, 0);
				m_pProxyField->SetPlayerCount(nPlayerCount);

				GProxyDynamicFieldMaster* pProxyFieldGroup = pProxyFieldManager->FindFieldGroup(uidFieldGroup);			
				pProxyFieldGroup->AddDynamicField(m_pProxyField);
			}
		}
	};

	TEST_FIXTURE(Fixture, TestWarpTypeCheck_TrialWarp)
	{
		// arrange
		SetTrialWarpInfo(1, TGT_AUTO, 1, 1);
		SetQuestInfo(1, QTT_PARTY, MAX_PARTY_MEMBER_COUNT);

		// act
		bool isTrialWarp = m_warpInfoChecker.IsTrialWarp();
		bool isSharedWarp = m_warpInfoChecker.IsSharedWarp();

		// assert
		CHECK(isTrialWarp);
		CHECK(!isSharedWarp);
	}

	TEST_FIXTURE(Fixture, TestWarpTypeCheck_SharedWarp)
	{
		// arrange
		SetSharedWarpInfo(1, 1);

		// act
		bool isSharedWarp = m_warpInfoChecker.IsSharedWarp();
		bool isTrialWarp = m_warpInfoChecker.IsTrialWarp();

		// assert
		CHECK(isSharedWarp);
		CHECK(!isTrialWarp);
	}

	TEST_FIXTURE(Fixture, TestWarpTypeCheck_AutoTrialWarp)
	{
		// arrange
		SetTrialWarpInfo(1, TGT_AUTO, 1, 1);
		SetQuestInfo(1, QTT_PARTY, MAX_PARTY_MEMBER_COUNT);

		// act
		bool isAutoTrialWarp = m_warpInfoChecker.IsAutoTrialWarp();
		bool isManualTrialWarp = m_warpInfoChecker.IsManualTrialWarp();

		// assert
		CHECK(isAutoTrialWarp);
		CHECK(!isManualTrialWarp);
	}

	TEST_FIXTURE(Fixture, TestWarpTypeCheck_ManualTrialWarp)
	{
		// arrange
		SetTrialWarpInfo(1, TGT_MANUAL, 1, 1);
		SetQuestInfo(1, QTT_PARTY, MAX_PARTY_MEMBER_COUNT);

		// act
		bool isManualTrialWarp = m_warpInfoChecker.IsManualTrialWarp();
		bool isAutoTrialWarp = m_warpInfoChecker.IsAutoTrialWarp();

		// assert
		CHECK(isManualTrialWarp);
		CHECK(!isAutoTrialWarp);
	}

	TEST_FIXTURE(Fixture, TestCheckCondition_SharedWarp)
	{
		// arrange
		SetSharedWarpInfo(1, 1);
		SetCheckResult(true);

		// act
		bool bCheckResult = m_warpInfoChecker.CheckCondition(m_pPlayer);
		
		// assert
		CHECK(bCheckResult);
	}

	TEST_FIXTURE(Fixture, TestCheckCondition_AutoTrialWarp)		
	{
		// arrange
		SetTrialWarpInfo(1, TGT_AUTO, 1, 1);
		SetQuestInfo(1, QTT_PARTY, MAX_PARTY_MEMBER_COUNT);
		SetCheckForPartyDisjunction(true);

		// act
		bool bCheckResult = m_warpInfoChecker.CheckCondition(m_pPlayer);
		
		// assert
		CHECK(bCheckResult);
	}

	TEST_FIXTURE(Fixture, TestCheckCondition_ManualTrialWarp)	
	{
		// arrange
		SetTrialWarpInfo(1, TGT_MANUAL, 1, 1);
		SetQuestInfo(1, QTT_PARTY, MAX_PARTY_MEMBER_COUNT);
		SetCheckResult(true);

		// act
		bool bCheckResult = m_warpInfoChecker.CheckCondition(m_pPlayer);
		
		// assert
		CHECK(bCheckResult);
	}

	TEST_FIXTURE(Fixture, TestCheckCondition_SingleQuestTrialWarp)
	{
		// arrange
		SetTrialWarpInfo(1, TGT_AUTO, 1, 1);
		SetQuestInfo(1, QTT_SINGLE, MAX_PARTY_MEMBER_COUNT);
		SetCheckResult(true);

		// act
		bool bCheckResult = m_warpInfoChecker.CheckCondition(m_pPlayer);

		// assert
		CHECK(bCheckResult);
	}
	
	TEST_FIXTURE(Fixture, TestCheckQuestField_SharedWarp)
	{
		// arrange
		SetSharedWarpInfo(1, 1);

		// act
		StandAloneModeHelper standAlone(false);
		GWarpInfoChecker::Result checkResult = m_warpInfoChecker.CheckQuestField(m_pPlayer);

		// assert
		CHECK_EQUAL(checkResult, GWarpInfoChecker::SUCCESS);
	}

	TEST_FIXTURE(Fixture, TestCheckQuestField_SingleQuest)
	{
		// arrange
		SetTrialWarpInfo(1, TGT_AUTO, 1, 1);
		SetQuestInfo(1, QTT_SINGLE, MAX_PARTY_MEMBER_COUNT);

		// act
		StandAloneModeHelper standAlone(false);
		GWarpInfoChecker::Result checkResult = m_warpInfoChecker.CheckQuestField(m_pPlayer);

		// assert
		CHECK_EQUAL(checkResult, GWarpInfoChecker::SUCCESS);		
	}

	TEST_FIXTURE(Fixture, TestCheckQuestField_NoParty)	
	{
		// arrange
		SetTrialWarpInfo(1, TGT_AUTO, 1, 1);
		SetQuestInfo(1, QTT_PARTY, MAX_PARTY_MEMBER_COUNT);

		// act
		StandAloneModeHelper standAlone(false);
		GWarpInfoChecker::Result checkResult = m_warpInfoChecker.CheckQuestField(m_pPlayer);

		// assert
		CHECK_EQUAL(checkResult, GWarpInfoChecker::SUCCESS);
	}

	TEST_FIXTURE(Fixture, TestCheckQuestField_NewField)	
	{
		// arrange
		SetTrialWarpInfo(1, TGT_AUTO, 1, 1);
		SetQuestInfo(1, QTT_PARTY, MAX_PARTY_MEMBER_COUNT);
		GParty* pParty = GUTHelper_Party::CreateParty(m_pPlayer);

		// act
		StandAloneModeHelper standAlone(false);
		GWarpInfoChecker::Result checkResult = m_warpInfoChecker.CheckQuestField(m_pPlayer);

		// assert
		CHECK_EQUAL(checkResult, GWarpInfoChecker::SUCCESS);
	}

	TEST_FIXTURE(Fixture, TestCheckQuestField_NewField_LimitPersonnel)	
	{
		// arrange
		int nQuestLimitPersonnel = 1;
		SetTrialWarpInfo(1, TGT_AUTO, 1, 1);		
		SetQuestInfo(1, QTT_PARTY, nQuestLimitPersonnel);		
		GParty* pParty = GUTHelper_Party::CreateParty(m_pPlayer);
		GUTHelper_Party::AddMember(pParty, m_pOtherPlayer);

		// act
		StandAloneModeHelper standAlone(false);
		GWarpInfoChecker::Result checkResult = m_warpInfoChecker.CheckQuestField(m_pPlayer);

		// assert
		CHECK_EQUAL(checkResult, GWarpInfoChecker::FAILURE);
	}

	TEST_FIXTURE(Fixture, TestCheckQuestField_ExistField)	
	{
		// arrange
		SetTrialWarpInfo(1, TGT_AUTO, 1, 1);
		SetQuestInfo(1, QTT_PARTY, MAX_PARTY_MEMBER_COUNT);
		
		MUID uidFieldGroup = SUnitTestUtil::NewUID();
		SetProxyFieldPlayerCount(uidFieldGroup, 1);
		GParty* pParty = GUTHelper_Party::CreateParty(m_pPlayer);	
		pParty->CreateField(1, uidFieldGroup);		

		// act
		StandAloneModeHelper standAlone(false);
		GWarpInfoChecker::Result checkResult = m_warpInfoChecker.CheckQuestField(m_pPlayer);

		// assert
		CHECK_EQUAL(checkResult, GWarpInfoChecker::SUCCESS);
	}

	TEST_FIXTURE(Fixture, TestCheckQuestField_ExistField_FullHouse)	
	{
		// arrange
		SetTrialWarpInfo(1, TGT_AUTO, 1, 1);
		SetQuestInfo(1, QTT_PARTY, MAX_PARTY_MEMBER_COUNT);

		MUID uidFieldGroup = SUnitTestUtil::NewUID();
		SetProxyFieldPlayerCount(uidFieldGroup, MAX_PARTY_MEMBER_COUNT);
		GParty* pParty = GUTHelper_Party::CreateParty(m_pPlayer);
		pParty->CreateField(1, uidFieldGroup);

		// act
		StandAloneModeHelper standAlone(false);
		GWarpInfoChecker::Result checkResult = m_warpInfoChecker.CheckQuestField(m_pPlayer);

		// assert
		CHECK_EQUAL(checkResult, GWarpInfoChecker::FULLHOUSE);
	}
}
