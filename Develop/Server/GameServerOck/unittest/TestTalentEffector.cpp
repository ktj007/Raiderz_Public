#include "stdafx.h"
#include "GTalentEffector.h"
#include "FBaseTalent.h"
#include "MockEntityNPC.h"

SUITE(TalentEffector)
{
	class TestTalentEffector : public GTalentEffector
	{
	public:
		using GTalentEffector::ModifyBackHitTalentInfo;
	};

	class FTalentEffector : public FBaseTalent
	{
	public:
		FTalentEffector()
		{
			
		}
		virtual ~FTalentEffector()
		{

		}

		TestTalentEffector		m_TalentEffector;
		MockEntityNPC			m_Target;
	};

	class FTalentEffector_ModifyBackHitTalentInfo : public FTalentEffector
	{
	public:
		FTalentEffector_ModifyBackHitTalentInfo()
		{
			m_pTalentInfo1 = NewTalentInfo(1);
			m_pTalentInfo1->m_nBackHitTalent = 2;

			m_pBackHitTalentInfo = NewTalentInfo(2);

			NewTalentInfo(3);
		}
		virtual ~FTalentEffector_ModifyBackHitTalentInfo()
		{

		}
		GTalentInfo*		m_pTalentInfo1;
		GTalentInfo*		m_pTalentInfo2;
		GTalentInfo*		m_pBackHitTalentInfo;
	};
	TEST_FIXTURE(FTalentEffector_ModifyBackHitTalentInfo, TestTalentEffector_ModifyBackHitTalentInfo)
	{
		m_Target.m_LastHitinfo.pTalentInfo = m_pTalentInfo1;
		m_Target.m_LastHitinfo.bBackHit = true;

		GTalentInfo* pRetTalentInfo = m_TalentEffector.ModifyBackHitTalentInfo(m_pTalentInfo1, &m_Target);

		CHECK(pRetTalentInfo == m_pBackHitTalentInfo);
	}

	TEST_FIXTURE(FTalentEffector_ModifyBackHitTalentInfo, TestTalentEffector_ModifyBackHitTalentInfo_Negative1)
	{
		m_Target.m_LastHitinfo.pTalentInfo = m_pTalentInfo2;		// 다른 탤런트로 Hit
		m_Target.m_LastHitinfo.bBackHit = true;

		GTalentInfo* pRetTalentInfo = m_TalentEffector.ModifyBackHitTalentInfo(m_pTalentInfo1, &m_Target);

		CHECK(pRetTalentInfo == m_pTalentInfo1);
	}

	TEST_FIXTURE(FTalentEffector_ModifyBackHitTalentInfo, TestTalentEffector_ModifyBackHitTalentInfo_Negative2)
	{
		m_Target.m_LastHitinfo.pTalentInfo = m_pTalentInfo1;
		m_Target.m_LastHitinfo.bBackHit = false;				// BackHit가 아님

		GTalentInfo* pRetTalentInfo = m_TalentEffector.ModifyBackHitTalentInfo(m_pTalentInfo1, &m_Target);

		CHECK(pRetTalentInfo == m_pTalentInfo1);
	}

	TEST_FIXTURE(FTalentEffector_ModifyBackHitTalentInfo, TestTalentEffector_ModifyBackHitTalentInfo_Negative3)
	{
		m_pTalentInfo1->m_nBackHitTalent = 29000;		// BackHit TalentInfo가 없음

		m_Target.m_LastHitinfo.pTalentInfo = m_pTalentInfo1;
		m_Target.m_LastHitinfo.bBackHit = true;

		GTalentInfo* pRetTalentInfo = m_TalentEffector.ModifyBackHitTalentInfo(m_pTalentInfo1, &m_Target);

		CHECK(pRetTalentInfo == m_pTalentInfo1);
	}

	TEST_FIXTURE(FTalentEffector_ModifyBackHitTalentInfo, TestTalentEffector_ModifyBackHitTalentInfo_Negative4)
	{
		// BackHit가 아니지만 m_pTalentInfo1->m_nBackHitTalent값이 0이고, 탤런트 정보 매니저에 0번 탤런트가 버그로 들어가 있음
		GTalentInfo* pTalentInfo0 = NewTalentInfo(0);

		m_pTalentInfo1->m_nBackHitTalent = 0;		

		m_Target.m_LastHitinfo.pTalentInfo = m_pTalentInfo1;
		m_Target.m_LastHitinfo.bBackHit = true;

		GTalentInfo* pRetTalentInfo = m_TalentEffector.ModifyBackHitTalentInfo(m_pTalentInfo1, &m_Target);

		CHECK(pRetTalentInfo == m_pTalentInfo1);
	}

}