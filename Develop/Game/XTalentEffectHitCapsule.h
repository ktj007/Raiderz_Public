#ifndef _XTALENT_EFFECT_HIT_CAPSULE_H
#define	_XTALENT_EFFECT_HIT_CAPSULE_H

#include "XBaseTalentEffectHitCapsule.h"

//////////////////////////////////////////////////////////////////////////
class XTalentEffectHitCapsule : public XBaseTalentEffectHitCapsule
{
protected:
	virtual	bool	CheckExistActor(MUID uidAttacker);
	virtual void	CreateEffect(wstring& strEffectName, vec3& vPos, vec3& vDir);

public:
	XTalentEffectHitCapsule();
	virtual ~XTalentEffectHitCapsule();

	void			AddTalentHitCapsuleImpact(const MUID& uid, XTalentInfo* pTalentInfo, XTalentEffectInfo* pTalentEffectInfo, TALENT_EFFECT_TYPE eType, vec3 vTalentDir);
};

#endif // _XTALENT_EFFECT_HIT_CAPSULE_H
