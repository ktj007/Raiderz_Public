#include "stdafx.h"
#include "GAttackDamageCalculator.h"
#include "GCriticalCalculator.h"
#include "GUTHelper_Item.h"
#include "GTestWrapper.h"
#include "GTestForward.h"
#include "FBasePlayer.h"
#include "GItemManager.h"

SUITE(TestAttackDamageCalculator)
{
	class TestAttackDamageCalculator : public GAttackDamageCalculator
	{
	public:
		using GAttackDamageCalculator::CalcTalentFactor1;
		using GAttackDamageCalculator::CalcTalentFactor2;
		using GAttackDamageCalculator::ClacTalentFactor2Fomula;
		using GAttackDamageCalculator::GetArmorFactor1Table;
		using GAttackDamageCalculator::GetArmorFactor2Table;
		using GAttackDamageCalculator::CalcTalentDamage;
		using GAttackDamageCalculator::doCalcWeaponDamage;

		using GAttackDamageCalculator::CalcNPCWeaponDamage;

		using GAttackDamageCalculator::CalcCharacterFactorForMagic;
		using GAttackDamageCalculator::CalcCharacterFactorForMelee;
		using GAttackDamageCalculator::CalcCharacterFactorForRange;
	};

	class MockCalculator: public TestAttackDamageCalculator
	{
	public:			
		MOCK_METHOD1(CalcCharacterFactorForMelee, float(GEntityPlayer* pAttacker));
		MOCK_METHOD1(CalcCharacterFactorForRange, float(GEntityPlayer* pAttacker));	
		MOCK_METHOD1(CalcCharacterFactorForMagic, float(GEntityPlayer* pAttacker));			
	};

	class FAttackDamageCalculator : public FBasePlayer
	{
	public:
		FAttackDamageCalculator()
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
		virtual ~FAttackDamageCalculator()
		{
			m_pField->Destroy();				
		}

		TestAttackDamageCalculator		m_Calc;


		GUTHelper		m_Helper;
		GTalentInfo*	m_pTalentInfo;
		GNPCInfo		m_NPCInfo[2];
		GEntityPlayer*	m_ppPlayer[2];
		PLAYER_INFO*	m_ppPlayerInfo[2];
		GEntityNPC*		m_ppNPC[2];
		MockField*		m_pField;

		DECLWRAPPER_FieldInfoMgr;
		DECLWRAPPER_FieldMgr;
		GTestMgrWrapper<GTalentInfoMgr>	m_TalentInfoMgrWrapper;
		GTestMgrWrapper<GItemManager>	m_ItemInfoMgrWrapper;
		GTestSysWrapper<GItemSystem>	m_ItemSystemWrapper;
	};

	TEST_FIXTURE(FAttackDamageCalculator, CalcNPCWeaponDamage)
	{
		GNPCInfo npcInfo;

		// 기본값 = 0
		CHECK_EQUAL(0, m_Calc.CalcNPCWeaponDamage(&npcInfo));
		CHECK_EQUAL(0, m_Calc.CalcNPCWeaponDamage(NULL));	// 예외 처리

		npcInfo.m_nMinDamage = 10;
		npcInfo.m_nMaxDamage = 10;
		CHECK_EQUAL(10, m_Calc.CalcNPCWeaponDamage(&npcInfo));

		// min, max 사이에서 random으로 결정
		npcInfo.m_nMinDamage = 10;
		npcInfo.m_nMaxDamage = 100;
		CHECK(m_Calc.CalcNPCWeaponDamage(&npcInfo) >= npcInfo.m_nMinDamage );
		CHECK(m_Calc.CalcNPCWeaponDamage(&npcInfo) <= npcInfo.m_nMaxDamage );
	}

	TEST_FIXTURE(FAttackDamageCalculator, TestCalcAttackDamage_CriticalTalentDamage)
	{
		GDamageRangedInfo damageInfo1(100, 10);

		CHECK_EQUAL(100, m_Calc.CalcTalentDamage(damageInfo1, true));
	}

	TEST_FIXTURE(FAttackDamageCalculator, TestCalcAttackDamage_TalentFactor1Table)
	{
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor1Table(ARMOR_CLOTH, false), 0.001f);
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor1Table(ARMOR_CLOTH, true), 0.001f);

		CHECK_CLOSE(0.8f, m_Calc.GetArmorFactor1Table(ARMOR_LIGHT, false), 0.001f);
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor1Table(ARMOR_LIGHT, true), 0.001f);

		CHECK_CLOSE(0.6f, m_Calc.GetArmorFactor1Table(ARMOR_MEDIUM, false), 0.001f);
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor1Table(ARMOR_MEDIUM, true),  0.001f);

		CHECK_CLOSE(0.4f, m_Calc.GetArmorFactor1Table(ARMOR_HEAVY, false), 0.001f);
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor1Table(ARMOR_HEAVY, true), 0.001f);
	}

	TEST_FIXTURE(FAttackDamageCalculator, TestCalcAttackDamage_TalentFactor1Table_Negative)
	{
		// 범위를 벗어난 값
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor1Table(ARMOR_MAX, false), 0.001f);
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor1Table(ARMOR_MAX, true), 0.001f);
	}

	TEST_FIXTURE(FAttackDamageCalculator, TestCalcAttackDamage_TalentFactor2Table)
	{
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor2Table(ARMOR_CLOTH, WEAPON_1H_SLASH),	0.001f);
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor2Table(ARMOR_CLOTH, WEAPON_1H_BLUNT),	0.001f);
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor2Table(ARMOR_CLOTH, WEAPON_TWO_HANDED),	0.001f);
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor2Table(ARMOR_CLOTH, WEAPON_1H_PIERCE),	0.001f);
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor2Table(ARMOR_CLOTH, WEAPON_ARCHERY),		0.001f);
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor2Table(ARMOR_CLOTH, WEAPON_STAFF),		0.001f);

		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor2Table(ARMOR_LIGHT, WEAPON_1H_SLASH),	0.001f);
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor2Table(ARMOR_LIGHT, WEAPON_1H_BLUNT),	0.001f);
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor2Table(ARMOR_LIGHT, WEAPON_TWO_HANDED),	0.001f);
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor2Table(ARMOR_LIGHT, WEAPON_1H_PIERCE),	0.001f);
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor2Table(ARMOR_LIGHT, WEAPON_ARCHERY),		0.001f);
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor2Table(ARMOR_LIGHT, WEAPON_STAFF),		0.001f);

		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor2Table(ARMOR_MEDIUM, WEAPON_1H_SLASH),	0.001f);
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor2Table(ARMOR_MEDIUM, WEAPON_1H_BLUNT)	,0.001f);
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor2Table(ARMOR_MEDIUM, WEAPON_TWO_HANDED),	 0.001f);
		CHECK_CLOSE(0.7f, m_Calc.GetArmorFactor2Table(ARMOR_MEDIUM, WEAPON_1H_PIERCE),	0.001f);
		CHECK_CLOSE(0.7f, m_Calc.GetArmorFactor2Table(ARMOR_MEDIUM, WEAPON_ARCHERY),	0.001f);
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor2Table(ARMOR_MEDIUM, WEAPON_STAFF),		0.001f);

		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor2Table(ARMOR_HEAVY, WEAPON_1H_SLASH),	0.001f);
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor2Table(ARMOR_HEAVY, WEAPON_1H_BLUNT),	0.001f);
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor2Table(ARMOR_HEAVY, WEAPON_TWO_HANDED),	0.001f);
		CHECK_CLOSE(0.7f, m_Calc.GetArmorFactor2Table(ARMOR_HEAVY, WEAPON_1H_PIERCE),	0.001f);
		CHECK_CLOSE(0.5f,m_Calc.GetArmorFactor2Table(ARMOR_HEAVY, WEAPON_ARCHERY),		 0.001f);
		CHECK_CLOSE(1.0f, m_Calc.GetArmorFactor2Table(ARMOR_HEAVY, WEAPON_STAFF),		0.001f);
	}

	TEST_FIXTURE(FAttackDamageCalculator, TestCalcAttackDamage_TalentFactor2_OneHandedWeapon)
	{
		/// Fixture 설치
		GEntityPlayer* pAttacker = m_ppPlayer[0];

		GUTHelper_Item::EquipNewWeapon(pAttacker, WEAPON_1H_SLASH); ///< 1HS 장착

		GTalentInfo tiHit;
		tiHit.m_nID = GUnitTestUtil::NewID();
		tiHit.m_nCategory = TC_MELEE;

		float fResultValue = m_Calc.CalcTalentFactor2(pAttacker, &tiHit);
		CHECK_EQUAL(1, fResultValue);


		/// SUT 실행
		const int PASSIVE_TALENT_RANK = 1;
		{
			// 한손무기 숙련 Rank 1 탤런트 학습
			GUTHelper_Talent helperTalent;
			GTalentInfo* pOneHanded1 = helperTalent.NewExtraPassiveTalentInfo(TEPT_BASE_WEAPON_ONE_HANDED);
			pOneHanded1->m_nRank = PASSIVE_TALENT_RANK;
			helperTalent.Train(pAttacker, pOneHanded1);
		}
		fResultValue = m_Calc.CalcTalentFactor2(pAttacker, &tiHit);

		/// 검증
		int nBaseTalentRank = PASSIVE_TALENT_RANK;
		int nSpecializationTalentRank = 0;
		float fExpectValue = m_Calc.ClacTalentFactor2Fomula(nBaseTalentRank, nSpecializationTalentRank);

		CHECK(1.0f < fExpectValue);
		CHECK_CLOSE(fExpectValue, fResultValue, 0.001f);
	}

	TEST_FIXTURE(FAttackDamageCalculator, TestCalcAttackDamage_TalentFactor2_OneHandedWeapon_Equal_1HS_With_1HB)
	{
		/// Fixture 설치
		GEntityPlayer* pAttacker = m_ppPlayer[0];

		const int PASSIVE_TALENT_RANK = 1;
		GUTHelper_Talent helperTalent;
		GTalentInfo* pOneHanded1 = helperTalent.NewExtraPassiveTalentInfo(TEPT_BASE_WEAPON_ONE_HANDED);
		pOneHanded1->m_nRank = PASSIVE_TALENT_RANK;
		pOneHanded1->m_nTalentLine = 100;
		helperTalent.Train(pAttacker, pOneHanded1);	///< 한손무기 숙련 Rank 1 탤런트 학습

		GTalentInfo tiHit;
		tiHit.m_nID = GUnitTestUtil::NewID();
		tiHit.m_nCategory = TC_MELEE;

		/// SUT 실행
		GUTHelper_Item::EquipNewWeapon(pAttacker, WEAPON_1H_SLASH); ///< 1HS 장착
		float fResultMod_1HS = m_Calc.CalcTalentFactor2(pAttacker, &tiHit);

		GUTHelper_Item::EquipNewWeapon(pAttacker, WEAPON_1H_BLUNT); ///< 1HS 장착
		float fResultMod_1HB = m_Calc.CalcTalentFactor2(pAttacker, &tiHit);

		/// 검증
		int nBaseTalentRank = PASSIVE_TALENT_RANK;
		int nSpecializationTalentRank = 0;
		float fExpectValue = m_Calc.ClacTalentFactor2Fomula(nBaseTalentRank, nSpecializationTalentRank);

		CHECK_CLOSE(fExpectValue, fResultMod_1HS, 0.001f);
		CHECK_CLOSE(fExpectValue, fResultMod_1HB, 0.001f);
	}

	TEST_FIXTURE(FAttackDamageCalculator, TestCalcAttackDamage_doCalcWeaponDamage)
	{
		GEntityPlayer* pAttacker = m_ppPlayer[0];
		test.talent.TrainEquipUsableAllTalent(pAttacker);

		GItemData* pItemData = GUTHelper_Item::NewWeaponItemData(INVALID_ID, WEAPON_1H_SLASH);
		pItemData->m_nMinDamage = 100;
		pItemData->m_nMaxDamage = 100;
		GItem* pWeaponItem = GUTHelper_Item::NewItem(pItemData->m_nID);

		int nNormalDamage = m_Calc.doCalcWeaponDamage(pAttacker, pWeaponItem, false);

		// 내구도 0
		pWeaponItem->m_nDurability = 0;
		int nDurabilityZeroDamage = m_Calc.doCalcWeaponDamage(pAttacker, pWeaponItem, false);

		// 무기착용에 필요한 탤런트 안배움
		pWeaponItem->m_nDurability = 100;
		test.talent.UntrainAll(pAttacker);
		int nNoTrainEquipTalentDamage = m_Calc.doCalcWeaponDamage(pAttacker, pWeaponItem, false);


		CHECK_CLOSE(nDurabilityZeroDamage, nNormalDamage * 0.2f, 1.0f);
		CHECK_CLOSE(nNoTrainEquipTalentDamage, nNormalDamage * 0.2f, 1.0f);

		SAFE_DELETE(pWeaponItem);
	}

	TEST_FIXTURE(FAttackDamageCalculator, TestCalcAttackDamage_doCalcWeaponDamage_Critical)
	{
		GEntityPlayer* pAttacker = m_ppPlayer[0];
		test.talent.TrainEquipUsableAllTalent(pAttacker);

		GItemData* pItemData = GUTHelper_Item::NewWeaponItemData(INVALID_ID, WEAPON_1H_SLASH);
		pItemData->m_nMinDamage = 100;
		pItemData->m_nMaxDamage = 200;
		GItem* pWeaponItem = GUTHelper_Item::NewItem(pItemData->m_nID);

		int nNormalDamage = m_Calc.doCalcWeaponDamage(pAttacker, pWeaponItem, true);
		CHECK_CLOSE(nNormalDamage, pItemData->m_nMaxDamage, 0.001f);

		// 내구도 0
		pWeaponItem->m_nDurability = 0;
		int nDurabilityZeroDamage = m_Calc.doCalcWeaponDamage(pAttacker, pWeaponItem, true);

		// 무기착용에 필요한 탤런트 안배움
		pWeaponItem->m_nDurability = 100;
		test.talent.UntrainAll(pAttacker);
		int nNoTrainEquipTalentDamage = m_Calc.doCalcWeaponDamage(pAttacker, pWeaponItem, true);


		CHECK_CLOSE(nDurabilityZeroDamage, pItemData->m_nMaxDamage*0.2f, 0.001f);
		CHECK_CLOSE(nNoTrainEquipTalentDamage, pItemData->m_nMaxDamage*0.2f, 0.001f);

		SAFE_DELETE(pWeaponItem);
	}

	TEST_FIXTURE(FAttackDamageCalculator, TestCalcAttackDamage_CalcBuffDefenceFactor)
	{
		class GAttackDamageCalculator_ForCalcBuffDefenceFactorTest : public GAttackDamageCalculator
		{
		public:
			using GAttackDamageCalculator::CalcBuffDefenceFactor;
		} attackDamageCalculator;

		GTalentInfo talentInfo;
		talentInfo.m_nID = 1;

		// 버프 받음
		GBuffInfo* pBuff = test.buff.NewBuffInfo(123);
		pBuff->m_Condition.nEffectConditionType = TC_BUFF_GAIN;
		pBuff->m_ActorModifier.fDefenceAmp[DA_SLASH] = 0.1f;
		pBuff->m_ActorModifier.fDefenceAmp[DA_BLUNT] = 0.2f;
		pBuff->m_ActorModifier.fDefenceAmp[DA_PIERCE] = 1.0f;

		pBuff->m_ActorModifier.fDefenceAmp[DA_HOLY] = 0.1f;
		pBuff->m_ActorModifier.fDefenceAmp[DA_UNHOLY] = -0.1f;
		pBuff->m_ActorModifier.fDefenceAmp[DA_FIRE] = 0.5f;
		pBuff->m_ActorModifier.fDefenceAmp[DA_COLD] = -0.6f;
		pBuff->m_ActorModifier.fDefenceAmp[DA_LIGHTNING] = 0.7f;
		pBuff->m_ActorModifier.fDefenceAmp[DA_POISON] = 0.8f;

		GEntityPlayer* pVictim = m_ppPlayer[0];

		test.buff.GainBuffSelf(pVictim, pBuff);

		// melee --
		talentInfo.m_nCategory = TC_MELEE;

		// slash
		talentInfo.m_nDamageAttrib = DA_SLASH;
		CHECK_CLOSE(1.1f, attackDamageCalculator.CalcBuffDefenceFactor(pVictim, pVictim, &talentInfo), 0.001f);

		// blunt
		talentInfo.m_nDamageAttrib = DA_BLUNT;
		CHECK_CLOSE(1.2f, attackDamageCalculator.CalcBuffDefenceFactor(pVictim, pVictim, &talentInfo), 0.001f);

		// pierce
		talentInfo.m_nDamageAttrib = DA_PIERCE;
		CHECK_CLOSE(2.0f, attackDamageCalculator.CalcBuffDefenceFactor(pVictim, pVictim, &talentInfo), 0.001f);

		// magic --
		talentInfo.m_nCategory = TC_MAGIC;

		// holy
		talentInfo.m_nDamageAttrib = DA_HOLY;
		CHECK_CLOSE(1.1f, attackDamageCalculator.CalcBuffDefenceFactor(pVictim, pVictim, &talentInfo), 0.001f);

		// unholy
		talentInfo.m_nDamageAttrib = DA_UNHOLY;
		CHECK_CLOSE(0.9f, attackDamageCalculator.CalcBuffDefenceFactor(pVictim, pVictim, &talentInfo), 0.001f);

		// fire
		talentInfo.m_nDamageAttrib = DA_FIRE;
		CHECK_CLOSE(1.5f, attackDamageCalculator.CalcBuffDefenceFactor(pVictim, pVictim, &talentInfo), 0.001f);

		// cold
		talentInfo.m_nDamageAttrib = DA_COLD;
		CHECK_CLOSE(0.4f, attackDamageCalculator.CalcBuffDefenceFactor(pVictim, pVictim, &talentInfo), 0.001f);

		// lightning
		talentInfo.m_nDamageAttrib = DA_LIGHTNING;
		CHECK_CLOSE(1.7f, attackDamageCalculator.CalcBuffDefenceFactor(pVictim, pVictim, &talentInfo), 0.001f);

		// poison
		talentInfo.m_nDamageAttrib = DA_POISON;
		CHECK_CLOSE(1.8f, attackDamageCalculator.CalcBuffDefenceFactor(pVictim, pVictim, &talentInfo), 0.001f);

	}

	TEST_FIXTURE(FAttackDamageCalculator, TestCalcAttackDamage_BuffDefence)
	{
		class GCriticalCalculator_ForBuffDefenceTest : public GCriticalCalculator
		{
		public:
			virtual float CalcCriticalDamageFactor(GEntityActor* pAttacker, const GTalentInfo* pTalentInfo)
			{
				return 1.0f;
			}
		};

		class GAttackDamageCalculator_ForCalcBuffDefenceFactorTest : public GAttackDamageCalculator
		{
		protected:
			virtual float CalcTalentFactor1(GEntityActor* pAttacker, DAMAGE_ATTRIB nDamageAttrib, float fWeaponApplyRatepTalentInfo) override { return 1.0f; }
			virtual float CalcArmorFactor1( GEntityActor* pAttacker, const GTalentInfo* pTalentInfo ) override  { return 1.0f; }
			virtual float CalcBuffPlusFactor(GEntityActor* pAttacker, const GTalentInfo* pTalentInfo) override  { return 0.0f; }
			virtual float CalcCharacterFactor(GEntityPlayer* pAttacker, const GTalentInfo* pTalentInfo) override  { return 1.0f; }
			virtual int CalcPlayerWeaponDamage(GEntityPlayer* pAttacker, const GTalentInfo* pTalentInfo, bool bCritical) override { return 0; }

			virtual float CalcBuffDefenceFactor( GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo ) override
			{
				return 0.5f;	// 방어력이 반으로 떨어짐
			}
		public:
			GAttackDamageCalculator_ForCalcBuffDefenceFactorTest(GCriticalCalculator* pCriticalCalculator) : GAttackDamageCalculator(pCriticalCalculator) {}
			virtual ~GAttackDamageCalculator_ForCalcBuffDefenceFactorTest() {}
		};

		GAttackDamageCalculator* pAttackDamageCalculator = new GAttackDamageCalculator_ForCalcBuffDefenceFactorTest(new GCriticalCalculator_ForBuffDefenceTest);

		// 대미지 = 10
		GDamageRangedInfo damageInfo(50, 50);
		CHECK_CLOSE(25.0f, pAttackDamageCalculator->CalcAttackDamage(m_ppPlayer[0], m_ppPlayer[1], m_pTalentInfo, false, damageInfo), 0.001f);

		delete pAttackDamageCalculator;
	}

	TEST_FIXTURE(FAttackDamageCalculator, CalcCharacterFactorForMagic)
	{
		GEntityPlayer* pAttacker = m_ppPlayer[0];
		PLAYER_INFO* pPlayerInfo = pAttacker->GetPlayerInfo();

		// 최소값
		pPlayerInfo->nINT = 1;
		CHECK_EQUAL(0.8f, m_Calc.CalcCharacterFactorForMagic(pAttacker));

		// 기준값
		pPlayerInfo->nINT = 100;
		CHECK_EQUAL(1.0f, m_Calc.CalcCharacterFactorForMagic(pAttacker));

		// 최대값
		pPlayerInfo->nINT = 255;
		CHECK_EQUAL(1.31f, m_Calc.CalcCharacterFactorForMagic(pAttacker));
	}

	TEST_FIXTURE(FAttackDamageCalculator, CalcCharacterFactorMelee)
	{
		GEntityPlayer* pAttacker = m_ppPlayer[0];
		PLAYER_INFO* pPlayerInfo = pAttacker->GetPlayerInfo();

		// 최소값
		pPlayerInfo->nSTR = 1;
		CHECK_EQUAL(0.8f, m_Calc.CalcCharacterFactorForMelee(pAttacker));
		
		// 기준값
		pPlayerInfo->nSTR = 100;
		CHECK_EQUAL(1.0f, m_Calc.CalcCharacterFactorForMelee(pAttacker));

		// 최대값
		pPlayerInfo->nSTR = 255;
		CHECK_EQUAL(1.31f, m_Calc.CalcCharacterFactorForMelee(pAttacker));
	}

	TEST_FIXTURE(FAttackDamageCalculator, CalcCharacterFactorForRange)
	{
		GEntityPlayer* pAttacker = m_ppPlayer[0];
		PLAYER_INFO* pPlayerInfo = pAttacker->GetPlayerInfo();

		// 최소값
		pPlayerInfo->nSTR = 1;
		pPlayerInfo->nDEX = 1;
		CHECK_EQUAL(0.8f, m_Calc.CalcCharacterFactorForRange(pAttacker));

		// 기준값
		pPlayerInfo->nSTR = 100;
		pPlayerInfo->nDEX = 100;
		CHECK_EQUAL(1.0f, m_Calc.CalcCharacterFactorForRange(pAttacker));

		// 최대값
		pPlayerInfo->nSTR = 255;
		pPlayerInfo->nDEX = 255;
		CHECK_EQUAL(1.31f, m_Calc.CalcCharacterFactorForRange(pAttacker));
	}

	TEST_FIXTURE(FAttackDamageCalculator, CalcCharacterFactor_MagicAttack)
	{
		GEntityPlayer* pAttacker = m_ppPlayer[0];

		MockCalculator calc;
		EXPECT_CALL(calc, CalcCharacterFactorForMagic(pAttacker));

		// 마법 공격
		GTalentInfo* pTalentInfo = test.talent.NewMagicTalentInfo();
		calc.CalcCharacterFactor(pAttacker, pTalentInfo);
	}

	TEST_FIXTURE(FAttackDamageCalculator, CalcCharacterFactor_MeleeAttack)
	{
		GEntityPlayer* pAttacker = m_ppPlayer[0];

		MockCalculator calc;
		EXPECT_CALL(calc, CalcCharacterFactorForMelee(pAttacker));

		// 근접 공격
		GTalentInfo* pTalentInfo = test.talent.NewMeleeTalentInfo();
		calc.CalcCharacterFactor(pAttacker, pTalentInfo);
	}

	TEST_FIXTURE(FAttackDamageCalculator, CalcCharacterFactor_RangeAttack)
	{
		GEntityPlayer* pAttacker = m_ppPlayer[0];

		MockCalculator calc;
		EXPECT_CALL(calc, CalcCharacterFactorForRange(pAttacker));

		// 원거리 공격
		GTalentInfo* pTalentInfo = test.talent.NewArcheryTalentInfo();
		calc.CalcCharacterFactor(pAttacker, pTalentInfo);
	}

	TEST_FIXTURE(FAttackDamageCalculator, CalcCharacterFactor_PiercingAttack)
	{
		GEntityPlayer* pAttacker = m_ppPlayer[0];

		MockCalculator calc;
		EXPECT_CALL(calc, CalcCharacterFactorForRange(pAttacker));

		// 찌르기 공격
		GTalentInfo* pTalentInfo = test.talent.NewMeleeTalentInfo();
		test.item.EquipNewWeapon(pAttacker, WEAPON_1H_PIERCE);

		calc.CalcCharacterFactor(pAttacker, pTalentInfo);
	}

	TEST_FIXTURE(FAttackDamageCalculator, CalcCharacterFactor_DualWeaponAttack)
	{
		GEntityPlayer* pAttacker = m_ppPlayer[0];

		MockCalculator calc;
		EXPECT_CALL(calc, CalcCharacterFactorForRange(pAttacker));

		// 듀얼 무기 공격
		GTalentInfo* pTalentInfo = test.talent.NewMeleeTalentInfo();
		GItem* pRightWeapon = test.item.GainWeaponItem(pAttacker, SUnitTestUtil::NewID(), WEAPON_1H_PIERCE);
		GItem* pLeftWeapon = test.item.GainWeaponItem(pAttacker, SUnitTestUtil::NewID(), WEAPON_1H_PIERCE);
		
		test.item.EquipItem(pAttacker, pRightWeapon, ITEMSLOT_RWEAPON);
		test.item.EquipItem(pAttacker, pLeftWeapon, ITEMSLOT_LWEAPON);

		calc.CalcCharacterFactor(pAttacker, pTalentInfo);
	}
}
