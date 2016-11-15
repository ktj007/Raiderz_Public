#include "stdafx.h"
#include "GFieldInfoMgr.h"
#include "GTestForward.h"
#include "GModuleBuff.h"
#include "GServer.h"
#include "GFieldMgr.h"
#include "FBasePlayer.h"
#include "GModuleCombat.h"

SUITE(Buff_Effect)
{
	struct FBuff : public FBasePlayer
	{
		FBuff()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = NewEntityPlayer(m_pField, vec3(100,100,0));
			m_pLinkTester = test.network.NewLink(m_pPlayer);
			swprintf_s(m_pPlayer->GetPlayerInfo()->szName, L"Tester");

			gsys.pServer->FrameCounterClear();
			SetIgnoreCommonCommand(m_pLinkTester);
		}

		~FBuff()
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

		void GradualyLostBuff(int nBuffID)
		{
			for (int i=0; i<5; i++)
			{
				// 주기적 효과
				CHECK_EQUAL(true, m_pPlayer->HasBuff(nBuffID));
				m_pField->Update(2.0f);
			}
		}

		GTalentInfo* CreateTestTalent_BuffPassiveExtra(int nBuffID, float fDurationTime, BUFF_PASSIVE_EXTRA_ATTRIB_ eAttrib)
		{
			GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
			pBuff->m_nPassiveExtraAttrib = eAttrib;
			pBuff->m_fDuration = fDurationTime;
			GTalentInfo* pBuffTalent=test.buff.NewBuffTalent(pBuff);
			return pBuffTalent;
		}



		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;
		MockLink*		m_pLinkTester;

		DECLWRAPPER_FieldInfoMgr;
		DECLWRAPPER_FieldMgr;
		DECLWRAPPER_SimpleCombatCalc;
		DECLWRAPPER_BuffMgr;
	};

	TEST_FIXTURE(FBuff, OnStatus)
	{
		DECLWRAPPER_MissCombatCalc;

		int nBuffID = 123;
		GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
		pBuff->m_Condition.nEffectConditionType = TC_MISS_ME;			// 자신이 맞았을때
		pBuff->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;	// 자신에게
		// 적용될 속성
		pBuff->m_InstantModifier.nHP.nMod = 10;	
		pBuff->m_InstantModifier.nHP.fPer = 2.0f;	
		pBuff->m_InstantModifier.nEN.nMod = 20;	
		pBuff->m_InstantModifier.nEN.fPer = 3.0f;	
		pBuff->m_InstantModifier.nSTA.nMod = 10;
		pBuff->m_InstantModifier.nSTA.fPer = 2.0f;

		test.buff.GainBuffSelf(m_pPlayer, pBuff);
		CHECK_EQUAL(true, m_pPlayer->HasBuff(nBuffID));

		// 공격 이벤트
		m_pPlayer->SetHP(10);
		m_pPlayer->SetEN(10);
		m_pPlayer->SetSTA(10);
		GEntityPlayer* pAttacker = NewEntityPlayer(m_pField, m_pPlayer->GetPos()+vec3(10,10,0));
		GTalentInfo* p1DmgTalent = test.talent.MakeDamageTalent(1);
		test.talent.UseTalent(pAttacker, p1DmgTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(m_pPlayer->GetHP(), (10+10)*(1.0f+2.0f));
		CHECK_EQUAL(m_pPlayer->GetEN(), (10+20)*(1.0f+3.0f));
		CHECK_EQUAL(m_pPlayer->GetSTA(), (10+10)*(1.0f+2.0f));
	}

	TEST_FIXTURE(FBuff, OnHeal)
	{
		DECLWRAPPER_MissCombatCalc;

		int nBuffID = 123;
		GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
		pBuff->m_Condition.nEffectConditionType = TC_MISS_ME;			// 자신이 맞았을때
		pBuff->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;	// 자신에게
		// 적용될 속성
		pBuff->m_nMinHeal = 10;
		pBuff->m_nMaxHeal = 10;

		test.buff.GainBuffSelf(m_pPlayer, pBuff);
		CHECK_EQUAL(true, m_pPlayer->HasBuff(nBuffID));

		// 공격 이벤트
		m_pPlayer->SetHP(10);
		GEntityPlayer* pAttacker = NewEntityPlayer(m_pField, m_pPlayer->GetPos()+vec3(10,10,0));
		GTalentInfo* p1DmgTalent = test.talent.MakeDamageTalent(1);
		test.talent.UseTalent(pAttacker, p1DmgTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(m_pPlayer->GetHP(), 10+10);
	}

	TEST_FIXTURE(FBuff, OnHealPecent)
	{
		DECLWRAPPER_MissCombatCalc;

		int nBuffID = 123;
		GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
		pBuff->m_Condition.nEffectConditionType = TC_MISS_ME;			// 자신이 맞았을때
		pBuff->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;	// 자신에게

		// 적용될 속성
		pBuff->m_fPercentageHeal = 0.5f;

		test.buff.GainBuffSelf(m_pPlayer, pBuff);
		CHECK_EQUAL(true, m_pPlayer->HasBuff(nBuffID));

		// 공격 이벤트
		m_pPlayer->GetChrInfo()->nMaxHP = 800;
		m_pPlayer->SetHP(10);
		GEntityPlayer* pAttacker = NewEntityPlayer(m_pField, m_pPlayer->GetPos()+vec3(10,10,0));
		GTalentInfo* p1DmgTalent = test.talent.MakeDamageTalent(1);
		test.talent.UseTalent(pAttacker, p1DmgTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(m_pPlayer->GetHP(), 10+400);
	}

	TEST_FIXTURE(FBuff, OnStatus_WhenGain)
	{
		int nBuffID = 123;
		GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
		pBuff->m_Condition.nEffectConditionType = TC_BUFF_GAIN;	// 버프를 얻을때
		pBuff->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;	// 자신에게
		// 적용될 속성
		pBuff->m_InstantModifier.nHP.nMod = 10;	
		pBuff->m_InstantModifier.nHP.fPer = 2.0f;	
		pBuff->m_InstantModifier.nEN.nMod = 20;	
		pBuff->m_InstantModifier.nEN.fPer = 3.0f;	
		pBuff->m_InstantModifier.nSTA.nMod = 10;
		pBuff->m_InstantModifier.nSTA.fPer = 2.0f;

		m_pPlayer->SetHP(10);
		m_pPlayer->SetEN(10);
		m_pPlayer->SetSTA(10);

		test.buff.GainBuffSelf(m_pPlayer, pBuff);
		CHECK_EQUAL(true, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(m_pPlayer->GetHP(), (10+10)*(1.0f+2.0f));
		CHECK_EQUAL(m_pPlayer->GetEN(), (10+20)*(1.0f+3.0f));
		CHECK_EQUAL(m_pPlayer->GetSTA(), (10+10)*(1.0f+2.0f));
	}

	TEST_FIXTURE(FBuff, OnStatus_WhenLost)
	{
		int nBuffID = 123;
		float fDuration = 10.0f;
		GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
		pBuff->m_Condition.nEffectConditionType = TC_BUFF_GONE;	// 버프를 얻을때
		pBuff->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;	// 자신에게
		// 적용될 속성
		pBuff->m_InstantModifier.nHP.nMod = 10;	
		pBuff->m_InstantModifier.nHP.fPer = 2.0f;	
		pBuff->m_InstantModifier.nEN.nMod = 20;	
		pBuff->m_InstantModifier.nEN.fPer = 3.0f;	
		pBuff->m_InstantModifier.nSTA.nMod = 10;
		pBuff->m_InstantModifier.nSTA.fPer = 2.0f;

		m_pPlayer->SetHP(10);
		m_pPlayer->SetEN(10);
		m_pPlayer->SetSTA(10);

		test.buff.GainBuffSelf(m_pPlayer, pBuff, fDuration);
		CHECK_EQUAL(true, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(m_pPlayer->GetHP(), 10);
		CHECK_EQUAL(m_pPlayer->GetEN(), 10);
		CHECK_EQUAL(m_pPlayer->GetSTA(), 10);
		m_pField->Update(fDuration);
		CHECK_EQUAL(false, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(m_pPlayer->GetHP(), (10+10)*(1.0f+2.0f));
		CHECK_EQUAL(m_pPlayer->GetEN(), (10+20)*(1.0f+3.0f));
		CHECK_EQUAL(m_pPlayer->GetSTA(), (10+10)*(1.0f+2.0f));
	}

	TEST_FIXTURE(FBuff, OnMaxStatus)
	{
		int nBuffID = 123;
		float fDuration = 10.0f;
		GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
		pBuff->m_fPeriod = 2.0f;
		pBuff->m_fDuration = fDuration;
		pBuff->m_Condition.nEffectConditionType = TC_BUFF_GAIN;
		pBuff->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;	// 자신에게
		// 적용될 속성
		pBuff->m_ActorModifier.nHPMax.nMod = 10;	
		pBuff->m_ActorModifier.nHPMax.fPer = 2.0f;	
		pBuff->m_ActorModifier.nENMax.nMod = 20;	
		pBuff->m_ActorModifier.nENMax.fPer = 3.0f;	
		pBuff->m_ActorModifier.nSTAMax.nMod = 30;
		pBuff->m_ActorModifier.nSTAMax.fPer = 4.0f;
		
		GTalentInfo* pBuffTalent= test.buff.NewBuffTalent(pBuff);
		pBuffTalent->m_Buff1.EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
				
		int nOldMaxHP = m_pPlayer->GetMaxHP();
		int nOldMaxEN = m_pPlayer->GetMaxEN();
		int nOldMaxSTA = m_pPlayer->GetMaxSTA();

		// 버프 얻음
		test.talent.UseTalent(m_pPlayer, pBuffTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(true, m_pPlayer->HasBuff(nBuffID));

		// 버프 잃음
		GradualyLostBuff(nBuffID);
		CHECK_EQUAL(false, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(m_pPlayer->GetMaxHP(), nOldMaxHP);
		CHECK_EQUAL(m_pPlayer->GetMaxEN(), nOldMaxEN);
		CHECK_EQUAL(m_pPlayer->GetMaxSTA(), nOldMaxSTA);
	}

	TEST_FIXTURE(FBuff, OnAbiliity)
	{
		int nBuffID = 123;
		float fDuration = 10.0f;
		GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
		pBuff->m_fPeriod = 2.0f;
		pBuff->m_Condition.nEffectConditionType = TC_BUFF_GAIN;
		// 적용될 속성
		pBuff->m_PlayerModifier.nSTR.nMod = 10;	
		pBuff->m_PlayerModifier.nSTR.fPer = 2.0f;	
		pBuff->m_PlayerModifier.nDEX.nMod = 20;	
		pBuff->m_PlayerModifier.nDEX.fPer = 3.0f;	
		pBuff->m_PlayerModifier.nINT.nMod = 30;
		pBuff->m_PlayerModifier.nINT.fPer = 4.0f;
		pBuff->m_PlayerModifier.nCHA.nMod = 10;
		pBuff->m_PlayerModifier.nCHA.fPer = 2.0f;
		pBuff->m_PlayerModifier.nCON.nMod = 20;
		pBuff->m_PlayerModifier.nCON.fPer = 3.0f;
		GTalentInfo* pBuffTalent= test.buff.NewBuffTalent(pBuff);
		pBuffTalent->m_Buff1.EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
		pBuff->m_fDuration = fDuration;

		// 버프 얻음
		m_pPlayer->GetPlayerInfo()->nLevel = 1;
		m_pPlayer->GetPlayerInfo()->nSTR = 10;
		m_pPlayer->GetPlayerInfo()->nDEX = 10;
		m_pPlayer->GetPlayerInfo()->nINT = 10;
		m_pPlayer->GetPlayerInfo()->nCHA = 10;
		m_pPlayer->GetPlayerInfo()->nCON = 10;
		// 버프 얻음
		test.talent.UseTalent(m_pPlayer, pBuffTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(true, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(m_pPlayer->GetSTR(), (10+10)*(1.0f+2.0f));
		CHECK_EQUAL(m_pPlayer->GetDEX(), (10+20)*(1.0f+3.0f));
		CHECK_EQUAL(m_pPlayer->GetINT(), (10+30)*(1.0f+4.0f));
		CHECK_EQUAL(m_pPlayer->GetCHA(), (10+10)*(1.0f+2.0f));
		CHECK_EQUAL(m_pPlayer->GetCON(), (10+20)*(1.0f+3.0f));

		// 버프 잃음
		GradualyLostBuff(nBuffID);
		CHECK_EQUAL(false, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(m_pPlayer->GetSTR(), 10);
		CHECK_EQUAL(m_pPlayer->GetDEX(), 10);
		CHECK_EQUAL(m_pPlayer->GetINT(), 10);
		CHECK_EQUAL(m_pPlayer->GetCHA(), 10);
		CHECK_EQUAL(m_pPlayer->GetCON(), 10);
	}

	TEST_FIXTURE(FBuff, OnMoveSpeed)
	{
		int nBuffID = 123;
		float fDuration = 10.0f;
		GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
		// 적용될 속성
		pBuff->m_Condition.nEffectConditionType = TC_BUFF_GAIN;
		pBuff->m_ActorModifier.fMoveSpeed = 2.0f;	
		GTalentInfo* pBuffTalent= test.buff.NewBuffTalent(pBuff);
		pBuffTalent->m_Buff1.EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
		pBuff->m_fDuration = fDuration;
		pBuff->m_fPeriod = 2.0f;

		// 버프 얻음
		float fSpeed = (float)MoveSpeedByWeapon[WEAPON_NONE];
		test.talent.UseTalent(m_pPlayer, pBuffTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(true, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(fSpeed*(1.0f+2.0f), m_pPlayer->GetWalkSpeed());

		// 버프 잃음
		GradualyLostBuff(nBuffID);
		CHECK_EQUAL(false, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(fSpeed, m_pPlayer->GetWalkSpeed());
	}

	TEST_FIXTURE(FBuff, OnAP)
	{
		int nBuffID = 123;
		float fDuration = 10.0f;
		GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
		// 적용될 속성
		pBuff->m_Condition.nEffectConditionType = TC_BUFF_GAIN;
		pBuff->m_ActorModifier.nAP.nMod = 10;
		pBuff->m_ActorModifier.nAP.fPer = 2.0f;
		GTalentInfo* pBuffTalent= test.buff.NewBuffTalent(pBuff);
		pBuffTalent->m_Buff1.EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
		pBuff->m_fDuration = fDuration;
		pBuff->m_fPeriod = 2.0f;

		// 버프 얻음
		m_pPlayer->GetChrInfo()->nAP = 10;
		test.talent.UseTalent(m_pPlayer, pBuffTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(true, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(m_pPlayer->GetAP(), (10+10)*(1.0f+2.0f));

		// 버프 잃음
		GradualyLostBuff(nBuffID);
		CHECK_EQUAL(false, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(m_pPlayer->GetAP(), 10);
	}

	TEST_FIXTURE(FBuff, OnABS)
	{
		int nBuffID = 123;
		float fDuration = 10.0f;
		GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
		// 적용될 속성
		pBuff->m_Condition.nEffectConditionType = TC_BUFF_GAIN;
		pBuff->m_ActorModifier.nABS.nMod = 10;
		pBuff->m_ActorModifier.nABS.fPer = 2.0f;
		GTalentInfo* pBuffTalent= test.buff.NewBuffTalent(pBuff);
		pBuffTalent->m_Buff1.EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
		pBuff->m_fDuration = fDuration;
		pBuff->m_fPeriod = 2.0f;

		// 버프 얻음
		m_pPlayer->GetChrInfo()->nABS = 10;
		test.talent.UseTalent(m_pPlayer, pBuffTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(true, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(m_pPlayer->GetABS(), (10+10)*(1.0f+2.0f));

		// 버프 잃음
		GradualyLostBuff(nBuffID);
		CHECK_EQUAL(false, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(m_pPlayer->GetABS(), 10);
	}

	TEST_FIXTURE(FBuff, CounterAttack)
	{
		int nBuffID = 123;
		float fDuration = 10.0f;
		GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
		// 적용될 속성
		pBuff->m_Condition.nEffectConditionType = TC_BUFF_GAIN;
		pBuff->m_ActorModifier.nABS.nMod = 10;
		GTalentInfo* pBuffTalent= test.talent.MakeDamageTalent(10);
		test.talent.SetMeleeTalentInfo(pBuffTalent, pBuffTalent->m_nID);
		pBuffTalent->m_Buff1.nID = nBuffID;
		pBuffTalent->m_Buff1.EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
		pBuffTalent->m_Buff1.EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
		pBuffTalent->m_Buff1.nEnchantEvent = TC_COUNTER;
		pBuff->m_fDuration = fDuration;
		pBuffTalent->m_HitInfo.m_vSegments.clear();	// 기존 피격정보 제거
		test.talent.AddSimpleTalentInfoHitInfo(pBuffTalent, 10);	// 10초후에 피격
		test.talent.AddSimpleTalentInfoHitInfo(pBuffTalent, 11);	// 11초후에 피격 (탤런트를 1초 더 유지시키기 위한 편법, 탤런트가 종료된 후에 피격이 되고 있음)
		pBuffTalent->m_bUseInvincibleTime = true;	// 무적 사용 여부
		pBuffTalent->m_fInvincibleTimeStart = 0.0f;	// 무적 시작 시간
		pBuffTalent->m_fInvincibleTimeEnd = 10.0f;	// 무적 종료 시간
		pBuffTalent->Cooking();

		// 버프 얻음
		m_pPlayer->GetChrInfo()->nABS = 10;
		CHECK_EQUAL(false, m_pPlayer->HasBuff(nBuffID));	// 반격으로 인한 버프 걸리지 않음
		GEntityPlayer* pAttacker = NewEntityPlayer(m_pField, m_pPlayer->GetPos()+vec3(10,10,0));
		test.duel.NewMockDuel_Fighting(m_pPlayer, pAttacker); // 적대관계 설정
		GTalentInfo* p1DmgTalent = test.talent.MakeDamageTalent(1);
		test.talent.UseTalent(m_pPlayer, pBuffTalent, pAttacker->GetUID());
		CHECK_EQUAL(true, m_pPlayer->IsNowInvincibility());
		test.talent.UseTalent(pAttacker, p1DmgTalent, m_pPlayer->GetUID());
		m_pPlayer->GetModuleCombat()->Update(10.0f); // 사용중인 탤런트가 피격될만한 시간이 지남
		CHECK_EQUAL(true, m_pPlayer->HasBuff(nBuffID));	// 반격으로 인한 버프 걸림
		CHECK_EQUAL(m_pPlayer->GetABS(), 20);
	}

	TEST_FIXTURE(FBuff, RestorePeriodActorModifierEffect)
	{
		int nBuffID = 123;
		float fDuration = 10.0f;
		GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
		pBuff->m_fPeriod = 2.0f;
		pBuff->m_Condition.nEffectConditionType = TC_BUFF_GAIN_N_PERIOD;
		// 적용될 속성
		pBuff->m_PlayerModifier.nSTR.nMod = 10;	
		pBuff->m_PlayerModifier.nSTR.fPer = 2.0f;	
		GTalentInfo* pBuffTalent= test.buff.NewBuffTalent(pBuff);
		pBuffTalent->m_Buff1.EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
		pBuff->m_fDuration = fDuration;

		// 버프 얻음
		m_pPlayer->GetPlayerInfo()->nLevel = 1;
		m_pPlayer->GetPlayerInfo()->nSTR = 10;
		// 버프 얻음
		test.talent.UseTalent(m_pPlayer, pBuffTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(true, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(m_pPlayer->GetSTR(), 60);

		// 주기 효과
		m_pField->Update(2.0f);	
		CHECK_EQUAL(m_pPlayer->GetSTR(), 150);

		// 주기 효과
		m_pField->Update(2.0f);	
		CHECK_EQUAL(m_pPlayer->GetSTR(), 280);

		// 버프 잃음
		m_pField->Update(fDuration);	
		CHECK_EQUAL(false, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(m_pPlayer->GetSTR(), 10);
	}

	TEST_FIXTURE(FBuff, RestorePeriodActorModifierEffectTwice)
	{
		int nBuffID = 123;
		float fDuration = 10.0f;
		GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
		pBuff->m_fPeriod = 2.0f;
		pBuff->m_fDuration = fDuration;
		pBuff->m_Condition.nEffectConditionType = TC_BUFF_GAIN_N_PERIOD;
		pBuff->m_PlayerModifier.nSTR.nMod = 10;	
		pBuff->m_PlayerModifier.nSTR.fPer = 2.0f;	
		GTalentInfo* pBuffTalent= test.buff.NewBuffTalent(pBuff);
		pBuffTalent->m_Buff1.EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;

		int nBuffID2 = 234;
		GBuffInfo* pBuff2=test.buff.NewBuffInfo(nBuffID2);
		pBuff2->m_fPeriod = 2.0f;
		pBuff2->m_fDuration = fDuration;
		pBuff2->m_Condition.nEffectConditionType = TC_BUFF_GAIN_N_PERIOD;
		pBuff2->m_PlayerModifier.nSTR.nMod = 10;	
		pBuff2->m_PlayerModifier.nSTR.fPer = 2.0f;	
		GTalentInfo* pBuffTalent2= test.buff.NewBuffTalent(pBuff2);
		pBuffTalent2->m_Buff1.EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;

		// 버프 얻음
		m_pPlayer->GetPlayerInfo()->nLevel = 1;
		m_pPlayer->GetPlayerInfo()->nSTR = 10;
		// 버프 얻음
		test.talent.UseTalent(m_pPlayer, pBuffTalent, m_pPlayer->GetUID());
		test.talent.UseTalent(m_pPlayer, pBuffTalent2, m_pPlayer->GetUID());
		CHECK_EQUAL(true, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(true, m_pPlayer->HasBuff(nBuffID2));

		// 버프 잃음
		m_pField->Update(fDuration);	
		CHECK_EQUAL(false, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(false, m_pPlayer->HasBuff(nBuffID2));
		CHECK_EQUAL(m_pPlayer->GetSTR(), 10);
	}

	TEST_FIXTURE(FBuff, RestoreDuplicateBuffPeriodActorModifierEffect)
	{
		int nBuffID = 123;
		float fDuration = 10.0f;
		GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
		// 적용될 속성
		pBuff->m_Condition.nEffectConditionType = TC_BUFF_GAIN_N_PERIOD;
		pBuff->m_ActorModifier.fMoveSpeed = 2.0f;	
		GTalentInfo* pBuffTalent= test.buff.NewBuffTalent(pBuff);
		pBuffTalent->m_Buff1.EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
		pBuff->m_fDuration = fDuration;
		pBuff->m_fPeriod = 2.0f;

		// 버프 얻음
		float fSpeed = (float)MoveSpeedByWeapon[WEAPON_NONE];
		test.talent.UseTalent(m_pPlayer, pBuffTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(true, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(m_pPlayer->GetWalkSpeed(), (fSpeed)*(1.0f+2.0f));
		m_pPlayer->GetModuleBuff()->Update(pBuff->m_fPeriod);
		CHECK_EQUAL(m_pPlayer->GetWalkSpeed(), (fSpeed)*(1.0f+4.0f));
		test.talent.UseTalent(m_pPlayer, pBuffTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(m_pPlayer->GetWalkSpeed(), (fSpeed)*(1.0f+2.0f));

		// 버프 잃음
		GradualyLostBuff(nBuffID);
		CHECK_EQUAL(false, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(m_pPlayer->GetWalkSpeed(), fSpeed);
	}
}
