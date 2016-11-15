#include "StdAfx.h"
#include "GTestForward.h"
#include "GEntityPlayer.h"
#include "GModuleCombat.h"
#include "GServer.h"
#include "GFieldMgr.h"
#include "FBasePlayer.h"
#include "GModuleBuff.h"

SUITE(TalentFocus)
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
			pLink->AddIgnoreID(MC_ACTION_USE_TALENT);
			pLink->AddIgnoreID(MC_ACTION_TALENT_HIT);
			pLink->AddIgnoreID(MC_ACTION_TALENT_HIT_NO_MF);
		}

		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;
		MockLink*		m_pLinkTester;

		DECLWRAPPER_Field;
		DECLWRAPPER_TalentMgr;
		DECLWRAPPER_SimpleCombatCalc;
		DECLWRAPPER_FakeSystem;
	};

	TEST_FIXTURE(FTalentFocus, NoFocusInStart)
	{
		CHECK_EQUAL(false, m_pPlayer->HasFocus());
	}

	TEST_FIXTURE(FTalentFocus, DoNotGainNPC)
	{	
		DECLWRAPPER_NPCInfoMgr;
		GEntityNPC* pTestNPC = GUTHelper_NPC::SpawnNPC(m_pField);
		GEntityPlayer* pAttacker = NewEntityPlayer(m_pField, vec3(100,100,0));
		GTalentInfo* p1DmgTalent = test.talent.MakeDamageTalent(1);

		// 방어
		int nGuardTalentID = 100;
		test.talent.NewGuardTalentInfo(nGuardTalentID);
		pTestNPC->doGuard(nGuardTalentID);

		// 공격
		test.talent.UseTalent(pAttacker, p1DmgTalent, pTestNPC->GetUID());

		// 반격 포커스를 얻지 못함
		GModuleCombat* pmCombat =  pTestNPC->GetModuleCombat();
		CHECK_EQUAL(false, pmCombat->HasFocus());
	}

	TEST_FIXTURE(FTalentFocus, UseTalentRequriedFocus)
	{
		// 반격 포커스를 활성화할 수 있는 무기를 장착
		test.item.EquipNewWeapon(m_pPlayer, WEAPON_1H_BLUNT);

		GEntityPlayer* pAttacker = NewEntityPlayer(m_pField, vec3(100,100,0));
		GTalentInfo* p1DmgTalent = test.talent.MakeDamageTalent(1);

		// 반격 포커스를 얻음
		test.focus.GainCounter(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_COUNTER));

		// 반격 포커스가 필요한 탤런트를 사용
		GTalentInfo* pTalentRequriedCounterFocus = test.talent.MakeDamageTalent(10);
		pTalentRequriedCounterFocus->m_nRequireFocus = TFT_COUNTER;	// 반격 포커스가 필요하다
		CHECK_EQUAL(pAttacker->GetHP(), 300);
		test.talent.UseTalent(m_pPlayer, pTalentRequriedCounterFocus, pAttacker->GetUID());
		CHECK_EQUAL(pAttacker->GetHP(), 290);	// 피해10을 입었다.

		// 광포 포커스가 필요한 탤런트를 사용
		GTalentInfo* pTalentRequriedBerserkFocus = test.talent.MakeDamageTalent(10);
		pTalentRequriedBerserkFocus->m_nRequireFocus = TFT_BERSERK;	// 광포 포커스가 필요하다
		CHECK_EQUAL(pAttacker->GetHP(), 290);
		test.talent.UseTalent(m_pPlayer, pTalentRequriedBerserkFocus, pAttacker->GetUID());
		CHECK_EQUAL(pAttacker->GetHP(), 290);	// 피해를 입지 않았다.
	}

	TEST_FIXTURE(FTalentFocus, GainFocusAfterUseTalent)
	{
		test.focus.LearnLicense_Counter(m_pPlayer);
		test.buff.NewBuffInfo(BUFF_FOCUS_ID[TFT_COUNTER]);

		// 반격 포커스를 활성화할 수 있는 무기를 장착
		test.item.EquipNewWeapon(m_pPlayer, WEAPON_1H_BLUNT);

		GModuleCombat* pmCombat =  m_pPlayer->GetModuleCombat();
		GEntityPlayer* pDefender = NewEntityPlayer(m_pField, vec3(100,100,0));

		//  포커스를 얻는 탤런트를 사용
		GTalentInfo* pTalentGainCounterFocus = test.talent.MakeDamageTalent(10);
		pTalentGainCounterFocus->m_nTiming = TC_FINISH_TALENT;
		pTalentGainCounterFocus->m_nNextFocus = TFT_COUNTER;	// 반격 포커스를 얻는다.

		// 반격 포커스를 얻음
		CHECK_EQUAL(false, pmCombat->HasFocus());
		m_pPlayer->GetModuleCombat()->UseTalent(pTalentGainCounterFocus, pDefender->GetUID(), true);
		m_pField->Update(0.0f);	// 탤런트를 쓰기 위한 틱
		CHECK_EQUAL(true, pmCombat->HasFocus(TFT_COUNTER));
	}

	TEST_FIXTURE(FTalentFocus, AnytimeUseTalentWithNoRequrieFocus)
	{
		// 반격 포커스를 활성화할 수 있는 무기를 장착
		test.item.EquipNewWeapon(m_pPlayer, WEAPON_1H_BLUNT);

		GModuleCombat* pmCombat =  m_pPlayer->GetModuleCombat();
		GEntityPlayer* pDefender = NewEntityPlayer(m_pField, vec3(100,100,0));
		GTalentInfo* p1DmgTalent = test.talent.MakeDamageTalent(1);

		// 포커스가 없을때 공격 가능
		CHECK_EQUAL(false, pmCombat->HasFocus());
		CHECK_EQUAL(pDefender->GetHP(), 300);
		test.talent.UseTalent(m_pPlayer, p1DmgTalent, pDefender->GetUID());
		CHECK_EQUAL(pDefender->GetHP(), 299);

		// 반격 포커스를 얻음
		test.focus.GainCounter(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_COUNTER));

		// 반격 포커스가 있어도 포커스가 필요없는 탤런트는 사용 가능
		CHECK_EQUAL(true, pmCombat->HasFocus(TFT_COUNTER));
		CHECK_EQUAL(pDefender->GetHP(), 299);
		test.talent.UseTalent(m_pPlayer, p1DmgTalent, pDefender->GetUID());
		CHECK_EQUAL(pDefender->GetHP(), 298);
	}

	TEST_FIXTURE(FTalentFocus, ResetFocusWhenDie)
	{
		// 반격 포커스를 활성화할 수 있는 무기를 장착
		test.item.EquipNewWeapon(m_pPlayer, WEAPON_1H_BLUNT);

		// 반격 포커스를 얻음
		test.focus.GainCounter(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_COUNTER));

		// 죽으면 포커스를 잃는다.
		m_pPlayer->doDie();
		CHECK_EQUAL(false, m_pPlayer->HasFocus());
	}

	TEST_FIXTURE(FTalentFocus, NotLostFocusAfterUseNoFocusTalent)
	{
		// 반격 포커스를 활성화할 수 있는 무기를 장착
		test.item.EquipNewWeapon(m_pPlayer, WEAPON_1H_BLUNT);

		GEntityPlayer* pDefender = NewEntityPlayer(m_pField, vec3(100,100,0));
		GTalentInfo* pCounterTalent = test.talent.MakeDamageTalent(1);

		// 반격 포커스를 얻음
		test.focus.GainCounter(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_COUNTER));

		// 반격 포커스를 얻은후에 다음포커스가 없는 탤런트를 사용함
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_COUNTER));
		test.talent.UseTalent(m_pPlayer, pCounterTalent, pDefender->GetUID());

		// 다음포커스 속성이 없는 탤런트를 써도 포커스를 잃지 않는다.
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_COUNTER));
	}

	TEST_FIXTURE(FTalentFocus, GainMultiFocus)
	{
		static const float TICK_TIME = 0.1f;

		// 포커스를 활성화할 수 있는 무기를 장착
		test.item.EquipNewWeapon(m_pPlayer, WEAPON_ARCHERY);

		// 포커스 아무것도 없음
		CHECK_EQUAL(false, m_pPlayer->HasFocus());
		CHECK_EQUAL(false, m_pPlayer->HasFocus(TFT_SNEAK));
		CHECK_EQUAL(false, m_pPlayer->HasFocus(TFT_PRECISION));

		// 두가지 포커스를 얻어본다.
		test.focus.ToggleSneak(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_SNEAK));
		CHECK_EQUAL(false, m_pPlayer->HasFocus(TFT_PRECISION));
		test.focus.TogglePrecision(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_SNEAK));
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_PRECISION));

		// 하나씩 해제해 본다.
		test.focus.ToggleSneak(m_pPlayer);
		m_pPlayer->GetModuleBuff()->Update(TICK_TIME);
		CHECK_EQUAL(false, m_pPlayer->HasFocus(TFT_SNEAK));
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_PRECISION));
		test.focus.TogglePrecision(m_pPlayer);
		m_pPlayer->GetModuleBuff()->Update(TICK_TIME);
		CHECK_EQUAL(false, m_pPlayer->HasFocus(TFT_SNEAK));
		CHECK_EQUAL(false, m_pPlayer->HasFocus(TFT_PRECISION));

		// 다시 걸어본다.
		test.focus.ToggleSneak(m_pPlayer);
		m_pPlayer->GetModuleBuff()->Update(TICK_TIME);		
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_SNEAK));
		CHECK_EQUAL(false, m_pPlayer->HasFocus(TFT_PRECISION));
		test.focus.TogglePrecision(m_pPlayer);
		m_pPlayer->GetModuleBuff()->Update(TICK_TIME);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_SNEAK));
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_PRECISION));
	}

	TEST_FIXTURE(FTalentFocus, PlayerRefCount)
	{
		CHECK_EQUAL(m_pField->GetPlayerQty(), 1);
		CHECK_EQUAL(TFT_SIZE+1, m_pPlayer->GetReferenceCount());

		// 캐릭터 삭제
		DeleteEntityPlayer(m_pPlayer);
		CHECK_EQUAL(0, m_pPlayer->GetReferenceCount());	// EntityPlayer 자체의 카운터만 삭제
		CHECK_EQUAL(m_pField->GetPlayerQty(), 0);
	}

	TEST_FIXTURE(FTalentFocus, GainFocusOnUseTalent)
	{
		test.buff.NewBuffInfo(BUFF_FOCUS_ID[TFT_ADVENT]);
		test.focus.GainItemForAdvent(m_pPlayer);

		GTalentInfo* pAdventTalent = test.talent.MakeDamageTalent(1);
		pAdventTalent->m_nTiming = TC_USE_TALENT;	// 탤런트 쓸때 포커스가 걸리게 하기 위함
		pAdventTalent->m_nRequireFocus = TFT_NONE;
		pAdventTalent->m_nNextFocus = TFT_ADVENT;		

		GEntityPlayer* pDefender = NewEntityPlayer(m_pField, vec3(100,100,0));
		test.talent.UseTalent(m_pPlayer, pAdventTalent, pDefender->GetUID());
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_ADVENT));
	}

	TEST_FIXTURE(FTalentFocus, GainFocusOnHitTalent)
	{
		test.buff.NewBuffInfo(BUFF_FOCUS_ID[TFT_ADVENT]);
		test.focus.GainItemForAdvent(m_pPlayer);

		GTalentInfo* pAdventTalent = test.talent.MakeDamageTalent(1);
		pAdventTalent->m_nTiming = TC_HIT_TALENT;	// 공격을 성공할때 포커스가 걸리게 하기 위함
		pAdventTalent->m_nRequireFocus = TFT_NONE;
		pAdventTalent->m_nNextFocus = TFT_ADVENT;		

		GEntityPlayer* pDefender = NewEntityPlayer(m_pField, vec3(100,100,0));
		test.talent.UseTalent(m_pPlayer, pAdventTalent, pDefender->GetUID());
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_ADVENT));
	}

	TEST_FIXTURE(FTalentFocus, ResetDruationTimeWhenUseFocusTalent)
	{
		test.focus.LearnLicense_Berserk(m_pPlayer);
		test.focus.GainItemForBerserk(m_pPlayer);
		CHECK_EQUAL(false, m_pPlayer->HasFocus());
		test.focus.GainBerserk(m_pPlayer);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_BERSERK));

		GTalentInfo* pFocusTalent = test.talent.MakeDamageTalent(1);
		pFocusTalent->m_nRequireFocus = TFT_BERSERK;
		pFocusTalent->m_nNextFocus = TFT_BERSERK;

		m_pField->Update(GConst::FOCUS_BERSERK_DURATION_TIME/2);

		GEntityPlayer* pDefender = NewEntityPlayer(m_pField, vec3(100,100,0));
		test.talent.UseTalent(m_pPlayer, pFocusTalent, pDefender->GetUID());

		// 연계포커스로 이어졌으므로 포커스 유지시간이 초기화됨
		m_pField->Update(GConst::FOCUS_BERSERK_DURATION_TIME/2);
		CHECK_EQUAL(true, m_pPlayer->HasFocus(TFT_BERSERK));

		// 지속시간이 만료되어 광포 포커스를 잃음
		m_pField->Update(GConst::FOCUS_BERSERK_DURATION_TIME/2);
		CHECK_EQUAL(false, m_pPlayer->HasFocus(TFT_BERSERK));
	}

	TEST_FIXTURE(FTalentFocus, LostFocusWhenChangeEquipIncorrectItem)
	{
		test.focus.LearnLicense_Counter(m_pPlayer);
		test.buff.NewBuffInfo(BUFF_FOCUS_ID[TFT_COUNTER]);

		// 반격 포커스를 활성화할 수 있는 무기를 장착
		test.item.EquipNewWeapon(m_pPlayer, WEAPON_1H_BLUNT);

		GModuleCombat* pmCombat =  m_pPlayer->GetModuleCombat();
		GEntityPlayer* pDefender = NewEntityPlayer(m_pField, vec3(100,100,0));

		//  포커스를 얻는 탤런트를 사용
		GTalentInfo* pTalentGainCounterFocus = test.talent.MakeDamageTalent(10);
		pTalentGainCounterFocus->m_nTiming = TC_USE_TALENT;
		pTalentGainCounterFocus->m_nNextFocus = TFT_COUNTER;	// 반격 포커스를 얻는다.

		// 반격 포커스를 얻음
		CHECK_EQUAL(false, pmCombat->HasFocus());
		m_pPlayer->GetModuleCombat()->UseTalent(pTalentGainCounterFocus, pDefender->GetUID(), true);
		m_pField->Update(0.0f);	// 탤런트를 쓰기 위한 틱
		CHECK_EQUAL(true, pmCombat->HasFocus(TFT_COUNTER));

		// 반격 포커스를 활성화할 수 없는 무기 장착
		test.item.EquipNewWeapon(m_pPlayer, WEAPON_STAFF);
		CHECK_EQUAL(false, pmCombat->HasFocus(TFT_COUNTER));
	}
}
