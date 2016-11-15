#include "StdAfx.h"
#include "GTestForward.h"
#include "GUseCostMgr.h"
#include "GFieldMgr.h"
#include "FBasePlayer.h"

SUITE(UseCostMgr)
{
	struct FUseCostMgr : public FBasePlayer
	{
		FUseCostMgr()
		{ 
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pTester = NewEntityPlayer(m_pField, vec3(100,100,0));
			swprintf_s(m_pTester->GetPlayerInfo()->szName, L"Tester");

			m_pSampleTalent = test.talent.NewMeleeTalentInfo();
			m_pSampleTalent->m_nHPCost = 100;
			m_pSampleTalent->m_nENCost = 200;
			m_pSampleTalent->m_nSTACost = 20;
		}

		~FUseCostMgr() 
		{
			m_pField->Destroy();
		}

		void CheckFailed(GTalentInfo* m_pSampleTalent)
		{
			int nOldHP = m_pTester->GetHP();
			int nOldEN = m_pTester->GetEN();
			int nOldSTA = m_pTester->GetSTA();
			CHECK_EQUAL(false, m_UseCostMgr.Pay_TalentCost(m_pTester, m_pSampleTalent));
			CHECK_EQUAL(nOldHP, m_pTester->GetHP());
			CHECK_EQUAL(nOldEN, m_pTester->GetEN());
			CHECK_EQUAL(nOldSTA, m_pTester->GetSTA());
		}

		GTalentInfo*	m_pSampleTalent;
		GUseCostMgr		m_UseCostMgr;
		MockField*		m_pField;
		GEntityPlayer*	m_pTester;

		DECLWRAPPER_Field;
	};

	TEST_FIXTURE(FUseCostMgr, UseCostPay)
	{
		int nOldHP = m_pTester->GetHP();
		int nOldEN = m_pTester->GetEN();
		int nOldSTA = m_pTester->GetSTA();
		CHECK_EQUAL(true, m_UseCostMgr.Pay_TalentCost(m_pTester, m_pSampleTalent));
		CHECK_EQUAL(m_pTester->GetHP(), nOldHP-m_pSampleTalent->m_nHPCost);
		CHECK_EQUAL(m_pTester->GetEN(), nOldEN-m_pSampleTalent->m_nENCost);
		CHECK_EQUAL(m_pTester->GetSTA(), nOldSTA-m_pSampleTalent->m_nSTACost);
	}

	TEST_FIXTURE(FUseCostMgr, RefusePayRequest_HP)
	{
		m_pTester->SetHP(0);
		CheckFailed(m_pSampleTalent);
	}

	TEST_FIXTURE(FUseCostMgr, RefusePayRequest_EN)
	{
		m_pTester->SetEN(0);
		CheckFailed(m_pSampleTalent);
	}

	TEST_FIXTURE(FUseCostMgr, RefusePayRequest_STA)
	{
		m_pTester->SetSTA(0);
		CheckFailed(m_pSampleTalent);
	}
}

