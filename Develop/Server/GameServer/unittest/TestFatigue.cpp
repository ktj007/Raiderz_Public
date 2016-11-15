#include "stdafx.h"
#include "GUTHelper.h"
#include "MockField.h"
#include "GFatigueSystem.h"
#include "GFieldMgr.h"
#include "GUTHelper_Field.h"
#include "CSDef_Fatigue.h"

SUITE(FatigueSystem)
{
	TEST(GetFatigueType)
	{
		MockField* pField = GUTHelper_Field::DefaultMockField();
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(pField);

		pPlayer->GetPlayerInfo()->nFatigueQuantity = WORST_FATIGUE;
		CHECK_EQUAL(FT_TOO_BAD, gsys.pFatigueSystem->GetFatigueType(pPlayer->GetPlayerInfo()->nFatigueQuantity));

		pPlayer->GetPlayerInfo()->nFatigueQuantity = WORST_FATIGUE + 1;
		CHECK_EQUAL(FT_BAD, gsys.pFatigueSystem->GetFatigueType(pPlayer->GetPlayerInfo()->nFatigueQuantity));

		pPlayer->GetPlayerInfo()->nFatigueQuantity = BAD_FATIGUE;
		CHECK_EQUAL(FT_BAD, gsys.pFatigueSystem->GetFatigueType(pPlayer->GetPlayerInfo()->nFatigueQuantity));

		pPlayer->GetPlayerInfo()->nFatigueQuantity = BAD_FATIGUE + 1;
		CHECK_EQUAL(FT_NORMAL, gsys.pFatigueSystem->GetFatigueType(pPlayer->GetPlayerInfo()->nFatigueQuantity));

		pPlayer->GetPlayerInfo()->nFatigueQuantity = NORMAL_FATIGUE;
		CHECK_EQUAL(FT_NORMAL, gsys.pFatigueSystem->GetFatigueType(pPlayer->GetPlayerInfo()->nFatigueQuantity));

		pPlayer->GetPlayerInfo()->nFatigueQuantity = NORMAL_FATIGUE + 1;
		CHECK_EQUAL(FT_GOOD, gsys.pFatigueSystem->GetFatigueType(pPlayer->GetPlayerInfo()->nFatigueQuantity));

		pPlayer->GetPlayerInfo()->nFatigueQuantity = GOOD_FATIGUE;
		CHECK_EQUAL(FT_GOOD, gsys.pFatigueSystem->GetFatigueType(pPlayer->GetPlayerInfo()->nFatigueQuantity));

		pPlayer->GetPlayerInfo()->nFatigueQuantity = GOOD_FATIGUE + 1;
		CHECK_EQUAL(FT_VERY_GOOD, gsys.pFatigueSystem->GetFatigueType(pPlayer->GetPlayerInfo()->nFatigueQuantity));

		pPlayer->GetPlayerInfo()->nFatigueQuantity = BEST_FATIGUE;
		CHECK_EQUAL(FT_VERY_GOOD, gsys.pFatigueSystem->GetFatigueType(pPlayer->GetPlayerInfo()->nFatigueQuantity));

		pField->Destroy();
	}

	TEST(SetFatigueByCommand)
	{
		MockField* pField = GUTHelper_Field::DefaultMockField();
		const_cast<GFieldInfo*>(pField->GetInfo())->m_nType = FIELDTYPE_TOWN;
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(pField);
		pPlayer->GetPlayerInfo()->nFatigueQuantity = 1000;

		gsys.pFatigueSystem->SetFatigueByCommand(pPlayer, 50);
		CHECK_EQUAL(BEST_FATIGUE * 50 / 100, pPlayer->GetPlayerInfo()->nFatigueQuantity);

		gsys.pFatigueSystem->SetFatigueByCommand(pPlayer, 110);
		CHECK(BEST_FATIGUE >= pPlayer->GetPlayerInfo()->nFatigueQuantity);

		pField->Destroy();
	}

	TEST(IncFatigueByBlankTime)
	{
		MockField* pField = GUTHelper_Field::DefaultMockField();
		const_cast<GFieldInfo*>(pField->GetInfo())->m_nType = FIELDTYPE_TOWN;
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(pField);
		pPlayer->GetPlayerInfo()->nFatigueQuantity = 1000;

		int nBeforeFatigue = pPlayer->GetPlayerInfo()->nFatigueQuantity;
		gsys.pFatigueSystem->IncFatigueByBlankTime(pPlayer, 5);
		CHECK_EQUAL(nBeforeFatigue + 5*INC_FATIGUE_PER_HOUR_BY_BLANKTIME, pPlayer->GetPlayerInfo()->nFatigueQuantity);

		gsys.pFatigueSystem->IncFatigueByBlankTime(pPlayer, INT_MAX);
		CHECK(BEST_FATIGUE >= pPlayer->GetPlayerInfo()->nFatigueQuantity);

		pField->Destroy();
	}

	namespace DecFatigueByTime
	{
		TEST(NotHuntingGround)
		{
			MockField* pField = GUTHelper_Field::DefaultMockField();
			const_cast<GFieldInfo*>(pField->GetInfo())->m_nType = FIELDTYPE_TOWN;
			GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(pField);
			pPlayer->GetPlayerInfo()->nFatigueQuantity = 1000;

			int nBeforeFatigue = pPlayer->GetPlayerInfo()->nFatigueQuantity;
			pPlayer->Update(59.9f);
			CHECK_EQUAL(pPlayer->GetPlayerInfo()->nFatigueQuantity, nBeforeFatigue);
			pPlayer->Update(0.1f);
			CHECK_EQUAL(pPlayer->GetPlayerInfo()->nFatigueQuantity, nBeforeFatigue - DEC_FATIGUE_PER_MINUTE_ON_NOT_HUNTING_GROUND);
			pPlayer->Update(60.0f);
			CHECK_EQUAL(pPlayer->GetPlayerInfo()->nFatigueQuantity, nBeforeFatigue - (DEC_FATIGUE_PER_MINUTE_ON_NOT_HUNTING_GROUND * 2));
			CHECK(pPlayer->GetPlayerInfo()->nFatigueQuantity >= WORST_FATIGUE);

			pField->Destroy();
		}

		TEST(HuntingGround)
		{
			MockField* pField = GUTHelper_Field::DefaultMockField();
			const_cast<GFieldInfo*>(pField->GetInfo())->m_nType = FIELDTYPE_HUNTING_GROUND;
			GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(pField);
			pPlayer->GetPlayerInfo()->nFatigueQuantity = 1000;

			int nBeforeFatigue = pPlayer->GetPlayerInfo()->nFatigueQuantity;
			pPlayer->Update(59.9f);
			CHECK_EQUAL(pPlayer->GetPlayerInfo()->nFatigueQuantity, nBeforeFatigue);
			pPlayer->Update(0.1f);
			CHECK_EQUAL(pPlayer->GetPlayerInfo()->nFatigueQuantity, nBeforeFatigue - DEC_FATIGUE_PER_MINUTE_ON_HUNTING_GROUND);
			pPlayer->Update(60.0f);
			CHECK_EQUAL(pPlayer->GetPlayerInfo()->nFatigueQuantity, nBeforeFatigue - (DEC_FATIGUE_PER_MINUTE_ON_HUNTING_GROUND * 2));
			CHECK(pPlayer->GetPlayerInfo()->nFatigueQuantity >= WORST_FATIGUE);

			pField->Destroy();
		}
	}
}