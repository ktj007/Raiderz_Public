#include "StdAfx.h"
#include "GMatchRule_SameTeamMember.h"
#include "GDef.h"
#include "GGlobal.h"
#include "GPartySystem.h"

GMatchRule_SameTeamMember::GMatchRule_SameTeamMember()
{
}

bool GMatchRule_SameTeamMember::IsMatch( const BattleArena::Team& teamRequester, const BattleArena::Team& teamOpponent )
{
	// 파티 존제 확인
	if (NULL == gsys.pPartySystem->FindParty(teamRequester.uidParty))	return false;
	if (NULL == gsys.pPartySystem->FindParty(teamOpponent.uidParty))	return false;


	// 접속 멤버수 확인
	int nRequesterCount = gsys.pPartySystem->CalculateOnlineMemberCount(teamRequester.uidParty);
	int nOpponentCount = gsys.pPartySystem->CalculateOnlineMemberCount(teamOpponent.uidParty);

	return nRequesterCount == nOpponentCount;
}
