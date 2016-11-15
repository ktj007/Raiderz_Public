#include "stdafx.h"
#include "GPlayerLevelUpdater.h"
#include "GDef.h"
#include "GCalculator.h"
#include "GExpCalculator.h"
#include "GEntityPlayer.h"
#include "GPlayerTalent.h"
#include "GInteractionSystem.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "GCommand.h"
#include "CCommandTable.h"
#include "GDBTaskCharData.h"
#include "GPlayerNPCIconSender.h"
#include "CSConditionInfo.h"
#include "GExpSystem.h"
#include "GDB_CODE.h"
#include "GTimeCalculator.h"

GPlayerLevelUpdater::GPlayerLevelUpdater(GEntityPlayer* pPlayer)
: m_pPlayer(pPlayer)
{

}

void GPlayerLevelUpdater::LevelUp(const int nGainXP, const int nNpcID)
{
	PLAYER_INFO* pPlayerInfo = m_pPlayer->GetPlayerInfo();
	if (NULL == pPlayerInfo) return;

	int nNewLevel = GExpCalculator::CalcNewLevel(pPlayerInfo->nLevel, pPlayerInfo->nXP+nGainXP);
	int nNewExp = GExpCalculator::CalcNewExp(pPlayerInfo->nLevel, pPlayerInfo->nXP+nGainXP);

	if (nNewLevel == pPlayerInfo->nLevel) return;

	GDBT_CHAR_LEVEL_UP_DATA data(m_pPlayer->GetAID()
		, m_pPlayer->GetUID()
		, m_pPlayer->GetCID()
		, GDB_CODE::CD_L_KILL_NPC_GAIN_XP_LEVEL_UP
		, GTimeCalculator::GetTimeAsString(GTimeSec(0))
		, nNewExp
		, nGainXP
		, nNewLevel
		, m_pPlayer->GetMoney()
		, m_pPlayer->GetTalent().GetTP() + (nNewLevel - m_pPlayer->GetLevel())
		, pPlayerInfo->nPlayTimeSec
		, pPlayerInfo->GetDeltaPlayTime()
		, nNpcID
		, m_pPlayer->GetFieldID()
		, m_pPlayer->GetPos());

	gsys.pDBManager->CharLevelUp(data);
}

void GPlayerLevelUpdater::LevelUpForDBTask(int nLevel, int nExp, int nGainExp)
{
	int nAddTP = nLevel - m_pPlayer->GetLevel();
	// SetLevelForGM으로 레벨을 낮춘경우
	if (nLevel < m_pPlayer->GetLevel())
	{
		nAddTP = 0;
	}

	m_pPlayer->GetTalent().AddTP(nAddTP);
	m_pPlayer->GetPlayerInfo()->nLevel = nLevel;
	m_pPlayer->GetPlayerInfo()->nXP = nExp;
	m_pPlayer->RefreshStatusFromLevelup();

	gsys.pExpSystem->RouteAddExp(m_pPlayer, nGainExp);
	
	MCommand* pNewCommand = MakeNewCommand(MC_CHAR_LEVEL_UP, 
		11, 
		NEW_UID(m_pPlayer->GetUID()), 
		NEW_UCHAR(m_pPlayer->GetLevel()), 
		NEW_USHORT(m_pPlayer->GetMaxHPProto()), 
		NEW_USHORT(m_pPlayer->GetMaxENProto()), 
		NEW_USHORT(m_pPlayer->GetMaxSTAProto()),
		NEW_USHORT(m_pPlayer->GetTalent().GetTP()),
		NEW_USHORT(m_pPlayer->GetSTRProto()),
		NEW_USHORT(m_pPlayer->GetDEXProto()),
		NEW_USHORT(m_pPlayer->GetINTProto()),
		NEW_USHORT(m_pPlayer->GetCHAProto()),
		NEW_USHORT(m_pPlayer->GetCONProto()));

	m_pPlayer->RouteToThisCell(pNewCommand);

	m_pPlayer->GetNPCIconSender().SendByPlayerConditionChange(CT_LEVEL);
}


void GPlayerLevelUpdater::SetLevelForGM( int nNewLevel, int nNewExp)
{
	PLAYER_INFO* pPlayerInfo = m_pPlayer->GetPlayerInfo();
	if (NULL == pPlayerInfo) return;

	int nAddTP = nNewLevel - m_pPlayer->GetLevel();
	// SetLevelForGM으로 레벨을 낮춘경우
	if (nNewLevel < m_pPlayer->GetLevel())
	{
		nAddTP = 0;
	}

	GDBT_CHAR_LEVEL_UP_DATA data(m_pPlayer->GetAID()
		, m_pPlayer->GetUID()
		, m_pPlayer->GetCID()
		, GDB_CODE::CD_L_KILL_NPC_GAIN_XP_LEVEL_UP
		, GTimeCalculator::GetTimeAsString(GTimeSec(0))
		, nNewExp
		, 0
		, nNewLevel
		, m_pPlayer->GetMoney()
		, m_pPlayer->GetTalent().GetTP() + nAddTP
		, pPlayerInfo->nPlayTimeSec
		, pPlayerInfo->GetDeltaPlayTime()
		, 0
		, m_pPlayer->GetFieldID()
		, m_pPlayer->GetPos());

	gsys.pDBManager->CharLevelUpSync(data);
}