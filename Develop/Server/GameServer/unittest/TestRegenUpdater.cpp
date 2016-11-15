#include "StdAfx.h"
#include "GTestForward.h"
#include "FBasePlayer.h"
#include "GPlayerDoing.h"
#include "GPlayerSit.h"

SUITE(TestRegenUpdater)
{
	struct FRegenUpdater : public FBasePlayer
	{
		FRegenUpdater()
		{
			// 관련 상수값 백업
			BACKUP_HP_REGEN_AMOUNT			= GConst::HP_REGEN_AMOUNT;
			BACKUP_HP_REGEN_RATE_IN_COMBAT	= GConst::HP_REGEN_RATE_IN_COMBAT;
			BACKUP_EN_REGEN_AMOUNT			= GConst::EN_REGEN_AMOUNT;
			BACKUP_EN_REGEN_RATE_IN_COMBAT	= GConst::EN_REGEN_RATE_IN_COMBAT;
			BACKUP_STA_REGEN_AMOUNT			= GConst::STA_REGEN_AMOUNT;
			BACKUP_STA_REGEN_RATE_IN_COMBAT = GConst::STA_REGEN_RATE_IN_COMBAT;
			BACKUP_SITTING_REGEN_RATE		= GConst::SITTING_REGEN_RATE;

			// 테스트용 상수값
			GConst::HP_REGEN_AMOUNT								= 4;
			GConst::HP_REGEN_RATE_IN_COMBAT						= 2.0f;
			GConst::EN_REGEN_AMOUNT								= 12;
			GConst::EN_REGEN_RATE_IN_COMBAT						= 1.5f;
			GConst::STA_REGEN_AMOUNT							= 17;
			GConst::STA_REGEN_RATE_IN_COMBAT					= 0.7f;
			GConst::SITTING_REGEN_RATE							= 1.5f;

			// 초기화
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = NewEntityPlayer(m_pField);
			m_pNPC = GUTHelper_NPC::SpawnNPC(m_pField);
			m_pPlayer->SetRegenActive(true);
			m_pNPC->SetRegenActive(true);
		}

		~FRegenUpdater()
		{
			// 관련 상수값 복구
			GConst::HP_REGEN_AMOUNT			= BACKUP_HP_REGEN_AMOUNT;
			GConst::HP_REGEN_RATE_IN_COMBAT	= BACKUP_HP_REGEN_RATE_IN_COMBAT;
			GConst::EN_REGEN_AMOUNT			= BACKUP_EN_REGEN_AMOUNT;
			GConst::EN_REGEN_RATE_IN_COMBAT	= BACKUP_EN_REGEN_RATE_IN_COMBAT;
			GConst::STA_REGEN_AMOUNT		= BACKUP_STA_REGEN_AMOUNT;
			GConst::STA_REGEN_RATE_IN_COMBAT = BACKUP_STA_REGEN_RATE_IN_COMBAT;
			GConst::SITTING_REGEN_RATE		= BACKUP_SITTING_REGEN_RATE;

			m_pField->Destroy();
		}

		int		BACKUP_HP_REGEN_AMOUNT;
		float	BACKUP_HP_REGEN_RATE_IN_COMBAT;
		int		BACKUP_EN_REGEN_AMOUNT;
		float	BACKUP_EN_REGEN_RATE_IN_COMBAT;
		int		BACKUP_STA_REGEN_AMOUNT;
		float	BACKUP_STA_REGEN_RATE_IN_COMBAT;
		float	BACKUP_SITTING_REGEN_RATE;

		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;
		GEntityNPC*		m_pNPC;
		DECLWRAPPER_FieldInfoMgr;
		DECLWRAPPER_FieldMgr;
	};

	TEST_FIXTURE(FRegenUpdater, RegularRegen_Player)
	{
		m_pPlayer->SetHP(1);
		m_pPlayer->SetEN(1);
		m_pPlayer->SetSTA(1);

		m_pField->Update(GConst::REGEN_TICK + 0.1f);

		CHECK_EQUAL(1+GConst::HP_REGEN_AMOUNT, m_pPlayer->GetHP());
		CHECK_EQUAL(1+GConst::EN_REGEN_AMOUNT, m_pPlayer->GetEN());
		CHECK_EQUAL(1+GConst::STA_REGEN_AMOUNT, m_pPlayer->GetSTA());
	}

	TEST_FIXTURE(FRegenUpdater, RegularRegen_Player_WhenSit)
	{
		m_pPlayer->SetHP(1);
		m_pPlayer->SetEN(1);
		m_pPlayer->SetSTA(1);

		m_pPlayer->GetSit().Sit();

		m_pField->Update(GConst::REGEN_TICK + 0.1f);

		CHECK_EQUAL(1+ (int)(GConst::HP_REGEN_AMOUNT * GConst::SITTING_REGEN_RATE), m_pPlayer->GetHP());
		CHECK_EQUAL(1+ (int)(GConst::EN_REGEN_AMOUNT * GConst::SITTING_REGEN_RATE), m_pPlayer->GetEN());
		CHECK_EQUAL(1+ (int)(GConst::STA_REGEN_AMOUNT * GConst::SITTING_REGEN_RATE), m_pPlayer->GetSTA());
	}


	TEST_FIXTURE(FRegenUpdater, RegularRegen_NPC)
	{
		m_pNPC->SetHP(1);
		m_pNPC->SetEN(1);
		m_pNPC->SetSTA(1);

		m_pField->Update(GConst::REGEN_TICK + 0.1f);

		CHECK_EQUAL(1+GConst::HP_REGEN_AMOUNT, m_pNPC->GetHP());
		CHECK_EQUAL(1+GConst::EN_REGEN_AMOUNT, m_pNPC->GetEN());
		CHECK_EQUAL(1, m_pNPC->GetSTA());	// NPC는 STA가 리젠되지 않음
	}

	TEST_FIXTURE(FRegenUpdater, CombatRegen_Player)
	{
		m_pPlayer->SetHP(1);
		m_pPlayer->SetEN(1);
		m_pPlayer->SetSTA(1);

		MakeCombat(m_pPlayer, m_pNPC);
		m_pField->Update(GConst::REGEN_TICK + 0.1f);

		CHECK_EQUAL(int(1+GConst::HP_REGEN_AMOUNT*GConst::HP_REGEN_RATE_IN_COMBAT), m_pPlayer->GetHP());
		CHECK_EQUAL(int(1+GConst::EN_REGEN_AMOUNT*GConst::EN_REGEN_RATE_IN_COMBAT), m_pPlayer->GetEN());
		CHECK_EQUAL(int(1+GConst::STA_REGEN_AMOUNT*GConst::STA_REGEN_RATE_IN_COMBAT), m_pPlayer->GetSTA());
	}

	TEST_FIXTURE(FRegenUpdater, CombatRegen_NPC)
	{
		m_pNPC->SetHP(1);
		m_pNPC->SetEN(1);
		m_pNPC->SetSTA(1);

		GUTHelper_NPC::MakeCombat(m_pNPC, m_pPlayer);
		m_pField->Update(GConst::REGEN_TICK + 0.1f);

		CHECK_EQUAL(int(1+GConst::HP_REGEN_AMOUNT*GConst::HP_REGEN_RATE_IN_COMBAT), m_pNPC->GetHP());
		CHECK_EQUAL(int(1+GConst::EN_REGEN_AMOUNT*GConst::EN_REGEN_RATE_IN_COMBAT), m_pNPC->GetEN());
		CHECK_EQUAL(1, m_pNPC->GetSTA());	// NPC는 STA가 리젠되지 않음
	}

	TEST_FIXTURE(FRegenUpdater, GuardRegen_Player)
	{
		m_pPlayer->SetHP(1);
		m_pPlayer->SetEN(1);
		m_pPlayer->SetSTA(1);

		test.talent.Guard(m_pPlayer);
		m_pField->Update(GConst::REGEN_TICK + 0.1f);

		CHECK_EQUAL(1+GConst::HP_REGEN_AMOUNT, m_pPlayer->GetHP());
		CHECK_EQUAL(1+GConst::EN_REGEN_AMOUNT, m_pPlayer->GetEN());
		CHECK_EQUAL(1, m_pPlayer->GetSTA());
	}
}