#include "StdAfx.h"
#include "GTestForward.h"
#include "GEntityPlayer.h"
#include "GModuleCombat.h"
#include "GServer.h"
#include "GFieldMgr.h"
#include "FBasePlayer.h"

SUITE(TalentFocus_Berserk)
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
			pLink->AddIgnoreID(MC_ITEM_UPDATE_DURABILITY);
			pLink->AddIgnoreID(MC_FIELD_IN_PLAYER);
			pLink->AddIgnoreID(MC_FIELD_OUT_PLAYER);
			pLink->AddIgnoreID(MC_ACTION_USE_TALENT);
			pLink->AddIgnoreID(MC_ACTION_TALENT_HIT);
			pLink->AddIgnoreID(MC_ACTION_TALENT_HIT_NO_MF);
			pLink->AddIgnoreID(MC_BUFF_GAIN);
			pLink->AddIgnoreID(MC_BUFF_LOST);
			pLink->AddIgnoreID(MC_CHAR_ENEMY_INFO);
		}

		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;
		MockLink*		m_pLinkTester;

		GTestMgrWrapper<GFieldInfoMgr>				m_FieldInfoMgr;
		GTestMgrWrapper<GFieldMgr>					m_FieldMgr;
		GTestSysWrapper2<GSystem, GUnitTestUtil::FakeSystem>  m_FakeSystemWrapper;
		GTestSysWrapper2<GCombatCalculator, SimpleCombatCalculator>				m_CombatCalculator;
		GTestSysWrapper2<GAttackDamageCalculator, SimpleAttackDamageCalculator>	m_AttackDamageCalculator;
	};

	TEST_FIXTURE(FTalentFocus, Activate)
	{
		test.focus.LearnLicense_Berserk(m_pPlayer);
		test.focus.GainItemForBerserk(m_pPlayer);
		
		m_pLinkTester->AddIgnoreID(MC_FOCUS_UPDATE_BUBBLE);
		m_pLinkTester->AddIgnoreID(MC_TRAINING_TRAIN);
		BeginCommandRecord();

		CHECK_EQUAL(false, m_pPlayer->HasFocus());
		test.focus.GainBerserk(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_BERSERK));
		
		// 패킷 체크
		const minet::MCommand& Command = m_pLinkTester->GetCommand(0);
		CHECK_EQUAL(Command.GetID(), MC_FOCUS_GAIN);
		if (Command.GetParameterCount() < 2)						 { CHECK(false); return; }
		MUID uidActor;
		TALENT_FOCUS_TYPE nFocusType;
		if (!Command.GetParameter(&uidActor,	0, MPT_UID))		{ CHECK(false); return; }
		if (!Command.GetParameter(&nFocusType,	1, MPT_INT))		{ CHECK(false); return; }
		CHECK_EQUAL(uidActor, m_pPlayer->GetUID());
		CHECK_EQUAL(nFocusType, TFT_BERSERK);
	}

	TEST_FIXTURE(FTalentFocus, LostFocusByExpiredDurationTime)
	{
		test.focus.LearnLicense_Berserk(m_pPlayer);
		test.focus.GainItemForBerserk(m_pPlayer);

		CHECK_EQUAL(false, m_pPlayer->HasFocus());
		test.focus.GainBerserk(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_BERSERK));

		SetIgnoreCommonCommand(m_pLinkTester);
		BeginCommandRecord();

		// 지속시간이 만료되어 광포 포커스를 잃음
		m_pField->Update(GConst::FOCUS_BERSERK_DURATION_TIME);
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
		CHECK_EQUAL(nFocusType, TFT_BERSERK);
	}

	TEST_FIXTURE(FTalentFocus, UseBerserkTalent)
	{
		test.focus.LearnLicense_Berserk(m_pPlayer);
		test.focus.GainItemForBerserk(m_pPlayer);
		
		GEntityPlayer* pDefender = NewEntityPlayer(m_pField, vec3(100,100,0));
		GTalentInfo* pBerserkTalent = test.talent.MakeDamageTalent(1);
		pBerserkTalent->m_nRequireFocus = TFT_BERSERK;
		pBerserkTalent->m_nNextFocus = TFT_NONE;
		GTalentInfo* pOtherTalent = test.talent.MakeDamageTalent(1);
		pOtherTalent->m_nRequireFocus = TFT_COUNTER;
		pOtherTalent->m_nNextFocus = TFT_NONE;

		CHECK_EQUAL(false, m_pPlayer->HasFocus());
		test.focus.GainBerserk(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_BERSERK));

		// 다른 포커스 탤런트를 쓸 수 없다.
		int nOldHP = pDefender->GetHP();
		test.talent.UseTalent(m_pPlayer, pOtherTalent, pDefender->GetUID());
		CHECK_EQUAL(nOldHP, pDefender->GetHP());	// 탤런트를 사용하지 못했음

		// 광포 포커스가 필요한 탤런트를 사용
		nOldHP = pDefender->GetHP();
		test.talent.UseTalent(m_pPlayer, pBerserkTalent, pDefender->GetUID());
		CHECK_EQUAL(nOldHP-1, pDefender->GetHP());	// 탤런트를 사용했음

		// NextFocus가 없어도 광포이 사라지지 않음
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_BERSERK));
	}

	//TEST_FIXTURE(FTalentFocus, OnlyGainOnePointBySingleAttack)
	//{
	//	EquipFocusEanbleItem();
	//	GEntityPlayer* pDefender = m_Helper.NewEntityPlayer(m_pField, vec3(100,100,0));
	//	GEntityPlayer* pDefender2 = m_Helper.NewEntityPlayer(m_pField, vec3(100,100,0));

	//	SetIgnoreCommonCommand(m_pLinkTester);
	//	BeginCommandRecord();

	//	
	//	GTalentInfo* p1DmgTalent = m_Helper.MakeSimpleTalent(1);

	//	// 광포상태가 될때까지 공격
	//	for (int i=0; i<GConst::FOCUS_BERSERK_HIT_COUNT; ++i)
	//	{
	//		CHECK_EQUAL(pmCombat->GetCurrentFocus(), TFT_NONE);
	//		m_pPlayer->GetModuleCombat()->UseTalent(p1DmgTalent, TALENT_TARGET_INFO(), true);
	//		m_pField->Update(0.0f);
	//	}

	//	// 광포 포커스를 얻음
	//	CHECK_EQUAL(pmCombat->GetCurrentFocus(), TFT_BERSERK);
	//}

	TEST_FIXTURE(FTalentFocus, NoGainBubbleInActiveFocus)
	{
		test.focus.LearnLicense_Berserk(m_pPlayer);
		test.focus.GainItemForBerserk(m_pPlayer);

		GModuleCombat* pmCombat = m_pPlayer->GetModuleCombat();
		GTalentInfo* p1DmgTalent = test.talent.MakeDamageTalent(1);

		// 포커스 얻음
		CHECK_EQUAL(false, m_pPlayer->HasFocus());
		test.focus.GainBerserk(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_BERSERK));

		CHECK_EQUAL(m_pField->GetPlayerQty(), 1);	// test.focus.GainBerserk()에서 공격대상을 하나 만듬

		CHECK_EQUAL(m_pPlayer->GetBerserkBubble(), 0);	// 포커스가 막 걸려서 버블은 0개
		test.talent.UseTalent(m_pPlayer, p1DmgTalent);
		CHECK_EQUAL(m_pPlayer->GetBerserkBubble(), 0);	// 포커스가 걸린 상태여서 버블이 늘어나지 않는다.

		// 지속시간이 만료되어 광포 포커스를 잃음
		m_pField->Update(GConst::FOCUS_BERSERK_DURATION_TIME);
		CHECK_EQUAL(false, m_pPlayer->HasFocus());

		// 포커스가 없어서 다시 버블이 증가된다.
		CHECK_EQUAL(m_pPlayer->GetBerserkBubble(), 0);
		GEntityPlayer* pDefender = NewEntityPlayer(m_pField, vec3(100,100,0));
		test.talent.UseTalent(m_pPlayer, p1DmgTalent);
		CHECK_EQUAL(m_pPlayer->GetBerserkBubble(), 1);
	}

	TEST_FIXTURE(FTalentFocus, RelayHitTimeExpire)
	{
		test.focus.LearnLicense_Berserk(m_pPlayer);
		test.focus.GainItemForBerserk(m_pPlayer);
		
		GEntityPlayer* pDefender = NewEntityPlayer(m_pField, vec3(100,100,0));
		GTalentInfo* p1DmgTalent = test.talent.MakeDamageTalent(1);

		// 1회 공격
		CHECK_EQUAL(m_pPlayer->GetBerserkBubble(), 0);
		test.talent.UseTalent(m_pPlayer, p1DmgTalent, pDefender->GetUID());
		CHECK_EQUAL(m_pPlayer->GetBerserkBubble(), 1);	// 버블증가

		// 2회 공격
		test.talent.UseTalent(m_pPlayer, p1DmgTalent, pDefender->GetUID());
		CHECK_EQUAL(m_pPlayer->GetBerserkBubble(), 2);	// 버블증가

		// 연타 만료시간이 지남
		m_pField->Update(GConst::FOCUS_BERSERK_RELAYHIT_TIME);

		// 3회 공격
		test.talent.UseTalent(m_pPlayer, p1DmgTalent, pDefender->GetUID());
		CHECK_EQUAL(m_pPlayer->GetBerserkBubble(), 1);	// 버블이 초기화되서 1
	}

	TEST_FIXTURE(FTalentFocus, WeaponCheck)
	{
		test.focus.LearnLicense_Berserk(m_pPlayer);

		// 포커스 얻을 수 없는 무기 장착하고 포커스 얻기 시도
		CHECK_EQUAL(false, m_pPlayer->HasFocus());
		test.item.EquipNewWeapon(m_pPlayer, WEAPON_1H_SLASH);
		test.focus.GainBerserk(m_pPlayer);
		CHECK_EQUAL(false, m_pPlayer->HasFocus(TFT_BERSERK));

		// 포커스 얻을 수 있는 무기(양손무기) 장착하고 포커스 얻기 시도
		CHECK_EQUAL(false, m_pPlayer->HasFocus(TFT_BERSERK));
		test.item.EquipNewWeapon(m_pPlayer, WEAPON_TWO_HANDED);
		test.focus.GainBerserk(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_BERSERK));
	}

	TEST_FIXTURE(FTalentFocus, GainBubbleByOnlyRightEquipment)
	{
		test.focus.LearnLicense_Berserk(m_pPlayer);
		GEntityPlayer* pDefender = NewEntityPlayer(m_pField, vec3(100,100,0));
		GTalentInfo* p1DmgTalent = test.talent.MakeDamageTalent(1);

		// 포커스 얻을 수 없는 무기 장착하고 버블 얻기 시도
		CHECK_EQUAL(false, m_pPlayer->HasFocus());
		test.item.EquipNewWeapon(m_pPlayer, WEAPON_1H_SLASH);
		CHECK_EQUAL(m_pPlayer->GetBerserkBubble(), 0);
		test.talent.UseTalent(m_pPlayer, p1DmgTalent, pDefender->GetUID());
		CHECK_EQUAL(m_pPlayer->GetBerserkBubble(), 0);	// 버블 증가 안됨

		// 포커스 얻을 수 있는 무기(양손무기) 장착하고 버블 얻기 시도
		CHECK_EQUAL(false, m_pPlayer->HasFocus(TFT_BERSERK));
		test.item.EquipNewWeapon(m_pPlayer, WEAPON_TWO_HANDED);
		CHECK_EQUAL(m_pPlayer->GetBerserkBubble(), 0);
		test.talent.UseTalent(m_pPlayer, p1DmgTalent, pDefender->GetUID());
		CHECK_EQUAL(m_pPlayer->GetBerserkBubble(), 1);	// 버블 증가 됐음
	}

	TEST_FIXTURE(FTalentFocus, UpdateBubble)
	{
		TVALID(GConst::FOCUS_BERSERK_HIT_COUNT >= 1);

		test.focus.LearnLicense_Berserk(m_pPlayer);
		test.focus.GainItemForBerserk(m_pPlayer);

		GEntityPlayer* pDefender = NewEntityPlayer(m_pField, vec3(100,100,0));
		GTalentInfo* p1DmgTalent = test.talent.MakeDamageTalent(1);

		BeginCommandRecord();

		// 1회 공격
		CHECK_EQUAL(m_pPlayer->GetBerserkBubble(), 0);
		test.talent.UseTalent(m_pPlayer, p1DmgTalent, pDefender->GetUID());
		m_pField->Update(0.0f); // 버블 갱신에 필요한 틱
		CHECK_EQUAL(m_pPlayer->GetBerserkBubble(), 1);	// 버블증가

		// 패킷 체크
		const minet::MCommand& Command = m_pLinkTester->GetCommand(0);
		CHECK_EQUAL(Command.GetID(), MC_FOCUS_UPDATE_BUBBLE);
		if (Command.GetParameterCount() < 2)						 { CHECK(false); return; }
		TALENT_FOCUS_TYPE nFocusType;
		uint8 nBubbleQty;
		if (!Command.GetParameter(&nFocusType,	0, MPT_INT))		{ CHECK(false); return; }
		if (!Command.GetParameter(&nBubbleQty,	1, MPT_UCHAR))		{ CHECK(false); return; }
		CHECK_EQUAL(nFocusType, TFT_BERSERK);
		CHECK_EQUAL(nBubbleQty, 1*100/GConst::FOCUS_BERSERK_HIT_COUNT);
	}

	TEST_FIXTURE(FTalentFocus, Attribute_BubbleHit)
	{
		test.focus.LearnLicense_Berserk(m_pPlayer);
		test.focus.GainItemForBerserk(m_pPlayer);
		GEntityPlayer* pDefender = NewEntityPlayer(m_pField, vec3(100,100,0));

		CHECK_EQUAL(m_pPlayer->GetBerserkBubble(), 0);
		GTalentInfo* pBubbleHitTalent = test.talent.MakeDamageTalent(1);	// 버블이 증가되는 탤런트
		test.talent.UseTalent(m_pPlayer, pBubbleHitTalent, pDefender->GetUID());
		CHECK_EQUAL(m_pPlayer->GetBerserkBubble(), 1);	// 버블증가
		GTalentInfo* pNoBubbleHitTalent = test.talent.MakeDamageTalent(1);	// 버블이 증가되지 않는 탤런트
		pNoBubbleHitTalent->m_bBubbleHit = false;
		test.talent.UseTalent(m_pPlayer, pNoBubbleHitTalent, pDefender->GetUID());
		CHECK_EQUAL(m_pPlayer->GetBerserkBubble(), 1);	// 버블증가 안됨
	}

	TEST_FIXTURE(FTalentFocus, NeedLicense)
	{
		test.focus.GainItemForBerserk(m_pPlayer);

		CHECK_EQUAL(false, m_pPlayer->HasFocus());
		test.focus.GainBerserk(m_pPlayer, false);
		CHECK_EQUAL(false, m_pPlayer->HasFocus());

		// 라이센스 배우기
		GTalentInfo* pTalent = test.talent.NewTalentInfo();
		pTalent->m_nTalentType = TT_SKILL;
		pTalent->m_nSkillType = ST_EXTRA_PASSIVE;
		pTalent->m_nExtraPassive = TEPT_FOCUS_USE_BERSERK;
		test.talent.Train(m_pPlayer, pTalent);
		test.focus.GainBerserk(m_pPlayer, false);
		CHECK_EQUAL(true, m_pPlayer->HasFocus());
	}
}