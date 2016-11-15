#pragma once

#include "GTalentDef.h"
#include "GTalentEffector.h"

class GTalentInfo;
class GEntityActor;
class GTalent;

//////////////////////////////////////////////////////////////////////////
//
//	GTalentProjectile
//
//////////////////////////////////////////////////////////////////////////

class GTalentProjectile
{
public:
	GTalentProjectile();

	bool			Init(GTalent* pTalent);
	void			LaunchMissile( const vec3& vTargetPos, CSProjectileInfo& rProjectileInfo );
	void			LaunchGuidedMissile( MUID uidTarget, int nCapsuleGroup, int nCapsuleIndex, CSProjectileInfo& rProjectileInfo );
private:
	bool			m_bInitialize;
	GTalentInfo*	m_pTalentInfo;
	GEntityActor*	m_pOwner;
	float			m_fHitDelay;
	TALENT_TARGET_INFO	m_Target;
	GTalentEffector m_TalentEffector;
};