#ifndef _XBASE_TALENT_EFFECT_HIT_CAPSULE_H
#define	_XBASE_TALENT_EFFECT_HIT_CAPSULE_H

#include "XTalentInfo.h"
#include "RActor.h"

using namespace rs3;

//////////////////////////////////////////////////////////////////////////
class XBaseTalentEffectHitCapsule
{
private:

	// ÅÅ·±Æ® ÆÇÁ¤ Ä¸½¶¿¡ ÀÇÇÑ È¿°ú
	struct TALENT_HIT_CAPSULE_IMPACT
	{
		TALENT_EFFECT_TYPE	nType;
		float				fElapsedTime;
		float				fMaxTime;
		XTalentInfo*		pTalentInfo;
		XTalentEffectInfo*	pTalentEffectInfo;
		MUID				uidAttacker;
		MMatrix				matAttackerWorld;
		vec3				vDirAttacker;

		TALENT_HIT_CAPSULE_IMPACT()
		{
			nType				= USE_EFFECT;
			fElapsedTime		= 0.0f;
			fMaxTime			= 0.0f;
			pTalentInfo			= NULL;
			pTalentEffectInfo	= NULL;
			uidAttacker.SetZero();

			matAttackerWorld.MakeIdentity();
			vDirAttacker		= vec3::ZERO;
		}
	};

	vector<TALENT_HIT_CAPSULE_IMPACT> m_vecTalentHitCapsuleImpact;

private:
	void			UpdateTalentHitCapsuleImpact(float fDelta);
	void			CheckTalentHitCapsuleEffect(XTalentInfo* pTalentInfo, XTalentEffectInfo* pTalentEffectInfo, TALENT_EFFECT_TYPE eType, float fPreTime, float fCurTime, MMatrix& matWorld, vec3& vDir);
	void			CreateTalentHitCapsuleEffect(XTalentInfo* pTalentInfo, XTalentEffectInfo* pTalentEffectInfo, TALENT_EFFECT_TYPE eType, vector<MCapsule>& vecTalentHitCapsule, MMatrix& matWorld, vec3& vDir);
	float			GetMaxCheckTime( XTalentInfo* pTalentInfo );

protected:
	virtual	bool	CheckExistActor(MUID uidAttacker) { return false; }
	virtual void	CreateEffect(tstring& strEffectName, vec3& vPos, vec3& vDir) {}

public:
	XBaseTalentEffectHitCapsule();
	virtual ~XBaseTalentEffectHitCapsule();

	void			Update(float fDelta);

	void			ClearAllTalentHitCapsuleImpact();
	virtual void	AddTalentHitCapsuleImpact(RActor* pActor, XTalentInfo* pTalentInfo, XTalentEffectInfo* pTalentEffectInfo, TALENT_EFFECT_TYPE eType, vec3 vTalentDir, MUID uid = MUID::ZERO);
};

#endif // _XTALENT_EFFECT_HIT_CAPSULE_H
