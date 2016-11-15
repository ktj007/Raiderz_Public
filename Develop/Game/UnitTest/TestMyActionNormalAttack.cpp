#include "stdafx.h"
#include "XMyActionNormalAttack.h"
#include "FMyActionState.h"
#include "XMyPlayer.h"

SUITE(MyActionNormalAttack)
{
	class TestMyActionNormalAttack : public XMyActionNormalAttack
	{
	public:
		using XMyActionNormalAttack::CheckSTA;

		TestMyActionNormalAttack(XModuleMyControl* pOwner) : XMyActionNormalAttack(pOwner) {}
		virtual ~TestMyActionNormalAttack() {}
		
		int m_nMyStatusSTA;
		void ExpectMyStatusSTA(int nSTA)
		{
			m_nMyStatusSTA = nSTA;
		}
		virtual int GetMyStatusSTA() override
		{
			return m_nMyStatusSTA;
		}
	};

	class FMyActionNormalAttack : public FMyActionState
	{
	public:
		FMyActionNormalAttack()
		{
			InitTalentInfo();

			m_pMyActionNormalAttack = new TestMyActionNormalAttack(m_pMyPlayer->GetModuleMyControl());
		}
		~FMyActionNormalAttack()
		{
			delete m_pMyActionNormalAttack;
		}

		void InitTalentInfo()
		{
			m_nTalentID = 1;

			XUnitTestHelper helper;
			m_pTalentInfo = helper.CreateMeleeTalentInfo(m_nTalentID);
		}

		TestMyActionNormalAttack*		m_pMyActionNormalAttack;

		int	m_nTalentID;
		XTalentInfo* m_pTalentInfo;
	};

	TEST_FIXTURE(FMyActionNormalAttack, TestCheckSTA)
	{
		m_pTalentInfo->m_nSTACost = 100;

		m_pMyActionNormalAttack->ExpectMyStatusSTA(5);
		CHECK_EQUAL(false, m_pMyActionNormalAttack->CheckSTA(m_pTalentInfo->m_nID));

		m_pMyActionNormalAttack->ExpectMyStatusSTA(100);
		CHECK_EQUAL(true, m_pMyActionNormalAttack->CheckSTA(m_pTalentInfo->m_nID));

		m_pMyActionNormalAttack->ExpectMyStatusSTA(101);
		CHECK_EQUAL(true, m_pMyActionNormalAttack->CheckSTA(m_pTalentInfo->m_nID));

	}
}