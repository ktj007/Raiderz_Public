#include "StdAfx.h"
#include "GTestForward.h"
#include "GEntityPlayer.h"
#include "GModuleCombat.h"
#include "GServer.h"
#include "GFieldMgr.h"
#include "FBasePlayer.h"

SUITE(TalentFocus_Counter)
{
	struct FTalentFocus : public FBasePlayer
	{
		FTalentFocus()
		{ 
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = NewEntityPlayer(m_pField, vec3(100,100,0));
			m_pLinkTester = test.network.NewLink(m_pPlayer);
			swprintf_s(m_pPlayer->GetPlayerInfo()->szName, L"Tester");

			gsys.pServer->FrameCounterClear();
			SetIgnoreCommonCommand(m_pLinkTester);
		}

		~FTalentFocus() 
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
			pLink->AddIgnoreID(MC_ACTION_GUARD);
			pLink->AddIgnoreID(MC_FIELD_IN_PLAYER);
			pLink->AddIgnoreID(MC_FIELD_OUT_PLAYER);
			pLink->AddIgnoreID(MC_ACTION_USE_TALENT);
			pLink->AddIgnoreID(MC_ACTION_TALENT_HIT);
			pLink->AddIgnoreID(MC_ACTION_TALENT_HIT_NO_MF);
			pLink->AddIgnoreID(MC_BUFF_GAIN);
			pLink->AddIgnoreID(MC_BUFF_LOST);
			pLink->AddIgnoreID(MC_ITEM_UPDATE_DURABILITY);
			pLink->AddIgnoreID(MC_TRAINING_TRAIN);
			
		}

		void GainAbsoluteGuardBuff(GEntityActor* pEffector)
		{
			DCHECK(pEffector);

			// 절대방어 버프 얻기
			int nBuffID = 123;
			GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
			pBuff->m_nPassiveExtraAttrib = BUFPEA_ABSOLUTE_GUARD;
			CSBuffEnchantInfo infoEnchant;
			infoEnchant.nID = nBuffID;
			infoEnchant.EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
			infoEnchant.EffectInfo.m_nRelation = CSEffectInfo::RELATION_PARTY;
			test.buff.GainBuffSelf(pEffector, pBuff, &infoEnchant);
		}

		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;
		MockLink*		m_pLinkTester;

		GTestMgrWrapper<GFieldInfoMgr>				m_FieldInfoMgr;
		GTestMgrWrapper<GTalentInfoMgr>				m_TalentInfoMgr;
		GTestMgrWrapper<GFieldMgr>					m_FieldMgr;
		GTestSysWrapper2<GSystem, GUnitTestUtil::FakeSystem>  m_FakeSystemWrapper;
		GTestSysWrapper2<GCombatCalculator, SimpleCombatCalculator>				m_CombatCalculator;
		GTestSysWrapper2<GAttackDamageCalculator, SimpleAttackDamageCalculator>	m_AttackDamageCalculator;
	};

	TEST_FIXTURE(FTalentFocus, Activate)
	{	
		test.focus.GainItemForCounter(m_pPlayer);

		BeginCommandRecord();
		
		// 반격 포커스를 얻음
		test.focus.GainCounter(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_COUNTER));

		// 패킷 체크
		const minet::MCommand& Command = m_pLinkTester->GetCommand(0);
		CHECK_EQUAL(Command.GetID(), MC_FOCUS_GAIN);
		if (Command.GetParameterCount() < 2)						 { CHECK(false); return; }
		MUID uidActor;
		TALENT_FOCUS_TYPE nFocusType;
		if (!Command.GetParameter(&uidActor,	0, MPT_UID))		{ CHECK(false); return; }
		if (!Command.GetParameter(&nFocusType,	1, MPT_INT))		{ CHECK(false); return; }
		CHECK_EQUAL(uidActor, m_pPlayer->GetUID());
		CHECK_EQUAL(nFocusType, TFT_COUNTER);
	}
	
	TEST_FIXTURE(FTalentFocus, GainCounterFocusOnAbsoluteGuard)
	{	
		test.focus.LearnLicense_Counter(m_pPlayer);
		test.item.EquipNewShield(m_pPlayer);
		test.focus.GainItemForCounter(m_pPlayer);

		// 포커스 버프
		test.buff.NewBuffInfo(BUFF_FOCUS_ID[TFT_COUNTER]);
		GEntityPlayer* pAttacker = NewEntityPlayer(m_pField, vec3(100,100,0));
		GTalentInfo* p1DmgTalent = test.talent.MakeDamageTalent(1);

		// 절대 방어
		GainAbsoluteGuardBuff(m_pPlayer);
		GTalentInfo* pGuardTalent = test.talent.NewGuardTalentInfo();
		m_pPlayer->doGuard(pGuardTalent->m_nID);

		// 공격
		test.talent.UseTalent(pAttacker, p1DmgTalent, m_pPlayer->GetUID());

		// 포커스 얻음
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_COUNTER));
	}

	TEST_FIXTURE(FTalentFocus, LostFocusAfterUseFocusTalent)
	{
		test.focus.GainItemForCounter(m_pPlayer);

		// 반격 포커스가 필요한 탤런트
		GTalentInfo* pCounterTalent = test.talent.MakeDamageTalent(1);
		pCounterTalent->m_nRequireFocus = TFT_COUNTER;

		// 반격 포커스를 얻음
		test.focus.GainCounter(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_COUNTER));

		BeginCommandRecord();

		// 반격 포커스를 얻은후에 반격 포커스가 필요한 탤런트를 사용
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_COUNTER));
		GEntityPlayer* pDefender = NewEntityPlayer(m_pField, vec3(100,100,0));
		test.talent.UseTalent(m_pPlayer, pCounterTalent, pDefender->GetUID());
		m_pField->Update(0.1f);	// 탤런트를 쓰기 위한 틱

		// 다음포커스 속성이 없기때문에 포커스를 잃는다.
		CHECK_EQUAL(false, m_pPlayer->HasFocus());

		// 패킷 체크
		const minet::MCommand& Command = m_pLinkTester->GetCommand(0);
		CHECK_EQUAL(Command.GetID(), MC_FOCUS_LOST);
		if (Command.GetParameterCount() < 2)						 { CHECK(false); return; }
		MUID uidActor;
		TALENT_FOCUS_TYPE nFocusType;
		if (!Command.GetParameter(&uidActor,	0, MPT_UID))		{ CHECK(false); return; }
		if (!Command.GetParameter(&nFocusType,	1, MPT_INT))		{ CHECK(false); return; }
		CHECK_EQUAL(uidActor, m_pPlayer->GetUID());
		CHECK_EQUAL(nFocusType, TFT_COUNTER);
	}

	TEST_FIXTURE(FTalentFocus, LostFocusByExpiredDurationTime)
	{
		// 포커스 얻기
		test.focus.GainItemForCounter(m_pPlayer);
		CHECK_EQUAL(false, m_pPlayer->HasFocus());
		test.focus.GainCounter(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_COUNTER));

		BeginCommandRecord();

		// 지속시간이 만료되어 포커스를 잃음
		m_pField->Update(GConst::FOCUS_COUNTER_DURATION_TIME);
		CHECK_EQUAL(false, m_pPlayer->HasFocus());

		// 패킷 체크
		const minet::MCommand& Command = m_pLinkTester->GetCommand(0);
		CHECK_EQUAL(Command.GetID(), MC_FOCUS_LOST);
		if (Command.GetParameterCount() < 2)						 { CHECK(false); return; }
		MUID uidActor;
		TALENT_FOCUS_TYPE nFocusType;
		if (!Command.GetParameter(&uidActor,	0, MPT_UID))		{ CHECK(false); return; }
		if (!Command.GetParameter(&nFocusType,	1, MPT_INT))		{ CHECK(false); return; }
		CHECK_EQUAL(uidActor, m_pPlayer->GetUID());
		CHECK_EQUAL(nFocusType, TFT_COUNTER);
	}

	TEST_FIXTURE(FTalentFocus, ContinueFocusAfterUseFocusTalent)
	{
		test.buff.NewBuffInfo(BUFF_FOCUS_ID[TFT_COUNTER]);
		test.focus.GainItemForCounter(m_pPlayer);

		GEntityPlayer* pDefender = NewEntityPlayer(m_pField, vec3(100,100,0));
		GTalentInfo* pCounterTalent = test.talent.MakeDamageTalent(1);
		pCounterTalent->m_nRequireFocus = TFT_COUNTER;
		pCounterTalent->m_nNextFocus = TFT_COUNTER;

		// 반격 포커스를 얻음
		test.focus.GainCounter(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_COUNTER));;

		// 반격 포커스를 얻은후에 반격 포커스가 필요한 탤런트를 사용
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_COUNTER));
		test.talent.UseTalent(m_pPlayer, pCounterTalent, pDefender->GetUID());

		// 다음포커스 속성이 있기때문에 유지된다.
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_COUNTER));
	}

	TEST_FIXTURE(FTalentFocus, NeedLicense)
	{
		test.focus.GainItemForCounter(m_pPlayer);

		CHECK_EQUAL(false, m_pPlayer->HasFocus());
		test.focus.GainCounter(m_pPlayer, false);
		CHECK_EQUAL(false, m_pPlayer->HasFocus());

		// 라이센스 배우기
		GTalentInfo* pTalent = test.talent.NewTalentInfo();
		pTalent->m_nTalentType = TT_SKILL;
		pTalent->m_nSkillType = ST_EXTRA_PASSIVE;
		pTalent->m_nExtraPassive = TEPT_FOCUS_USE_COUNTER;
		test.talent.Train(m_pPlayer, pTalent);
		test.focus.GainCounter(m_pPlayer, false);
		CHECK_EQUAL(true, m_pPlayer->HasFocus());
	}
}