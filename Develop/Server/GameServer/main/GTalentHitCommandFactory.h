#pragma once

#include "GDef.h"
#include "CSMotionFactorInfo.h"

#include "CTransData.h"

class GEntityActor;
class GTalentInfo;

class GTalentHitCommandFactory
{
protected:
	void BuildParam_ExKnockBack( TD_TALENT_HIT_EX_KNOCKBACK &out_td_talent_hit_ex_knockback, GEntityActor* pAttacker, GEntityActor* pVictim, GTalentInfo* pTalentInfo, MF_STATE nMFState );
	bool CheckNoMotionFactor( unsigned short nDamage, int nMFWeight, uint32 nFlags );
	bool IsKnockBackCommand(MF_STATE nMFState);
public:
	GTalentHitCommandFactory()	{}
	~GTalentHitCommandFactory()	{}

	MCommand* MakeHit(GEntityActor* pAttacker, GEntityActor* pVictim, GTalentInfo* pTalentInfo, unsigned short nDamage, MF_STATE nMFState, int nMFWeight, uint16 nActHitTime, uint32 nFlags);

	MCommand* MakeImmume(GEntityActor* pAttacker, GEntityActor* pVictim, GTalentInfo* pTalentInfo);

	MCommand* MakeMiss( const GEntityActor* pAttacker, const GEntityActor* pVictim, GTalentInfo* pTalentInfo );

	MCommand* MakeBuffHit(const GEntityActor* pVictim, int nBuffID, unsigned short nDamage, MF_STATE nMFState, int nMFWeight, uint32 nFlags);
	MCommand* MakeAvoid( GEntityActor* pAttacker, GEntityActor* pVictim, GTalentInfo* pTalentInfo );
};


