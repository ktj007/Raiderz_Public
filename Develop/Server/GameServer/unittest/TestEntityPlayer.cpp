#include "stdafx.h"
#include "GUTHelper.h"
#include "GEntityPlayer.h"
#include "MockField.h"
#include "GModuleCombat.h"
#include "GTalent.h"
#include "MockDBManager.h"
#include "GTestWrapper.h"
#include "GDBManager.h"
#include "FBaseMockLink.h"
#include "GUTHelper_Field.h"
#include "CSBasicAttributeCalculator.h"

SUITE(EntityPlayer)
{
	class MockDatabaseEntityPlayer : public MockDBManager
	{
	public :
		MockDatabaseEntityPlayer() {}

		virtual bool CharLevelUp(GDBT_CHAR_LEVEL_UP_DATA& data) override
		{
			return true;
		}
	};


	struct FEntityPlayer : public FBaseMockLink
	{
		FEntityPlayer()
		{
			pField = GUTHelper_Field::DefaultMockField();			
			pPlayer = GUTHelper::NewEntityPlayer(pField);
		}

		virtual ~FEntityPlayer()
		{
			pField->Destroy();
		}

		typedef GTestSysWrapper2<GDBManager, MockDatabaseEntityPlayer> EntityPlayerMockDB;

		GUTHelper			helper;
		MockField*			pField;		
		GEntityPlayer*		pPlayer;
		EntityPlayerMockDB	MockDB;
	};


// 데미지만 받았을때는 탤런트가 캔슬되지 않는다.
// 탤런트가 캔슬되는 경우는 모션팩터가 발동했을때만이다.
TEST_FIXTURE(FEntityPlayer, doDamageDoNotCanecelTalent)
{
	GTalentInfo* pTalentInfo = helper.NewTalentInfo();
	pTalentInfo->m_nSkillType = ST_MELEE;
	pTalentInfo->m_bNeedTraining = false;
	TALENT_TARGET_INFO targetInfo;
	pPlayer->GetModuleCombat()->UseTalent(pTalentInfo, targetInfo, true);

	pPlayer->doDamage(pPlayer->GetUID(), DA_SLASH, 1);

	CHECK(TALENT_PHASE_CANCELED != pPlayer->GetModuleCombat()->GetTalent()->GetPhase());
}

class MockPlayTimeUpdate : public GPlayTimeUpdater
{
public :
	float GetPlayTimeDelta() { return m_fPlayTimeDelta; }
};

TEST(UpdateCharacterPlayTime)
{
	PLAYER_INFO			pi;
	MockPlayTimeUpdate	updater;

	pi.nPlayTimeSec = 0;

	// 0.5초전.
	updater.Update(0.5f, &pi);
	CHECK_EQUAL(0, pi.nPlayTimeSec );
	// 0.1초전.
	updater.Update(0.4f, &pi);
	CHECK_EQUAL(0, pi.nPlayTimeSec);

	// 1초.
	updater.Update(0.1f, &pi);
	CHECK_EQUAL(1, pi.nPlayTimeSec);	

	CHECK_EQUAL(0.0f, updater.GetPlayTimeDelta());
}

}
