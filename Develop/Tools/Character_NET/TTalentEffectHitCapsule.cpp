#include "stdafx.h"
#include "TTalentEffectHitCapsule.h"
#include "TEffectCharacterToolSinglyInvoker.h"

TTalentEffectHitCapsule::TTalentEffectHitCapsule()
{
	m_pActor = NULL;
}

TTalentEffectHitCapsule::~TTalentEffectHitCapsule()
{

}

void TTalentEffectHitCapsule::AddTalentHitCapsuleImpact( TCharacter* pActor, TTalentInfo* pTalentInfo, XTalentEffectInfo* pTalentEffectInfo, TALENT_EFFECT_TYPE eType )
{
	m_pActor= pActor;
	XBaseTalentEffectHitCapsule::AddTalentHitCapsuleImpact(pActor, pTalentInfo, pTalentEffectInfo, eType, m_pActor->GetDirection());
}

void TTalentEffectHitCapsule::Update( float fDelta, TCharacter* pActor )
{
	m_pActor= pActor;

	if(pActor == NULL)
		return;

	XBaseTalentEffectHitCapsule::Update(fDelta);
}

bool TTalentEffectHitCapsule::CheckExistActor( MUID uidAttacker )
{
	if(m_pActor == NULL)
		return false;

	return true;
}

void TTalentEffectHitCapsule::CreateEffect( string& strEffectName, vec3& vPos, vec3& vDir )
{
	TEffectCharacterToolSinglyInvoker effectSinglyInvoker;
	effectSinglyInvoker.Invoke(strEffectName, vPos, vDir, vec3(vec3::AXISZ));
}