#include "StdAfx.h"
#include "GGlueQuestPVP.h"
#include "GQuestPVP.h"

GGlueQuestPVP::GGlueQuestPVP(GQuestPVP* pOwner)
: m_pOwner(pOwner), m_bDisableBeginEvent(false)
{
}

GGlueQuestPVP::~GGlueQuestPVP(void)
{
}

GQuestPVP* GGlueQuestPVP::GetOwner() const
{
	return m_pOwner;
}

bool GGlueQuestPVP::IsInvalidOwner() const
{
	return NULL == m_pOwner;
}

bool GGlueQuestPVP::IsNull( void ) const
{
	if (IsInvalidOwner())		return true;
	return false;
}

int GGlueQuestPVP::GetTeamCount(int nTeam)
{
	if (IsInvalidOwner())		return 0;

	return GetOwner()->GetTeamCount(static_cast<QUESTPVP_TEAM>(nTeam));
}

void GGlueQuestPVP::BeginEvent(int nEventID)
{
	if (IsInvalidOwner()) return;
	if (m_bDisableBeginEvent) return;

	GetOwner()->BeginEvent(nEventID);
}

void GGlueQuestPVP::EndEvent( int nEventID, int nWinnerTeam )
{
	if (IsInvalidOwner())		return;

	GetOwner()->EndEvent(nEventID, static_cast<QUESTPVP_TEAM>(nWinnerTeam));
}

void GGlueQuestPVP::EnableMarkPos( int nNPCID )
{
	if (IsInvalidOwner())		return;

	GetOwner()->EnableMarkPos(nNPCID);
}

void GGlueQuestPVP::DisableMarkPos( int nNPCID )
{
	if (IsInvalidOwner())		return;

	GetOwner()->DisableMarkPos(nNPCID);
}

void GGlueQuestPVP::DisableBeginEvent()
{
	m_bDisableBeginEvent = true;
}
