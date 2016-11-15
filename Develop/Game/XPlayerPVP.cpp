#include "StdAfx.h"
#include "XPlayerPVP.h"

XPlayerPVP::XPlayerPVP(void)
//: m_bDuel(false)
: m_nBattleArenaFaction(0)
, m_ePvPAreaFaction(ePvPAreaTeam_NA)
{
}

XPlayerPVP::~XPlayerPVP(void)
{
}

void XPlayerPVP::SetBattleArenaEnemy()
{
	m_nBattleArenaFaction = 1;
}

void XPlayerPVP::ReleaseBattleArenaEnemy()
{
	m_nBattleArenaFaction = 0;
}

bool XPlayerPVP::IsAlly( XPlayerPVP* target )
{
	if (m_nBattleArenaFaction != target->m_nBattleArenaFaction) return false;
//	if (target->m_bDuel == true) return false;

	if (IsAllyInPvPArea(target) == false) return false;

	return true;
}

bool XPlayerPVP::IsEnemy( XPlayerPVP* target )
{
	return !IsAlly(target);
}

//void XPlayerPVP::SetDuel()
//{
//	m_bDuel = true;
//}

//void XPlayerPVP::ReleaseDuel()
//{
//	m_bDuel = false;
//}

void XPlayerPVP::SetPvPAreaFaction( bool bFaction1, bool bFaction2 )
{
	if (bFaction1) m_ePvPAreaFaction = ePvPAreaTeam_1;
	else if (bFaction2) m_ePvPAreaFaction = ePvPAreaTeam_2;
	else m_ePvPAreaFaction = ePvPAreaTeam_NA;
}

void XPlayerPVP::ReleasePvPAreaFaction()
{
	m_ePvPAreaFaction = ePvPAreaTeam_NA;
}

bool XPlayerPVP::IsAllyInPvPArea( XPlayerPVP* target )
{
	if (m_ePvPAreaFaction != ePvPAreaTeam_NA && target->m_ePvPAreaFaction != ePvPAreaTeam_NA)
	{
		if (m_ePvPAreaFaction != target->m_ePvPAreaFaction) return false;
	}

	return true;
}
