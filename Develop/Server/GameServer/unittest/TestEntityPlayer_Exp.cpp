#include "stdafx.h"
#include "GUTHelper.h"
#include "GUTHelper_Field.h"
#include "GEntityPlayer.h"
#include "MockField.h"
#include "FBaseMockLink.h"
#include "CSBasicAttributeCalculator.h"
#include "GExpSystem.h"

SUITE(EntityPlayer_Exp)
{
	class FEntityPlayer_Exp : public FBaseMockLink
	{
	public:
		FEntityPlayer_Exp()
		{
			pField = GUTHelper_Field::DefaultMockField();			
			pPlayer = GUTHelper::NewEntityPlayer(pField);
		}

		virtual ~FEntityPlayer_Exp()
		{
			pField->Destroy();
		}

		GUTHelper			helper;
		MockField*			pField;		
		GEntityPlayer*		pPlayer;
	};

	TEST_FIXTURE(FEntityPlayer_Exp, CheckPlayerStatus_WhenLevelUp)
	{
		pPlayer->GetPlayerInfo()->nLevel = 1;
		pPlayer->GetPlayerInfo()->nXP = 0;
		pPlayer->GetPlayerInfo()->nFatigueQuantity = 3000;

		gsys.pExpSystem->AddExp(pPlayer, 1050, 0);

		CHECK_EQUAL(pPlayer->GetLevel(), 2);

		int nLevel2_MaxHP = CSBasicAttributeCalculator::CalcMaxHP(2, pPlayer->GetCON());
		int nLevel2_MaxEN = CSBasicAttributeCalculator::CalcMaxEN(2, pPlayer->GetINT());
		int nLevel2_MaxSP = CSBasicAttributeCalculator::CalcMaxSP(2, pPlayer->GetDEX());

		CHECK_EQUAL(nLevel2_MaxHP, pPlayer->GetHP());
		CHECK_EQUAL(nLevel2_MaxEN, pPlayer->GetEN());
		CHECK_EQUAL(nLevel2_MaxSP, pPlayer->GetSTA());
	}

}