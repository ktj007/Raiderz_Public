#include "stdafx.h"
#include "SUnitTest.h"
#include "GTestForward.h"
#include "GTalentInfo.h"
#include "GActorCooltimeChecker.h"
#include "FBaseGame.h"


SUITE(Talent_Cooltime)
{

	/// 탤런트 테스트를 쉽게 하기 위한 픽스쳐
	class TestTalentFixture : public FBaseGame
	{
	public:
		TestTalentFixture()
		{
		}
		virtual ~TestTalentFixture()
		{
		}
	};

TEST_FIXTURE(TestTalentFixture, LifeCycle)
{
	int nTalentID1 = 777;
	float fCoolTime = 5.0f;

	GActorCooltimeChecker& checker = m_pPlayer->GetActorCooltimeChecker();
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID1));

	GTalentInfo* pTalent1 = test.talent.NewTalentInfo(nTalentID1);
	pTalent1->m_fCoolTime = fCoolTime;

	test.talent.UseTalent(m_pPlayer, pTalent1, m_pPlayer->GetUID());
	CHECK_EQUAL(true, checker.IsCooldown(nTalentID1));

	checker.Update(fCoolTime);
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID1));
}

TEST_FIXTURE(TestTalentFixture, Cooltime_ForTalentLine)
{
	int nTalentLine = 111;
	int nTalentID1 = 777;
	int nTalentID2 = 778;
	int nTalentID3 = 779;
	float fCoolTime = 5.0f;

	GActorCooltimeChecker& checker = m_pPlayer->GetActorCooltimeChecker();
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID1));
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID2));
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID3));

	GTalentInfo* pTalent1 = test.talent.NewTalentInfo(nTalentID1, nTalentLine);
	GTalentInfo* pTalent2 = test.talent.NewTalentInfo(nTalentID2, nTalentLine);
	GTalentInfo* pTalent3 = test.talent.NewTalentInfo(nTalentID3);
	pTalent1->m_fCoolTime = fCoolTime;
	pTalent2->m_fCoolTime = fCoolTime;
	pTalent3->m_fCoolTime = fCoolTime;

	test.talent.UseTalent(m_pPlayer, pTalent1, m_pPlayer->GetUID());
	CHECK_EQUAL(true, checker.IsCooldown(nTalentID1));
	CHECK_EQUAL(true, checker.IsCooldown(nTalentID2));
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID3));

	checker.Update(fCoolTime);
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID1));
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID2));
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID3));
}


TEST_FIXTURE(TestTalentFixture, Cooltime_ForCooltimeLine)
{
	int nCooltimeLine = 111;
	int nTalentID1 = 777;
	int nTalentID2 = 778;
	int nTalentID3 = 779;
	float fCoolTime = 5.0f;

	GActorCooltimeChecker& checker = m_pPlayer->GetActorCooltimeChecker();
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID1));
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID2));
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID3));

	GTalentInfo* pTalent1 = test.talent.NewTalentInfo(nTalentID1, 0, 0, nCooltimeLine);
	GTalentInfo* pTalent2 = test.talent.NewTalentInfo(nTalentID2, 0, 0, nCooltimeLine);
	GTalentInfo* pTalent3 = test.talent.NewTalentInfo(nTalentID3);
	pTalent1->m_fCoolTime = fCoolTime;
	pTalent2->m_fCoolTime = fCoolTime;
	pTalent3->m_fCoolTime = fCoolTime;

	test.talent.UseTalent(m_pPlayer, pTalent1, m_pPlayer->GetUID());
	CHECK_EQUAL(true, checker.IsCooldown(nTalentID1));
	CHECK_EQUAL(true, checker.IsCooldown(nTalentID2));
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID3));

	checker.Update(fCoolTime);
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID1));
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID2));
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID3));
}

TEST_FIXTURE(TestTalentFixture, Cooltime_ComplexCase)
{
	int nTalentLine = 111;
	int nCooltimeLine = 222;
	int nTalentID1 = 777;
	int nTalentID2 = 778;
	int nTalentID3 = 779;
	int nTalentID4 = 780;
	float fCoolTime = 5.0f;

	GActorCooltimeChecker& checker = m_pPlayer->GetActorCooltimeChecker();
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID1));
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID2));
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID3));
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID4));

	GTalentInfo* pTalent1 = test.talent.NewTalentInfo(nTalentID1, nTalentLine, 0, nCooltimeLine);
	GTalentInfo* pTalent2 = test.talent.NewTalentInfo(nTalentID2, nTalentLine, 0, INVALID_ID);
	GTalentInfo* pTalent3 = test.talent.NewTalentInfo(nTalentID3, 0, 0, nCooltimeLine);
	GTalentInfo* pTalent4 = test.talent.NewTalentInfo(nTalentID4);
	pTalent1->m_fCoolTime = fCoolTime;
	pTalent2->m_fCoolTime = fCoolTime;
	pTalent3->m_fCoolTime = fCoolTime;
	pTalent4->m_fCoolTime = fCoolTime;

	test.talent.UseTalent(m_pPlayer, pTalent1, m_pPlayer->GetUID());
	CHECK_EQUAL(true, checker.IsCooldown(nTalentID1));
	CHECK_EQUAL(true, checker.IsCooldown(nTalentID2));
	CHECK_EQUAL(true, checker.IsCooldown(nTalentID3));
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID4));

	checker.Update(fCoolTime);
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID1));
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID2));
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID3));
	CHECK_EQUAL(false, checker.IsCooldown(nTalentID4));
}

}