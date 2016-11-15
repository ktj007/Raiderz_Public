#include "stdafx.h"
#include "GUTHelper.h"
#include "GEntityPlayer.h"
#include "MockField.h"
#include "MockLink.h"
#include "GTestWrapper.h"
#include "GCalculator.h"
#include "GPlayerObjectManager.h"
#include "GPlayerFalling.h"
#include "GModuleBuff.h"
#include "GBuffInfo.h"
#include "GConst.h"
#include "CCommandTable.h"
#include "FBaseMockLink.h"
#include "GUTHelper_Field.h"

SUITE(PlayerFalling)
{
	struct Fixture : public FBaseMockLink
	{
		Fixture()
		{
			InitMockField();
			InitTestPlayers();
			InitTestModule();
		}
		virtual ~Fixture()
		{
			m_pField->Destroy();
		}
		void InitMockField()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
		}
		void InitTestPlayers()
		{
			m_pPlayer = GUTHelper::NewEntityPlayer(m_pField);
			m_pLink = NewLink(m_pPlayer);

			m_pSpectator = GUTHelper::NewEntityPlayer(m_pField);
			m_pSpectatorLink = NewLink(m_pSpectator);

			m_pLink->ResetCommands();
			m_pSpectatorLink->ResetCommands();
		}
		void InitTestModule()
		{
			m_pPlayerFalling = m_pPlayer->GetPlayerFalling();
		}

		GTestMgrWrapper<GPlayerObjectManager>	m_PlayerObjectManager;

		GUTHelper		m_helper;
		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;
		MockLink*		m_pLink;
		GEntityPlayer*	m_pSpectator;
		MockLink*		m_pSpectatorLink;
		GPlayerFalling*	m_pPlayerFalling;
	};

TEST_FIXTURE(Fixture, PlayerGainDamage)
{
	int nOriginalHP = m_pPlayer->GetHP();

	/// 6 미터 추락 - 땅에 떨어짐
	float nFallingHeight = 600;
	m_pPlayerFalling->EndFalling(false, nFallingHeight);
	CHECK_EQUAL(nOriginalHP-16,	m_pPlayer->GetHP());

	CHECK_EQUAL(1,							m_pLink->GetCommandCount());
	CHECK_EQUAL(MC_ACTION_FALLING_DAMAGE,	m_pLink->GetCommand(0).GetID());

	CHECK_EQUAL(1,							m_pSpectatorLink->GetCommandCount());
	CHECK_EQUAL(MC_ACTION_FALLING_DAMAGE,	m_pSpectatorLink->GetCommand(0).GetID());

	UIID nTestPlayerUIID = UIID_INVALID;
	UIID nFromSpectatorUIID = UIID_INVALID;

	m_pLink->GetCommand(0).GetParameter(&nTestPlayerUIID, 0, MPT_USHORT, sizeof(unsigned short));
	m_pSpectatorLink->GetCommand(0).GetParameter(&nFromSpectatorUIID, 0, MPT_USHORT, sizeof(unsigned short));

	CHECK_EQUAL(nTestPlayerUIID, nFromSpectatorUIID);
}

TEST_FIXTURE(Fixture, PlayerDeath)
{
	int nOriginalHP = m_pPlayer->GetHP();

	float nFallingHeight = 0;

	/// 5.99 미터 추락
	nFallingHeight = 599;
	m_pPlayerFalling->EndFalling(false, nFallingHeight);
	CHECK_EQUAL(nOriginalHP,	m_pPlayer->GetHP());

	/// 35 미터 추락
	nFallingHeight = 3500;
	m_pPlayerFalling->EndFalling(false, nFallingHeight);
	CHECK_EQUAL(0,				m_pPlayer->GetHP());
}

TEST_FIXTURE(Fixture, PlayerDeathAndFalling)
{
	/// 플레이어 죽음
	m_pPlayer->doDie();

	m_pLink->ResetCommands();

	/// 30 미터 추락
	float nFallingHeight = 3000;
	m_pPlayerFalling->EndFalling(false, nFallingHeight);

	CHECK_EQUAL(0,	m_pLink->GetCommandCount());
}

