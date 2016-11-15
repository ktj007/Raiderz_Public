#include "stdafx.h"
#include "GTestForward.h"
#include "GSharedField.h"
#include "GPlayerPVPArea.h"
#include "FBaseMockLink.h"
#include "GPlayerFactions.h"
#include "CSFactionCalculator.h"
#include "CSDef_QuestPVP.h"

SUITE(FieldPVP)
{
	const uint8 nFaction1 = 88;
	const uint8 nFaction2 = 99;

	struct FFieldPVP : public FBaseMockLink
	{
		FFieldPVP()
		{
			// 필드 셋팅
			pFieldInfo = GUTHelper_Field::NewFieldInfo(777);
			pFieldInfo->SetFieldMinXY_ForTest(-1000, -1000);
			pFieldInfo->SetFieldMaxXY_ForTest(1000, 1000);
			pFieldInfo->Init(L"", L"");
			PVP_AREA_INFO infoPVP;
			infoPVP.nFaction1 = nFaction1;
			infoPVP.nFaction2 = nFaction2;
			infoPVP.rcArea = MRect(-500, -500, 500, 500);
			pFieldInfo->m_PvPAreas.push_back(infoPVP);
			pFieldInfo->Cooking();

			// 팩션 셋팅
			GFactionInfo* pFactionInfo =  test.faction.NewFactionInfo(0, infoPVP.nFaction1);
			GFactionInfo* pFactionInfo2 = test.faction.NewFactionInfo(0, infoPVP.nFaction2);

			pSharedField = GUTHelper_Field::NewSharedField(pFieldInfo);
			pPlayerAttacker = test.player.NewPlayer(pSharedField, vec3(0,0,0));
			pPlayerVictim = test.player.NewPlayer(pSharedField, vec3(0,0,0));
			pPlayerAttacker->GetPlayerFactions().Increase(nFaction1, CSFactionCalculator::GetNormalMaxQuantity());
			pPlayerVictim->GetPlayerFactions().Increase(nFaction2, CSFactionCalculator::GetNormalMaxQuantity());
		}
		virtual ~FFieldPVP()
		{
		}

		GFieldInfo*		pFieldInfo;
		GSharedField*	pSharedField;
		GEntityPlayer*	pPlayerAttacker;
		GEntityPlayer*	pPlayerVictim;

		DECLWRAPPER_Field;
		DECLWRAPPER_FactionInfoMgr;
		DECLWRAPPER_FactionRelationInfoMgr;
	};

	TEST_FIXTURE(FFieldPVP, ChangeFlag)
	{
		// 좌표 0,0
		CHECK_EQUAL(true, pPlayerAttacker->GetPlayerPVPArea().IsLocatedPvPArea());
		
		pPlayerAttacker->SetPos(vec3(-701, -701, 0));
		CHECK_EQUAL(false, pPlayerAttacker->GetPlayerPVPArea().IsLocatedPvPArea());

		pPlayerAttacker->SetPos(vec3(100, 100, 0));
		CHECK_EQUAL(true, pPlayerAttacker->GetPlayerPVPArea().IsLocatedPvPArea());

		pPlayerAttacker->SetPos(vec3(801, 801, 0));
		CHECK_EQUAL(false, pPlayerAttacker->GetPlayerPVPArea().IsLocatedPvPArea());
	}

	TEST_FIXTURE(FFieldPVP, IsHittable)
	{
		// 공격할 수 없는 지역
		pPlayerAttacker->SetPos(vec3(-701, -701, 0));
		pPlayerVictim->SetPos(vec3(-701, -701, 0));
		CHECK_EQUAL(false, pPlayerAttacker->GetPlayerPVPArea().IsLocatedPvPArea());
		CHECK_EQUAL(false, pPlayerAttacker->GetPlayerPVPArea().IsHittable(pPlayerVictim));

		// 공격할 수 있는 지역
		pPlayerAttacker->SetPos(vec3(0, 0, 0));
		pPlayerVictim->SetPos(vec3(0, 0, 0));
		CHECK_EQUAL(true, pPlayerAttacker->GetPlayerPVPArea().IsLocatedPvPArea());
		CHECK_EQUAL(true, pPlayerAttacker->GetPlayerPVPArea().IsHittable(pPlayerVictim));
	}

	TEST_FIXTURE(FFieldPVP, GetTeam)
	{
		// 픽스쳐 설정값
		CHECK_EQUAL(QT_ONE, pPlayerAttacker->GetPlayerPVPArea().GetTeam());
		CHECK_EQUAL(QT_TWO, pPlayerVictim->GetPlayerPVPArea().GetTeam());

		// 팩션이 없을 경우 팀이 없음
		pPlayerAttacker->GetPlayerFactions().Decrease(nFaction1, CSFactionCalculator::GetNormalMaxQuantity());
		CHECK_EQUAL(QT_NONE, pPlayerAttacker->GetPlayerPVPArea().GetTeam());

		// 둘 다 충분한 팩션값일 경우 더 높은 팩션의 팀이 선택
		pPlayerVictim->GetPlayerFactions().Increase(nFaction1, CSFactionCalculator::GetNormalMaxQuantity()+1);
		CHECK_EQUAL(QT_ONE, pPlayerVictim->GetPlayerPVPArea().GetTeam());
	}

	TEST_FIXTURE(FFieldPVP, CheckHit)
	{
		GTalentInfo* pAttackTalent = test.talent.MakeDamageTalent(10); 
		pAttackTalent->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ENEMY;

		// 공격할 수 없는 지역
		pPlayerAttacker->SetPos(vec3(-701, -701, 0));
		pPlayerVictim->SetPos(vec3(-701, -701, 0));
		CHECK_EQUAL(false, pPlayerAttacker->GetPlayerPVPArea().IsLocatedPvPArea());
		pPlayerVictim->SetHP(100);
		test.talent.UseTalent(pPlayerAttacker, pAttackTalent, pPlayerVictim->GetUID());
		CHECK_EQUAL(100, pPlayerVictim->GetHP());

		// 공격할 수 있는 지역
		pPlayerAttacker->SetPos(vec3(0, 0, 0));
		pPlayerVictim->SetPos(vec3(0, 0, 0));
		CHECK_EQUAL(true, pPlayerAttacker->GetPlayerPVPArea().IsLocatedPvPArea());
		pPlayerVictim->SetHP(100);
		test.talent.UseTalent(pPlayerAttacker, pAttackTalent, pPlayerVictim->GetUID());
		CHECK_EQUAL(90, pPlayerVictim->GetHP());
	}

	TEST_FIXTURE(FFieldPVP, MakeTDCacheInfo)
	{
		// 픽스쳐 설정값
		CHECK_EQUAL(QT_ONE, pPlayerAttacker->GetPlayerPVPArea().GetTeam());
		CHECK_EQUAL(QT_TWO, pPlayerVictim->GetPlayerPVPArea().GetTeam());

		TD_UPDATE_CACHE_PLAYER cache;
		pPlayerAttacker->MakeTDCacheInfo(cache);
		CHECK(CheckBitSet(cache.nStatusFlag, UPS_FIELDPVP_TEAM1));
		CHECK(!CheckBitSet(cache.nStatusFlag, UPS_FIELDPVP_TEAM2));
		pPlayerVictim->MakeTDCacheInfo(cache);
		CHECK(!CheckBitSet(cache.nStatusFlag, UPS_FIELDPVP_TEAM1));
		CHECK(CheckBitSet(cache.nStatusFlag, UPS_FIELDPVP_TEAM2));
	}
}