#include "stdafx.h"
#include "GPlayerFieldGateRetryer.h"
#include "GUTHelper.h"
#include "GUTHelper_Field.h"
#include "FBasePlayer.h"
#include "FBaseMockLink.h"
#include "FNotStandAlone.h"
#include "FBaseNetClient.h"
#include "FBaseFieldInfo.h"
#include "FBaseFieldMgr.h"
#include "GEntityPlayer.h"
#include "MockField.h"
#include "SUnitTestUtil.h"
#include "GSharedField.h"
#include "CCommandTable_Master.h"
#include "GPlayerFieldShared.h"
#include "GPlayerFieldDynamic.h"


SUITE(PlayerFieldGateRetryer)
{
	vec3 vCommonPos = vec3(1000, 1000, 0);
	vec3 vCommonDir = vec3(0, 0, 1);

	struct Fixture : public FNotStandAlone, FBasePlayer, FBaseMockLink, FBaseNetClient, FBaseFieldInfo, FBaseFieldMgr
	{
		Fixture()
		{
			InitTestField();
			InitTestPlayer();

			m_pNetAgent->ResetSendCommands();
		}
		~Fixture()
		{

		}
		void InitTestField()
		{
			m_pField = GUTHelper_Field::NewSharedField();
			m_pDestField = GUTHelper_Field::NewSharedField();
		}
		void InitTestPlayer()
		{
			
			m_pPlayer = GUTHelper::NewEntityPlayer(m_pField);
			m_pLink = NewLink(m_pPlayer);

			m_SUT_pGateRetryer = &m_pPlayer->GetPlayerField().GetPlayerFieldGateRetryer();
		}

		GSharedField*	m_pField;
		GSharedField*	m_pDestField;
		GEntityPlayer*	m_pPlayer;
		MockLink*		m_pLink;

		GPlayerFieldGateRetryer* m_SUT_pGateRetryer;
	};

	TEST_FIXTURE(Fixture, TestPlayerFieldGateRetryer_Fixture)
	{
		ASSERT_CHECK(NULL != m_SUT_pGateRetryer);
		CHECK_EQUAL(false, m_SUT_pGateRetryer->IsRunRetryer());
	}
	
	TEST_FIXTURE(Fixture, TestPlayerFieldGateRetryer_StartRetryer)
	{
		/// SUT 실행
		m_SUT_pGateRetryer->StartRetryer(m_pDestField->GetID(), vCommonPos, vCommonDir);

		/// 검증
		CHECK_EQUAL(true, m_SUT_pGateRetryer->IsRunRetryer());
	}

	TEST_FIXTURE(Fixture, TestPlayerFieldGateRetryer_Update_Failure_NotRunRetryer)
	{
		CHECK_EQUAL(false, m_SUT_pGateRetryer->IsRunRetryer());

		/// SUT 실행
		m_SUT_pGateRetryer->Update(GPlayerFieldGateRetryer::TIME_TO_UPDATE_SEC);

		/// 검증
		CHECK_EQUAL(false, m_SUT_pGateRetryer->IsRunRetryer());
		CHECK_EQUAL(0, m_pNetAgent->GetSendCommandCount());
	}

	TEST_FIXTURE(Fixture, TestPlayerFieldGateRetryer_Update_Failure_LackTime)
	{
		m_SUT_pGateRetryer->StartRetryer(m_pDestField->GetID(), vCommonPos, vCommonDir);

		/// SUT 실행
		m_SUT_pGateRetryer->Update(0.0f);

		/// 검증
		CHECK_EQUAL(true, m_SUT_pGateRetryer->IsRunRetryer());
		CHECK_EQUAL(0, m_pNetAgent->GetSendCommandCount());
	}

	TEST_FIXTURE(Fixture, TestPlayerFieldGateRetryer_Update_Success)
	{
		/// Fixture 설치
		m_SUT_pGateRetryer->StartRetryer(m_pDestField->GetID(), vCommonPos, vCommonDir);

		/// SUT 실행
		m_SUT_pGateRetryer->Update(GPlayerFieldGateRetryer::TIME_TO_UPDATE_SEC);

		/// 검증
		CHECK_EQUAL(false, m_SUT_pGateRetryer->IsRunRetryer());
		ASSERT_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MMC_GATE_ASK_SHARED_FIELD, m_pNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(m_pPlayer->GetUID(), m_pNetAgent->GetSendParam<MUID>(0, 0));

		TD_GATE_INFO* pGateInfo;
		m_pNetAgent->GetSendCommand(0).GetSingleBlob(pGateInfo, 1);
		CHECK_EQUAL(m_pDestField->GetID(), pGateInfo->nFieldID);
		CHECK_EQUAL(vCommonPos, pGateInfo->vPosition);
		CHECK_EQUAL(vCommonDir, pGateInfo->vDirection);
	}

	TEST_FIXTURE(Fixture, TestPlayerFieldGateRetryer_StartRetryer_NotAllowedToOveralappedGate)
	{
		/// Fixture 설치
		m_SUT_pGateRetryer->StartRetryer(m_pDestField->GetID(), vCommonPos, vCommonDir);
		CHECK_EQUAL(true, m_SUT_pGateRetryer->IsRunRetryer());

		/// SUT 실행
		bool bRet = m_pPlayer->GetPlayerField().GetPlayerFieldShared().Gate(m_pDestField->GetID(), PRIME_CHANNELID, vCommonPos, vCommonDir);

		/// 검증
		CHECK_EQUAL(false, bRet);
		CHECK_EQUAL(0, m_pNetAgent->GetSendCommandCount());

		/// SUT 실행
		int nFieldGroupID = SUnitTestUtil::NewID();
		int nFieldID = SUnitTestUtil::NewID();
		bRet = m_pPlayer->GetPlayerField().GetPlayerFieldDynamic().Gate(DFT_TRIAL, nFieldGroupID, nFieldID, vCommonPos, vCommonDir);

		/// 검증
		CHECK_EQUAL(false, bRet);
		CHECK_EQUAL(0, m_pNetAgent->GetSendCommandCount());
	}

}