namespace debuff
{
	struct FixtureDebuff : public Fixture
	{
		FixtureDebuff()
		{
			InitTestDebuffs();
		}
		virtual ~FixtureDebuff()
		{
			ClearPlayerBuffs();
		}
		void InitTestDebuffs()
		{
			GBuffInfo* pBuffInfo = NULL;
			pBuffInfo = m_helper.NewBuffInfo(GConst::FALLING_DEBUFF_LV1_ID);
			pBuffInfo->m_nType = BUFT_DEBUFF;

			pBuffInfo = m_helper.NewBuffInfo(GConst::FALLING_DEBUFF_LV2_ID);
			pBuffInfo->m_nType = BUFT_DEBUFF;
		}
		void ClearPlayerBuffs()
		{
			m_pPlayer->GetModuleBuff()->LazyLostAll();
			m_pPlayer->GetModuleBuff()->Update(0.0f);
		}
		void RefreshPlayerHP()
		{
			m_pPlayer->SetHP(m_pPlayer->GetMaxHP());
		}

		GTestMgrWrapper<GBuffInfoMgr> m_BuffInfoManager;
	};

	TEST_FIXTURE(FixtureDebuff, PlayerGainDebuffLevel1)
	{
		CHECK_EQUAL(0,	m_pPlayer->GetModuleBuff()->GetBuffQty());

		/// 6 미터 추락
		m_pPlayerFalling->EndFalling(false, 600);
		CHECK_EQUAL(1,								m_pPlayer->GetModuleBuff()->GetBuffQty());
		CHECK_EQUAL(GConst::FALLING_DEBUFF_LV1_ID,	m_pPlayer->GetModuleBuff()->GetAllBuffs()[0]);

		CHECK_EQUAL(2,							m_pLink->GetCommandCount());
		CHECK_EQUAL(MC_BUFF_GAIN,				m_pLink->GetCommand(0).GetID());
		CHECK_EQUAL(MC_ACTION_FALLING_DAMAGE,	m_pLink->GetCommand(1).GetID());

		CHECK_EQUAL(2,							m_pSpectatorLink->GetCommandCount());
		CHECK_EQUAL(MC_BUFF_GAIN,				m_pSpectatorLink->GetCommand(0).GetID());
		CHECK_EQUAL(MC_ACTION_FALLING_DAMAGE,	m_pSpectatorLink->GetCommand(1).GetID());

		MUID uidTestPlayer = 0;
		MUID uidFromSpectator = 0;

		m_pLink->GetCommand(0).GetParameter(&uidTestPlayer, 0, MPT_UID, sizeof(MUID));
		m_pSpectatorLink->GetCommand(0).GetParameter(&uidFromSpectator, 0, MPT_UID, sizeof(MUID));

		CHECK_EQUAL(uidTestPlayer, uidFromSpectator);

		/// 버프 해제 & 체력 초기화
		ClearPlayerBuffs();
		RefreshPlayerHP();
		m_pLink->ResetCommands();
		m_pSpectatorLink->ResetCommands();

		/// 15.99 미터 추락
		m_pPlayerFalling->EndFalling(false, 1599);
		CHECK_EQUAL(1,								m_pPlayer->GetModuleBuff()->GetBuffQty());
		CHECK_EQUAL(GConst::FALLING_DEBUFF_LV1_ID,	m_pPlayer->GetModuleBuff()->GetAllBuffs()[0]);
	}

	TEST_FIXTURE(FixtureDebuff, PlayerGainDebuffLevel2)
	{
		CHECK_EQUAL(0,	m_pPlayer->GetModuleBuff()->GetBuffQty());

		/// 16 미터 추락
		m_pPlayerFalling->EndFalling(false, 1600);
		CHECK_EQUAL(1,								m_pPlayer->GetModuleBuff()->GetBuffQty());
		CHECK_EQUAL(GConst::FALLING_DEBUFF_LV2_ID,	m_pPlayer->GetModuleBuff()->GetAllBuffs()[0]);

		/// 버프 해제 & 체력 초기화
		ClearPlayerBuffs();
		RefreshPlayerHP();

		/// 34.99 미터 추락
		m_pPlayerFalling->EndFalling(false, 3499);
		CHECK_EQUAL(1,								m_pPlayer->GetModuleBuff()->GetBuffQty());
		CHECK_EQUAL(GConst::FALLING_DEBUFF_LV2_ID,	m_pPlayer->GetModuleBuff()->GetAllBuffs()[0]);
	}
}

}