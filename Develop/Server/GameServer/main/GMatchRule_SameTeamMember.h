#pragma once

#include "GBattleArenaMatcher.h"

class GMatchRule_SameTeamMember : public GBattleArenaMatcher::Rule, public MTestMemPool<GMatchRule_SameTeamMember>
{
public:
	GMatchRule_SameTeamMember();

	virtual bool IsMatch(const BattleArena::Team& teamRequester, const BattleArena::Team& teamOpponent) override;
};
