#include "StdAfx.h"
#include "GPlayerPVPArea.h"
#include "GEntityPlayer.h"
#include "GFieldAttrGrid.h"
#include "GFieldInfo.h"
#include "GFactionCalculator.h"
#include "GPlayerFactions.h"
#include "CSDef_QuestPVP.h"


//////////////////////////////////////////////////////////////////////////
//
// _local
//
//////////////////////////////////////////////////////////////////////////

GFieldAttrGrid::Cell* GetFieldAttrCell(GEntityPlayer* pPlayer)
{
	const GFieldInfo* pFieldInfo = pPlayer->GetFieldInfo();
	if (NULL == pFieldInfo)
		return NULL;

	return pFieldInfo->GetAttrGrid()->FindCell(pPlayer->GetPos().x, pPlayer->GetPos().y);
}

//////////////////////////////////////////////////////////////////////////
//
// GPlayerPVPArea
//
//////////////////////////////////////////////////////////////////////////

GPlayerPVPArea::GPlayerPVPArea(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
{
}

GPlayerPVPArea::~GPlayerPVPArea(void)
{
}

bool GPlayerPVPArea::IsHittable( GEntityActor* pAttacker )
{
	VALID_RET(pAttacker, false);

	// PVP 지역에 있는 확인 ----------------------------
	if (!IsLocatedPvPArea())
		return false;

	if (!pAttacker->IsPlayer())
		return false;

	GEntityPlayer* pAttackPlayer = ToEntityPlayer(pAttacker);
	VALID_RET(pAttackPlayer, false);
	if (!pAttackPlayer->GetPlayerPVPArea().IsLocatedPvPArea())
		return false;

	// 공격 가능한 팩션인지 확인 ----------------------------
	QUESTPVP_TEAM nMyTeam = GetTeam();
	if (nMyTeam == QT_NONE)
		return false;

	QUESTPVP_TEAM nOtherTeam = pAttackPlayer->GetPlayerPVPArea().GetTeam();
	if (nOtherTeam == QT_NONE)
		return false;

	if (nMyTeam == nOtherTeam)
		return false;
	
	return true;
}

bool GPlayerPVPArea::IsLocatedPvPArea()
{
	GFieldAttrGrid::Cell* pCell = GetFieldAttrCell(m_pOwner);
	if (!pCell)
		return false;

	return pCell->bPvPEnable;
}

QUESTPVP_TEAM GPlayerPVPArea::GetTeam()
{
	GFieldAttrGrid::Cell* pCell = GetFieldAttrCell(m_pOwner);
	if (!pCell)
		return QT_NONE;

	uint16 nFaction1Qty = m_pOwner->GetPlayerFactions().GetQuantity(pCell->nPvPFaction1);
	uint16 nFaction2Qty = m_pOwner->GetPlayerFactions().GetQuantity(pCell->nPvPFaction2);
	GFactionCalculator faction_calculator;
	bool bTeamOne = faction_calculator.IsNormalThanRelation(nFaction1Qty);
	bool bTeamTwo = faction_calculator.IsNormalThanRelation(nFaction2Qty);
	
	if (bTeamOne && !bTeamTwo)
		return QT_ONE;
	
	if (!bTeamOne && bTeamTwo)
		return QT_TWO;

	if (bTeamOne && bTeamTwo)
		return (nFaction1Qty >= nFaction2Qty)?QT_ONE:QT_TWO;

	return QT_NONE;
}

bool GPlayerPVPArea::IsLocateAndBelongTeam()
{
	if (false == IsLocatedPvPArea()) return false;
	if (QT_NONE == GetTeam()) return false;

	return true;
}
