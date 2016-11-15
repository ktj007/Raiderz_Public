#include "StdAfx.h"
#include "GPlayerCombatChecker.h"
#include "GEntityActor.h"
#include "GCommand.h"
#include "CCommandTable.h"

GPlayerCombatChecker::GPlayerCombatChecker(GEntityActor* pOwner)
: m_pOwner(pOwner)
, m_bIsNowCombat(false)
{
	m_pOwner->AttachObserver(this);
}

GPlayerCombatChecker::~GPlayerCombatChecker(void)
{
	m_pOwner->DetachObserver(this);
}

void GPlayerCombatChecker::OnAddHateTarget( GEntityActor* pEnemy )
{
	VALID(pEnemy);
	MCommand* pNewCmd = MakeNewCommand(MC_CHAR_ADD_ENEMYNPC, 1, NEW_UID(pEnemy->GetUID()));
	m_pOwner->RouteToMe(pNewCmd);
}

void GPlayerCombatChecker::OnLostHateTarget( MUID uidEnemy )
{
	MCommand* pNewCmd = MakeNewCommand(MC_CHAR_DEL_ENEMYNPC, 1, NEW_UID(uidEnemy));
	m_pOwner->RouteToMe(pNewCmd);
}

void GPlayerCombatChecker::Update( float fDelta )
{
	bool bTickState = 
		m_pOwner->IsNowCombat();

	if (bTickState == m_bIsNowCombat)
		return; // 변화 없음

	m_bIsNowCombat = bTickState;

	if (m_bIsNowCombat)
	{
		m_pOwner->OnCombatBegin(m_pOwner);
	}
	else
	{
		m_pOwner->OnCombatEnd(m_pOwner);

		// 패킷 송신
		MCommand* pNewCmd = MakeNewCommand(MC_ACTION_END_COMBAT, 0, NULL);
		m_pOwner->RouteToMe(pNewCmd);
	}
}
