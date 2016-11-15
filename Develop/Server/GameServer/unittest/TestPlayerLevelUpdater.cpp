#include "stdafx.h"
#include "GPlayerLevelUpdater.h"
#include "FBaseGame.h"
#include "GPlayerTalent.h"
#include "GExpCalculator.h"

SUITE(PlayerLevelUpdater)
{
	class FPlayerLevelUpdater : public FBaseGame
	{
	public:
		FPlayerLevelUpdater()
		{

		}
		~FPlayerLevelUpdater()
		{

		}
	};

	TEST_FIXTURE(FPlayerLevelUpdater, PlayerLevelUpdater_LevelUp)
	{
		PLAYER_INFO* pPlayerInfo = m_pPlayer->GetPlayerInfo();
		pPlayerInfo->nLevel = 1;

		// 레벨업하기 전
		int nBeforeLevel = pPlayerInfo->nLevel;
		int nBeforeTP = m_pPlayer->GetTalent().GetTP();

		GPlayerLevelUpdater levelUpdater(m_pPlayer);		
		levelUpdater.LevelUp(GExpCalculator::GetLevelUpRequiredXP(pPlayerInfo->nLevel), 0);

		// 레벨업 후
		CHECK_EQUAL(nBeforeLevel + 1, pPlayerInfo->nLevel);
		CHECK_EQUAL(nBeforeTP + 1, m_pPlayer->GetTalent().GetTP());

		CHECK_EQUAL(MC_CHAR_LEVEL_UP, m_pLink->GetCommandID(1));
		CHECK_EQUAL(nBeforeLevel + 1, m_pLink->GetParam<unsigned char>(1, 1));	// level
		CHECK_EQUAL(nBeforeTP + 1, m_pLink->GetParam<unsigned short>(1, 5));	// TP
		CHECK_EQUAL(m_pPlayer->GetSTRProto(), m_pLink->GetParam<unsigned short>(1, 6));
		CHECK_EQUAL(m_pPlayer->GetDEXProto(), m_pLink->GetParam<unsigned short>(1, 7));
		CHECK_EQUAL(m_pPlayer->GetINTProto(), m_pLink->GetParam<unsigned short>(1, 8));
		CHECK_EQUAL(m_pPlayer->GetCHAProto(), m_pLink->GetParam<unsigned short>(1, 9));
		CHECK_EQUAL(m_pPlayer->GetCONProto(), m_pLink->GetParam<unsigned short>(1, 10));
		CHECK_EQUAL(nBeforeLevel + 1, pPlayerInfo->nLevel);
		CHECK_EQUAL(0, pPlayerInfo->nXP);
	}

	TEST_FIXTURE(FPlayerLevelUpdater, PlayerLevelUpdater_LevelDown)
	{
		PLAYER_INFO* pPlayerInfo = m_pPlayer->GetPlayerInfo();
		pPlayerInfo->nLevel = 2;
		pPlayerInfo->nXP = 500;

		int nBeforeTP = m_pPlayer->GetTalent().GetTP();

		int nNewLevel = 1;
		int nNewXP = 0;

		GPlayerLevelUpdater levelUpdater(m_pPlayer);
		levelUpdater.SetLevelForGM(nNewLevel, nNewXP);

		// 레벨다운 후
		CHECK_EQUAL(nNewLevel, pPlayerInfo->nLevel);
		CHECK_EQUAL(nBeforeTP, m_pPlayer->GetTalent().GetTP());

		ASSERT_EQUAL(MC_CHAR_LEVEL_UP, m_pLink->GetCommandID(1));
		CHECK_EQUAL(nNewLevel, m_pLink->GetParam<unsigned char>(1, 1));	// level
		CHECK_EQUAL(nBeforeTP, m_pLink->GetParam<unsigned short>(1, 5));	// TP
		CHECK_EQUAL(m_pPlayer->GetSTRProto(), m_pLink->GetParam<unsigned short>(1, 6));
		CHECK_EQUAL(m_pPlayer->GetDEXProto(), m_pLink->GetParam<unsigned short>(1, 7));
		CHECK_EQUAL(m_pPlayer->GetINTProto(), m_pLink->GetParam<unsigned short>(1, 8));
		CHECK_EQUAL(m_pPlayer->GetCHAProto(), m_pLink->GetParam<unsigned short>(1, 9));
		CHECK_EQUAL(m_pPlayer->GetCONProto(), m_pLink->GetParam<unsigned short>(1, 10));
	}
}