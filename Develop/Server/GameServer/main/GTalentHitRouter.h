#pragma once

#include "GTalentHitCommandFactory.h"


class GTalentHitRouter
{
private:
	GTalentHitCommandFactory	m_TalentHitCommandFactory;
public:
	GTalentHitRouter()
	{

	}
	~GTalentHitRouter()
	{

	}

	void RouteTalentHit(GEntityActor* pAttacker, GEntityActor* pVictim, GTalentInfo* pTalentInfo, unsigned short nDamage, MF_STATE nMFState, int nMFWeight, uint16 nActHitTime, uint32 nFlags);

	void Route_Resist(GEntityActor* pAttacker, GEntityActor* pVictim, GTalentInfo* pTalentInfo, uint16 nActHitTime, uint32 nFlags);

	void Route_MotionFactor(GEntityActor* pAttacker, GEntityActor* pVictim, MF_STATE nMFState, int nMFWeight, uint16 nActHitTime);

	void Route_Immune(GEntityActor* pAttacker, GEntityActor* pVictim, GTalentInfo* pTalentInfo);

	void Route_Miss(const GEntityActor* pAttacker, const GEntityActor* pVictim, GTalentInfo* pTalentInfo);

	void RouteBuffHit(const GEntityActor* pVictim, int nBuffID, unsigned short nDamage, MF_STATE nMFState, int nMFWeight, uint32 nFlags);
	void Route_Avoid( GEntityActor* pAttacker, GEntityActor* pVictim, GTalentInfo* pTalentInfo );
};