#include "StdAfx.h"
#include "GTestForward.h"
#include "GEntityPlayer.h"
#include "GModuleCombat.h"
#include "GServer.h"
#include "GFieldMgr.h"
#include "FBasePlayer.h"

SUITE(TalentFocus_Precision)
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
			pLink->AddIgnoreID(MC_BUFF_GAIN);
			pLink->AddIgnoreID(MC_BUFF_LOST);
			pLink->AddIgnoreID(MC_ACTION_USE_TALENT);
			pLink->AddIgnoreID(MC_ACTION_TALENT_HIT);
			pLink->AddIgnoreID(MC_ACTION_TALENT_HIT_NO_MF);
		}

		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;
		MockLink*		m_pLinkTester;

		GTestMgrWrapper<GFieldInfoMgr>				m_FieldInfoMgr;
		GTestMgrWrapper<GBuffInfoMgr>				m_BuffInfoMgr;
		GTestMgrWrapper<GFieldMgr>					m_FieldMgr;
		GTestSysWrapper2<GSystem, GUnitTestUtil::FakeSystem>  m_FakeSystemWrapper;
		GTestSysWrapper2<GCombatCalculator, SimpleCombatCalculator>				m_CombatCalculator;
		GTestSysWrapper2<GAttackDamageCalculator, SimpleAttackDamageCalculator>	m_AttackDamageCalculator;
	};

	TEST_FIXTURE(FTalentFocus, Activate)
	{
		test.focus.GainItemForPrecision(m_pPlayer);

		BeginCommandRecord();

		CHECK_EQUAL(false, m_pPlayer->HasFocus());
		test.focus.TogglePrecision(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_PRECISION));
		
		// 패킷 체크
		const minet::MCommand& Command = m_pLinkTester->GetCommand(0);
		CHECK_EQUAL(Command.GetID(), MC_FOCUS_GAIN);
		if (Command.GetParameterCount() < 2)						 { CHECK(false); return; }
		MUID uidActor;
		TALENT_FOCUS_TYPE nFocusType;
		if (!Command.GetParameter(&uidActor,	0, MPT_UID))		{ CHECK(false); return; }
		if (!Command.GetParameter(&nFocusType,	1, MPT_INT))		{ CHECK(false); return; }
		CHECK_EQUAL(uidActor, m_pPlayer->GetUID());
		CHECK_EQUAL(nFocusType, TFT_PRECISION);
	}

	TEST_FIXTURE(FTalentFocus, Toggle)
	{
		test.focus.GainItemForPrecision(m_pPlayer);

		CHECK_EQUAL(false, m_pPlayer->HasBuff(BUFF_FOCUS_ID[TFT_PRECISION]));
		CHECK_EQUAL(false, m_pPlayer->HasFocus());
		test.focus.TogglePrecision(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasBuff(BUFF_FOCUS_ID[TFT_PRECISION]));
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_PRECISION));
		test.focus.TogglePrecision(m_pPlayer);
		CHECK_EQUAL(false, m_pPlayer->HasBuff(BUFF_FOCUS_ID[TFT_PRECISION]));
		CHECK_EQUAL(false, m_pPlayer->HasFocus());
		test.focus.TogglePrecision(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasBuff(BUFF_FOCUS_ID[TFT_PRECISION]));
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_PRECISION));
		test.focus.TogglePrecision(m_pPlayer);
		CHECK_EQUAL(false, m_pPlayer->HasBuff(BUFF_FOCUS_ID[TFT_PRECISION]));
		CHECK_EQUAL(false, m_pPlayer->HasFocus());
	}

	TEST_FIXTURE(FTalentFocus, TickPay)
	{
		int nTickCost_STA = 5;
		test.focus.GainItemForPrecision(m_pPlayer);

		m_pPlayer->GetChrInfo()->nMaxSTA = 10000;
		m_pPlayer->SetSTA(nTickCost_STA*10);	// 10초간 버틸 수 있는 스태미너

		// 포커스 버프 만들기
		GBuffInfo* pFocusBuff = test.buff.NewBuffInfo(BUFF_FOCUS_ID[TFT_PRECISION]);
		pFocusBuff->m_nSTACost = nTickCost_STA;

		// 포커스 얻기
		CHECK_EQUAL(false, m_pPlayer->HasFocus());
		test.focus.TogglePrecision(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_PRECISION));

		SetIgnoreCommonCommand(m_pLinkTester);
		BeginCommandRecord();

		// 스태미너 전부 달때까지 대기
		for (int i=0; i<10*2; i++)		// 10초 버틸 스테미나를 가지고 있고, 반틱(.5초)마다 체크하므로 20번
		{
			CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_PRECISION));
			m_pField->Update(0.5f);
		}

		// 포커스를 잃었음
		m_pField->Update(1.0f);	// 버프 제거에 필요한 틱
		CHECK_EQUAL(false, m_pPlayer->HasFocus());
		CHECK_EQUAL(m_pPlayer->GetSTA(), 0);

		// 패킷 체크
		const minet::MCommand& Command = m_pLinkTester->GetCommand(0);
		CHECK_EQUAL(Command.GetID(), MC_FOCUS_LOST);
		if (Command.GetParameterCount() < 2)						 { CHECK(false); return; }
		MUID uidActor;
		TALENT_FOCUS_TYPE nFocusType;
		if (!Command.GetParameter(&uidActor,	0, MPT_UID))		{ CHECK(false); return; }
		if (!Command.GetParameter(&nFocusType,	1, MPT_INT))		{ CHECK(false); return; }
		CHECK_EQUAL(uidActor, m_pPlayer->GetUID());
		CHECK_EQUAL(nFocusType, TFT_PRECISION);
	}

	TEST_FIXTURE(FTalentFocus, UsePrecisionTalent)
	{
		test.focus.GainItemForPrecision(m_pPlayer);

		GEntityPlayer* pDefender = NewEntityPlayer(m_pField, vec3(100,100,0));
		GTalentInfo* pPrecisionTalent = test.talent.MakeDamageTalent(1);
		pPrecisionTalent->m_nRequireFocus = TFT_PRECISION;
		pPrecisionTalent->m_nNextFocus = TFT_NONE;
		GTalentInfo* pOtherTalent = test.talent.MakeDamageTalent(1);
		pOtherTalent->m_nRequireFocus = TFT_COUNTER;
		pOtherTalent->m_nNextFocus = TFT_NONE;

		test.focus.TogglePrecision(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_PRECISION));

		// 다른 포커스 탤런트를 쓸 수 없다.
		int nOldHP = pDefender->GetHP();
		test.talent.UseTalent(m_pPlayer, pOtherTalent, pDefender->GetUID());
		CHECK_EQUAL(nOldHP, pDefender->GetHP());	// 탤런트를 사용하지 못했음

		// 포커스가 필요한 탤런트를 사용
		nOldHP = pDefender->GetHP();
		test.talent.UseTalent(m_pPlayer, pPrecisionTalent, pDefender->GetUID());
		CHECK_EQUAL(nOldHP-1, pDefender->GetHP());	// 탤런트를 사용했음

		// NextFocus가 없어도 포커스가 사라지지 않음
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_PRECISION));
	}

	TEST_FIXTURE(FTalentFocus, WeaponCheck)
	{
		// 포커스 얻을 수 없는 무기 장착하고 포커스 얻기 시도
		CHECK_EQUAL(false, m_pPlayer->HasFocus());
		test.item.EquipNewWeapon(m_pPlayer, WEAPON_1H_PIERCE);
		test.focus.TogglePrecision(m_pPlayer);
		CHECK_EQUAL(false, m_pPlayer->HasFocus(TFT_PRECISION));

		// 포커스 얻을 수 있는 무기(활) 장착하고 포커스 얻기 시도
		CHECK_EQUAL(false, m_pPlayer->HasFocus());
		test.item.EquipNewWeapon(m_pPlayer, WEAPON_ARCHERY);
		test.focus.TogglePrecision(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_PRECISION));
	}

	TEST_FIXTURE(FTalentFocus, GainFocusBuffWhenFocusing)
	{
		CHECK_EQUAL(false, m_pPlayer->HasBuff(BUFF_FOCUS_ID[TFT_PRECISION]));

		// 포커스 얻기
		test.focus.GainItemForPrecision(m_pPlayer);
		test.focus.TogglePrecision(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_PRECISION));

		// 버프를 얻음
		CHECK_EQUAL(true, m_pPlayer->HasBuff(BUFF_FOCUS_ID[TFT_PRECISION]));
		m_pField->Update(10.0f);	
		CHECK_EQUAL(true, m_pPlayer->HasBuff(BUFF_FOCUS_ID[TFT_PRECISION])); // 영구히 유지되는 버프
	}
}