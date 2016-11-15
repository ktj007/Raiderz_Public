#include "StdAfx.h"
#include "GTestForward.h"
#include "GServer.h"
#include "GFieldMgr.h"
#include "FBasePlayer.h"
#include "GModuleBuff.h"

SUITE(Talent_Passive)
{
	struct FTalent : public FBasePlayer
	{
		FTalent()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = NewEntityPlayer(m_pField, vec3(100,100,0));
			m_pLinkTester = test.network.NewLink(m_pPlayer);
			swprintf_s(m_pPlayer->GetPlayerInfo()->szName, L"Tester");

			gsys.pServer->FrameCounterClear();
			SetIgnoreCommonCommand(m_pLinkTester);
		}

		~FTalent()
		{
			m_pField->Destroy();
			test.network.ClearLinks();
		}

		void BeginCommandRecord()
		{
			m_pLinkTester->ResetCommands();
		}

		void SetIgnoreCommonCommand(MockLink* pLink)
		{
			pLink->AddIgnoreID(MC_ACTION_USE_TALENT);
			pLink->AddIgnoreID(MC_ACTION_TALENT_HIT);
			pLink->AddIgnoreID(MC_ACTION_TALENT_HIT_NO_MF);
		}


		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;
		MockLink*		m_pLinkTester;

		DECLWRAPPER_FieldInfoMgr;
		DECLWRAPPER_FieldMgr;
		DECLWRAPPER_TalentMgr;
		DECLWRAPPER_SimpleCombatCalc;
	};

	TEST_FIXTURE(FTalent, ModEffect)
	{
		GTalentInfo* pPassiveTalent = test.talent.NewPassiveTalentInfo(1);
		pPassiveTalent->m_ActorModifier.nHPMax.nMod = 100;	
		pPassiveTalent->m_ActorModifier.nENMax.nMod = 100;	
		pPassiveTalent->m_ActorModifier.nENMax.fPer = 0.5f;	
		pPassiveTalent->m_ActorModifier.nSTAMax.nMod = 100;	

		pPassiveTalent->m_ActorModifier.nAP.nMod = 100;
		pPassiveTalent->m_ActorModifier.nAP.fPer = 0.5f;
		
		m_pPlayer->GetPlayerInfo()->nAP = 100;

		CHECK_EQUAL(m_pPlayer->GetMaxHP(), 800);
		CHECK_EQUAL(m_pPlayer->GetMaxEN(), 450);
		CHECK_EQUAL(m_pPlayer->GetMaxSTA(), 100);
		CHECK_EQUAL(m_pPlayer->GetAP(), 100);

		test.talent.Train(m_pPlayer, pPassiveTalent);
		CHECK_EQUAL(m_pPlayer->GetMaxHP(), 900);
		CHECK_EQUAL(m_pPlayer->GetMaxEN(), 825);
		CHECK_EQUAL(m_pPlayer->GetMaxSTA(), 200);
		CHECK_EQUAL(m_pPlayer->GetAP(), 300);

		test.talent.UntrainAll(m_pPlayer);
		CHECK_EQUAL(m_pPlayer->GetMaxHP(), 800);
		CHECK_EQUAL(m_pPlayer->GetMaxEN(), 450);
		CHECK_EQUAL(m_pPlayer->GetMaxSTA(), 100);
		CHECK_EQUAL(m_pPlayer->GetAP(), 100);
	}


	TEST_FIXTURE(FTalent, NoApplyInstantModByPassiveTalent)
	{
		GTalentInfo* pPassiveTalent = test.talent.NewPassiveTalentInfo(1);
		pPassiveTalent->m_InstantModifier.nHP.nMod = 100;	
		pPassiveTalent->m_InstantModifier.nEN.nMod = 100;	
		pPassiveTalent->m_InstantModifier.nEN.fPer = 0.5f;	
		pPassiveTalent->m_InstantModifier.nSTA.nMod = 100;	

		SetStatus(m_pPlayer, 100, 100, 100);

		CHECK_EQUAL(m_pPlayer->GetHP(), 100);
		CHECK_EQUAL(m_pPlayer->GetEN(), 100);
		CHECK_EQUAL(m_pPlayer->GetSTA(), 100);
		test.talent.Train(m_pPlayer, pPassiveTalent);
		CHECK_EQUAL(m_pPlayer->GetHP(), 100);
		CHECK_EQUAL(m_pPlayer->GetEN(), 100);
		CHECK_EQUAL(m_pPlayer->GetSTA(), 100);
		test.talent.UntrainAll(m_pPlayer);
		CHECK_EQUAL(m_pPlayer->GetHP(), 100);
		CHECK_EQUAL(m_pPlayer->GetEN(), 100);
		CHECK_EQUAL(m_pPlayer->GetSTA(), 100);
	}

	TEST_FIXTURE(FTalent, ExtraPassive)
	{
		CHECK_EQUAL(m_pPlayer->GetPassiveValue(TEPT_GUARD), 0);

		{
			GTalentInfo* pPassiveTalent = test.talent.NewExtraPassiveTalentInfo(TEPT_GUARD);
			pPassiveTalent->m_nTalentLine = 777;
			pPassiveTalent->m_nRank = 1;
			pPassiveTalent->m_nExtraPassiveParam = 1;
			test.talent.Train(m_pPlayer, pPassiveTalent);
		}
		
		CHECK_EQUAL(m_pPlayer->GetPassiveValue(TEPT_GUARD), 1);

		{
			GTalentInfo* pPassiveTalent = test.talent.NewExtraPassiveTalentInfo(TEPT_GUARD);
			pPassiveTalent->m_nTalentLine = 778;
			pPassiveTalent->m_nRank = 1;
			pPassiveTalent->m_nExtraPassiveParam = 1;
			test.talent.Train(m_pPlayer, pPassiveTalent);
		}

		CHECK_EQUAL(m_pPlayer->GetPassiveValue(TEPT_GUARD), 2);
	}

	//TEST_FIXTURE(FTalent, WeaponMastery)
	//{
	//	// 같은 탤런트계열의 다른 랭크들 적용
	//	GTalentInfo* pSlashMasteryRank1 = test.talent.NewExtraPassiveTalentInfo(TEPT_MASTERY_SLASH);
	//	pSlashMasteryRank1->m_nTalentLine = 777;
	//	pSlashMasteryRank1->m_nRank = 1;
	//	pSlashMasteryRank1->m_nExtraPassiveValue = 1;
	//	test.talent.LearnTalent(m_pPlayer, pSlashMasteryRank1);
	//	CHECK_EQUAL(m_pPlayer->GetWeaponMasteryRank(WEAPON_1H_SLASH), 1);

	//	GTalentInfo* pSlashMasteryRank3 = test.talent.NewExtraPassiveTalentInfo(TEPT_MASTERY_SLASH);
	//	pSlashMasteryRank3->m_nTalentLine = 777;
	//	pSlashMasteryRank3->m_nRank = 3;
	//	pSlashMasteryRank3->m_nExtraPassiveValue = 3;
	//	test.talent.LearnTalent(m_pPlayer, pSlashMasteryRank3);
	//	CHECK_EQUAL(m_pPlayer->GetWeaponMasteryRank(WEAPON_1H_SLASH), 3);	// 더 높은 효과라 바뀜

	//	GTalentInfo* pSlashMasteryRank2 = test.talent.NewExtraPassiveTalentInfo(TEPT_MASTERY_SLASH);
	//	pSlashMasteryRank2->m_nTalentLine = 777;
	//	pSlashMasteryRank2->m_nRank = 2;
	//	pSlashMasteryRank2->m_nExtraPassiveValue = 2;
	//	test.talent.LearnTalent(m_pPlayer, pSlashMasteryRank2);
	//	CHECK_EQUAL(m_pPlayer->GetWeaponMasteryRank(WEAPON_1H_SLASH), 3);	// 이전보다 낮은 효과라 무시

	//	// 다른 탤런트계열의 랭크를 적용
	//	GTalentInfo* pSlashMasteryRankB = test.talent.NewExtraPassiveTalentInfo(TEPT_MASTERY_SLASH);
	//	pSlashMasteryRankB->m_nTalentLine = 123;
	//	pSlashMasteryRankB->m_nRank = 1;
	//	pSlashMasteryRankB->m_nExtraPassiveValue = 10;
	//	test.talent.LearnTalent(m_pPlayer, pSlashMasteryRankB);
	//	CHECK_EQUAL(m_pPlayer->GetWeaponMasteryRank(WEAPON_1H_SLASH), 3+10);	// 효과 중첩
	//} 

	TEST_FIXTURE(FTalent, FocusBerserkDecreaseMaxBubble)
	{
		TVALID(GConst::FOCUS_BERSERK_HIT_COUNT >= 3);
		test.focus.GainItemForBerserk(m_pPlayer);
		test.focus.GainBerserk(m_pPlayer, true, GConst::FOCUS_BERSERK_HIT_COUNT-2);	// 필요 버블보다 2번 덜침
		CHECK_EQUAL(false, m_pPlayer->HasFocus(TFT_BERSERK));
		CHECK_EQUAL(m_pPlayer->GetBerserkBubble(), GConst::FOCUS_BERSERK_HIT_COUNT-2);
		m_pField->Update(10000.0f);	// 버블 없앰
		CHECK_EQUAL(m_pPlayer->GetBerserkBubble(), 0);

		// 패시브 탤런트를 배우고 재시도
		GTalentInfo* pPassive = test.talent.NewExtraPassiveTalentInfo(TEPT_FOCUS_BERSERK_DECREASE_MAXBUBBLE);
		pPassive->m_nExtraPassiveParam = 2;	// 필요 버블를 2개 줄여줌
		test.talent.Train(m_pPlayer, pPassive);
		test.focus.GainBerserk(m_pPlayer, true, GConst::FOCUS_BERSERK_HIT_COUNT-2);	// 필요 버블보다 2번 덜침
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_BERSERK));
	}

	TEST_FIXTURE(FTalent, FocusBerserkExtendTime)
	{
		test.focus.GainItemForBerserk(m_pPlayer);
		test.focus.GainBerserk(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_BERSERK));
		m_pField->Update(GConst::FOCUS_ENLIGHTEN_DURATION_TIME);	// 포커스 시간 만료
		CHECK_EQUAL(false, m_pPlayer->HasFocus(TFT_BERSERK));

		// 패시브 탤런트를 배운후에는 지속시간이 늘어난다.
		GTalentInfo* pPassive = test.talent.NewExtraPassiveTalentInfo(TEPT_FOCUS_BERSERK_EXTENDTIME);
		pPassive->m_nExtraPassiveParam = 12;	// 지속시간이 12초만큼 늘어남
		test.talent.Train(m_pPlayer, pPassive);
		test.focus.GainBerserk(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_BERSERK));
		m_pField->Update(GConst::FOCUS_BERSERK_DURATION_TIME);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_BERSERK));
		m_pField->Update(11.0f);	// 예전보다 11초 더 지남
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_BERSERK));
		m_pField->Update(1.0f);	// 12초후에 소멸
		CHECK_EQUAL(false, m_pPlayer->HasFocus(TFT_BERSERK));
	}

	TEST_FIXTURE(FTalent, FocusCounterExtendTime)
	{
		test.focus.GainItemForCounter(m_pPlayer);
		test.focus.GainCounter(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_COUNTER));
		m_pField->Update(GConst::FOCUS_COUNTER_DURATION_TIME);	// 포커스 시간 만료
		CHECK_EQUAL(false, m_pPlayer->HasFocus(TFT_COUNTER));

		// 패시브 탤런트를 배운후에는 지속시간이 늘어난다.
		GTalentInfo* pPassive = test.talent.NewExtraPassiveTalentInfo(TEPT_FOCUS_COUNTER_EXTENDTIME);
		pPassive->m_nExtraPassiveParam = 12;	// 지속시간이 12초만큼 늘어남
		test.talent.Train(m_pPlayer, pPassive);
		test.focus.GainCounter(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_COUNTER));
		m_pField->Update(GConst::FOCUS_COUNTER_DURATION_TIME);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_COUNTER));
		m_pField->Update(11.0f);	// 예전보다 11초 더 지남
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_COUNTER));
		m_pField->Update(1.0f);	// 12초후에 소멸
		CHECK_EQUAL(false, m_pPlayer->HasFocus(TFT_COUNTER));
	}

	TEST_FIXTURE(FTalent, TalentShoutRadius)
	{
		int nBuffID = 123;
		const float DURATION_TIME = 10.0f;
		GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
		GTalentInfo* pBuffTalent= test.buff.NewBuffTalent(pBuff);
		pBuffTalent->m_Buff1.EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
		pBuffTalent->m_Buff1.EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
		pBuffTalent->m_Buff1.EffectInfo.m_fRadius  = 0.0f;
		pBuff->m_fDuration = DURATION_TIME;
		pBuffTalent->m_strSpecialization = TALENT_PROPERTY_SHOUT;	// 외침 속성 탤런트

		// 효과 받을 액터들
		GEntityPlayer* pPlayer1 = NewEntityPlayer(m_pField);
		pPlayer1->SetPos(vec3(300, 0, 0));
		GEntityPlayer* pPlayer2 = NewEntityPlayer(m_pField);
		pPlayer2->SetPos(vec3(0, 300, 0));
		
		// 버프 유효거리가 0이라 시전자만 효과를 본다.
		CHECK_EQUAL(false, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(false, pPlayer1->HasBuff(nBuffID));
		CHECK_EQUAL(false, pPlayer2->HasBuff(nBuffID));
		test.talent.UseTalent(m_pPlayer, pBuffTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(true, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(false, pPlayer1->HasBuff(nBuffID));
		CHECK_EQUAL(false, pPlayer2->HasBuff(nBuffID));

		// 버프 해제
		m_pField->Update(DURATION_TIME);
		CHECK_EQUAL(false, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(false, pPlayer1->HasBuff(nBuffID));
		CHECK_EQUAL(false, pPlayer2->HasBuff(nBuffID));

		// 함성 연마를 배우면 효과범위가 늘어난다.
		GTalentInfo* pPassive = test.talent.NewExtraPassiveTalentInfo(TEPT_TALENT_SHOUT_RADIUS);
		pPassive->m_nExtraPassiveParam = 10;	// 외치기 범위가 10미터 늘어남
		test.talent.Train(m_pPlayer, pPassive);
		test.talent.UseTalent(m_pPlayer, pBuffTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(true, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(true, pPlayer1->HasBuff(nBuffID));
		CHECK_EQUAL(true, pPlayer2->HasBuff(nBuffID));

	}

	TEST_FIXTURE(FTalent, TestPlayerPassiveRank)
	{
		GPlayerPassiveTalent	playerPassiveTalent;

		const int MAX_TALENTS = 5;

		GTalentInfo* pPassiveTalent[MAX_TALENTS];
		for (int i = 0; i < MAX_TALENTS; i++)
		{
			pPassiveTalent[i] = test.talent.NewExtraPassiveTalentInfo(TEPT_SPECIALIZATION_SLASH, i+1);
			pPassiveTalent[i]->m_nTalentLine = 1;
			pPassiveTalent[i]->m_nRank = i+1;
			pPassiveTalent[i]->m_nExtraPassiveParam = i+1;
		}

		CHECK_EQUAL(playerPassiveTalent.GetRank(TEPT_SPECIALIZATION_SLASH), 0);

		playerPassiveTalent.GainEffect(pPassiveTalent[0]);
		CHECK_EQUAL(playerPassiveTalent.GetRank(TEPT_SPECIALIZATION_SLASH), 1);

		// 그냥 한번 더 같은 탤런트를 Gain해본다.
		playerPassiveTalent.GainEffect(pPassiveTalent[0]);
		CHECK_EQUAL(playerPassiveTalent.GetRank(TEPT_SPECIALIZATION_SLASH), 1);

		playerPassiveTalent.GainEffect(pPassiveTalent[1]);
		CHECK_EQUAL(playerPassiveTalent.GetRank(TEPT_SPECIALIZATION_SLASH), 2);
	}

	TEST_FIXTURE(FTalent, GuardMastery)
	{
		test.item.EquipNewShield(m_pPlayer);

		bool bEquippedShield = true;
		int nGuardLevel = m_pPlayer->GetPassiveValue(TEPT_GUARD);
		CHECK_EQUAL(0, nGuardLevel);
		CHECK_EQUAL(101050, CSTalentInfoHelper::NormalGuardTalentID(WEAPON_1H_SLASH, nGuardLevel, bEquippedShield));

		int nTalentLine = SUnitTestUtil::NewID();

		// 방어 연마를 배움
		GTalentInfo* pPassive = test.talent.NewExtraPassiveTalentInfo(TEPT_GUARD, SUnitTestUtil::NewID(), nTalentLine);
		pPassive->m_nExtraPassiveParam = 3;	// 숙련치가 3만큼 늘어남
		test.talent.Train(m_pPlayer, pPassive);

		nGuardLevel = m_pPlayer->GetPassiveValue(TEPT_GUARD);
		CHECK_EQUAL(3, nGuardLevel);
		CHECK_EQUAL(101053, CSTalentInfoHelper::NormalGuardTalentID(WEAPON_1H_SLASH, nGuardLevel, bEquippedShield));

		// 방패가 없는 경우에는 탤런트 아이디를 10 더한다.
		bEquippedShield = false;
		CHECK_EQUAL(101063, CSTalentInfoHelper::NormalGuardTalentID(WEAPON_1H_SLASH, nGuardLevel, bEquippedShield));

		// 방어 연마를 배움
		pPassive = test.talent.NewExtraPassiveTalentInfo(TEPT_GUARD, SUnitTestUtil::NewID(), nTalentLine);
		pPassive->m_nExtraPassiveParam = 5;	// 숙련치가 3만큼 늘어남
		test.talent.Train(m_pPlayer, pPassive);

		nGuardLevel = m_pPlayer->GetPassiveValue(TEPT_GUARD);
		CHECK_EQUAL(5, nGuardLevel);
		CHECK_EQUAL(101065, CSTalentInfoHelper::NormalGuardTalentID(WEAPON_1H_SLASH, nGuardLevel, bEquippedShield));
	} 

	TEST_FIXTURE(FTalent, SazahuSpecialization)
	{
		GEntityPlayer* pOtherPlayer = NewEntityPlayer(m_pField);
		int nBuffID = 1123;		// 디스펠 정책변경으로 일반 버프ID는 1000번 이상이어야합니다.
		CHECK_EQUAL(false, pOtherPlayer->HasBuff(nBuffID));
		GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
		pBuff->m_nType = BUFT_BUFF;
		test.buff.GainBuffSelf(pOtherPlayer, pBuff);
		CHECK_EQUAL(true, pOtherPlayer->HasBuff(nBuffID));

		GTalentInfo* pSazahuTalent=NULL;
		pSazahuTalent = test.talent.NewTalentInfo();
		pSazahuTalent->m_strSpecialization = TALENT_PROPERTY_SAZAHU;	// 사자후 속성 탤런트
		pSazahuTalent->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
		pSazahuTalent->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;

		// 사자후 탤런트 써보기
		test.talent.UseTalent(m_pPlayer, pSazahuTalent, pOtherPlayer->GetUID());
		pOtherPlayer->GetModuleBuff()->Update(0.1f); // 버프 삭제 처리 틱
		CHECK_EQUAL(true, pOtherPlayer->HasBuff(nBuffID));	// 버프가 사라지지 않음

		// 사자후 특화 배움
		GTalentInfo* pPassive = test.talent.NewExtraPassiveTalentInfo(TEPT_TALENT_SAZAHU_SPECIALIZATION);
		pPassive->m_nExtraPassiveParam = pPassive->m_nID;	// 사자후 특화 탤런트 ID (자기자신)
		pPassive->m_nExtraActive = TEAT_DISPEL_BUFF;	// 버프 디스펠
		pPassive->m_nExtraActiveParam1 = 10;	// 스택파워 10이하 버프		
		test.talent.Train(m_pPlayer, pPassive);

		// 사자후 탤런트 써보기
		test.talent.UseTalent(m_pPlayer, pSazahuTalent, pOtherPlayer->GetUID());
		pOtherPlayer->GetModuleBuff()->Update(0.1f); // 버프 삭제 처리 틱
		CHECK_EQUAL(false, pOtherPlayer->HasBuff(nBuffID));	// 버프가 사라짐
	}
}

