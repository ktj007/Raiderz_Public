#pragma once

class GEntityActor;
class GTalentInfo;



class FBaseCombat
{
public:
	FBaseCombat(void);
	~FBaseCombat(void);

	void Attack(GEntityActor* pAttacker, GEntityActor* pVictim, GTalentInfo* pAttackTalent=NULL);
};
