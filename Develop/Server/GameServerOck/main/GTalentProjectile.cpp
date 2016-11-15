#include "StdAfx.h"
#include "GTalentProjectile.h"
#include "GTalent.h"
#include "GEntityNPC.h"
#include "GTalent.h"
#include "GField.h"
#include "GTalentHitter.h"
#include "GModuleCombat.h"

//////////////////////////////////////////////////////////////////////////
//
//	GTalentProjectile
//
//////////////////////////////////////////////////////////////////////////

GTalentProjectile::GTalentProjectile()
: m_bInitialize(false)
, m_pTalentInfo(NULL)
, m_pOwner(NULL)
{
}

bool GTalentProjectile::Init(GTalent* pTalent)
{
	VALID_RET(pTalent, false);
	m_pTalentInfo = pTalent->GetInfo();
	VALID_RET(m_pTalentInfo, false);
	m_pOwner = pTalent->GetOwner();
	VALID_RET(m_pOwner, false);
	m_Target = pTalent->GetTarget();

	m_bInitialize = true;

	return true;
}

void GTalentProjectile::LaunchMissile( const vec3& vTargetPos, CSProjectileInfo& rProjectileInfo )
{
	if (!m_bInitialize)		return;
	VALID(TPT_MISSILE == rProjectileInfo.m_nType) 

	GTalentHitter_Projectile* pNewHitter = new GTalentHitter_Projectile();

	float fProjectileSpeed = rProjectileInfo.m_fSpeed;
	MMatrix matWorld;
	matWorld.SetLocalMatrix(m_pOwner->GetPos(), -m_pOwner->GetDir(), MVector3::AXISZ);
	vec3 vStartPos;
	matWorld.TransformVect(rProjectileInfo.m_vLocalStartPos, vStartPos);

	vec3 vProjectileDir;
	if (vTargetPos == vec3::ZERO)
	{
		vProjectileDir = m_pOwner->GetDir();
	}
	else
	{
		vProjectileDir = vTargetPos - vStartPos;
	}
	
	vProjectileDir.Normalize();
	vec3 vVelocity = vProjectileDir * fProjectileSpeed;

	pNewHitter->Setup(m_pOwner->GetField(), m_pOwner->GetUID(), vStartPos, vVelocity, m_pTalentInfo, &rProjectileInfo);

	VALID(m_pOwner->GetModuleCombat());
	m_pOwner->GetModuleCombat()->AddTargetHitter(pNewHitter);
}

void GTalentProjectile::LaunchGuidedMissile( MUID uidTarget, int nCapsuleGroup, int nCapsuleIndex, CSProjectileInfo& rProjectileInfo )
{
	if (!m_bInitialize)		return;
	if (CSEffectInfo::POINT_TARGET != m_pTalentInfo->m_EffectInfo.m_nPoint)
	{
		return;
	}
	VALID(TPT_MISSILE_GUIDED == rProjectileInfo.m_nType ||
		  TPT_HITCAPSULE_GUIDED == rProjectileInfo.m_nType) 

	float fHitDelay = 0.0f;
	GField* pField = m_pOwner->GetField();
	if (!pField) return;

	GEntityActor* pTarget = pField->FindActor(uidTarget);
	if (!pTarget) return;

	float fProjectileSpeed = rProjectileInfo.m_fSpeed;
	vec3 vStartPos = rProjectileInfo.m_vLocalStartPos;
	if (vStartPos.z == 0.0f) vStartPos.z += 100.0f; // test
	vStartPos += m_pOwner->GetPos();

	float fDistance = pTarget->GetPos().DistanceTo(vStartPos);
	fHitDelay = fDistance / fProjectileSpeed;

	GTalentHitter_TimeDelay* pNewHitter = GTalentHitter::NewTalentHitter< GTalentHitter_TimeDelay >();
	pNewHitter->Setup(m_pOwner->GetUID(), uidTarget, nCapsuleGroup, nCapsuleIndex, m_pTalentInfo, fHitDelay, m_TalentEffector);
	
	VALID(m_pOwner->GetModuleCombat());
	m_pOwner->GetModuleCombat()->AddTargetHitter(pNewHitter);
}
