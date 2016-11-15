#ifndef _XEXTRA_ACTIVE_TALENT_H
#define _XEXTRA_ACTIVE_TALENT_H

#include "XTalent.h"

class XExtraActiveTalent : public XBaseTalent, public MMemPool<XExtraActiveTalent>
{
protected:
	virtual void OnEnterPhaseAct();
public:
	XExtraActiveTalent(XObject* pOwner) : XBaseTalent(pOwner) {}
	virtual ~XExtraActiveTalent()	{}
	virtual XTalentClassID	GetID() { return TCID_EXTRA_ACTIVE; }
};


class XExtraActiveTalent_Grapple : public XBaseTalent, public MMemPool<XExtraActiveTalent_Grapple>
{
public:
	int			m_nGrappledHitFrame;
	float		m_fGrappledRemainTime;
	vec3		m_vGrappledPos;

private:
	void		GetGrappledHitInfo();

protected:
	virtual void OnEnterPhasePrepare();
	virtual void OnEnterPhaseAct();
	virtual void OnEnterPhaseExtraAct();
	virtual void OnEnterPhaseCanceled();
	virtual void OnEnterPhaseFinish();

	void		StartAttach();
	void		EndAttach();

public:
	XExtraActiveTalent_Grapple(XObject* pOwner) : XBaseTalent(pOwner), m_nGrappledHitFrame(0), m_fGrappledRemainTime(0.0f), m_vGrappledPos(vec3::ZERO) {}
	virtual ~XExtraActiveTalent_Grapple()	{}
	virtual XTalentClassID	GetID() { return TCID_EXTRA_ACTIVE_GRAPPLE; }
	virtual bool				IsNotUseActEventOnAct() { return true; }
};

class XExtraActiveTalent_Eat : public XExtraActiveTalent_Grapple
{
protected:
	virtual void OnEnterPhaseExtraAct2();
	virtual void OnEnterPhaseExtraAct3();

public:
	XExtraActiveTalent_Eat(XObject* pOwner) : XExtraActiveTalent_Grapple(pOwner) {}
	virtual ~XExtraActiveTalent_Eat()	{}
	virtual XTalentClassID	GetID() { return TCID_EXTRA_ACTIVE_EAT; }
};

#endif // _XEXTRA_ACTIVE_TALENT_H
