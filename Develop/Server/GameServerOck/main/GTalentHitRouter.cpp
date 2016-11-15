#include "stdafx.h"
#include "GTalentHitRouter.h"
#include "GCommand.h"
#include "GEntityActor.h"
#include "GCommandCenter.h"

void GTalentHitRouter::RouteTalentHit( GEntityActor* pAttacker, GEntityActor* pVictim, GTalentInfo* pTalentInfo, unsigned short nDamage, MF_STATE nMFState, int nMFWeight, uint16 nActHitTime, uint32 nFlags )
{
	VALID(pVictim);

	MCommand* pNewCommand = m_TalentHitCommandFactory.MakeHit(pAttacker, pVictim, pTalentInfo, nDamage, nMFState, nMFWeight, nActHitTime, nFlags);

	PFI_B(9097, "GTalentHitRouter::RouteTalentHit-RouteToThisCell");
	pVictim->RouteToThisCell(pNewCommand);
	PFI_E(9097);
}

void GTalentHitRouter::Route_Resist( GEntityActor* pAttacker, GEntityActor* pVictim, GTalentInfo* pTalentInfo, uint16 nActHitTime, uint32 nFlags )
{
	RouteTalentHit(pAttacker, pVictim, pTalentInfo, 0, MF_NONE, 0, nActHitTime, nFlags);
}

void GTalentHitRouter::Route_MotionFactor( GEntityActor* pAttacker, GEntityActor* pVictim, MF_STATE nMFState, int nMFWeight, uint16 nActHitTime )
{
	RouteTalentHit(pAttacker, pVictim, NULL, 0, nMFState, nMFWeight, nActHitTime, 0);
}

void GTalentHitRouter::Route_Immune( GEntityActor* pAttacker, GEntityActor* pVictim, GTalentInfo* pTalentInfo )
{
	VALID(pVictim);

	MCommand* pNewCommand = m_TalentHitCommandFactory.MakeImmume(pAttacker, pVictim, pTalentInfo);
	pVictim->RouteToThisCell(pNewCommand);
}

void GTalentHitRouter::Route_Avoid( GEntityActor* pAttacker, GEntityActor* pVictim, GTalentInfo* pTalentInfo )
{
	VALID(pVictim);

	MCommand* pNewCommand = m_TalentHitCommandFactory.MakeAvoid(pAttacker, pVictim, pTalentInfo);
	pVictim->RouteToThisCell(pNewCommand);
}

void GTalentHitRouter::Route_Miss( const GEntityActor* pAttacker, const GEntityActor* pVictim, GTalentInfo* pTalentInfo )
{
	VALID(pVictim);

	MCommand* pNewCommand = m_TalentHitCommandFactory.MakeMiss(pAttacker, pVictim, pTalentInfo);
	pVictim->RouteToThisCell(pNewCommand);
}

void GTalentHitRouter::RouteBuffHit( const GEntityActor* pVictim, int nBuffID, unsigned short nDamage, MF_STATE nMFState, int nMFWeight, uint32 nFlags )
{
	VALID(pVictim);

	MCommand* pNewCommand = m_TalentHitCommandFactory.MakeBuffHit(pVictim, nBuffID, nDamage, nMFState, nMFWeight, nFlags);
	pVictim->RouteToThisCell(pNewCommand);
}