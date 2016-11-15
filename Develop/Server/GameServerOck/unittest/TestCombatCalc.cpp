#include "stdafx.h"
#include "GMath.h"
#include "GCalculator.h"
#include "GCombatCalculator.h"
#include "GDodgeCalculator.h"
#include "GResistCalculator.h"
#include "GDefenseFactorCalculator.h"
#include "GGuardCalculator.h"
#include "GCriticalCalculator.h"
#include "GUTHelper.h"
#include "GTestWrapper.h"
#include "SUnitTestUtil.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GItemSystem.h"
#include "GItemManager.h"
#include "GTestForward.h"
#include "FBasePlayer.h"
#include "GSharedField.h"
#include "GItemEquiper.h"
#include "GTestForward.h"

SUITE(CombatCalc)
{
	TEST(TestCalc_Round4Combat)
	{
		CHECK_CLOSE(GMath::Round4Combat(10.1234f, 1), 10.1f, 0.0001f);
		CHECK_CLOSE(GMath::Round4Combat(10.1234f, 0), 10.0f, 0.0001f);
		CHECK_CLOSE(GMath::Round4Combat(10.123456f, 3), 10.123f, 0.0001f);
		CHECK_CLOSE(GMath::Round4Combat(10.5678f, 2), 10.57f, 0.0001f);
		CHECK_CLOSE(GMath::Round4Combat(-5.5432f, 2), -5.54f, 0.0001f);
	}

	struct TestCombatCalcFixture : public FBasePlayer
	{
		TestCombatCalcFixture()
		{
			m_pField = GUTHelper_Field::DefaultMockField();

			m_pTalentInfo = m_Helper.NewMeleeTalentInfo(1);

			m_NPCInfo[0].nID = 1;
			m_NPCInfo[1].nID = 2;

			for (int i = 0; i < 2; i++)
			{
				m_ppPlayer[i] = NewEntityPlayer(m_pField, vec3(100,100,0));
				m_ppPlayerInfo[i] = m_ppPlayer[i]->GetPlayerInfo();

				m_ppNPC[i] = m_Helper.NewEntityNPC(m_pField);
				m_ppNPC[i]->InitFromNPCInfo(&m_NPCInfo[i], NULL, 0);
			}

		}
		~TestCombatCalcFixture()
		{
			m_pField->Destroy();
		}

		void DisallowVictimDodgePercent(GEntityActor* pActor)
		{
			// 점프, 이동, 가드, IDLE때만 계산이 허용되므로,
			// 점프, 이동, 가드 상태가 아니며 IDLE 상태도 아니면 계산하지 않는다.

			test.talent.CancelTalent(pActor);
			test.talent.UseTalent(pActor, m_pTalentInfo);

			pActor->doLand();
			pActor->doGuardReleased();
			pActor->doStop();
		}

		GUTHelper		m_Helper;
		GTalentInfo*	m_pTalentInfo;
		GNPCInfo		m_NPCInfo[2];
		GEntityPlayer*	m_ppPlayer[2];
		PLAYER_INFO*	m_ppPlayerInfo[2];
		GEntityNPC*		m_ppNPC[2];
		MockField*		m_pField;

		DECLWRAPPER_FieldInfoMgr;
		DECLWRAPPER_FieldMgr;
		DECLWRAPPER_ItemMgr;
		GTestMgrWrapper<GTalentInfoMgr>	m_TalentInfoMgrWrapper;
		GTestMgrWrapper<GItemManager>	m_ItemInfoMgrWrapper;
		GTestSysWrapper<GItemSystem>	m_ItemSystemWrapper;
	};

	TEST_FIXTURE(TestCombatCalcFixture, TestCalc_Dodge)
	{
		// MDN 예제 1
		m_ppPlayerInfo[0]->nDEX = 100;
		m_ppPlayerInfo[0]->nLevel = 50;

		m_ppPlayerInfo[1]->nDEX = 100;
		m_ppPlayerInfo[1]->nLevel = 50;

		m_pTalentInfo->m_nDodgeDifficulty = 230;

		CHECK_CLOSE(4.75f, GDodgeCalculator::CalcDodgePercent(m_ppPlayer[0], m_ppPlayer[1], m_pTalentInfo), 0.001f);


		// MDN 예제 2
		m_NPCInfo[0].nGrade = 7;
		m_NPCInfo[0].nLevel = 50;

		m_ppPlayerInfo[0]->nDEX = 100;
		m_ppPlayerInfo[0]->nLevel = 50;

		m_pTalentInfo->m_nDodgeDifficulty = 230;

		// 레벨에 따른 스탯 보정치
		CHECK_CLOSE(6.25f, GDodgeCalculator::CalcDodgePercent(m_ppNPC[0], m_ppPlayer[0], m_pTalentInfo), 0.001f);

		// MDN 예제 3
		m_NPCInfo[0].nGrade = 7;
		m_NPCInfo[0].nLevel = 50;

		m_ppPlayerInfo[0]->nDEX = 255;
		m_ppPlayerInfo[0]->nLevel = 50;

		m_pTalentInfo->m_nDodgeDifficulty = 230;

		// 레벨에 따른 스탯 보정치
		CHECK_CLOSE(14.25f, GDodgeCalculator::CalcDodgePercent(m_ppNPC[0], m_ppPlayer[0], m_pTalentInfo), 0.001f);
		
		// MDN 예제 4
		m_NPCInfo[0].nGrade = 3;
		m_NPCInfo[0].nLevel = 30;

		m_ppPlayerInfo[0]->nDEX = 100;
		m_ppPlayerInfo[0]->nLevel = 30;

		m_pTalentInfo->m_nDodgeDifficulty = 230;

		// 레벨에 따른 스탯 보정치
		CHECK_CLOSE(9.25f, GDodgeCalculator::CalcDodgePercent(m_ppNPC[0], m_ppPlayer[0], m_pTalentInfo), 0.001f);

	}

	class TestDodgeCalculator: public GDodgeCalculator
	{
	public:
		using GDodgeCalculator::CalcAttackTalentDodgePercent;
		using GDodgeCalculator::CalcLevelFactorDodgePercent;
		using GDodgeCalculator::CalcAttackerDodgePercent;
		using GDodgeCalculator::CalcVictimDodgePercent;
	};

	TEST_FIXTURE(TestCombatCalcFixture, TestCalcAttackTalentDodgePercent)
	{
		GTalentInfo* pTalentInfo = test.talent.NewTalentInfo();		
		
		pTalentInfo->m_nDodgeDifficulty = 0;
		CHECK_EQUAL(63.75f, TestDodgeCalculator::CalcAttackTalentDodgePercent(pTalentInfo));
		
		// 권장 최하선

		pTalentInfo->m_nDodgeDifficulty = 220;
		CHECK_EQUAL(8.75f, TestDodgeCalculator::CalcAttackTalentDodgePercent(pTalentInfo));

		// 권장 230 ~ 255

		pTalentInfo->m_nDodgeDifficulty = 230;
		CHECK_EQUAL(6.25f, TestDodgeCalculator::CalcAttackTalentDodgePercent(pTalentInfo));

		pTalentInfo->m_nDodgeDifficulty = 255;
		CHECK_EQUAL(0.0f, TestDodgeCalculator::CalcAttackTalentDodgePercent(pTalentInfo));
	}

	TEST_FIXTURE(TestCombatCalcFixture, TestCalcLevelFactorDodgePercent)
	{
		m_ppPlayer[0]->SetLevel(20);
		m_ppPlayer[1]->SetLevel(10);

		CHECK_EQUAL(-3.0f, TestDodgeCalculator::CalcLevelFactorDodgePercent(m_ppPlayer[0], m_ppPlayer[1]));


		m_ppPlayer[0]->SetLevel(10);
		m_ppPlayer[1]->SetLevel(10);

		CHECK_EQUAL(-1.5f, TestDodgeCalculator::CalcLevelFactorDodgePercent(m_ppPlayer[0], m_ppPlayer[1]));


		m_ppPlayer[0]->SetLevel(10);
		m_ppPlayer[1]->SetLevel(20);

		CHECK_EQUAL(0.0f, TestDodgeCalculator::CalcLevelFactorDodgePercent(m_ppPlayer[0], m_ppPlayer[1]));
	}

	TEST_FIXTURE(TestCombatCalcFixture, TestCalcAttackerDodgePercent_ForPlayer)
	{
		// -3% ~ 10%

		m_ppPlayerInfo[0]->nDEX = 1;
		CHECK_EQUAL(-3.0f, TestDodgeCalculator::CalcAttackerDodgePercent(m_ppPlayer[0]));

		m_ppPlayerInfo[0]->nDEX = 100;
		CHECK_EQUAL(2.0f, TestDodgeCalculator::CalcAttackerDodgePercent(m_ppPlayer[0]));

		m_ppPlayerInfo[0]->nDEX = 255;
		CHECK_EQUAL(10.0f, TestDodgeCalculator::CalcAttackerDodgePercent(m_ppPlayer[0]));
	}
	
	TEST_FIXTURE(TestCombatCalcFixture, TestCalcAttackerDodgePercent_ForNPC)
	{
		m_NPCInfo[0].nLevel = 50;
		m_NPCInfo[0].nGrade = 10;		

		CHECK_EQUAL(7.0f, TestDodgeCalculator::CalcAttackerDodgePercent(m_ppNPC[0]));


		m_NPCInfo[0].nLevel = 50;
		m_NPCInfo[0].nGrade = 5;		

		CHECK_EQUAL(2.0f, TestDodgeCalculator::CalcAttackerDodgePercent(m_ppNPC[0]));
	}

	TEST_FIXTURE(TestCombatCalcFixture, TestCalcVictimeDodgePercent_ForPlayer)
	{
		// 기본적으로 공격자 회피율 계산과 동일; -3% ~ 10%
		m_ppPlayerInfo[0]->nDEX = 1;
		CHECK_EQUAL(-3.0f, TestDodgeCalculator::CalcVictimDodgePercent(m_ppPlayer[0]));

		m_ppPlayerInfo[0]->nDEX = 100;
		CHECK_EQUAL(2.0f, TestDodgeCalculator::CalcVictimDodgePercent(m_ppPlayer[0]));

		m_ppPlayerInfo[0]->nDEX = 255;
		CHECK_EQUAL(10.0f, TestDodgeCalculator::CalcVictimDodgePercent(m_ppPlayer[0]));


		// 점프, 가드, 이동, IDLE 중 해당사항 없으면 계산하지 않는다.
		DisallowVictimDodgePercent(m_ppPlayer[0]);
		CHECK_EQUAL(0.0f, TestDodgeCalculator::CalcVictimDodgePercent(m_ppPlayer[0]));
		
		// 이동 중이면 계산
		DisallowVictimDodgePercent(m_ppPlayer[0]);
		m_ppPlayer[0]->doMove();
		CHECK_EQUAL(10.0f, TestDodgeCalculator::CalcVictimDodgePercent(m_ppPlayer[0]));

		// 점프 중이면 계산
		DisallowVictimDodgePercent(m_ppPlayer[0]);
		m_ppPlayer[0]->doJump();
		CHECK_EQUAL(10.0f, TestDodgeCalculator::CalcVictimDodgePercent(m_ppPlayer[0]));

		// 가드 중이면 계산
		DisallowVictimDodgePercent(m_ppPlayer[0]);
		GTalentInfo* pGuardTalentInfo = test.talent.NewGuardTalentInfo();
		m_ppPlayer[0]->doGuard(pGuardTalentInfo->m_nID);
		CHECK_EQUAL(10.0f, TestDodgeCalculator::CalcVictimDodgePercent(m_ppPlayer[0]));
		
		// IDLE이면 계산
		DisallowVictimDodgePercent(m_ppPlayer[0]);
		test.talent.CancelTalent(m_ppPlayer[0]);		
		CHECK_EQUAL(10.0f, TestDodgeCalculator::CalcVictimDodgePercent(m_ppPlayer[0]));
	}

	TEST_FIXTURE(TestCombatCalcFixture, TestCalcVictimeDodgePercent_ForNPC)
	{
		// NPC는 방어자 회피율 계산 허용조건에 상관없이 항상 허용
		DisallowVictimDodgePercent(m_ppNPC[0]);


		m_NPCInfo[0].nLevel = 50;
		m_NPCInfo[0].nGrade = 10;		

		CHECK_EQUAL(7.0f, TestDodgeCalculator::CalcVictimDodgePercent(m_ppNPC[0]));


		m_NPCInfo[0].nLevel = 50;
		m_NPCInfo[0].nGrade = 5;		

		CHECK_EQUAL(2.0f, TestDodgeCalculator::CalcVictimDodgePercent(m_ppNPC[0]));
	}

	TEST(Test_TRT_FR_Is_Zero)
	{
		CHECK_EQUAL(0, TRT_FR);
	}

	TEST_FIXTURE(TestCombatCalcFixture, TestCalc_Resist)
	{
		// MDN 예제 1
		m_ppPlayerInfo[0]->nLevel = 30;
		m_ppPlayerInfo[1]->nLevel = 30;

		m_pTalentInfo->m_Resist.m_nMethod = TRM_BINARY;
		m_pTalentInfo->m_Resist.m_nDifficulty = 200;

		for (int i = TRT_FR; i <= TRT_UR; i++)
		{
			m_ppPlayerInfo[1]->Resist.nValue[i] = 200;

			m_pTalentInfo->m_Resist.m_nType = static_cast<TALENT_RESIST_TYPE>(i);
			CHECK_CLOSE(GResistCalculator::CalcResistPercent(m_ppPlayer[0], m_ppPlayer[1], m_pTalentInfo->m_Resist), 26.48f, 0.001f);
		}

		// MDN 예제 2
		m_NPCInfo[0].nGrade = 5;
		m_NPCInfo[0].nLevel = 30;

		m_ppPlayerInfo[0]->nLevel = 30;

		m_pTalentInfo->m_Resist.m_nMethod = TRM_BINARY;
		m_pTalentInfo->m_Resist.m_nDifficulty = 200;

		for (int i = TRT_FR; i <= TRT_UR; i++)
		{
			m_ppPlayerInfo[0]->Resist.nValue[i] = 200;

			m_pTalentInfo->m_Resist.m_nType = static_cast<TALENT_RESIST_TYPE>(i);
			CHECK_CLOSE(GResistCalculator::CalcResistPercent(m_ppNPC[0], m_ppPlayer[0], m_pTalentInfo->m_Resist), 19.96f, 0.001f);
		}
	}

	TEST_FIXTURE(TestCombatCalcFixture, TestCalc_Critical)
	{
		m_NPCInfo[0].nGrade = 3;
		m_NPCInfo[0].nLevel = 30;
		m_NPCInfo[0].m_fCriticalMelee = 0.1f;
		m_NPCInfo[0].m_fCriticalRange = 0.1f;
		m_NPCInfo[0].m_fCriticalMagic = 0.1f;

		m_ppPlayerInfo[0]->nLevel = 30;

		GCriticalCalculator criticalCalculator;
		CHECK_CLOSE(criticalCalculator.CalcCriticalPercent(m_ppNPC[0], m_ppPlayer[0], m_pTalentInfo), 10.0f, 0.001f);

		GItemData* pItemData = m_Helper.NewItemData(ITEMSLOT_RWEAPON);
		pItemData->m_nWeaponType = WEAPON_1H_SLASH;

		GUTHelper_Item::EquipNewItem(m_ppPlayer[0]);

		m_pTalentInfo->m_nCategory = TC_MELEE;

		m_ppPlayerInfo[0]->nLevel = 30;		
		m_ppPlayerInfo[0]->nDEX = 100;

		m_NPCInfo[0].nGrade = 3;
		m_NPCInfo[0].nLevel = 30;
		m_NPCInfo[0].m_fCriticalMelee = 0.1f;
		m_NPCInfo[0].m_fCriticalRange = 0.1f;
		m_NPCInfo[0].m_fCriticalMagic = 0.1f;


		CHECK_CLOSE(criticalCalculator.CalcCriticalPercent(m_ppPlayer[0], m_ppNPC[0], m_pTalentInfo), 4.0f, 0.001f);
	}

	TEST_FIXTURE(TestCombatCalcFixture, TestCalc_CriticalForMe)
	{
		m_NPCInfo[0].nGrade = 3;
		m_NPCInfo[0].nLevel = 30;
		m_NPCInfo[0].m_fCriticalMelee = 0.1f;
		m_NPCInfo[0].m_fCriticalRange = 0.1f;
		m_NPCInfo[0].m_fCriticalMagic = 0.1f;

		m_ppPlayerInfo[0]->nLevel = 30;

		GCriticalCalculator criticalCalculator;
		CHECK_CLOSE(criticalCalculator.CalcCriticalPercent(m_ppNPC[0], m_ppPlayer[0], m_pTalentInfo), 10.0f, 0.001f);

		m_ppPlayer[0]->GetChrStatus()->ActorModifier.fCriticalMeleeAmpForMe = 1.0f;
		CHECK_CLOSE(criticalCalculator.CalcCriticalPercent(m_ppNPC[0], m_ppPlayer[0], m_pTalentInfo), 100.0f, 0.001f);
	}

	TEST_FIXTURE(TestCombatCalcFixture, TestCalc_Riposte)
	{
/*		TODO : 효성씨 나중에 수정

		DECLWRAPPER_SimpleCombatCalc;
		DECLWRAPPER_BuffMgr;

		MockField* pField = GUTHelper_Field::DefaultMockField();
		GEntityPlayer* pAttacker = NewEntityPlayer(pField);
		GEntityPlayer* pVictim = NewEntityPlayer(pField);
		sprintf_s(pAttacker->GetPlayerInfo()->szName, "Attacker");
		sprintf_s(pVictim->GetPlayerInfo()->szName, "Victim");

		int nBuffID = 123;
		GBuffInfo* pRiposteBuff=test.buff.NewBuffInfo(nBuffID);
		pRiposteBuff->m_nExtraAttrib = BUFEA_RIPOSTE;
		pRiposteBuff->m_Riposte.nType = TC_RANGE;	// 원거리 공격만 되받아침
		pRiposteBuff->m_Riposte.fRate = 0.5f;		// 되받아칠 피해량

		// 되받아치기 버프를 피격자가 받음
		test.buff.GainBuffSelf(pVictim, pRiposteBuff);	
		TVALID_EQ(true, pVictim->HasBuff(nBuffID));

		// 다른 타입의 공격을 되받아치기가 안됨
		GTalentInfo* pMeleeTalent = test.talent.NewMeleeTalentInfo();
		test.talent.AddSimpleTalentInfoHitInfo(pMeleeTalent);
		test.talent.SetTalentDamage(pMeleeTalent, 10);
		CHECK_EQUAL(pAttacker->GetHP(), pAttacker->GetMaxHP());
		CHECK_EQUAL(pVictim->GetHP(), pVictim->GetMaxHP());
		test.talent.UseTalent(pAttacker, pMeleeTalent, pVictim->GetUID());
		CHECK_EQUAL(pAttacker->GetHP(), pAttacker->GetMaxHP());	
		CHECK_EQUAL(pVictim->GetHP(), pVictim->GetMaxHP()-10);	// 피격자가 피격
		pAttacker->SetHP(pAttacker->GetMaxHP());
		pVictim->SetHP(pVictim->GetMaxHP());

		// 공격 되받아치기
		GTalentInfo* pRangeTalent = test.talent.NewArcheryTalentInfo();
		test.talent.AddSimpleTalentInfoHitInfo(pRangeTalent);
		test.talent.SetTalentDamage(pRangeTalent, 10);
		CHECK_EQUAL(pAttacker->GetHP(), pAttacker->GetMaxHP());
		CHECK_EQUAL(pVictim->GetHP(), pVictim->GetMaxHP());
		test.talent.UseTalent(pAttacker, pRangeTalent, pVictim->GetUID());
		CHECK_EQUAL(pAttacker->GetHP(), pAttacker->GetMaxHP()-5);	// 공격자가 절반의 피해로 피격
		CHECK_EQUAL(pVictim->GetHP(), pVictim->GetMaxHP());	
		pAttacker->SetHP(pAttacker->GetMaxHP());
		pVictim->SetHP(pVictim->GetMaxHP());

		// 되받아치기 버프가 풀림
		test.buff.LostBuff(pVictim, nBuffID);	
		TVALID_EQ(false, pVictim->HasBuff(nBuffID));

		// 공격 되받아치기 실패
		CHECK_EQUAL(pAttacker->GetHP(), pAttacker->GetMaxHP());
		CHECK_EQUAL(pVictim->GetHP(), pVictim->GetMaxHP());
		test.talent.UseTalent(pAttacker, pRangeTalent, pVictim->GetUID());
		CHECK_EQUAL(pAttacker->GetHP(), pAttacker->GetMaxHP());	
		CHECK_EQUAL(pVictim->GetHP(), pVictim->GetMaxHP()-10);	// 피격자가 피격

		pField->Destroy();
*/
	}

	TEST_FIXTURE(TestCombatCalcFixture, TestCalc_FullCombat)
	{
		GItemData* pItemData = m_Helper.NewItemData(ITEMSLOT_RWEAPON);
		pItemData->m_nWeaponType = WEAPON_1H_SLASH;
		pItemData->m_ItemType = ITEMTYPE_WEAPON;
		pItemData->m_nMinDamage = pItemData->m_nMaxDamage = 60;

		GUTHelper_Item::EquipNewItem(m_ppPlayer[0], pItemData->m_nID);

		m_pTalentInfo->m_nCategory = TC_MELEE;
		m_pTalentInfo->m_nMinDamage = m_pTalentInfo->m_nMaxDamage = 50;

		m_ppPlayerInfo[0]->nLevel = 30;
		m_ppPlayerInfo[0]->nSTR = 100; 
		m_ppPlayerInfo[0]->nDEX = 100;
		m_ppPlayerInfo[0]->nINT = 100;

		m_NPCInfo[0].nGrade = 3;
		m_NPCInfo[0].nLevel = 30;
		m_NPCInfo[0].nAP = 500;
		m_NPCInfo[0].m_fCriticalMelee = 0.1f;
		m_NPCInfo[0].m_fCriticalRange = 0.1f;
		m_NPCInfo[0].m_fCriticalMagic = 0.1f;

		GDefenseFactorCalculator defenseFactorCalculator;

		CHECK_CLOSE(defenseFactorCalculator.CalcDefenseFactor(m_ppPlayer[0], m_ppNPC[0], m_pTalentInfo), 1.38f, 0.001f);
		// 레벨에 따른 스탯 보정치
		CHECK_EQUAL(160, gsys.pCombatCalculator->CalcDamage(m_ppPlayer[0], m_ppNPC[0], m_pTalentInfo, false, 1.0f, GDamageRangedInfo(m_pTalentInfo->m_nMaxDamage,m_pTalentInfo->m_nMinDamage)));
	}

	TEST_FIXTURE(TestCombatCalcFixture, TestCalc_1K_CalcDamage)
	{
		GItemData* pItemData = m_Helper.NewItemData(ITEMSLOT_RWEAPON);
		pItemData->m_nWeaponType = WEAPON_1H_SLASH;
		pItemData->m_ItemType = ITEMTYPE_WEAPON;
		pItemData->m_nMinDamage = pItemData->m_nMaxDamage = 60;

		GUTHelper_Item::EquipNewItem(m_ppPlayer[0], pItemData->m_nID);

		m_pTalentInfo->m_nCategory = TC_MELEE;
		m_pTalentInfo->m_nMinDamage = m_pTalentInfo->m_nMaxDamage = 50;
		
		for(int i = 0; i < MF_PRIORITY_SIZE; i++)
		{
			m_pTalentInfo->m_MotionFactorGroup.var[i].nValue = 100;
		}

		m_ppPlayerInfo[0]->nLevel = 30;
		m_ppPlayerInfo[0]->nSTR = 100; 
		m_ppPlayerInfo[0]->nDEX = 100;
		m_ppPlayerInfo[0]->nINT = 100;

		m_NPCInfo[0].nGrade = 3;
		m_NPCInfo[0].nLevel = 30;
		m_NPCInfo[0].nAP = 500;
		m_NPCInfo[0].m_fCriticalMelee = 0.1f;
		m_NPCInfo[0].m_fCriticalRange = 0.1f;
		m_NPCInfo[0].m_fCriticalMagic = 0.1f;

		// 반복 누적으로 오차가 쌓이는지 확인하기위해,
		// 최초 결과값과 이후 1000번 반복후 결과 값을 비교한다.
		float fFirstMotionFactor = 1.0f;
		int nFirstDamage = gsys.pCombatCalculator->CalcDamage(m_ppPlayer[0], m_ppNPC[0], m_pTalentInfo, false, 1.0f, GDamageRangedInfo(m_pTalentInfo->m_nMaxDamage,m_pTalentInfo->m_nMinDamage), &fFirstMotionFactor);

		float fMotionFactor;
		for(int i = 0; i < 1000; i++)
		{			
			gsys.pCombatCalculator->CalcDamage(m_ppPlayer[0], m_ppNPC[0], m_pTalentInfo, false, 1.0f, GDamageRangedInfo(m_pTalentInfo->m_nMaxDamage,m_pTalentInfo->m_nMinDamage), &fMotionFactor);
		}

		// 레벨에 따른 스탯 보정치
		CHECK_EQUAL(160, gsys.pCombatCalculator->CalcDamage(m_ppPlayer[0], m_ppNPC[0], m_pTalentInfo, false, 1.0f, GDamageRangedInfo(m_pTalentInfo->m_nMaxDamage,m_pTalentInfo->m_nMinDamage), &fMotionFactor));
		CHECK_CLOSE(fFirstMotionFactor, fMotionFactor, MMath::TOLER);

		for(int i = 0; i < MF_PRIORITY_SIZE; i++)
		{
			CHECK_EQUAL(100, m_pTalentInfo->m_MotionFactorGroup.var[i].nValue);
		}
	}

	TEST(TestCalc_GuardAPFactor)
	{
		CHECK_CLOSE(GGuardCalculator::CalcAPFactor(0, true, 1000), 2.0f, 0.0001f);
		CHECK_CLOSE(GGuardCalculator::CalcAPFactor(1, true, 1900), 4.0f, 0.0001f);
		CHECK_CLOSE(GGuardCalculator::CalcAPFactor(2, true, 300), 6.0f, 0.0001f);
		CHECK_CLOSE(GGuardCalculator::CalcAPFactor(3, true, 100), 8.0f, 0.0001f);
		CHECK_CLOSE(GGuardCalculator::CalcAPFactor(4, true, 1700), 10.0f, 0.0001f);
		CHECK_CLOSE(GGuardCalculator::CalcAPFactor(5, true, 120), 12.0f, 0.0001f);

		CHECK_CLOSE(GGuardCalculator::CalcAPFactor(0, false, 1000), 1.0f, 0.0001f);
		CHECK_CLOSE(GGuardCalculator::CalcAPFactor(1, false, 1900), 2.0f, 0.0001f);
		CHECK_CLOSE(GGuardCalculator::CalcAPFactor(2, false, 300), 3.0f, 0.0001f);
		CHECK_CLOSE(GGuardCalculator::CalcAPFactor(3, false, 100), 4.0f, 0.0001f);
		CHECK_CLOSE(GGuardCalculator::CalcAPFactor(4, false, 1700), 5.0f, 0.0001f);
		CHECK_CLOSE(GGuardCalculator::CalcAPFactor(5, false, 120), 6.0f, 0.0001f);

		// 지속 시간이 지났을 경우
		CHECK_CLOSE(GGuardCalculator::CalcAPFactor(5, false, 2001), 1.0f, 0.0001f);
		CHECK_CLOSE(GGuardCalculator::CalcAPFactor(0, false, 5001), 1.0f, 0.0001f);
	}

	// 크리티컬 공식
	class GCriticalCalculator_ForTest : public GCriticalCalculator
	{
	public:
		using GCriticalCalculator::CalcActorCriticalPercent;
		using GCriticalCalculator::CalcTalentPercent;
		using GCriticalCalculator::CalcBuffDamagePercent;
		using GCriticalCalculator::CalcDamageAttribPercent;
		using GCriticalCalculator::_CalcDamageBonusPercent;
		using GCriticalCalculator::_GetSpecializationRankForDamageAttrib;
		using GCriticalCalculator::_CalcNPCCriticalPercent;
		using GCriticalCalculator::_CalcPlayerCriticalPercent;
		using GCriticalCalculator::_CalcDamageAttribPercent;
	};

	TEST(TestCalcCriticalDamage_BonusPercent)
	{
		GCriticalCalculator_ForTest criticalCalculator;
		
		const int RANK_NONE = 0;
		const int RANK1 = 1;
		const int RANK2 = 2;
		const int RANK3 = 3;
		const int RANK4 = 4;
		const int RANK5 = 5;

		CHECK_EQUAL(criticalCalculator._CalcDamageBonusPercent(DA_NONE, RANK_NONE), 0);
		CHECK_EQUAL(criticalCalculator._CalcDamageBonusPercent(DA_SLASH, RANK_NONE), 0);
		CHECK_EQUAL(criticalCalculator._CalcDamageBonusPercent(DA_SLASH, RANK1), 4);
		CHECK_EQUAL(criticalCalculator._CalcDamageBonusPercent(DA_SLASH, RANK5), 20);

		CHECK_EQUAL(criticalCalculator._CalcDamageBonusPercent(DA_BLUNT, RANK_NONE), 0);
		CHECK_EQUAL(criticalCalculator._CalcDamageBonusPercent(DA_BLUNT, RANK1), 4);
		CHECK_EQUAL(criticalCalculator._CalcDamageBonusPercent(DA_BLUNT, RANK5), 20);

		CHECK_EQUAL(criticalCalculator._CalcDamageBonusPercent(DA_PIERCE, RANK_NONE), 0);
		CHECK_EQUAL(criticalCalculator._CalcDamageBonusPercent(DA_PIERCE, RANK1), 5);
		CHECK_EQUAL(criticalCalculator._CalcDamageBonusPercent(DA_PIERCE, RANK5), 25);

		CHECK_EQUAL(criticalCalculator._CalcDamageBonusPercent(DA_HOLY, RANK1), 0);
		CHECK_EQUAL(criticalCalculator._CalcDamageBonusPercent(DA_FIRE, RANK5), 0);
	}

	GTalentInfo* LearnExtraPassiveTalent(GEntityPlayer* pPlayer, 
										 TALENT_EXTRA_PASSIVE_TYPE nExtraPassiveType, 
										 int nTalentID, 
										 int nTalentLine, 
										 int nRank)
	{
		GTalentInfo* pTalentInfo = test.talent.NewExtraPassiveTalentInfo(nExtraPassiveType, nTalentID);
		pTalentInfo->m_bNeedTraining = true;
		pTalentInfo->m_nTalentLine = nTalentLine;
		pTalentInfo->m_nRank = nRank;
		pTalentInfo->m_nExtraPassiveParam = nRank;
		test.talent.Train(pPlayer, pTalentInfo);
		return pTalentInfo;
	}

	TEST_FIXTURE(TestCombatCalcFixture, TestCalcCriticalDamage_SpecializationRank)
	{
		GCriticalCalculator_ForTest calc;

		LearnExtraPassiveTalent(m_ppPlayer[0],TEPT_SPECIALIZATION_SLASH, 1000, 1, 1);
		CHECK_EQUAL(m_ppPlayer[0]->GetPassiveRank(TEPT_SPECIALIZATION_SLASH), 1);

		CHECK_EQUAL(calc._GetSpecializationRankForDamageAttrib(m_ppPlayer[0], DA_SLASH), 1);
		CHECK_EQUAL(calc._GetSpecializationRankForDamageAttrib(m_ppPlayer[0], DA_BLUNT), 0);
		CHECK_EQUAL(calc._GetSpecializationRankForDamageAttrib(m_ppPlayer[0], DA_PIERCE), 0);
	}

	TEST_FIXTURE(TestCombatCalcFixture, TestCalcCriticalDamage_BonusPercent2)
	{
		LearnExtraPassiveTalent(m_ppPlayer[0],TEPT_SPECIALIZATION_SLASH, 1000, 1, 1);

		CHECK_EQUAL(m_ppPlayer[0]->GetPassiveRank(TEPT_SPECIALIZATION_SLASH), 1);


		m_pTalentInfo->m_nCategory = TC_MELEE;
		m_pTalentInfo->m_nMinDamage = m_pTalentInfo->m_nMaxDamage = 50;
		m_pTalentInfo->m_nDamageAttrib = DA_SLASH;	// 슬래쉬 타입

		GCriticalCalculator calc;
		float fDamageFactor = calc.CalcCriticalDamageFactor(m_ppPlayer[0], m_pTalentInfo);

		CHECK_CLOSE(fDamageFactor, (1.5f + 0.04f), 0.001f);	// 120% + 4%(bonus)
	}

	TEST_FIXTURE(TestCombatCalcFixture, CalcPenetrationFactorForMelee)
	{
		GTalentInfo* pTalentInfo = test.talent.NewMeleeTalentInfo();

		m_ppPlayerInfo[0]->nLevel = 0;
		m_ppPlayerInfo[0]->nSTR = 1;
		CHECK_CLOSE(0.0f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);

		m_ppPlayerInfo[0]->nSTR = 150;
		CHECK_CLOSE(0.0f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);

		// 150 초과 부터 적용
		m_ppPlayerInfo[0]->nSTR = 151;
		CHECK_CLOSE(0.0f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);

		// 능력치 최대 21%
		m_ppPlayerInfo[0]->nSTR = 255;
		CHECK_CLOSE(0.208f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);


		// 버프 추가
		m_ppPlayer[0]->GetChrStatus()->ActorModifier.fModMeleePene = 0.01f;
		CHECK_CLOSE(0.218f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);

		m_ppPlayer[0]->GetChrStatus()->ActorModifier.fModMeleePene = 0.10f;
		CHECK_CLOSE(0.308f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);

		m_ppPlayer[0]->GetChrStatus()->ActorModifier.fModMeleePene = 0.14f;
		CHECK_CLOSE(0.348f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);


		// 최대 35% 넘을 수 없다.
		m_ppPlayer[0]->GetChrStatus()->ActorModifier.fModMeleePene = 0.20f;
		CHECK_CLOSE(0.35f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);
	}

	TEST_FIXTURE(TestCombatCalcFixture, CalcPenetrationFactorForRange)
	{
		GTalentInfo* pTalentInfo = test.talent.NewArcheryTalentInfo();

		m_ppPlayerInfo[0]->nLevel = 0;
		m_ppPlayerInfo[0]->nDEX = 1;
		CHECK_CLOSE(0.0f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);

		m_ppPlayerInfo[0]->nDEX = 150;
		CHECK_CLOSE(0.0f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);

		// 150 초과 부터 적용
		m_ppPlayerInfo[0]->nDEX = 151;
		CHECK_CLOSE(0.0f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);

		// 능력치 최대 21%
		m_ppPlayerInfo[0]->nDEX = 255;
		CHECK_CLOSE(0.208f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);


		// 버프 추가
		m_ppPlayer[0]->GetChrStatus()->ActorModifier.fModRangePene = 0.01f;
		CHECK_CLOSE(0.218f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);

		m_ppPlayer[0]->GetChrStatus()->ActorModifier.fModRangePene  = 0.10f;
		CHECK_CLOSE(0.308f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);

		m_ppPlayer[0]->GetChrStatus()->ActorModifier.fModRangePene  = 0.14f;
		CHECK_CLOSE(0.348f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);


		// 최대 35% 넘을 수 없다.
		m_ppPlayer[0]->GetChrStatus()->ActorModifier.fModRangePene  = 0.20f;
		CHECK_CLOSE(0.35f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);


		// 찌르기 무기
		pTalentInfo = test.talent.NewMeleeTalentInfo();
		test.item.EquipNewWeapon(m_ppPlayer[0], WEAPON_1H_PIERCE);
		CHECK_CLOSE(0.35f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);

		// 듀얼 무기 공격
		GItem* pRightWeapon = test.item.GainWeaponItem(m_ppPlayer[0], SUnitTestUtil::NewID(), WEAPON_1H_PIERCE);
		GItem* pLeftWeapon = test.item.GainWeaponItem(m_ppPlayer[0], SUnitTestUtil::NewID(), WEAPON_1H_PIERCE);

		test.item.EquipItem(m_ppPlayer[0], pRightWeapon, ITEMSLOT_RWEAPON);
		test.item.EquipItem(m_ppPlayer[0], pLeftWeapon, ITEMSLOT_LWEAPON);

		CHECK_CLOSE(0.35f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);
	}

	TEST_FIXTURE(TestCombatCalcFixture, CalcPenetrationFactorForMagic)
	{
		GTalentInfo* pTalentInfo = test.talent.NewMagicTalentInfo();

		m_ppPlayerInfo[0]->nLevel = 0;
		m_ppPlayerInfo[0]->nINT = 1;
		CHECK_CLOSE(0.0f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);

		m_ppPlayerInfo[0]->nINT = 150;
		CHECK_CLOSE(0.0f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);

		// 150 초과 부터 적용
		m_ppPlayerInfo[0]->nINT = 151;
		CHECK_CLOSE(0.0f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);

		// 능력치 최대 21%
		m_ppPlayerInfo[0]->nINT = 255;
		CHECK_CLOSE(0.208f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);


		// 버프 추가
		m_ppPlayer[0]->GetChrStatus()->ActorModifier.fModMagicPene = 0.01f;
		CHECK_CLOSE(0.218f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);

		m_ppPlayer[0]->GetChrStatus()->ActorModifier.fModMagicPene  = 0.10f;
		CHECK_CLOSE(0.308f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);

		m_ppPlayer[0]->GetChrStatus()->ActorModifier.fModMagicPene  = 0.14f;
		CHECK_CLOSE(0.348f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);


		// 최대 35% 넘을 수 없다.
		m_ppPlayer[0]->GetChrStatus()->ActorModifier.fModMagicPene  = 0.20f;
		CHECK_CLOSE(0.35f, GCombatCalculator::CalcPenetrationFactor(m_ppPlayer[0], pTalentInfo), MMath::TOLER);
	}

	TEST(TestCalcCriticalPercent_PlayerCriticalPercent_ForMelee)
	{
		GCriticalCalculator_ForTest calc;
		PLAYER_INFO playerInfo;
		
		// -2.45 ~ 10.25%

		playerInfo.nDEX = 2;		
		CHECK_EQUAL(0.08f, calc._CalcPlayerCriticalPercent(TC_MELEE, &playerInfo));
		
		playerInfo.nDEX = 100;		
		CHECK_EQUAL(4.0f, calc._CalcPlayerCriticalPercent(TC_MELEE, &playerInfo));

		playerInfo.nDEX = 255;		
		CHECK_EQUAL(10.16f, calc._CalcPlayerCriticalPercent(TC_MELEE, &playerInfo));
	}

	TEST(TestCalcCriticalPercent_PlayerCriticalPercent_ForRange)
	{
		GCriticalCalculator_ForTest calc;
		PLAYER_INFO playerInfo;

		// -2.45 ~ 10.25%

		playerInfo.nDEX = 2;		
		CHECK_EQUAL(0.08f, calc._CalcPlayerCriticalPercent(TC_RANGE, &playerInfo));

		playerInfo.nDEX = 100;		
		CHECK_EQUAL(4.0f, calc._CalcPlayerCriticalPercent(TC_RANGE, &playerInfo));

		playerInfo.nDEX = 255;		
		CHECK_EQUAL(10.16f, calc._CalcPlayerCriticalPercent(TC_RANGE, &playerInfo));
	}

	TEST(TestCalcCriticalPercent_PlayerCriticalPercent_ForMagic)
	{
		GCriticalCalculator_ForTest calc;
		PLAYER_INFO playerInfo;
		
		// -0.98% ~ 8.2f

		playerInfo.nINT = 2;		
		CHECK_EQUAL(0.05f, calc._CalcPlayerCriticalPercent(TC_MAGIC, &playerInfo));

		playerInfo.nINT = 100;		
		CHECK_EQUAL(2.5f, calc._CalcPlayerCriticalPercent(TC_MAGIC, &playerInfo));

		playerInfo.nINT = 255;		
		CHECK_EQUAL(6.35f, calc._CalcPlayerCriticalPercent(TC_MAGIC, &playerInfo));
	}

	TEST(TestCalcCriticalPercent_NPCCriticalPercent)
	{
		GCriticalCalculator_ForTest calc;

		GNPCInfo npcInfo;
		npcInfo.m_fCriticalMelee = 0.05f;	// 5%
		npcInfo.m_fCriticalRange = 0.1f;	// 10%
		npcInfo.m_fCriticalMagic = 0.5f;	// 50%

		CHECK_CLOSE(calc._CalcNPCCriticalPercent(ST_MELEE, &npcInfo), 5.0f, 0.001f);
		CHECK_CLOSE(calc._CalcNPCCriticalPercent(ST_ARCHERY, &npcInfo), 10.0f, 0.001f);
		CHECK_CLOSE(calc._CalcNPCCriticalPercent(ST_MAGIC, &npcInfo), 50.0f, 0.001f);
		CHECK_CLOSE(calc._CalcNPCCriticalPercent(ST_EXTRA_ACTIVE, &npcInfo), 50.0f, 0.001f);

	}

	TEST(TestCalcCriticalPercent_DamageAttribPercent)
	{
		GCriticalCalculator_ForTest calc;

		const int RANK_NONE = 0;
		const int RANK1 = 1;
		const int RANK2 = 2;
		const int RANK3 = 3;
		const int RANK4 = 4;
		const int RANK5 = 5;

		CHECK_CLOSE(calc._CalcDamageAttribPercent(DA_NONE, RANK_NONE), 0, 0.001f);

		CHECK_CLOSE(calc._CalcDamageAttribPercent(DA_SLASH, RANK_NONE), 0, 0.001f);
		CHECK_CLOSE(calc._CalcDamageAttribPercent(DA_SLASH, RANK1), 1.0f, 0.001f);
		CHECK_CLOSE(calc._CalcDamageAttribPercent(DA_SLASH, RANK2), 2.0f, 0.001f);

		CHECK_CLOSE(calc._CalcDamageAttribPercent(DA_BLUNT, RANK_NONE), 0, 0.001f);
		CHECK_CLOSE(calc._CalcDamageAttribPercent(DA_BLUNT, RANK1), 1.0f, 0.001f);
		CHECK_CLOSE(calc._CalcDamageAttribPercent(DA_BLUNT, RANK2), 2.0f, 0.001f);

		CHECK_CLOSE(calc._CalcDamageAttribPercent(DA_PIERCE, RANK_NONE), 0, 0.001f);
		CHECK_CLOSE(calc._CalcDamageAttribPercent(DA_PIERCE, RANK1), 1.1f, 0.001f);
		CHECK_CLOSE(calc._CalcDamageAttribPercent(DA_PIERCE, RANK2), 2.2f, 0.001f);
		CHECK_CLOSE(calc._CalcDamageAttribPercent(DA_PIERCE, RANK5), 5.5f, 0.001f);

	}



	TEST(fn_MakeLevelFactor)
	{
		const int nTargetLevel=10;
		struct Case
		{
			const int nLevelFactor;
			const int nRequesterLevel;
		} cases[] = {
			{0, 16}
			,{1, 15}
			,{2, 14}
			,{3, 13}
			,{4, 12}
			,{5, 10}
			,{6, 8}
			,{7, 7}
			,{8, 6}
			,{9, 5}
			,{10, 4}
			,{11, 3}
			,{12, 2}
		};

		MockField* pField = GUTHelper_Field::DefaultMockField();
		GEntityPlayer* pReqeuster = GUTHelper::NewEntityPlayer(pField);
		GEntityPlayer* pTarget = GUTHelper::NewEntityPlayer(pField);

		pTarget->SetLevel(nTargetLevel);

		for (int i=0; i<ArraySize(cases); ++i) 
		{
			MakeLevelFactor(cases[i].nLevelFactor, pReqeuster, pTarget);
			CHECK_EQUAL(pTarget->GetLevel(), nTargetLevel);
			CHECK_EQUAL(pReqeuster->GetLevel(), cases[i].nRequesterLevel);
		}

		pField->Destroy();
	}

	TEST(PerfectGuardRate)
	{
		const int nDamage=100;
		struct Case
		{
			const int nLevelFactor;
			const float fGuardRate;
		} cases[] = {
			 {0, 100.0f}
			,{1, 100.0f}
			,{2, 94.9f}
			,{3, 89.4f}
			,{4, 83.5f}
			,{5, 77.1f}
			,{6, 70.0f}
			,{7, 62.0f}
			,{8, 52.4f}
			,{9,  40.0f}
			,{10, 10.0f}
			,{11, 10.0f}
		};

		MockField* pField = GUTHelper_Field::DefaultMockField();
		GEntityPlayer* pAttacker = GUTHelper::NewEntityPlayer(pField);
		GEntityPlayer* pGuarder = GUTHelper::NewEntityPlayer(pField);

		for (int i=0; i<ArraySize(cases); ++i) 
		{
			MakeLevelFactor(cases[i].nLevelFactor, pGuarder, pAttacker);	// 방어 입장에서의 레벨팩터
			CHECK_EQUAL(GGuardCalculator::CalcPerfectGuardRate(pAttacker, pGuarder), cases[i].fGuardRate);
		}

		pField->Destroy();
	}


	TEST(PartialGuardRate)
	{
		const int nDamage=100;
		struct Case
		{
			const int nLevelFactor;
			const float fGuardRate;
		} cases[] = {
			{0, 100.0f}
			,{1, 100.0f}
			,{2, 100.0f}
			,{3, 100.0f}
			,{4, 100.0f}
			,{5, 80.0f}
			,{6, 60.0f}
			,{7, 40.0f}
			,{8, 20.0f}
			,{9, 0.0f}
			,{10, 0.0f}
			,{11, 0.0f}
		};

		MockField* pField = GUTHelper_Field::DefaultMockField();
		GEntityPlayer* pAttacker = GUTHelper::NewEntityPlayer(pField);
		GEntityPlayer* pGuarder = GUTHelper::NewEntityPlayer(pField);

		for (int i=0; i<ArraySize(cases); ++i) 
		{
			MakeLevelFactor(cases[i].nLevelFactor, pGuarder, pAttacker);	// 방어 입장에서의 레벨팩터
			CHECK_EQUAL(GGuardCalculator::CalcPartialGuardRate(pAttacker, pGuarder), cases[i].fGuardRate);
		}

		pField->Destroy();
	}

	TEST(PartialGuardDamageMod_VarWeapon)
	{
		const int nDamage=100;
		struct Case
		{
			const WEAPON_TYPE nWeaponType;
			const float fDamageMod;
		} cases[] = {
			{WEAPON_NONE, 1.0f}
			,{WEAPON_1H_SLASH, 0.5f}
			,{WEAPON_1H_BLUNT, 0.5f}
			,{WEAPON_1H_PIERCE, 1.0f}
			,{WEAPON_TWO_HANDED, 0.45f}
			,{WEAPON_STAFF, 0.4f}
			,{WEAPON_ARCHERY, 1.0f}
			,{WEAPON_2H_BLUNT, 0.35f}
			,{WEAPON_DUAL_WIELD, 1.0f}
			,{WEAPON_DUAL_PIERCE, 0.5f}
		};

		MockField* pField = GUTHelper_Field::DefaultMockField();
		GEntityPlayer* pAttacker = GUTHelper::NewEntityPlayer(pField);
		GEntityPlayer* pGuarder = GUTHelper::NewEntityPlayer(pField);

		for (int i=0; i<ArraySize(cases); ++i) 
		{
			test.item.EquipNewWeapon(pGuarder, cases[i].nWeaponType); 
			CHECK_EQUAL(GGuardCalculator::CalcPartialGuardDamageMod(pGuarder, pAttacker), cases[i].fDamageMod);
		}

		pField->Destroy();
	}

	class MockTalentEffector: public GTalentEffector
	{
	public:
		using GTalentEffector::GainDamageAndHeal;
	};

	TEST_FIXTURE(TestCombatCalcFixture, TestGainDamageForStateMotionFactor)
	{
		GEntityPlayer* pEntityPlayer = m_Helper.NewEntityPlayer(m_pField, vec3(100, 99, 0));
		GEntityNPC* pEntityNPC = m_Helper.NewEntityNPC(m_pField, vec3(100, 100, 0));
		pEntityNPC->SetDir(vec3(0, 1, 0));

		GConst::STATE_FACTOR_FOR_MF_VALUE_BY_BACK = 1.2f;
		GTalentInfo* pTalentInfo = m_Helper.NewMeleeTalentInfo();
		
		for(int i = 0; i < MF_PRIORITY_SIZE; i++)
		{
			pTalentInfo->m_MotionFactorGroup.var[i].nValue = 10;
			pTalentInfo->m_MotionFactorGroup.var[i].nWeight = 1000;
		}

		GMotionFactor& MotionFactorResult = pEntityNPC->GetMotionfactorForTest();
		MotionFactorResult.ClearVars();

		GDamageRangedInfo DamageRangedInfo;
		DamageRangedInfo.nMinDamage = 10;
		DamageRangedInfo.nMaxDamage = 10;
		
		// TalentInfo 값 받아내고
		MOTION_FACTOR_GROUP	MotionFactorGroup;
		MotionFactorGroup.Clear();

		for(int i = 0; i < MF_PRIORITY_SIZE; i++)
		{			
			MotionFactorGroup.var[i].nValue = pTalentInfo->m_MotionFactorGroup.var[i].nValue;
			MotionFactorGroup.var[i].nWeight = pTalentInfo->m_MotionFactorGroup.var[i].nWeight;
		}

		MockTalentEffector TalentEffector;
		TalentEffector.GainDamageAndHeal(pEntityPlayer, pEntityNPC, pTalentInfo, DamageRangedInfo, GHealRangedInfo(0,0), FALSE);


		// TalentInfo가 변경되지 않음을 증명한다.
		for(int i = 0; i < MF_PRIORITY_SIZE; i++)
		{			
			CHECK_EQUAL(MotionFactorGroup.var[i].nValue, pTalentInfo->m_MotionFactorGroup.var[i].nValue);
			CHECK_EQUAL(MotionFactorGroup.var[i].nWeight, pTalentInfo->m_MotionFactorGroup.var[i].nWeight);
		}


		// StateMotionFactor 적용
		MOTION_FACTOR_GROUP& MotionFactorGroupResult = MotionFactorResult.GetFactorsForTest();
		for(int i = 0; i < MF_PRIORITY_SIZE; i++)
		{
			CHECK_EQUAL(MotionFactorGroupResult.var[i].nValue, 10 * GConst::STATE_FACTOR_FOR_MF_VALUE_BY_BACK);
		}		
	}

	TEST(ApplySpellPower)
	{
		MockField* pField = GUTHelper_Field::DefaultMockField();
		GEntityPlayer* pAttacker = GUTHelper::NewEntityPlayer(pField);
		GEntityPlayer* pGuarder = GUTHelper::NewEntityPlayer(pField);

		pGuarder->SetHP(100);
		CHECK_EQUAL(100, pGuarder->GetHP());

		GTalentInfo* pAtkTalent = test.talent.NewMagicTalentInfo();
		test.talent.SetTalentDamage(pAtkTalent, 10);
		pAtkTalent->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;	// 목표가 피격되도록 조정
		pAtkTalent->m_Resist.m_nDifficulty = 255;	// 저항 불가능하게 수정
		test.talent.UseTalent(pAttacker, pAtkTalent, pGuarder->GetUID());
		CHECK_EQUAL(90, pGuarder->GetHP());

		// 지팡이를 오른손에 장착
		pAtkTalent->m_WeaponReference = WR_RIGHT;
		GItem* pStaff = test.item.GainWeaponItem(pAttacker, 777, WEAPON_STAFF);
		pStaff->m_pItemData->m_fSpellPower = 3.0f; // 주문력 3
		GUTHelper_Item::EquipItem(pAttacker, pStaff, ITEMSLOT_RWEAPON);
		pGuarder->ClearLastHitInfo();

		test.talent.UseTalent(pAttacker, pAtkTalent, pGuarder->GetUID());
		CHECK_EQUAL(60, pGuarder->GetHP());

		pField->Destroy();
	}
}	// SUITE(CombatCalc)
