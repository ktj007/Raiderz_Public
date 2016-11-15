#include "stdafx.h"
#include "GTestForward.h"
#include "GGlobal.h"
#include "GServer.h"
#include "FBasePlayer.h"

SUITE(ImmuneHandler)
{
	struct FImmuneHandler : public FBasePlayer
	{
		FImmuneHandler()
		{
			m_pField = GUTHelper_Field::DefaultMockField();

			// 플레이어 생성
			MakePlayer(m_pAttacker, L"Attacker");
			MakePlayer(m_pTester, L"Tester");
		}

		~FImmuneHandler()
		{
			m_pField->Destroy();
			test.network.ClearLinks();
			m_pField->Destroy();
		}

		void MakePlayer(GEntityPlayer*& pPlayer, std::wstring strName)
		{
			pPlayer = NewEntityPlayer(m_pField, vec3(100,100,0));
			swprintf_s(pPlayer->GetPlayerInfo()->szName, strName.c_str());
		}

		MockField*		m_pField;
		GEntityPlayer*	m_pAttacker;
		GEntityPlayer*	m_pTester;

		DECLWRAPPER_FieldInfoMgr;
		DECLWRAPPER_FieldMgr;
		DECLWRAPPER_BuffMgr;
		DECLWRAPPER_SimpleCombatCalc;
	};

	TEST_FIXTURE(FImmuneHandler, GainExtraAttribBuff)
	{
		// 모든 버프 특수효과가 걸림
		for (int i=0; i<BUFPEA_MAX; i++)
		{
			int nBuffID = 1000+i;
			float fDuration = 100.0f;
			GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
			pBuff->m_nPassiveExtraAttrib = (BUFF_PASSIVE_EXTRA_ATTRIB_)i;
			test.buff.GainBuffSelf(m_pTester, pBuff, fDuration);
			CHECK_EQUAL(true, m_pTester->HasBuff(nBuffID));
			m_pField->Update(fDuration);
			CHECK_EQUAL(false, m_pTester->HasBuff(nBuffID));
		}
	}

	TEST_FIXTURE(FImmuneHandler, ImmuneBuff)
	{
		for (int i=0; i<BUFPEA_MAX; i++)
		{
			// 면역 설정
			immune::INFO info;
			SetBitSet(info.nImmuneBuff, (BUFF_PASSIVE_EXTRA_ATTRIB_)i);
			m_pTester->GetImmuneHandler().Init(info);

			// 버프 안걸리는지 확인
			int nBuffID = 1000+i;
			GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
			pBuff->m_nPassiveExtraAttrib = (BUFF_PASSIVE_EXTRA_ATTRIB_)i;
			test.buff.GainBuffSelf(m_pTester, pBuff);
			CHECK_EQUAL(false, m_pTester->HasBuff(nBuffID));
		}
	}

	TEST_FIXTURE(FImmuneHandler, TakeDamageType)
	{
		// 모든 공격타입이 피해를 입음
		for (int i=0; i<DA_MAX; i++)
		{
			GTalentInfo* pAttackTalent = test.talent.NewTalentInfo();
			test.talent.SetTalentDamage(pAttackTalent, 10);
			pAttackTalent->m_nDamageAttrib = (DAMAGE_ATTRIB)i;

			m_pTester->SetHP(100);
			test.talent.UseTalent(m_pAttacker, pAttackTalent, m_pTester->GetUID());
			CHECK_EQUAL(90, m_pTester->GetHP());
		}
	}

	TEST_FIXTURE(FImmuneHandler, ImmuneSkillType)
	{
		// 모든 스킬타입이 피해를 입음
		for (int i=0; i<ST_MAX; i++)
		{
			// 면역 설정
			immune::INFO info;
			SetBitSet(info.nImmuneSkillType, (TALENT_SKILL_TYPE)i);
			m_pTester->GetImmuneHandler().Init(info);

			// 피해 안입는지 확인
			GTalentInfo* pAttackTalent = test.talent.NewTalentInfo();
			test.talent.SetTalentDamage(pAttackTalent, 10);
			pAttackTalent->m_nSkillType = (TALENT_SKILL_TYPE)i;

			m_pTester->SetHP(100);
			test.duel.NewMockDuel_Fighting(m_pAttacker, m_pTester);
			test.talent.UseTalent(m_pAttacker, pAttackTalent, m_pTester->GetUID());
			CHECK_EQUAL(100, m_pTester->GetHP());
		}
	}

	TEST_FIXTURE(FImmuneHandler, ImmuneDrain)
	{
		// 면역 설정
		immune::INFO info;
		SetBitSet(info.nImmuneCustomType, immune::IMMUNE_DRAIN);
		m_pTester->GetImmuneHandler().Init(info);

		// 피해 안입는지 확인
		GTalentInfo* pAttackTalent = test.talent.NewTalentInfo();
		test.talent.SetTalentDamage(pAttackTalent, 10);
		pAttackTalent->m_Drain.m_nType = TALENT_DRAIN_HP;
		pAttackTalent->m_Drain.m_fRate = 100.0f;// 100% 확률

		m_pTester->SetHP(100);
		test.duel.NewMockDuel_Fighting(m_pAttacker, m_pTester);
		test.talent.UseTalent(m_pAttacker, pAttackTalent, m_pTester->GetUID());
		CHECK_EQUAL(100, m_pTester->GetHP());
	}

	TEST_FIXTURE(FImmuneHandler, ImmuneDispel)
	{
		// 면역 설정
		immune::INFO info;
		SetBitSet(info.nImmuneCustomType, immune::IMMUNE_DISPEL);
		m_pTester->GetImmuneHandler().Init(info);

		// 버프 얻음
		int nBuffID = 123;
		CHECK_EQUAL(false, m_pTester->HasBuff(nBuffID));
		GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
		pBuff->m_nType = BUFT_BUFF;
		test.buff.GainBuffSelf(m_pTester, pBuff);
		CHECK_EQUAL(true, m_pTester->HasBuff(nBuffID));

		// 디스펠에 면역
		GTalentInfo* pActiveTalent = test.talent.NewTalentInfo();
		test.talent.SetExtraActiveTalentInfo(pActiveTalent, 1, TEAT_DISPEL_BUFF);
		pActiveTalent->m_nExtraActiveParam1 = 1;	// 1개만 디스펠
		pActiveTalent->m_nExtraActiveParam2 = 10;	// 스택파워 10레벨이하 제거
		pActiveTalent->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
		pActiveTalent->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
		test.talent.UseTalent(m_pAttacker, pActiveTalent, m_pTester->GetUID());
		CHECK_EQUAL(true, m_pTester->HasBuff(nBuffID));	// 안사라짐
	}

	TEST(WeaponDamageAttrib)
	{
		MockField* m_pField = GUTHelper_Field::DefaultMockField();
		GEntityPlayer* m_pAttacker = GUTHelper::NewEntityPlayer(m_pField, vec3(100,100,0));
		GEntityNPC* pTargetNPC = GUTHelper_NPC::SpawnNPC(m_pField);

		GTalentInfo* pAttackTalent = test.talent.NewTalentInfo();
		test.talent.SetTalentDamage(pAttackTalent, 10);
		pAttackTalent->m_WeaponReference = WR_RIGHT;	// 오른손 무기타입에 따름
		GItem* pWeapon = test.item.EquipNewWeapon(m_pAttacker, WEAPON_1H_SLASH);

		pWeapon->m_pItemData->m_nDamageAttrib = DA_NONE;	// 무기 타입 없음
		test.talent.UseTalent(m_pAttacker, pAttackTalent, pTargetNPC->GetUID());
		CHECK_EQUAL(DA_NONE, pTargetNPC->GetLastDamagedInfo().nType);

		pWeapon->m_pItemData->m_nDamageAttrib = DA_SLASH;	// 무기 타입은 슬래쉬
		test.talent.UseTalent(m_pAttacker, pAttackTalent, pTargetNPC->GetUID());
		CHECK_EQUAL(DA_SLASH, pTargetNPC->GetLastDamagedInfo().nType);
	}
}