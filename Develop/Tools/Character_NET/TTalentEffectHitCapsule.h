#ifndef _TTALENT_EFFECT_HIT_CAPSULE_H
#define _TTALENT_EFFECT_HIT_CAPSULE_H

#pragma once
#include "XBaseTalentEffectHitCapsule.h"

class TTalentEffectHitCapsule : public XBaseTalentEffectHitCapsule
{
private:
	TCharacter*		m_pActor;

protected:
	virtual	bool	CheckExistActor(MUID uidAttacker);
	virtual void	CreateEffect(string& strEffectName, vec3& vPos, vec3& vDir);

public:
	TTalentEffectHitCapsule();
	virtual ~TTalentEffectHitCapsule();

	void			Update(float fDelta, TCharacter* pActor);

	void			AddTalentHitCapsuleImpact(TCharacter* pActor, TTalentInfo* pTalentInfo, XTalentEffectInfo* pTalentEffectInfo, TALENT_EFFECT_TYPE eType);
};

#endif