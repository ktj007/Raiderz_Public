#include "stdafx.h"
#include "XUnitTestHelper.h"
#include "XUnitTestUtil.h"
#include "FTestGame.h"

SUITE(TalentTraining)
{
	TEST_FIXTURE(FTestGame, TalentTraining)
	{
		XUnitTestHelper helper;

		// 탤런트 정보 추가
		int ID = XUnitTestUtil::NewID();
		helper.CreateMeleeTalentInfo(ID);

		int nCount = gvar.MyInfo.Talent.GetTalentCount();
		CHECK_EQUAL(nCount, 0);

		m_pNetAgent->OnRecv(MC_TRAINING_TRAIN, 2,
			NEW_INT(ID),
			NEW_BOOL(true));

		nCount = gvar.MyInfo.Talent.GetTalentCount();
		CHECK_EQUAL(nCount, 1);
	}
}