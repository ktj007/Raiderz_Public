#pragma once

#include "MemPool.h"

class GTalent;
class GTalentInfo;
class GEntityActor;

class GTalentRouter : public MTestMemPool<GTalentRouter>
{
private:
	GTalent*		m_pTalent;
	void			RouteActSpellDetail(int nTalentID);
	void			RouteAttackArcheryDetail();
public:
	GTalentRouter(GTalent* pTalent=NULL) : m_pTalent(pTalent) {}

	// 일반
	void			RouteUseTalent(GTalentInfo* pTalentInfo, vec3 vPos, vec3 vDir);
	void			RouteCancelTalent(GTalentInfo* pTalentInfo);
	void			RouteActTalent( GTalentInfo* pTalentInfo, vec3 vDir );
	void			RouteActTalentWithGround( vec3 vGroundPos, GTalentInfo* pTalentInfo, vec3 vDir );
	// 마법
	void			RouteActRecastingSpell(int nSpellPhase, int nRet);
	void			RouteActTalentProjectileMissile(int nTalentID, vec3 vTargetPos, MUID uidTarget, int8 nCapsuleGroupIndex, int8 nCapsuleIndex );
	void			RouteActSpellMagicArea( int nTalentID, vec3 vTargetPos );
	void			RouteTalentHeal( GEntityActor* pUser, GEntityActor* pTarget, int nTalentID, int nHealAmount );
	void			RouteBuffHeal( GEntityActor* pUser, GEntityActor* pTarget, int nBuffID, int nHealAmount );
	// 활
	void			RouteAttackArchery();

	// 특수
	void			RouteExtraActTalent(GTalentInfo* pTalentInfo);
};
