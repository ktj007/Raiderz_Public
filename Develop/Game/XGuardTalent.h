#ifndef _XGUARD_TALENT_H
#define _XGUARD_TALENT_H

#include "XTalent.h"


class XGuardTalent : public XBaseTalent, public MMemPool<XGuardTalent>
{
private:
	float						m_fGuardDurationTime;

	bool						CheckGuardTalentDurationTimeLimite(float fDelta);

protected:
	virtual void				OnEnterPhasePrepare();
	virtual void				OnEnterPhaseAct();
	virtual void				OnEnterPhaseFinish();

	virtual void				OnStart();
	virtual void				OnUpdate(float fDelta);

public:
	XGuardTalent(XObject* pOwner) : XBaseTalent(pOwner), m_fGuardDurationTime(0.0f) {}
	virtual ~XGuardTalent() {}
	virtual XTalentClassID		GetID() { return TCID_GUARD; }
};

#endif // _XGUARD_TALENT_H