#include "stdafx.h"
#include "GTestForward.h"
#include "GFactionInfo.h"
#include "GSharedField.h"
#include "GPlayerFactions.h"
#include "CSFactionCalculator.h"


SUITE(QPTeamCountSender)
{
	struct Fixture
	{
		Fixture()
		{
			vInPVPAreaPos = vec3(0,0,0);
			vOutPVPAreaPos = vec3(-999, -999, 0);

			// 팩션 셋팅
			GFactionInfo* pFactionInfo =  test.faction.NewFactionInfo(0);
			GFactionInfo* pFactionInfo2 = test.faction.NewFactionInfo(0);

			// 필드 셋팅
			pFieldInfo = GUTHelper_Field::NewFieldInfo();
			pFieldInfo->SetFieldMinXY_ForTest(-1000, -1000);
			pFieldInfo->SetFieldMaxXY_ForTest(1000, 1000);
			pFieldInfo->Init(L"", L"");
			PVP_AREA_INFO infoPVP;
			infoPVP.nFaction1 = pFactionInfo->m_nID;
			infoPVP.nFaction2 = pFactionInfo2->m_nID;
			infoPVP.rcArea = MRect(-500, -500, 500, 500);
			pFieldInfo->m_PvPAreas.push_back(infoPVP);
			pFieldInfo->Cooking();

			pSharedField = GUTHelper_Field::NewSharedField(pFieldInfo);
			pPlayer = test.player.NewPlayer(pSharedField, vInPVPAreaPos);
			pPlayer2 = test.player.NewPlayer(pSharedField, vInPVPAreaPos);
			pPlayer->GetPlayerFactions().Increase(pFactionInfo->m_nID, CSFactionCalculator::GetNormalMaxQuantity());
			pPlayer2->GetPlayerFactions().Increase(pFactionInfo2->m_nID, CSFactionCalculator::GetNormalMaxQuantity());

			pLink = test.network.NewLink(pPlayer);
			pLink2 = test.network.NewLink(pPlayer2);
		}

		GFieldInfo*		pFieldInfo;
		GSharedField*	pSharedField;
		GEntityPlayer*	pPlayer;
		GEntityPlayer*	pPlayer2;
		vec3			vOutPVPAreaPos;
		vec3			vInPVPAreaPos;
		MockLink*		pLink;
		MockLink*		pLink2;

		DECLWRAPPER_Field;
		DECLWRAPPER_FactionInfoMgr;
		DECLWRAPPER_FactionRelationInfoMgr;		
	};
	
	
	// 주기가 되기 전에 보내지 않는다.
	TEST_FIXTURE(Fixture, NotPeriod)
	{
		pSharedField->Update(GConst::QUESTPVP_INFO_SENDTIME-0.1f);

		CHECK_EQUAL(0, pLink->GetCommandCount());
		CHECK_EQUAL(0, pLink2->GetCommandCount());
	}

	// 
	TEST_FIXTURE(Fixture, SendTeamCount)
	{
		pSharedField->Update(GConst::QUESTPVP_INFO_SENDTIME);

		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(MC_QUESTPVP_TEAMCOUNT, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(1, pLink->GetParam<int>(0, 0));


		CHECK_EQUAL(1, pLink2->GetCommandCount());
		CHECK_EQUAL(MC_QUESTPVP_TEAMCOUNT, pLink2->GetCommand(0).GetID());
		CHECK_EQUAL(1, pLink2->GetParam<int>(0, 0));
	}
}