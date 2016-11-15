#ifndef _XPROJECTILE_CONTOLLER_H
#define _XPROJECTILE_CONTOLLER_H

using namespace rs3;
enum ProjectileColJudgmentType;

class XProjectileController
{
private:
	XObject*			m_pOwner;

private:
	void				GetMyProjectile(XObject* pAttacker, XTalentInfo* pTalentInfo, vector<XProjectile*>& outvecMyProjectile);
	XProjectile*		GetEndProjectile(XObject* pTarget, XTalentInfo* pTalentInfo, vector<XProjectile*>& vecMyProjectile);
	XProjectile*		GetNearProjectile(vector<XProjectile*>& vecMyProjectile, XTalentInfo* pTalentInfo);
	CSProjectileInfo*	GetProjectileInfo(XTalentInfo* pTalentInfo);

	void				ProjectileColJudgment(XObject* pTarget, XObject* pAttacker, XTalentInfo* pTalentInfo, ProjectileColJudgmentType colType);
	void				ShowEmptyProjrctileEffect(XObject* pTarget, XObject* pAttacker, XTalentInfo* pTalentInfo, ProjectileColJudgmentType colType);

public:
	XProjectileController();
	virtual ~XProjectileController();

	void				ProjectileHit(XObject* pTarget, XObject* pAttacker, XTalentInfo* pTalentInfo);
	void				ProjectileDefence(XObject* pTarget, XObject* pAttacker, XTalentInfo* pTalentInfo);
};

#endif