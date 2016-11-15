#include "StdAfx.h"
#include "GPlayerBattleArena.h"
#include "GGlobal.h"
#include "GBattleArenaMgr.h"
#include "GEntityPlayer.h"

GPlayerBattleArena::GPlayerBattleArena( GEntityPlayer* pOwner )
: m_pOwner(pOwner)
{

}


bool GPlayerBattleArena::IsEntered() const
{
	return (m_uidBattleArena.IsValid());
}

bool GPlayerBattleArena::IsNowPlaying() const
{
	if (GetStatus() == GBattleArena::PREPARE)
		return true;

	if (GetStatus() == GBattleArena::COUNTING)
		return true;

	if (GetStatus() == GBattleArena::PLAYING)
		return true;

	return false;
}

bool GPlayerBattleArena::IsNowDisplaySocreboard() const
{
	return (GetStatus() == GBattleArena::SCOREBOARD);
}

bool GPlayerBattleArena::RegisterEntry()
{
	return gmgr.pBattleArenaMgr->RegisterTeam(m_pOwner);
}

bool GPlayerBattleArena::DeregisterEntry()
{
	return gmgr.pBattleArenaMgr->DeregisterTeam(m_pOwner);
}

void GPlayerBattleArena::OnDie()
{
	GBattleArena* pBattleArena = gmgr.pBattleArenaMgr->FindBattleArena(m_uidBattleArena); 
	if (!pBattleArena)
		return;

	pBattleArena->OnTeamMemberDie(m_pOwner, gmgr.pBattleArenaMgr->FindTeam(m_pOwner->GetPartyUID()));
}

void GPlayerBattleArena::OnRebirthBySelf()
{
	GBattleArena* pBattleArena = gmgr.pBattleArenaMgr->FindBattleArena(m_uidBattleArena); 
	if (!pBattleArena)
		return;

	pBattleArena->OnRebirthTeamMemberBySelf(m_pOwner, gmgr.pBattleArenaMgr->FindTeam(m_pOwner->GetPartyUID()));
}

void GPlayerBattleArena::OnRebirthByOther()
{
	GBattleArena* pBattleArena = gmgr.pBattleArenaMgr->FindBattleArena(m_uidBattleArena); 
	if (!pBattleArena)
		return;

	pBattleArena->OnRebirthTeamMemberByOther(m_pOwner, gmgr.pBattleArenaMgr->FindTeam(m_pOwner->GetPartyUID()));
}

void GPlayerBattleArena::OnDisconnect()
{
	GBattleArena* pBattleArena = gmgr.pBattleArenaMgr->FindBattleArena(m_uidBattleArena); 
	if (!pBattleArena)
		return;

	pBattleArena->OnDisconnect(m_pOwner);
}

GBattleArena::Status GPlayerBattleArena::GetStatus() const
{
	GBattleArena* pBattleArena = gmgr.pBattleArenaMgr->FindBattleArena(m_uidBattleArena); 
	if (!pBattleArena)
		return GBattleArena::INVALID;

	return pBattleArena->GetStatus();
}